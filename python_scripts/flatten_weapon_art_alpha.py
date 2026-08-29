"""Flatten translucent Weapon Art PNGs onto an opaque background.

The default invocation is a dry-run. Pass ``--apply`` to write changes or
``--check`` to use this as a failing opacity gate.
"""

from __future__ import annotations

import argparse
import csv
from dataclasses import dataclass
from pathlib import Path

from PIL import Image


REPO_ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = REPO_ROOT / "python_scripts/weapon_art_icons/manifest.tsv"


@dataclass(frozen=True)
class Asset:
    art_id: int
    display_name: str
    role: str
    path: Path


def _parse_background(value: str) -> tuple[int, int, int]:
    try:
        channels = tuple(int(channel.strip()) for channel in value.split(","))
    except ValueError as exc:
        raise argparse.ArgumentTypeError("background must be R,G,B") from exc
    if len(channels) != 3 or any(channel < 0 or channel > 255 for channel in channels):
        raise argparse.ArgumentTypeError("background must contain three values from 0 to 255")
    return channels


def _manifest_assets() -> list[Asset]:
    with MANIFEST_PATH.open(encoding="utf-8-sig", newline="") as handle:
        reader = csv.DictReader(handle, delimiter="\t")
        if reader.fieldnames is None:
            raise ValueError(f"Missing TSV header: {MANIFEST_PATH}")
        required = {"ArtID", "DisplayName", "Master", "AtlasInput"}
        missing = required.difference(reader.fieldnames)
        if missing:
            raise ValueError(f"Manifest missing columns: {', '.join(sorted(missing))}")
        rows = list(reader)

    assets: list[Asset] = []
    seen: set[Path] = set()
    for row in rows:
        for role, column in (("master", "Master"), ("atlas", "AtlasInput")):
            relative = row[column].strip()
            if not relative:
                continue
            path = REPO_ROOT / relative
            if not path.is_file() or path in seen:
                continue
            seen.add(path)
            assets.append(Asset(int(row["ArtID"]), row["DisplayName"].strip(), role, path))
    return assets


def _nonopaque_count(path: Path) -> int:
    with Image.open(path) as image:
        if image.mode != "RGBA":
            return 0
        histogram = image.getchannel("A").histogram()
        return sum(histogram[:255])


def _flatten(path: Path, background: tuple[int, int, int]) -> None:
    with Image.open(path) as image:
        rgba = image.convert("RGBA")
        backdrop = Image.new("RGBA", rgba.size, (*background, 255))
        flattened = Image.alpha_composite(backdrop, rgba)
        flattened.save(path, format="PNG", optimize=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--apply", action="store_true", help="write flattened PNGs")
    mode.add_argument("--check", action="store_true", help="fail if any manifest PNG is translucent")
    parser.add_argument(
        "--background",
        type=_parse_background,
        default=(0, 0, 0),
        metavar="R,G,B",
        help="opaque composite background (default: 0,0,0)",
    )
    args = parser.parse_args()

    affected = [(asset, _nonopaque_count(asset.path)) for asset in _manifest_assets()]
    affected = [(asset, count) for asset, count in affected if count]
    if not affected:
        print("All Weapon Art manifest PNGs are opaque")
        return

    action = "Flattening" if args.apply else "Would flatten"
    for asset, count in affected:
        relative = asset.path.relative_to(REPO_ROOT)
        print(f"{action} ArtID {asset.art_id} {asset.display_name} [{asset.role}]: {relative} ({count} pixels)")
        if args.apply:
            _flatten(asset.path, args.background)

    if args.check:
        raise SystemExit(f"Found {len(affected)} translucent Weapon Art PNGs")
    if args.apply:
        print(f"Flattened {len(affected)} PNGs onto RGB{args.background}")
    else:
        print(f"Dry-run: {len(affected)} PNGs require flattening; pass --apply to write them")


if __name__ == "__main__":
    main()
