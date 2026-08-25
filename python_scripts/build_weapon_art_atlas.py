"""Build and wire the Spell Hotbar 2 Weapon Art icon atlas.

This intentionally avoids the legacy SWF/Inventory Injector toolchain. SH2 loads
the generated PNG and tab-separated UV catalogue directly.
"""

from __future__ import annotations

import argparse
import csv
import io
import math
from dataclasses import dataclass
from pathlib import Path

from PIL import Image, ImageChops


ICON_SIZE = 128
ATLAS_NAME = "icons_weapon_arts"
PLACEHOLDER_ICON = "GREATER_POWER"
SHIPPABLE_STATUSES = {"finalized", "silhouette_regen"}

REPO_ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = REPO_ROOT / "python_scripts/weapon_art_icons/manifest.tsv"
CATALOGUE_PATH = REPO_ROOT / "data/SKSE/Plugins/SpellHotbar/artdata/arts_ashes.csv"
IMAGES_DIR = REPO_ROOT / "data/SKSE/Plugins/SpellHotbar/images"
ATLAS_PATH = IMAGES_DIR / f"{ATLAS_NAME}.png"
ATLAS_CSV_PATH = IMAGES_DIR / f"{ATLAS_NAME}.csv"


@dataclass(frozen=True)
class IconEntry:
    art_id: int
    display_name: str
    key: str
    path: Path


def _runtime_icon_key(icon_name: str) -> str:
    """Return the key SH2 registers for an extra-atlas entry at runtime."""
    return f"{ATLAS_PATH.name}_{icon_name}"


def _next_power_of_two(value: int) -> int:
    return 1 << (value - 1).bit_length()


def _read_tsv(path: Path) -> tuple[list[str], list[dict[str, str]]]:
    with path.open("r", encoding="utf-8-sig", newline="") as handle:
        reader = csv.DictReader(handle, delimiter="\t")
        if reader.fieldnames is None:
            raise ValueError(f"Missing TSV header: {path}")
        return reader.fieldnames, list(reader)


def load_manifest() -> tuple[list[IconEntry], dict[int, str]]:
    fields, rows = _read_tsv(MANIFEST_PATH)
    required = {"ArtID", "DisplayName", "IconKey", "Status", "AtlasInput"}
    missing = required.difference(fields)
    if missing:
        raise ValueError(f"Manifest missing columns: {', '.join(sorted(missing))}")

    entries: list[IconEntry] = []
    all_statuses: dict[int, str] = {}
    seen_ids: set[int] = set()
    seen_keys: set[str] = set()
    for row in rows:
        art_id = int(row["ArtID"])
        if art_id in seen_ids:
            raise ValueError(f"Duplicate ArtID in manifest: {art_id}")
        seen_ids.add(art_id)
        status = row["Status"].strip()
        all_statuses[art_id] = status
        if status == "hard_failure":
            continue
        if status not in SHIPPABLE_STATUSES:
            raise ValueError(f"ArtID {art_id} has non-shippable or unknown status: {status!r}")

        key = row["IconKey"].strip()
        if not key:
            raise ValueError(f"ArtID {art_id} has no IconKey")
        if key in seen_keys:
            raise ValueError(f"Duplicate IconKey in manifest: {key}")
        seen_keys.add(key)

        relative_path = row["AtlasInput"].strip()
        if not relative_path:
            raise ValueError(f"ArtID {art_id} ({key}) has no AtlasInput")
        icon_path = REPO_ROOT / Path(relative_path)
        if not icon_path.is_file():
            raise FileNotFoundError(f"Missing atlas input for ArtID {art_id}: {icon_path}")
        with Image.open(icon_path) as image:
            if image.size != (ICON_SIZE, ICON_SIZE) or image.mode not in {"RGB", "RGBA"}:
                raise ValueError(
                    f"{icon_path} must be {ICON_SIZE}x{ICON_SIZE} RGB or RGBA; "
                    f"got {image.size[0]}x{image.size[1]} {image.mode}"
                )
        entries.append(IconEntry(art_id, row["DisplayName"].strip(), key, icon_path))

    entries.sort(key=lambda entry: entry.art_id)
    if not entries:
        raise ValueError("Manifest contains no usable icons")
    return entries, all_statuses


def build_atlas(entries: list[IconEntry]) -> tuple[Image.Image, list[dict[str, str]]]:
    row_length = math.ceil(math.sqrt(len(entries)))
    atlas_size = _next_power_of_two(row_length * ICON_SIZE)
    columns = atlas_size // ICON_SIZE
    atlas = Image.new("RGBA", (atlas_size, atlas_size), (0, 0, 0, 0))
    coordinates: list[dict[str, str]] = []

    for index, entry in enumerate(entries):
        x = (index % columns) * ICON_SIZE
        y = (index // columns) * ICON_SIZE
        with Image.open(entry.path) as source:
            atlas.paste(source.convert("RGBA"), (x, y))
        coordinates.append(
            {
                "IconName": entry.key,
                "u0": f"{x / atlas_size:.6f}",
                "v0": f"{y / atlas_size:.6f}",
                "u1": f"{(x + ICON_SIZE) / atlas_size:.6f}",
                "v1": f"{(y + ICON_SIZE) / atlas_size:.6f}",
            }
        )
    return atlas, coordinates


def _atlas_csv_text(coordinates: list[dict[str, str]]) -> str:
    output = io.StringIO(newline="")
    writer = csv.DictWriter(
        output, fieldnames=["IconName", "u0", "v0", "u1", "v1"], delimiter="\t", lineterminator="\n"
    )
    writer.writeheader()
    writer.writerows(coordinates)
    return output.getvalue()


def _updated_catalogue_text(entries: list[IconEntry], statuses: dict[int, str]) -> str:
    fields, rows = _read_tsv(CATALOGUE_PATH)
    required = {"ArtID", "DisplayName", "Icon"}
    missing = required.difference(fields)
    if missing:
        raise ValueError(f"Catalogue missing columns: {', '.join(sorted(missing))}")

    entries_by_id = {entry.art_id: entry for entry in entries}
    catalogue_ids: set[int] = set()
    for row in rows:
        art_id = int(row["ArtID"])
        catalogue_ids.add(art_id)
        if art_id not in statuses:
            continue
        if art_id in entries_by_id:
            entry = entries_by_id[art_id]
            if row["DisplayName"] != entry.display_name:
                raise ValueError(
                    f"DisplayName mismatch for ArtID {art_id}: "
                    f"manifest={entry.display_name!r}, catalogue={row['DisplayName']!r}"
                )
            row["Icon"] = _runtime_icon_key(entry.key)
        else:
            row["Icon"] = PLACEHOLDER_ICON

    missing_ids = set(statuses).difference(catalogue_ids)
    if missing_ids:
        raise ValueError(f"Manifest ArtIDs missing from catalogue: {sorted(missing_ids)}")

    output = io.StringIO(newline="")
    writer = csv.DictWriter(output, fieldnames=fields, delimiter="\t", lineterminator="\n")
    writer.writeheader()
    writer.writerows(rows)
    return output.getvalue()


def write_outputs() -> tuple[int, int, tuple[int, int]]:
    entries, statuses = load_manifest()
    atlas, coordinates = build_atlas(entries)
    IMAGES_DIR.mkdir(parents=True, exist_ok=True)
    atlas.save(ATLAS_PATH, format="PNG", optimize=True)
    ATLAS_CSV_PATH.write_text(_atlas_csv_text(coordinates), encoding="utf-8", newline="")
    CATALOGUE_PATH.write_text(_updated_catalogue_text(entries, statuses), encoding="utf-8", newline="")
    return len(entries), len(statuses) - len(entries), atlas.size


def check_outputs() -> tuple[int, int, tuple[int, int]]:
    entries, statuses = load_manifest()
    expected_atlas, coordinates = build_atlas(entries)
    expected_csv = _atlas_csv_text(coordinates)
    expected_catalogue = _updated_catalogue_text(entries, statuses)

    if not ATLAS_PATH.is_file() or not ATLAS_CSV_PATH.is_file():
        raise FileNotFoundError("Weapon Art atlas outputs do not exist; run the builder first")
    with Image.open(ATLAS_PATH) as actual_atlas:
        actual_rgba = actual_atlas.convert("RGBA")
        if actual_rgba.size != expected_atlas.size or ImageChops.difference(actual_rgba, expected_atlas).getbbox():
            raise ValueError(f"Atlas does not match manifest inputs: {ATLAS_PATH}")
    if ATLAS_CSV_PATH.read_text(encoding="utf-8") != expected_csv:
        raise ValueError(f"Atlas CSV does not match manifest inputs: {ATLAS_CSV_PATH}")
    if CATALOGUE_PATH.read_text(encoding="utf-8") != expected_catalogue:
        raise ValueError(f"Ashes catalogue icon assignments are stale: {CATALOGUE_PATH}")
    return len(entries), len(statuses) - len(entries), expected_atlas.size


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="validate existing outputs without changing files")
    args = parser.parse_args()
    usable, placeholders, atlas_size = check_outputs() if args.check else write_outputs()
    action = "Validated" if args.check else "Built"
    print(
        f"{action} {atlas_size[0]}x{atlas_size[1]} Weapon Art atlas: "
        f"{usable} assigned icons, {placeholders} placeholders"
    )


if __name__ == "__main__":
    main()
