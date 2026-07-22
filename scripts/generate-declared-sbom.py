#!/usr/bin/env python3
# SPDX-FileCopyrightText: CERN
# SPDX-License-Identifier: GPL-3.0-or-later
"""
Automatically discover LGC2 third-party components and generate SBOMs.

Discovery sources (no hand-maintained component list required):
  1. CMake FetchContent_Declare (GIT_REPOSITORY / GIT_TAG)
  2. .gitmodules (vendored submodules)
  3. source/pyLGC/tests/requirements.txt
  4. NOTICE.md and lib/SurveyLib/NOTICE.md (license + path-based deps)
  5. README.md dependency table (versions for Eigen / TUT when present)

Workflow when adding a dependency:
  - FetchContent / submodule / requirements.txt → picked up automatically
  - Path-based libs (Eigen, TUT, …) → add a ### section under NOTICE
    (already required for attribution); SBOM generation parses it
  - Re-run this script or push to CI — no sbom/*.json editing

Usage:
  python scripts/generate-declared-sbom.py
  python scripts/generate-declared-sbom.py --out-dir sbom --version 2.11.0
"""

from __future__ import annotations

import argparse
import configparser
import json
import re
import uuid
from datetime import datetime, timezone
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]

# Map free-text NOTICE license strings → SPDX ids
LICENSE_ALIASES = {
    "mpl-2.0": "MPL-2.0",
    "mit": "MIT",
    "bsd-3-clause": "BSD-3-Clause",
    "bsd 3-clause": "BSD-3-Clause",
    "bsd-2-clause": "BSD-2-Clause",
    'bsd 2-clause "simplified" license': "BSD-2-Clause",
    "bsd 2-clause": "BSD-2-Clause",
    "gnu general public license v3.0 or later": "GPL-3.0-or-later",
    "gnu general public license v3.0": "GPL-3.0",
    "gpl-3.0-or-later": "GPL-3.0-or-later",
    "gpl-3.0": "GPL-3.0",
    "custom permissive license": "LicenseRef-counted-ptr",
}

DISPLAY_NAMES = {
    "treehh": "tree.hh",
    "rapidjson": "RapidJSON",
    "surveylib": "SurveyLib",
    "susoftcmakecommon": "SUSoftCMakeCommon",
}


def _now_iso() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def _bom_ref(name: str, version: str) -> str:
    safe = "".join(c if c.isalnum() or c in ".-_" else "-" for c in f"{name}@{version}")
    return f"pkg:{safe}"


def _license_entry(spdx_id: str) -> dict:
    return {"license": {"id": spdx_id}}


def normalize_license(text: str) -> str:
    raw = (text or "").strip()
    if not raw:
        return "NOASSERTION"
    key = raw.lower().strip()
    if key in LICENSE_ALIASES:
        return LICENSE_ALIASES[key]
    # Already looks like SPDX
    if re.fullmatch(r"[A-Za-z0-9.+-]+", raw) or raw.startswith("LicenseRef-"):
        return raw
    for alias, spdx in LICENSE_ALIASES.items():
        if alias in key:
            return spdx
    return raw


def display_name(name: str) -> str:
    return DISPLAY_NAMES.get(name.lower(), name)


def guess_purl(name: str, version: str, homepage: str) -> str:
    home = (homepage or "").rstrip("/")
    ver = version or "unknown"
    m = re.search(r"github\.com[/:]([^/]+)/([^/.]+)", home)
    if m:
        return f"pkg:github/{m.group(1)}/{m.group(2)}@{ver}"
    m = re.search(r"gitlab(?:\.com|\.cern\.ch)/(.+?)(?:\.git)?$", home)
    if m:
        path = m.group(1).removesuffix(".git")
        return f"pkg:generic/{path.replace('/', '-')}@{ver}"
    slug = re.sub(r"[^a-z0-9]+", "-", name.lower()).strip("-")
    return f"pkg:generic/{slug}@{ver}"


def merge_component(store: dict[str, dict], item: dict) -> None:
    """Merge by lowercase name; prefer concrete versions and non-empty fields."""
    key = item["name"].lower()
    if key not in store:
        store[key] = item
        return
    cur = store[key]
    weak = {"", "NOASSERTION", "unknown", None}
    for field in ("license", "homepage", "purl", "description", "cpe", "via"):
        new_val = item.get(field)
        if not new_val or new_val in weak:
            continue
        if not cur.get(field) or cur.get(field) in weak:
            cur[field] = new_val
    # Prefer git sha / semver over placeholders
    placeholders = {"", "unknown", "unpinned", "vendored", "NOASSERTION"}
    if item.get("version") and item["version"] not in placeholders:
        if cur.get("version") in placeholders:
            cur["version"] = item["version"]
        elif (
            item["version"][0].isdigit()
            and not str(cur.get("version", "x"))[0].isdigit()
        ):
            cur["version"] = item["version"]
        elif len(item["version"]) == 40 and len(cur.get("version", "")) != 40:
            cur["version"] = item["version"]
    sources = set(filter(None, [cur.get("source"), item.get("source")]))
    cur["source"] = " + ".join(sorted(sources))
    if item.get("scope") == "required":
        cur["scope"] = "required"


# ---------------------------------------------------------------------------
# Discoverers
# ---------------------------------------------------------------------------


def discover_fetchcontent(root: Path) -> list[dict]:
    comps: list[dict] = []
    pattern = re.compile(
        r"FetchContent_Declare\s*\(\s*([A-Za-z0-9._-]+)\s*(.*?)\n\s*\)",
        re.DOTALL | re.MULTILINE,
    )
    skip_parts = {
        "build",
        "build32",
        "out",
        ".git",
        ".vs",
        ".vscode",
        "sbom",
        "CMakeFiles",
        "_deps",
    }
    candidates: list[Path] = []
    for path in list(root.rglob("*.cmake")) + list(root.rglob("CMakeLists.txt")):
        if any(part in skip_parts for part in path.parts):
            continue
        candidates.append(path)
    for path in candidates:
        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        for match in pattern.finditer(text):
            name = match.group(1)
            body = match.group(2)
            repo_m = re.search(r'GIT_REPOSITORY\s+"([^"]+)"', body) or re.search(
                r"GIT_REPOSITORY\s+(\S+)", body
            )
            tag_m = re.search(r"GIT_TAG\s+(\S+)", body)
            if not repo_m:
                continue
            repo = repo_m.group(1).strip().strip('"')
            tag = tag_m.group(1).strip().strip('"') if tag_m else "unknown"
            # Drop inline cmake comments on GIT_TAG line
            tag = tag.split("#")[0].strip()
            nice = display_name(name)
            comps.append(
                {
                    "name": nice,
                    "version": tag,
                    "type": "library",
                    "license": "NOASSERTION",
                    "purl": guess_purl(nice, tag, repo),
                    "homepage": repo,
                    "scope": "required",
                    "description": f"CMake FetchContent ({path.relative_to(root).as_posix()})",
                    "source": path.relative_to(root).as_posix(),
                }
            )
    return comps


def discover_submodules(root: Path) -> list[dict]:
    gitmodules = root / ".gitmodules"
    if not gitmodules.is_file():
        return []
    # configparser needs section headers; .gitmodules already has [submodule "…"]
    parser = configparser.ConfigParser()
    parser.read(gitmodules, encoding="utf-8")
    comps = []
    for section in parser.sections():
        path = parser.get(section, "path", fallback="")
        url = parser.get(section, "url", fallback="")
        if not path:
            continue
        name = Path(path).name
        nice = display_name(name)
        # Resolve relative gitlab URLs to a stable homepage when possible
        homepage = url
        if url.startswith("../../"):
            homepage = f"https://gitlab.cern.ch/apc/susofts/{url[len('../../') :]}".removesuffix(
                ".git"
            )
        elif url.endswith(".git"):
            homepage = url[: -len(".git")]
        comps.append(
            {
                "name": nice,
                "version": "vendored",
                "type": "library",
                "license": "NOASSERTION",
                "purl": guess_purl(nice, "vendored", homepage),
                "homepage": homepage,
                "scope": "required",
                "description": f"Git submodule at {path}",
                "source": ".gitmodules",
            }
        )
    return comps


def discover_requirements(root: Path) -> list[dict]:
    req = root / "source" / "pyLGC" / "tests" / "requirements.txt"
    if not req.is_file():
        return []
    comps = []
    for line in req.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        # name==version or bare name
        m = re.match(r"^([A-Za-z0-9_.-]+)\s*([=<>!~]=?\s*(\S+))?", line)
        if not m:
            continue
        name = m.group(1)
        version = m.group(3) if m.group(3) else "unpinned"
        comps.append(
            {
                "name": name,
                "version": version,
                "type": "library",
                "license": "NOASSERTION",
                "purl": f"pkg:pypi/{name}"
                + (f"@{version}" if version != "unpinned" else ""),
                "homepage": f"https://pypi.org/project/{name}/",
                "scope": "optional",
                "description": "Python dependency for pyLGC tests",
                "source": "source/pyLGC/tests/requirements.txt",
            }
        )
    return comps


def parse_notice_components(notice_path: Path, via: str | None = None) -> list[dict]:
    if not notice_path.is_file():
        return []
    text = notice_path.read_text(encoding="utf-8")
    # Only the Third-Party Components section
    m = re.search(
        r"##\s+Third-Party Components\s*(.*?)(?=\n##\s|\Z)",
        text,
        re.DOTALL | re.IGNORECASE,
    )
    body = m.group(1) if m else text
    comps = []
    for block in re.finditer(
        r"###\s+(`?)([^`\n]+)\1\s*\n(.*?)(?=\n###\s|\Z)", body, re.DOTALL
    ):
        name = block.group(2).strip()
        section = block.group(3)
        lic_m = re.search(r"-\s*\*?\*?License\*?\*?:\s*(.+)", section, re.IGNORECASE)
        src_m = re.search(r"-\s*\*?\*?Source\*?\*?:\s*(\S+)", section, re.IGNORECASE)
        ver_m = re.search(r"-\s*\*?\*?Version\*?\*?:\s*(\S+)", section, re.IGNORECASE)
        license_id = normalize_license(lic_m.group(1) if lic_m else "")
        homepage = src_m.group(1).strip() if src_m else ""
        version = ver_m.group(1).strip() if ver_m else "unknown"
        item = {
            "name": name,
            "version": version,
            "type": "library",
            "license": license_id,
            "purl": guess_purl(name, version, homepage),
            "homepage": homepage,
            "scope": "required",
            "description": f"Documented in {notice_path.relative_to(REPO_ROOT).as_posix()}",
            "source": notice_path.relative_to(REPO_ROOT).as_posix(),
        }
        if via:
            item["via"] = via
        comps.append(item)
    return comps


def discover_readme_versions(root: Path) -> dict[str, str]:
    """Parse README dependency table rows like | **Eigen** | 3.4.0 | … |"""
    readme = root / "README.md"
    if not readme.is_file():
        return {}
    versions: dict[str, str] = {}
    for line in readme.read_text(encoding="utf-8").splitlines():
        m = re.match(r"\|\s*\*?\*?([A-Za-z0-9._-]+)\*?\*?\s*\|\s*\[?([^\]|]+)", line)
        if not m:
            continue
        name, ver = m.group(1), m.group(2).strip()
        # Skip header-ish rows
        if name.lower() in {"dependency", "name", "tool"}:
            continue
        # Strip link markup leftovers
        ver = ver.split("]")[0].strip()
        if re.search(r"\d", ver):
            versions[name.lower()] = ver
    return versions


def discover_all(root: Path) -> dict:
    store: dict[str, dict] = {}

    fc = discover_fetchcontent(root)
    sm = discover_submodules(root)
    req = discover_requirements(root)
    notice = parse_notice_components(root / "NOTICE.md")
    survey_notice = parse_notice_components(
        root / "lib" / "SurveyLib" / "NOTICE.md", via="SurveyLib"
    )

    for item in fc + sm + req + notice + survey_notice:
        merge_component(store, item)

    # Enrich versions from README where NOTICE/cmake left them unknown
    readme_vers = discover_readme_versions(root)
    placeholders = {"unknown", "unpinned", "vendored", "", "NOASSERTION"}
    for key, ver in readme_vers.items():
        if key in store and store[key].get("version") in placeholders:
            store[key]["version"] = ver
            src = store[key].get("source", "")
            store[key]["source"] = f"{src} + README.md".strip(" +")
            store[key]["purl"] = guess_purl(
                store[key]["name"], ver, store[key].get("homepage", "")
            )

    components = sorted(store.values(), key=lambda c: c["name"].lower())
    return {
        "project": {
            "name": "LGC2",
            "description": "CERN LGC survey and alignment software",
            "license": "GPL-3.0-or-later",
            "type": "application",
            "purl": "pkg:generic/cern/lgc2",
            "homepage": "https://github.com/geodetic-metrology-tools/LGC2",
        },
        "components": components,
        "discovery": {
            "fetchcontent": len(fc),
            "submodules": len(sm),
            "requirements": len(req),
            "notice_sections": len(notice) + len(survey_notice),
            "merged_total": len(components),
        },
    }


# ---------------------------------------------------------------------------
# SBOM writers (unchanged shape)
# ---------------------------------------------------------------------------


def build_cyclonedx(inventory: dict, version: str) -> dict:
    project = inventory["project"]
    root_ref = _bom_ref(project["name"], version)
    timestamp = _now_iso()

    components = []
    depends_on = []
    for item in inventory["components"]:
        ref = _bom_ref(item["name"], item.get("version", "unknown"))
        depends_on.append(ref)
        comp = {
            "bom-ref": ref,
            "type": item.get("type", "library"),
            "name": item["name"],
            "version": item.get("version", "unknown"),
            "scope": item.get("scope", "required"),
            "licenses": [_license_entry(item.get("license", "NOASSERTION"))],
            "purl": item.get("purl"),
            "description": item.get("description"),
            "externalReferences": [],
            "properties": [
                {
                    "name": "lgc:inventory:source",
                    "value": item.get("source", "discovered"),
                },
                {"name": "lgc:inventory:discovered", "value": "true"},
            ],
        }
        if item.get("homepage"):
            comp["externalReferences"].append(
                {"type": "website", "url": item["homepage"]}
            )
        if item.get("cpe"):
            comp["cpe"] = item["cpe"]
        if item.get("via"):
            comp["properties"].append(
                {"name": "lgc:inventory:via", "value": item["via"]}
            )
        if not comp["externalReferences"]:
            del comp["externalReferences"]
        if not comp.get("purl"):
            del comp["purl"]
        if not comp.get("description"):
            del comp["description"]
        components.append(comp)

    return {
        "bomFormat": "CycloneDX",
        "specVersion": "1.6",
        "serialNumber": f"urn:uuid:{uuid.uuid4()}",
        "version": 1,
        "metadata": {
            "timestamp": timestamp,
            "tools": {
                "components": [
                    {
                        "type": "application",
                        "name": "lgc-auto-sbom",
                        "version": "2.0.0",
                        "author": "CERN LGC2",
                    }
                ]
            },
            "component": {
                "bom-ref": root_ref,
                "type": project.get("type", "application"),
                "name": project["name"],
                "version": version,
                "description": project.get("description"),
                "licenses": [_license_entry(project["license"])],
                "purl": project.get("purl"),
            },
            "properties": [
                {"name": "lgc:sbom:kind", "value": "auto-discovered"},
            ],
        },
        "components": components,
        "dependencies": [
            {"ref": root_ref, "dependsOn": depends_on},
            *[{"ref": ref} for ref in depends_on],
        ],
    }


def build_spdx(inventory: dict, version: str) -> dict:
    project = inventory["project"]
    timestamp = _now_iso()
    doc_ns = f"https://cern.ch/lgc2/spdx/{uuid.uuid4()}"
    root_id = "SPDXRef-Package-LGC2"

    packages = [
        {
            "name": project["name"],
            "SPDXID": root_id,
            "versionInfo": version,
            "downloadLocation": project.get("homepage", "NOASSERTION"),
            "filesAnalyzed": False,
            "licenseConcluded": project["license"],
            "licenseDeclared": project["license"],
            "copyrightText": "CERN",
            "primaryPackagePurpose": "APPLICATION",
        }
    ]
    relationships = [
        {
            "spdxElementId": "SPDXRef-DOCUMENT",
            "relationshipType": "DESCRIBES",
            "relatedSpdxElement": root_id,
        }
    ]

    for idx, item in enumerate(inventory["components"], start=1):
        pkg_id = f"SPDXRef-Package-{idx}-{item['name']}".replace(".", "-").replace(
            " ", "-"
        )
        packages.append(
            {
                "name": item["name"],
                "SPDXID": pkg_id,
                "versionInfo": item.get("version", "NOASSERTION"),
                "downloadLocation": item.get("homepage", "NOASSERTION"),
                "filesAnalyzed": False,
                "licenseConcluded": item.get("license", "NOASSERTION"),
                "licenseDeclared": item.get("license", "NOASSERTION"),
                "copyrightText": "NOASSERTION",
                "primaryPackagePurpose": "LIBRARY",
                "comment": item.get("description", ""),
            }
        )
        relationships.append(
            {
                "spdxElementId": root_id,
                "relationshipType": "DEPENDS_ON",
                "relatedSpdxElement": pkg_id,
            }
        )

    return {
        "spdxVersion": "SPDX-2.3",
        "dataLicense": "CC0-1.0",
        "SPDXID": "SPDXRef-DOCUMENT",
        "name": f"{project['name']}-auto",
        "documentNamespace": doc_ns,
        "creationInfo": {
            "created": timestamp,
            "creators": ["Organization: CERN", "Tool: lgc-auto-sbom-2.0.0"],
            "licenseListVersion": "3.25",
        },
        "packages": packages,
        "relationships": relationships,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", type=Path, default=REPO_ROOT / "sbom")
    parser.add_argument("--version", default="")
    parser.add_argument("--compare-syft-dir", type=Path, default=None)
    args = parser.parse_args()

    version = args.version.strip()
    if not version:
        try:
            import subprocess

            version = (
                subprocess.check_output(
                    ["git", "rev-parse", "--short", "HEAD"],
                    cwd=REPO_ROOT,
                    text=True,
                ).strip()
                or "local"
            )
        except Exception:
            version = "local"

    inventory = discover_all(REPO_ROOT)
    args.out_dir.mkdir(parents=True, exist_ok=True)

    # Snapshot of what was discovered (artifact only — not hand-edited)
    snap_path = args.out_dir / "discovered-components.json"
    snap_path.write_text(json.dumps(inventory, indent=2) + "\n", encoding="utf-8")

    cdx = build_cyclonedx(inventory, version)
    spdx = build_spdx(inventory, version)
    cdx_path = args.out_dir / "lgc-declared.cdx.json"
    spdx_path = args.out_dir / "lgc-declared.spdx.json"
    cdx_path.write_text(json.dumps(cdx, indent=2) + "\n", encoding="utf-8")
    spdx_path.write_text(json.dumps(spdx, indent=2) + "\n", encoding="utf-8")

    print(f"Discovery: {inventory.get('discovery')}")
    print(f"Wrote {snap_path}")
    print(f"Wrote {cdx_path}")
    print(f"Wrote {spdx_path}")
    print(f"Components ({len(cdx['components'])}):")
    for c in cdx["components"]:
        lic = c["licenses"][0]["license"]["id"]
        print(f"  - {c['name']}@{c['version']} [{lic}]")

    compare_dir = args.compare_syft_dir
    if compare_dir is None:
        nested = REPO_ROOT / "sbom" / "sbom"
        compare_dir = nested if nested.is_dir() else args.out_dir
    # Lightweight efficacy stub
    report = {
        "discovered": {
            "components": len(cdx["components"]),
            "names": [c["name"] for c in cdx["components"]],
            "sources": inventory.get("discovery"),
        },
        "note": "Inventory is auto-discovered; regenerate with scripts/generate-declared-sbom.py",
    }
    report_path = args.out_dir / "sbom-efficacy-report.json"
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote {report_path}")


if __name__ == "__main__":
    main()
