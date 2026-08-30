#!/usr/bin/env python3
"""Verify the source-level boundary between Spell Hotbar 2 and SMF.

This check intentionally treats CMake and the guest registration seam as public
architecture: SH2 must consume SMF's exported API and must not own an ImGui or
Direct3D host path of its own.
"""

from __future__ import annotations

import hashlib
from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
PLUGIN = ROOT / "skse_plugin"


def read(relative: str) -> str:
    path = ROOT / relative
    if not path.is_file():
        raise AssertionError(f"missing required file: {relative}")
    return path.read_text(encoding="utf-8")


def sha256(relative: str) -> str:
    path = ROOT / relative
    if not path.is_file():
        raise AssertionError(f"missing required file: {relative}")
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def require(text: str, pattern: str, label: str) -> None:
    if re.search(pattern, text, flags=re.MULTILINE) is None:
        raise AssertionError(f"missing {label}: /{pattern}/")


def forbid(text: str, pattern: str, label: str) -> None:
    match = re.search(pattern, text, flags=re.MULTILINE)
    if match is not None:
        line = text.count("\n", 0, match.start()) + 1
        raise AssertionError(f"forbidden {label} at line {line}: {match.group(0)!r}")


def verify() -> None:
    cmake = read("skse_plugin/CMakeLists.txt")
    render_header = read("skse_plugin/src/rendering/render_manager.h")
    render_source = read("skse_plugin/src/rendering/render_manager.cpp")
    guest = read("skse_plugin/src/smf/smf_guest.cpp")
    pin = read("skse_plugin/third_party/skse-menu-framework/UPSTREAM.md")

    require(cmake, r"set\(OUTPUT_FOLDER\s+\"\"\s+CACHE\s+PATH", "opt-in output folder")
    forbid(cmake, r"find_package\((?:imgui|directxtk)\b", "private UI dependency")
    forbid(cmake, r"target_link_libraries\([^\n]*(?:imgui::imgui|DirectXTK)", "private UI linkage")
    forbid(cmake, r"texture_loader\.(?:h|cpp)", "private texture loader source")

    for source in (render_header, render_source):
        forbid(source, r"ImGui_Impl(?:DX11|Win32)", "ImGui backend ownership")
        forbid(source, r"\b(?:WndProcHook|D3DInitHook|DXGIPresentHook)\b", "render hook ownership")
        forbid(source, r"ImGui::CreateContext\s*\(", "ImGui context ownership")
        forbid(source, r"SetWindowLongPtr", "WndProc ownership")

    require(guest, r"SKSEMenuFramework::AddHudElement\s*\(", "SMF HUD registration")
    require(guest, r"SKSEMenuFramework::AddInputEvent\s*\(\s*Input::process_event\s*\)", "SMF input callback registration")
    if len(re.findall(r"SKSEMenuFramework::AddWindow\s*\(", guest)) != 4:
        raise AssertionError("SMF guest must register exactly four native windows")
    require(guest, r"GetProcAddress\([^\n]*\"RegisterHudElement\"", "required SMF export validation")
    require(guest, r"GetProcAddress\([^\n]*\"AddWindow\"", "required SMF window export validation")
    require(guest, r"GetProcAddress\([^\n]*\"RegisterInpoutEvent\"", "required SMF input export validation")
    require(guest, r"required_version\s*=\s*3\.14F", "minimum SMF runtime version")
    require(guest, r"installed_version\s*<\s*required_version", "SMF runtime version gate")
    require(guest, r"window->BlockUserInput\.store\(true\)", "blocking SMF windows")
    require(
        guest,
        r"(?s)bool open_window\(.*?close_all_windows\(\).*?IsOpen\.store\(true\)",
        "mutually exclusive SMF windows",
    )
    require(render_source, r"synchronize_window_models_with_host\(\)", "host-close lifecycle synchronization")

    require(pin, r"1dcb70179076aae4ab626f43c5baab2735ca5877", "pinned SMF consumer-API commit")
    require(pin, r"928e01ab459822a8d233ab99f0419ea1de23c775", "matched SMF runtime commit")
    require(pin, r"48416E8220CA777E2FFFC2EF2BAF21F699AB2E6C409D437F44EEC5E311C3524C", "consumer-header SHA-256")
    require(pin, r"7FFE1954587C77DFBA1CF8EB9B2EA743671FA6E63F9E7A2F258119D42E14EEFE", "consumer-license SHA-256")

    expected_header_hash = "48416E8220CA777E2FFFC2EF2BAF21F699AB2E6C409D437F44EEC5E311C3524C"
    actual_header_hash = sha256("skse_plugin/third_party/skse-menu-framework/SKSEMenuFramework.h")
    if actual_header_hash != expected_header_hash:
        raise AssertionError(f"SMF consumer-header SHA-256 changed: {actual_header_hash}")

    expected_license_hash = "7FFE1954587C77DFBA1CF8EB9B2EA743671FA6E63F9E7A2F258119D42E14EEFE"
    actual_license_hash = sha256("skse_plugin/third_party/skse-menu-framework/LICENSE")
    if actual_license_hash != expected_license_hash:
        raise AssertionError(f"SMF consumer-license SHA-256 changed: {actual_license_hash}")


if __name__ == "__main__":
    try:
        verify()
    except AssertionError as exc:
        print(f"SMF guest boundary: FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
    print("SMF guest boundary: PASS")
