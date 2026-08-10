#!/usr/bin/env python3
# SPDX-FileCopyrightText: CERN
# SPDX-License-Identifier: GPL-3.0-or-later
"""
Generate / augment LGC2 CycloneDX SBOMs from dependency.csv (single source of truth).

dependency.csv columns:
  name, version, license, homepage, purl, scope, parent

Usage:
  python scripts/generate-declared-sbom.py load --out-dir sbom
  python scripts/generate-declared-sbom.py augment \\
      --deps-file dependency.csv \\
      --syft sbom/lgc-linux-build.cdx.json \\
      --output sbom/lgc-linux-augmented.cdx.json \\
      --version 2.11.0
  python scripts/generate-declared-sbom.py ci \\
      --out-dir sbom --syft sbom/lgc-linux-build.cdx.json --version $CI_COMMIT_SHORT_SHA
"""

from __future__ import annotations

import argparse
import csv
import json
import re
import subprocess
import uuid
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_DEPS_FILE = REPO_ROOT / "dependency.csv"

PROJECT_BINARY_NAMES = {"lgc", "pylgc_c", "libpylgc_c"}

REQUIRED_CSV_COLUMNS = {
    "name",
    "version",
    "license",
    "homepage",
    "purl",
    "scope",
    "parent",
}


def _now_iso() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def is_plausible_version(value: str | None) -> bool:
    v = (value or "").strip()
    if not v or v.upper() in {"UNKNOWN", "NOASSERTION", "UNPINNED", "VENDORED"}:
        return False
    if re.search(r"\b(gpl|mpl|bsd|mit|apache|license)\b", v, re.I):
        return False
    if re.fullmatch(r"[0-9a-f]{7,40}", v):
        return True
    if re.match(r"^\d+(\.\d+)*", v):
        return True
    if re.match(r"^\d{4}-\d{2}-\d{2}$", v):
        return True
    return False


def read_cmake_project_version(root: Path) -> str | None:
    cmake = root / "source" / "CMakeLists.txt"
    if not cmake.is_file():
        return None
    text = cmake.read_text(encoding="utf-8", errors="ignore")
    parts = {}
    for key in ("MAJOR", "MINOR", "PATCH"):
        m = re.search(rf'set\s*\(\s*EXE_VERSION_{key}\s+"([^"]+)"\s*\)', text)
        if m:
            parts[key] = m.group(1)
    if {"MAJOR", "MINOR", "PATCH"} <= parts.keys():
        return f"{parts['MAJOR']}.{parts['MINOR']}.{parts['PATCH']}"
    return None


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


def enrich_submodule_versions(components: list[dict], root: Path) -> None:
    """If version empty for known submodules, fill from git checkout SHA."""
    path_map = {
        "surveylib": root / "lib" / "SurveyLib",
        "susoftcmakecommon": root / "lib" / "SUSoftCMakeCommon",
    }
    for c in components:
        if c.get("version"):
            continue
        p = path_map.get(c["id"])
        if p and p.is_dir():
            sha = git_rev(p)
            if sha:
                c["version"] = sha
                c["purl"] = with_version(c.get("purl_base") or c.get("purl"), sha)


def with_version(purl_base: str | None, version: str | None) -> str | None:
    if not purl_base:
        return None
    base = purl_base.split("@", 1)[0]
    if version and is_plausible_version(version):
        return f"{base}@{version}"
    return base


def bom_ref_for(name: str, version: str | None) -> str:
    if version and is_plausible_version(version):
        return f"{name}@{version}"
    return name


def parse_dependency_csv(path: Path) -> list[dict]:
    if not path.is_file():
        raise SystemExit(f"dependency file not found: {path}")

    components: list[dict] = []
    with path.open(encoding="utf-8", newline="") as fh:
        reader = csv.DictReader(fh)
        if not reader.fieldnames:
            raise SystemExit(f"{path}: missing header row")
        headers = {h.strip() for h in reader.fieldnames if h}
        missing = REQUIRED_CSV_COLUMNS - headers
        if missing:
            raise SystemExit(f"{path}: missing columns: {', '.join(sorted(missing))}")

        for lineno, row in enumerate(reader, start=2):
            name = (row.get("name") or "").strip()
            if not name:
                continue  # skip blank rows

            version = (row.get("version") or "").strip() or None
            license_id = (row.get("license") or "").strip() or "NOASSERTION"
            homepage = (row.get("homepage") or "").strip()
            purl = (row.get("purl") or "").strip() or None
            scope = (row.get("scope") or "").strip() or "required"
            parent = (row.get("parent") or "").strip() or "LGC2"

            if scope not in {"required", "optional"}:
                raise SystemExit(
                    f"{path}:{lineno}: scope must be 'required' or 'optional', got {scope!r}"
                )
            if version and not is_plausible_version(version):
                raise SystemExit(
                    f"{path}:{lineno}: version {version!r} looks like a license/invalid value"
                )

            components.append(
                {
                    "id": name.lower(),
                    "name": name,
                    "version": version,
                    "license": license_id,
                    "homepage": homepage,
                    "purl_base": purl,
                    "purl": with_version(purl, version),
                    "scope": scope,
                    "parent": parent,
                    "type": "library",
                    "source": path.name,
                    "bom-ref": bom_ref_for(name, version),
                }
            )

    if not components:
        raise SystemExit(f"{path}: no dependencies defined")
    return components


def build_inventory(deps_file: Path, root: Path) -> dict:
    components = parse_dependency_csv(deps_file)
    enrich_submodule_versions(components, root)
    # refresh bom-ref / purl after enrichment
    for c in components:
        c["bom-ref"] = bom_ref_for(c["name"], c.get("version"))
        c["purl"] = with_version(c.get("purl_base") or c.get("purl"), c.get("version"))

    cmake_ver = read_cmake_project_version(root)
    graph = build_dependency_graph(components, project_name="LGC2")

    try:
        source = str(deps_file.resolve().relative_to(root.resolve()))
    except ValueError:
        source = str(deps_file)

    return {
        "project": {
            "name": "LGC2",
            "description": "CERN LGC survey and alignment software",
            "license": "GPL-3.0-or-later",
            "type": "application",
            "version": cmake_ver,
            "purl": with_version("pkg:generic/cern/lgc2", cmake_ver),
            "homepage": "https://github.com/geodetic-metrology-tools/LGC2",
        },
        "components": components,
        "dependencies": graph,
        "source": source,
    }


def build_dependency_graph(
    components: list[dict], project_name: str = "LGC2"
) -> list[dict]:
    """Build graph from the parent column in dependency.csv."""
    # id → bom-ref for resolving parent names (e.g. SurveyLib) to component refs
    refs = {c["id"]: c["bom-ref"] for c in components}
    # parent name (display) → list of child refs
    edges: dict[str, list[str]] = {}

    for c in components:
        parent = c.get("parent") or project_name
        edges.setdefault(parent, []).append(c["bom-ref"])

    deps: list[dict] = []
    # Root
    root_kids = sorted(set(edges.get(project_name, [])))
    deps.append({"ref": project_name, "dependsOn": root_kids})

    # Intermediate parents that are also components (e.g. SurveyLib)
    for parent, kids in sorted(edges.items()):
        if parent == project_name:
            continue
        parent_id = parent.lower()
        parent_ref = refs.get(parent_id, parent)
        deps.append({"ref": parent_ref, "dependsOn": sorted(set(kids))})

    # Leaves
    for c in components:
        ref = c["bom-ref"]
        if not any(d.get("ref") == ref for d in deps):
            deps.append({"ref": ref})

    # sanity: parents that are not LGC2 and not in component list
    known_parents = {project_name} | {c["name"] for c in components}
    for parent in edges:
        if parent not in known_parents:
            raise SystemExit(
                f"dependency.csv: unknown parent {parent!r} "
                f"(must be {project_name!r} or a listed dependency name)"
            )

    return deps


def component_to_cdx(comp: dict) -> dict:
    out: dict[str, Any] = {
        "bom-ref": comp["bom-ref"],
        "type": comp.get("type", "library"),
        "name": comp["name"],
        "scope": comp.get("scope", "required"),
        "licenses": [{"license": {"id": comp.get("license") or "NOASSERTION"}}],
        "properties": [
            {
                "name": "lgc:inventory:source",
                "value": comp.get("source", "dependency.csv"),
            },
            {"name": "lgc:inventory:declared", "value": "true"},
            {"name": "lgc:inventory:parent", "value": comp.get("parent") or "LGC2"},
        ],
    }
    if comp.get("version") and is_plausible_version(str(comp["version"])):
        out["version"] = comp["version"]
    if comp.get("purl"):
        out["purl"] = comp["purl"]
    if comp.get("homepage"):
        out["externalReferences"] = [{"type": "website", "url": comp["homepage"]}]
    return out


def strip_weak_cpes(component: dict) -> None:
    cpe = component.get("cpe")
    if isinstance(cpe, str):
        if ":*:" in cpe or cpe.endswith(":*") or ":UNKNOWN:" in cpe.upper():
            component.pop("cpe", None)
        elif str(component.get("version", "")).upper() in {
            "",
            "UNKNOWN",
            "NOASSERTION",
        }:
            component.pop("cpe", None)
    props = component.get("properties")
    if isinstance(props, list):
        component["properties"] = [
            p
            for p in props
            if not (
                isinstance(p, dict) and str(p.get("name", "")).startswith("syft:cpe")
            )
        ]


def normalize_syft_project_binaries(
    components: list[dict], product_version: str, cmake_version: str | None
) -> None:
    preferred = cmake_version or product_version
    for c in components:
        if c.get("type") == "file":
            strip_weak_cpes(c)
            continue
        name = (c.get("name") or "").lower().replace("-", "_").removeprefix("lib")
        if name in PROJECT_BINARY_NAMES:
            ver = str(c.get("version") or "")
            if not ver or ver.upper() in {"UNKNOWN", "NOASSERTION"}:
                c["version"] = preferred
                c.setdefault("properties", []).append(
                    {
                        "name": "lgc:inventory:version_source",
                        "value": "cmake-project-version"
                        if cmake_version
                        else "git-or-ci-version",
                    }
                )
            c["type"] = "library" if "pylgc" in name else "application"
            base = (
                "pkg:generic/cern/pylgc" if "pylgc" in name else "pkg:generic/cern/lgc"
            )
            c["purl"] = with_version(base, c.get("version"))
            c.pop("cpe", None)
            props = c.get("properties")
            if isinstance(props, list):
                c["properties"] = [
                    p
                    for p in props
                    if not (
                        isinstance(p, dict)
                        and str(p.get("name", "")).startswith("syft:cpe")
                    )
                ]
            c["bom-ref"] = bom_ref_for(c.get("name") or name, c.get("version"))
        else:
            strip_weak_cpes(c)


def augment_syft_cdx(syft: dict, inventory: dict, product_version: str) -> dict:
    out = json.loads(json.dumps(syft))
    out["specVersion"] = out.get("specVersion") or "1.6"
    out["serialNumber"] = f"urn:uuid:{uuid.uuid4()}"
    out["metadata"] = out.get("metadata") or {}
    out["metadata"]["timestamp"] = _now_iso()

    cmake_version = inventory["project"].get("version") or read_cmake_project_version(
        REPO_ROOT
    )
    root_version = cmake_version or product_version
    root_name = inventory["project"]["name"]

    tools = out["metadata"].setdefault("tools", {})
    tools.setdefault("components", []).append(
        {
            "type": "application",
            "name": "lgc-sbom-from-dependency-txt",
            "version": "4.0.0",
            "author": "CERN LGC2",
        }
    )

    meta = out["metadata"].setdefault("component", {})
    meta["type"] = "application"
    meta["name"] = root_name
    meta["version"] = root_version
    meta["bom-ref"] = f"{root_name}@{root_version}"
    meta["licenses"] = [{"license": {"id": inventory["project"]["license"]}}]
    meta["purl"] = with_version("pkg:generic/cern/lgc2", root_version)
    meta.pop("cpe", None)

    props = out["metadata"].setdefault("properties", [])
    props.append(
        {"name": "lgc:sbom:kind", "value": "syft-augmented-from-dependency-txt"}
    )
    props.append(
        {
            "name": "lgc:inventory:deps_file",
            "value": inventory.get("source", "dependency.csv"),
        }
    )
    if cmake_version and product_version and cmake_version != product_version:
        props.append({"name": "lgc:inventory:git_or_ci_ref", "value": product_version})

    existing = out.setdefault("components", [])
    normalize_syft_project_binaries(existing, product_version, cmake_version)

    by_name = {
        (c.get("name") or "").lower(): c for c in existing if c.get("type") != "file"
    }

    for disc in inventory["components"]:
        cdx = component_to_cdx(disc)
        key = disc["name"].lower()
        if key in by_name:
            target = by_name[key]
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
            strip_weak_cpes(target)
            target.pop("cpe", None)
        else:
            strip_weak_cpes(cdx)
            existing.append(cdx)
            by_name[key] = cdx

    root_ref = f"{root_name}@{root_version}"
    new_deps: list[dict] = []
    for d in inventory.get("dependencies") or []:
        entry: dict[str, Any] = {"ref": root_ref if d["ref"] == root_name else d["ref"]}
        if d.get("dependsOn"):
            entry["dependsOn"] = d["dependsOn"]
        new_deps.append(entry)

    declared = {c["name"].lower() for c in inventory["components"]}
    extra_refs = []
    for c in existing:
        if c.get("type") == "file":
            continue
        name = (c.get("name") or "").lower()
        if name in declared or name == "lgc2":
            continue
        ref = c.get("bom-ref") or c.get("name")
        if ref:
            extra_refs.append(ref)
            if not any(x.get("ref") == ref for x in new_deps):
                new_deps.append({"ref": ref})

    for d in new_deps:
        if d["ref"] == root_ref and extra_refs:
            d.setdefault("dependsOn", [])
            for r in extra_refs:
                if r not in d["dependsOn"]:
                    d["dependsOn"].append(r)

    out["dependencies"] = new_deps
    return out


def inventory_to_stub_cdx(inventory: dict, version: str) -> dict:
    root = inventory["project"]["name"]
    root_version = inventory["project"].get("version") or version
    comps = [component_to_cdx(c) for c in inventory["components"]]
    deps = []
    for d in inventory.get("dependencies") or []:
        entry: dict[str, Any] = {
            "ref": f"{root}@{root_version}" if d["ref"] == root else d["ref"]
        }
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
                        "name": "lgc-sbom-from-dependency-txt",
                        "version": "4.0.0",
                        "author": "CERN LGC2",
                    }
                ]
            },
            "component": {
                "bom-ref": f"{root}@{root_version}",
                "type": "application",
                "name": root,
                "version": root_version,
                "licenses": [{"license": {"id": inventory["project"]["license"]}}],
                "purl": inventory["project"].get("purl"),
            },
            "properties": [
                {"name": "lgc:sbom:kind", "value": "dependency-txt"},
                {
                    "name": "lgc:inventory:deps_file",
                    "value": inventory.get("source", "dependency.csv"),
                },
            ],
        },
        "components": comps,
        "dependencies": deps,
    }


def resolve_version_arg(cli_version: str) -> str:
    v = (cli_version or "").strip()
    if v:
        return v
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"], cwd=REPO_ROOT, text=True
        ).strip()
    except Exception:
        return "local"


def cmd_load(args: argparse.Namespace) -> None:
    inventory = build_inventory(args.deps_file, REPO_ROOT)
    args.out_dir.mkdir(parents=True, exist_ok=True)
    out = args.out_dir / "discovered-components.json"
    out.write_text(json.dumps(inventory, indent=2) + "\n", encoding="utf-8")
    print(
        f"Wrote {out} ({len(inventory['components'])} components from {args.deps_file})"
    )
    for c in inventory["components"]:
        ver = c.get("version") or "(no version)"
        print(
            f"  - {c['name']}: version={ver} license={c['license']} parent={c['parent']}"
        )


def cmd_augment(args: argparse.Namespace) -> None:
    inventory = build_inventory(args.deps_file, REPO_ROOT)
    version = resolve_version_arg(args.version)
    if args.syft and args.syft.is_file():
        syft = json.loads(args.syft.read_text(encoding="utf-8"))
        doc = augment_syft_cdx(syft, inventory, version)
    else:
        print("No Syft CDX provided; emitting dependency.csv-only CycloneDX")
        doc = inventory_to_stub_cdx(inventory, version)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(doc, indent=2) + "\n", encoding="utf-8")
    print(
        f"Wrote {args.output} "
        f"(components={len(doc.get('components') or [])}, "
        f"dependencies={len(doc.get('dependencies') or [])})"
    )


def cmd_ci(args: argparse.Namespace) -> None:
    args.out_dir.mkdir(parents=True, exist_ok=True)
    load_args = argparse.Namespace(deps_file=args.deps_file, out_dir=args.out_dir)
    cmd_load(load_args)
    output = args.output or (args.out_dir / "lgc-augmented.cdx.json")
    aug_args = argparse.Namespace(
        deps_file=args.deps_file,
        syft=args.syft,
        output=output,
        version=args.version,
    )
    cmd_augment(aug_args)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="cmd", required=True)

    p_load = sub.add_parser(
        "load", help="Parse dependency.csv → discovered-components.json"
    )
    p_load.add_argument("--deps-file", type=Path, default=DEFAULT_DEPS_FILE)
    p_load.add_argument("--out-dir", type=Path, default=REPO_ROOT / "sbom")
    p_load.set_defaults(func=cmd_load)

    # keep old name as alias
    p_disc = sub.add_parser("discover", help="Alias for 'load'")
    p_disc.add_argument("--deps-file", type=Path, default=DEFAULT_DEPS_FILE)
    p_disc.add_argument("--out-dir", type=Path, default=REPO_ROOT / "sbom")
    p_disc.set_defaults(func=cmd_load)

    p_aug = sub.add_parser("augment", help="Augment Syft CDX using dependency.csv")
    p_aug.add_argument("--deps-file", type=Path, default=DEFAULT_DEPS_FILE)
    p_aug.add_argument("--syft", type=Path, default=None)
    p_aug.add_argument(
        "--output", type=Path, default=REPO_ROOT / "sbom" / "lgc-augmented.cdx.json"
    )
    p_aug.add_argument("--version", default="")
    p_aug.set_defaults(func=cmd_augment)

    p_ci = sub.add_parser("ci", help="Load dependency.csv then augment Syft CDX")
    p_ci.add_argument("--deps-file", type=Path, default=DEFAULT_DEPS_FILE)
    p_ci.add_argument("--out-dir", type=Path, default=REPO_ROOT / "sbom")
    p_ci.add_argument("--syft", type=Path, required=True)
    p_ci.add_argument(
        "--output",
        type=Path,
        default=None,
        help="Augmented CycloneDX path (default: <out-dir>/lgc-augmented.cdx.json)",
    )
    p_ci.add_argument("--version", default="")
    p_ci.set_defaults(func=cmd_ci)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
