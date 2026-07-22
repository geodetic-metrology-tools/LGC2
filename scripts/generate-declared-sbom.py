#!/usr/bin/env python3
# SPDX-FileCopyrightText: CERN
# SPDX-License-Identifier: GPL-3.0-or-later
"""
Discover LGC2 dependencies and optionally augment a Syft CycloneDX SBOM.

Discovery (writes sbom/discovered-components.json):
  - CMake FetchContent_Declare (GIT_REPOSITORY + GIT_TAG / commit SHA)
  - CMake find_package(...)
  - Git submodules (.gitmodules + git rev-parse when available)
  - NOTICE.md / SurveyLib NOTICE.md  → licenses + path-based deps ONLY
  - README "Component | Version" table → versions ONLY (never the License table)
  - source/pyLGC/tests/requirements.txt

Augmentation (Syft CDX + discovery → one CycloneDX document):
  - Keeps Syft components (binaries / ELF hints)
  - Adds/overlays discovered libraries with correct version, license, PURL
  - Builds dependency relationships (direct + transitive via SurveyLib)
  - Preserves lgc:inventory:* properties

Usage:
  python scripts/generate-declared-sbom.py discover --out-dir sbom
  python scripts/generate-declared-sbom.py augment \\
      --discovery sbom/discovered-components.json \\
      --syft sbom/lgc-linux-build.cdx.json \\
      --output sbom/lgc-linux-augmented.cdx.json \\
      --version 2.11.0
"""

from __future__ import annotations

import argparse
import configparser
import json
import re
import subprocess
import uuid
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


REPO_ROOT = Path(__file__).resolve().parents[1]

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

# SPDX / license-looking tokens must never be stored as version
LICENSE_LIKE = set(LICENSE_ALIASES.keys()) | {
    "mpl-2.0",
    "gpl-3.0-or-later",
    "gpl-3.0",
    "bsd-2-clause",
    "bsd-3-clause",
    "mit",
    "apache-2.0",
    "noassertion",
}

DISPLAY_NAMES = {
    "treehh": "tree.hh",
    "rapidjson": "RapidJSON",
    "surveylib": "SurveyLib",
    "susoftcmakecommon": "SUSoftCMakeCommon",
    "python3": "Python3",
    "openmp": "OpenMP",
    "openssl": "OpenSSL",
    "doxygen": "Doxygen",
}

# Logical dependency graph edges (parent → children). Keys are lowercase names.
GRAPH_EDGES: dict[str, list[str]] = {
    "lgc2": [
        "surveylib",
        "susoftcmakecommon",
        "eigen",
        "tut",
        "tree.hh",
        "numpy",
        "pytest",
        "python3",
        "openmp",
    ],
    "surveylib": [
        "eigen",
        "tut",
        "rapidjson",
        "reframe",
        "counted_ptr.h",
        "susoftcmakecommon",
        "openmp",
    ],
}

SKIP_DIR_PARTS = {
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


def _now_iso() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def display_name(name: str) -> str:
    return DISPLAY_NAMES.get(name.lower(), name)


def normalize_license(text: str) -> str:
    raw = (text or "").strip()
    if not raw:
        return "NOASSERTION"
    key = raw.lower().strip()
    if key in LICENSE_ALIASES:
        return LICENSE_ALIASES[key]
    if re.fullmatch(r"[A-Za-z0-9.+-]+", raw) or raw.startswith("LicenseRef-"):
        return raw
    for alias, spdx in LICENSE_ALIASES.items():
        if alias in key:
            return spdx
    return "NOASSERTION"


def is_license_like(value: str) -> bool:
    v = (value or "").strip().lower()
    if not v:
        return False
    if v in LICENSE_LIKE:
        return True
    if v.startswith("license"):
        return True
    # "BSD 2-Clause", "GPL-3.0-or-later", etc.
    if re.search(r"\b(gpl|mpl|bsd|mit|apache|license)\b", v):
        return True
    return False


def is_plausible_version(value: str) -> bool:
    v = (value or "").strip()
    if not v or v in {"unknown", "unpinned", "vendored", "NOASSERTION"}:
        return False
    if is_license_like(v):
        return False
    # semver, date, git sha, pep440-ish
    if re.fullmatch(r"[0-9a-f]{7,40}", v):
        return True
    if re.match(r"^\d+(\.\d+)*", v):
        return True
    if re.match(r"^\d{4}-\d{2}-\d{2}$", v):
        return True
    return False


def guess_purl(name: str, version: str | None, homepage: str) -> str:
    """Build a Package URL. Version is only appended when it is a real version."""
    home = (homepage or "").rstrip("/")
    home = re.sub(r"\.git$", "", home)
    ver = version if version and is_plausible_version(version) else None

    gh = re.search(r"github\.com[/:]([^/]+)/([^/]+)$", home)
    if gh:
        org, repo = gh.group(1), gh.group(2)
        base = f"pkg:github/{org}/{repo}"
        return f"{base}@{ver}" if ver else base

    gl = re.search(r"gitlab(?:\.com|\.cern\.ch)/(.+)$", home)
    if gl:
        path = gl.group(1)
        # generic gitlab path encoding
        base = (
            f"pkg:golang/{path}" if False else f"pkg:generic/{path.replace('/', '%2F')}"
        )
        # Prefer clearer generic form used by many SBOMs:
        base = f"pkg:generic/{path.replace('/', '-')}"
        return f"{base}@{ver}" if ver else base

    if name.lower() in {"numpy", "pytest"}:
        base = f"pkg:pypi/{name.lower()}"
        return f"{base}@{ver}" if ver else base

    slug = re.sub(r"[^a-z0-9]+", "-", name.lower()).strip("-")
    base = f"pkg:generic/{slug}"
    return f"{base}@{ver}" if ver else base


def bom_ref_for(comp: dict) -> str:
    name = comp["name"]
    ver = comp.get("version")
    if ver and is_plausible_version(ver):
        return f"{name}@{ver}"
    return name


def git_rev(path: Path) -> str | None:
    try:
        out = subprocess.check_output(
            ["git", "-C", str(path), "rev-parse", "HEAD"],
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
        return out or None
    except Exception:
        return None


def iter_cmake_files(root: Path) -> list[Path]:
    files: list[Path] = []
    for path in list(root.rglob("*.cmake")) + list(root.rglob("CMakeLists.txt")):
        if any(part in SKIP_DIR_PARTS for part in path.parts):
            continue
        files.append(path)
    return files


# ---------------------------------------------------------------------------
# Discoverers — keep license and version on separate code paths
# ---------------------------------------------------------------------------


def discover_fetchcontent(root: Path) -> list[dict]:
    comps: list[dict] = []
    pattern = re.compile(
        r"FetchContent_Declare\s*\(\s*([A-Za-z0-9._-]+)\s*(.*?)\n\s*\)",
        re.DOTALL | re.MULTILINE,
    )
    for path in iter_cmake_files(root):
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
            tag = tag_m.group(1).strip().strip('"') if tag_m else None
            if tag:
                tag = tag.split("#")[0].strip()
            if tag and not is_plausible_version(tag):
                tag = None
            nice = display_name(name)
            comps.append(
                {
                    "id": nice.lower(),
                    "name": nice,
                    "version": tag,
                    "license": None,  # filled only from NOTICE
                    "type": "library",
                    "purl": guess_purl(nice, tag, repo),
                    "homepage": re.sub(r"\.git$", "", repo),
                    "scope": "required",
                    "discovery": "fetchcontent",
                    "source": path.relative_to(root).as_posix(),
                }
            )
    return comps


def discover_find_package(root: Path) -> list[dict]:
    comps: list[dict] = []
    # find_package(Name ...)  — skip ${module} indirections
    pattern = re.compile(r"find_package\s*\(\s*([A-Za-z][A-Za-z0-9._+-]*)")
    seen: set[str] = set()
    for path in iter_cmake_files(root):
        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        for match in pattern.finditer(text):
            raw = match.group(1)
            if raw.startswith("$"):
                continue
            nice = display_name(raw)
            key = nice.lower()
            if key in seen:
                continue
            seen.add(key)
            # Tooling packages are optional; runtime-ish ones required when linked
            scope = "optional" if key in {"doxygen", "python3"} else "required"
            comps.append(
                {
                    "id": key,
                    "name": nice,
                    "version": None,
                    "license": None,
                    "type": "library",
                    "purl": guess_purl(nice, None, ""),
                    "homepage": "",
                    "scope": scope,
                    "discovery": "find_package",
                    "source": path.relative_to(root).as_posix(),
                }
            )
    return comps


def discover_submodules(root: Path) -> list[dict]:
    gitmodules = root / ".gitmodules"
    if not gitmodules.is_file():
        return []
    parser = configparser.ConfigParser()
    parser.read(gitmodules, encoding="utf-8")
    comps = []
    for section in parser.sections():
        path = parser.get(section, "path", fallback="")
        url = parser.get(section, "url", fallback="")
        if not path:
            continue
        name = display_name(Path(path).name)
        homepage = url
        if url.startswith("../../"):
            homepage = (
                "https://gitlab.cern.ch/apc/susofts/" + url[len("../../") :]
            ).removesuffix(".git")
        elif url.endswith(".git"):
            homepage = url[: -len(".git")]
        version = git_rev(root / path)
        comps.append(
            {
                "id": name.lower(),
                "name": name,
                "version": version,  # commit SHA when checkout present
                "license": None,
                "type": "library",
                "purl": guess_purl(name, version, homepage),
                "homepage": homepage,
                "scope": "required",
                "discovery": "submodule",
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
        m = re.match(r"^([A-Za-z0-9_.-]+)\s*(?:[=<>!~]=?\s*(\S+))?", line)
        if not m:
            continue
        name = m.group(1)
        version = (
            m.group(2) if m.group(2) and is_plausible_version(m.group(2)) else None
        )
        comps.append(
            {
                "id": name.lower(),
                "name": name,
                "version": version,
                "license": None,
                "type": "library",
                "purl": guess_purl(name, version, f"https://pypi.org/project/{name}/"),
                "homepage": f"https://pypi.org/project/{name}/",
                "scope": "optional",
                "discovery": "requirements.txt",
                "source": "source/pyLGC/tests/requirements.txt",
            }
        )
    return comps


def parse_notice_licenses(notice_path: Path, via: str | None = None) -> list[dict]:
    """Parse NOTICE for name + license + homepage. Never treat license as version."""
    if not notice_path.is_file():
        return []
    text = notice_path.read_text(encoding="utf-8")
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
        # Optional explicit Version: line only (not License)
        ver_m = re.search(r"-\s*\*?\*?Version\*?\*?:\s*(\S+)", section, re.IGNORECASE)
        license_id = normalize_license(lic_m.group(1) if lic_m else "")
        homepage = src_m.group(1).strip() if src_m else ""
        version = None
        if ver_m:
            candidate = ver_m.group(1).strip()
            if is_plausible_version(candidate):
                version = candidate
        item = {
            "id": name.lower(),
            "name": name,
            "version": version,
            "license": license_id,
            "type": "library",
            "purl": guess_purl(name, version, homepage),
            "homepage": homepage,
            "scope": "required",
            "discovery": "notice",
            "source": notice_path.relative_to(REPO_ROOT).as_posix(),
        }
        if via:
            item["via"] = via
        comps.append(item)
    return comps


def discover_readme_versions(root: Path) -> dict[str, str]:
    """
    Only parse the build-requirements table:
      | Component | Version / Example | Purpose |
    Never parse the Third-Party license table (| Library | License | Source |).
    """
    readme = root / "README.md"
    if not readme.is_file():
        return {}
    versions: dict[str, str] = {}
    in_version_table = False
    for line in readme.read_text(encoding="utf-8").splitlines():
        header = line.lower()
        if "|" in line and "version" in header and "purpose" in header:
            in_version_table = True
            continue
        if "|" in line and "license" in header and "source" in header:
            in_version_table = False
            continue
        if not in_version_table:
            continue
        if re.match(r"^\|\s*-+", line):
            continue
        m = re.match(r"\|\s*\*?\*?([A-Za-z0-9._-]+)\*?\*?\s*\|\s*\[?([^\]|]+)", line)
        if not m:
            # blank / end of table
            if line.strip() == "" or not line.strip().startswith("|"):
                in_version_table = False
            continue
        name, ver_cell = m.group(1), m.group(2).strip()
        ver = ver_cell.split("]")[0].strip()
        # Skip non-dependency rows
        if name.lower() in {
            "c++",
            "cmake",
            "nsis",
            "git",
            "doxygen",
            "graphviz",
            "component",
        }:
            continue
        if is_plausible_version(ver):
            versions[name.lower()] = ver
    return versions


def merge_component(store: dict[str, dict], item: dict) -> None:
    key = item["id"]
    if key not in store:
        store[key] = dict(item)
        return
    cur = store[key]

    # Version: only accept plausible versions; never license strings
    new_ver = item.get("version")
    if new_ver and is_plausible_version(new_ver):
        cur_ver = cur.get("version")
        if not cur_ver or not is_plausible_version(cur_ver):
            cur["version"] = new_ver
        elif len(new_ver) == 40 and len(str(cur_ver)) != 40:
            # Prefer full git SHA
            cur["version"] = new_ver
        elif re.match(r"^\d+\.\d+", new_ver) and not re.match(
            r"^\d+\.\d+", str(cur_ver)
        ):
            cur["version"] = new_ver

    # License: only from NOTICE / explicit license field — never from version
    new_lic = item.get("license")
    if new_lic and new_lic != "NOASSERTION":
        if not cur.get("license") or cur.get("license") == "NOASSERTION":
            cur["license"] = new_lic

    for field in ("homepage", "description", "via", "type", "scope"):
        if item.get(field) and not cur.get(field):
            cur[field] = item[field]

    # Track discovery sources
    sources = []
    for s in (cur.get("source"), item.get("source")):
        if s:
            sources.extend(x.strip() for x in str(s).split("+"))
    cur["source"] = " + ".join(sorted({s for s in sources if s}))

    discoveries = []
    for d in (cur.get("discovery"), item.get("discovery")):
        if d:
            discoveries.extend(x.strip() for x in str(d).split("+"))
    cur["discovery"] = "+".join(sorted({d for d in discoveries if d}))

    # Refresh PURL from best version + homepage
    cur["purl"] = guess_purl(cur["name"], cur.get("version"), cur.get("homepage", ""))


def build_dependency_graph(
    components: list[dict], project_name: str = "LGC2"
) -> list[dict]:
    by_id = {c["id"]: c for c in components}
    # Ensure root
    refs = {cid: bom_ref_for(c) for cid, c in by_id.items()}
    root_ref = project_name

    deps: list[dict] = []
    # Root edges
    root_children = []
    for child_id in GRAPH_EDGES.get("lgc2", []):
        if child_id in by_id:
            root_children.append(refs[child_id])
    # Also attach anything not listed under surveylib as direct if unknown parent
    survey_kids = set(GRAPH_EDGES.get("surveylib", []))
    for cid, c in by_id.items():
        if cid in survey_kids:
            continue
        if cid in GRAPH_EDGES.get("lgc2", []):
            continue
        # orphans (e.g. OpenSSL, Doxygen) hang off root as optional tooling
        if c.get("discovery") == "find_package":
            root_children.append(refs[cid])

    deps.append({"ref": root_ref, "dependsOn": sorted(set(root_children))})

    for parent_id, children in GRAPH_EDGES.items():
        if parent_id == "lgc2":
            continue
        if parent_id not in by_id:
            continue
        child_refs = [refs[c] for c in children if c in by_id]
        deps.append({"ref": refs[parent_id], "dependsOn": sorted(set(child_refs))})

    # Leaf nodes
    for cid, ref in refs.items():
        if not any(d["ref"] == ref for d in deps):
            deps.append({"ref": ref})

    return deps


def discover_all(root: Path) -> dict:
    store: dict[str, dict] = {}

    buckets = {
        "fetchcontent": discover_fetchcontent(root),
        "find_package": discover_find_package(root),
        "submodules": discover_submodules(root),
        "requirements": discover_requirements(root),
        "notice": parse_notice_licenses(root / "NOTICE.md"),
        "surveylib_notice": parse_notice_licenses(
            root / "lib" / "SurveyLib" / "NOTICE.md", via="SurveyLib"
        ),
    }
    for items in buckets.values():
        for item in items:
            merge_component(store, item)

    # Versions from README version table only
    for key, ver in discover_readme_versions(root).items():
        if key in store:
            merge_component(
                store,
                {
                    "id": key,
                    "name": store[key]["name"],
                    "version": ver,
                    "license": None,
                    "source": "README.md",
                    "discovery": "readme_version",
                },
            )
        elif key in {"eigen", "tut"}:
            merge_component(
                store,
                {
                    "id": key,
                    "name": display_name(key),
                    "version": ver,
                    "license": None,
                    "type": "library",
                    "purl": guess_purl(display_name(key), ver, ""),
                    "homepage": "",
                    "scope": "required",
                    "source": "README.md",
                    "discovery": "readme_version",
                },
            )

    # Sanitize: wipe any license-like versions that slipped through
    for c in store.values():
        if c.get("version") and is_license_like(str(c["version"])):
            c["version"] = None
        if not c.get("license"):
            c["license"] = "NOASSERTION"
        c["purl"] = guess_purl(c["name"], c.get("version"), c.get("homepage", ""))
        c["bom-ref"] = bom_ref_for(c)

    components = sorted(store.values(), key=lambda c: c["name"].lower())
    graph = build_dependency_graph(components)

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
        "dependencies": graph,
        "discovery": {k: len(v) for k, v in buckets.items()}
        | {"merged_total": len(components)},
    }


# ---------------------------------------------------------------------------
# CycloneDX helpers + Syft augmentation
# ---------------------------------------------------------------------------


def component_to_cdx(comp: dict) -> dict:
    ref = comp.get("bom-ref") or bom_ref_for(comp)
    out: dict[str, Any] = {
        "bom-ref": ref,
        "type": comp.get("type", "library"),
        "name": comp["name"],
        "scope": comp.get("scope", "required"),
        "licenses": [{"license": {"id": comp.get("license") or "NOASSERTION"}}],
        "properties": [
            {
                "name": "lgc:inventory:source",
                "value": comp.get("source") or "discovered",
            },
            {
                "name": "lgc:inventory:discovered",
                "value": "true",
            },
            {
                "name": "lgc:inventory:discovery",
                "value": comp.get("discovery") or "unknown",
            },
        ],
    }
    if comp.get("version") and is_plausible_version(str(comp["version"])):
        out["version"] = comp["version"]
    if comp.get("purl"):
        out["purl"] = comp["purl"]
    if comp.get("homepage"):
        out["externalReferences"] = [{"type": "website", "url": comp["homepage"]}]
    if comp.get("via"):
        out["properties"].append({"name": "lgc:inventory:via", "value": comp["via"]})
    if comp.get("description"):
        out["description"] = comp["description"]
    return out


def discovery_to_stub_cdx(discovery: dict, version: str) -> dict:
    """Minimal CDX from discovery alone (used when no Syft input is provided)."""
    root = discovery["project"]["name"]
    comps = [component_to_cdx(c) for c in discovery["components"]]
    deps = []
    for d in discovery.get("dependencies") or []:
        entry = {"ref": d["ref"] if d["ref"] != root else f"{root}@{version}"}
        # remap root
        if d["ref"] == root:
            entry["ref"] = f"{root}@{version}"
        if d.get("dependsOn"):
            entry["dependsOn"] = d["dependsOn"]
        deps.append(entry)

    return {
        "bomFormat": "CycloneDX",
        "specVersion": "1.6",
        "serialNumber": f"urn:uuid:{uuid.uuid4()}",
        "version": 1,
        "metadata": {
            "timestamp": _now_iso(),
            "tools": {
                "components": [
                    {
                        "type": "application",
                        "name": "lgc-sbom-discover",
                        "version": "3.0.0",
                        "author": "CERN LGC2",
                    }
                ]
            },
            "component": {
                "bom-ref": f"{root}@{version}",
                "type": "application",
                "name": root,
                "version": version,
                "licenses": [{"license": {"id": discovery["project"]["license"]}}],
                "purl": discovery["project"].get("purl"),
            },
            "properties": [{"name": "lgc:sbom:kind", "value": "discovery-stub"}],
        },
        "components": comps,
        "dependencies": deps,
    }


def augment_syft_cdx(syft: dict, discovery: dict, product_version: str) -> dict:
    """Merge discovered metadata into a Syft CycloneDX document."""
    out = json.loads(json.dumps(syft))  # deep copy
    out["specVersion"] = out.get("specVersion") or "1.6"
    out["serialNumber"] = f"urn:uuid:{uuid.uuid4()}"
    out["metadata"] = out.get("metadata") or {}
    out["metadata"]["timestamp"] = _now_iso()

    tools = out["metadata"].setdefault("tools", {})
    tool_comps = tools.setdefault("components", [])
    tool_comps.append(
        {
            "type": "application",
            "name": "lgc-sbom-augment",
            "version": "3.0.0",
            "author": "CERN LGC2",
        }
    )

    meta_comp = out["metadata"].setdefault("component", {})
    meta_comp["name"] = discovery["project"]["name"]
    meta_comp["version"] = product_version
    meta_comp["bom-ref"] = f"{discovery['project']['name']}@{product_version}"
    meta_comp["licenses"] = [{"license": {"id": discovery["project"]["license"]}}]
    if discovery["project"].get("purl"):
        meta_comp["purl"] = discovery["project"]["purl"]

    props = out["metadata"].setdefault("properties", [])
    props.append({"name": "lgc:sbom:kind", "value": "syft-augmented-with-discovery"})

    existing = out.setdefault("components", [])
    by_name = {
        (c.get("name") or "").lower(): c for c in existing if c.get("type") != "file"
    }

    for disc in discovery["components"]:
        cdx = component_to_cdx(disc)
        key = disc["name"].lower()
        if key in by_name:
            target = by_name[key]
            # Overlay authoritative fields; keep Syft hashes / extra props
            if "version" in cdx:
                target["version"] = cdx["version"]
            target["licenses"] = cdx["licenses"]
            if cdx.get("purl"):
                target["purl"] = cdx["purl"]
            target["bom-ref"] = cdx["bom-ref"]
            target.setdefault("properties", []).extend(cdx.get("properties") or [])
            if cdx.get("externalReferences"):
                target["externalReferences"] = cdx["externalReferences"]
            target["scope"] = cdx.get("scope", target.get("scope", "required"))
        else:
            existing.append(cdx)
            by_name[key] = cdx

    # Rebuild dependency graph from discovery + keep Syft deps that don't conflict
    root_ref = f"{discovery['project']['name']}@{product_version}"
    new_deps = []
    for d in discovery.get("dependencies") or []:
        entry: dict[str, Any] = {
            "ref": root_ref if d["ref"] == discovery["project"]["name"] else d["ref"]
        }
        if d.get("dependsOn"):
            entry["dependsOn"] = d["dependsOn"]
        new_deps.append(entry)

    # Attach remaining Syft-only components (e.g. LGC binary package) under root
    discovered_names = {c["name"].lower() for c in discovery["components"]}
    syft_extra_refs = []
    for c in existing:
        if c.get("type") == "file":
            continue
        name = (c.get("name") or "").lower()
        if name in discovered_names or name in {"lgc2"}:
            continue
        ref = c.get("bom-ref") or c.get("name")
        if ref:
            syft_extra_refs.append(ref)
            # ensure leaf
            if not any(x.get("ref") == ref for x in new_deps):
                new_deps.append({"ref": ref})

    for d in new_deps:
        if d["ref"] == root_ref and syft_extra_refs:
            d.setdefault("dependsOn", [])
            for r in syft_extra_refs:
                if r not in d["dependsOn"]:
                    d["dependsOn"].append(r)

    out["dependencies"] = new_deps
    return out


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------


def cmd_discover(args: argparse.Namespace) -> None:
    discovery = discover_all(REPO_ROOT)
    args.out_dir.mkdir(parents=True, exist_ok=True)
    path = args.out_dir / "discovered-components.json"
    path.write_text(json.dumps(discovery, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote {path}")
    print(f"Discovery counts: {discovery['discovery']}")
    for c in discovery["components"]:
        ver = c.get("version") or "(no version)"
        lic = c.get("license") or "NOASSERTION"
        assert not is_license_like(str(ver)) or ver in {
            None,
            "(no version)",
        }, f"version/license mixup for {c['name']}: {ver}"
        print(f"  - {c['name']}: version={ver}  license={lic}  purl={c.get('purl')}")


def cmd_augment(args: argparse.Namespace) -> None:
    discovery = json.loads(args.discovery.read_text(encoding="utf-8"))
    version = (args.version or "").strip()
    if not version:
        try:
            version = subprocess.check_output(
                ["git", "rev-parse", "--short", "HEAD"], cwd=REPO_ROOT, text=True
            ).strip()
        except Exception:
            version = "local"

    if args.syft and args.syft.is_file():
        syft = json.loads(args.syft.read_text(encoding="utf-8"))
        out = augment_syft_cdx(syft, discovery, version)
    else:
        print("No Syft CDX provided/found; emitting discovery-only CycloneDX stub")
        out = discovery_to_stub_cdx(discovery, version)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(out, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote {args.output}")
    print(
        f"components={len(out.get('components') or [])} "
        f"dependencies={len(out.get('dependencies') or [])}"
    )


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="cmd", required=True)

    p_disc = sub.add_parser("discover", help="Write discovered-components.json")
    p_disc.add_argument("--out-dir", type=Path, default=REPO_ROOT / "sbom")
    p_disc.set_defaults(func=cmd_discover)

    p_aug = sub.add_parser("augment", help="Augment Syft CDX with discovery metadata")
    p_aug.add_argument(
        "--discovery",
        type=Path,
        default=REPO_ROOT / "sbom" / "discovered-components.json",
    )
    p_aug.add_argument("--syft", type=Path, default=None)
    p_aug.add_argument(
        "--output",
        type=Path,
        default=REPO_ROOT / "sbom" / "lgc-augmented.cdx.json",
    )
    p_aug.add_argument("--version", default="")
    p_aug.set_defaults(func=cmd_augment)

    # Convenience: discover + augment in one shot (CI)
    p_all = sub.add_parser("ci", help="Discover then augment Syft CDX (CI entrypoint)")
    p_all.add_argument("--out-dir", type=Path, default=REPO_ROOT / "sbom")
    p_all.add_argument("--syft", type=Path, required=True)
    p_all.add_argument("--version", default="")
    p_all.set_defaults(func=None)

    args = parser.parse_args()
    if args.cmd == "ci":
        version = (
            args.version.strip()
            or subprocess.check_output(
                ["git", "rev-parse", "--short", "HEAD"], cwd=REPO_ROOT, text=True
            ).strip()
        )
        disc_args = argparse.Namespace(out_dir=args.out_dir)
        cmd_discover(disc_args)
        aug_args = argparse.Namespace(
            discovery=args.out_dir / "discovered-components.json",
            syft=args.syft,
            output=args.out_dir / "lgc-augmented.cdx.json",
            version=version,
        )
        cmd_augment(aug_args)
    else:
        args.func(args)


if __name__ == "__main__":
    main()
