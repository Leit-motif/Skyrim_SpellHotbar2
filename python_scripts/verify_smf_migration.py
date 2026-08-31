#!/usr/bin/env python3
"""Static acceptance gate for the atomic SKSE Menu Framework migration.

This check intentionally focuses on architectural invariants that are easy to
regress during review.  It complements (and never replaces) a release build and
in-game acceptance.
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PLUGIN = ROOT / "skse_plugin"


@dataclass(frozen=True)
class CheckResult:
    name: str
    ok: bool
    detail: str


def read(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except FileNotFoundError:
        return ""


def required_files() -> CheckResult:
    paths = (
        PLUGIN / "src" / "smf" / "SKSEMenuFramework.h",
        PLUGIN / "src" / "smf" / "smf_guest.h",
        PLUGIN / "src" / "smf" / "smf_guest.cpp",
        PLUGIN / "src" / "mcp" / "mcp_pages.h",
        PLUGIN / "src" / "mcp" / "mcp_pages.cpp",
    )
    missing = [str(path.relative_to(ROOT)) for path in paths if not path.is_file()]
    return CheckResult(
        "required SMF source",
        not missing,
        "present" if not missing else "missing: " + ", ".join(missing),
    )


def no_private_imgui_dependencies() -> CheckResult:
    cmake = read(PLUGIN / "CMakeLists.txt")
    vcpkg = read(PLUGIN / "vcpkg.json")
    offenders = []
    for label, text in (("CMakeLists.txt", cmake), ("vcpkg.json", vcpkg)):
        for dependency in ("imgui", "directxtk"):
            if re.search(rf"(?i)\b{dependency}\b", text):
                offenders.append(f"{label}:{dependency}")
    return CheckResult(
        "SMF owns ImGui and textures",
        not offenders,
        "no private dependencies" if not offenders else "found " + ", ".join(offenders),
    )


def no_legacy_host_hooks() -> CheckResult:
    forbidden = (
        "ImGui::CreateContext",
        "ImGui_ImplWin32_Init",
        "ImGui_ImplDX11_Init",
        "D3D11CreateDeviceAndSwapChain",
        "SetWindowLongPtrA",
        "Input::install_hook",
        "RenderManager::install",
    )
    hits: list[str] = []
    for path in (PLUGIN / "src").rglob("*.cpp"):
        text = read(path)
        for token in forbidden:
            if token in text:
                hits.append(f"{path.relative_to(ROOT)}:{token}")
    return CheckResult(
        "single UI/input host",
        not hits,
        "legacy hooks absent" if not hits else "found " + ", ".join(hits),
    )


def serialization_formats_unchanged() -> CheckResult:
    storage = read(PLUGIN / "src" / "storage" / "storage.h")
    presets = read(PLUGIN / "src" / "storage" / "user_data_io.h")
    save_ok = bool(re.search(r"save_format\s*=\s*5(?:U)?\s*;", storage))
    preset_ok = bool(re.search(r"preset_save_version\s*=\s*2\s*;", presets))
    return CheckResult(
        "persistence compatibility",
        save_ok and preset_ok,
        f"co-save format 5={save_ok}; preset format 2={preset_ok}",
    )


def mcm_retired() -> CheckResult:
    mcm_source = ROOT / "papyrus" / "Scripts" / "Source" / "SpellHotbarMCM.psc"
    packaging_text = "\n".join(
        read(path)
        for path in (
            ROOT / "python_scripts" / "build_release_package.py",
            ROOT / "python_scripts" / "create_fomod_installer.py",
        )
    )
    stale = []
    if mcm_source.exists():
        stale.append(str(mcm_source.relative_to(ROOT)))
    if "SpellHotbarMCM.pex" in packaging_text:
        stale.append("release packaging references SpellHotbarMCM.pex")
    return CheckResult(
        "legacy MCM retired",
        not stale,
        "no MCM source/package entry" if not stale else "; ".join(stale),
    )


def upstream_only_diff(base: str) -> CheckResult:
    command = ["git", "diff", "--name-only", f"{base}...HEAD"]
    completed = subprocess.run(
        command, cwd=ROOT, check=False, capture_output=True, text=True
    )
    if completed.returncode != 0:
        return CheckResult("upstream-only scope", False, completed.stderr.strip())

    changed = [line.strip() for line in completed.stdout.splitlines() if line.strip()]
    addon_markers = (
        ".scratch/",
        "addon",
        "ability",
        "weapon_art",
        "skse_plugin/tests/fixtures/acceptance-matrix",
    )
    leaked = [
        path
        for path in changed
        if any(marker in path.casefold() for marker in addon_markers)
    ]
    return CheckResult(
        "upstream-only scope",
        not leaked,
        "no addon paths in diff" if not leaked else "leaked paths: " + ", ".join(leaked),
    )


def run(base: str) -> int:
    checks = (
        required_files(),
        no_private_imgui_dependencies(),
        no_legacy_host_hooks(),
        serialization_formats_unchanged(),
        mcm_retired(),
        upstream_only_diff(base),
    )

    for result in checks:
        status = "PASS" if result.ok else "FAIL"
        print(f"[{status}] {result.name}: {result.detail}")

    failed = sum(not result.ok for result in checks)
    print(f"\n{len(checks) - failed}/{len(checks)} static migration checks passed")
    if failed:
        print("Static acceptance is incomplete; build and runtime acceptance are separate gates.")
        return 1
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--base",
        default="upstream/master",
        help="Git base used to detect addon-only paths (default: upstream/master)",
    )
    args = parser.parse_args()
    return run(args.base)


if __name__ == "__main__":
    sys.exit(main())
