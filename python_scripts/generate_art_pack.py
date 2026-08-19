"""Generate an Art Pack from installed OAR submods.

Reads OpenAnimationReplacer folders that replace AABL_Attack_A.hkx (or that
gate on the Ashes of War items plugin) and emits:

1. arts.csv rows for the Weapon Art catalogue
2. Spell Hotbar 2 OAR submods (config.json only) that CompareValues
   SpellHotbar_ArtSelector and point at the author's clip via
   overrideAnimationsFolder

Never copies .hkx files. Never writes user.json onto foreign folders.
Selector 0 is left to the original worn-item configs.
"""

from __future__ import annotations

import argparse
import json
import shutil
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable, Iterable

AABL_CLIP = "aabl_attack_a.hkx"
AOW_ITEMS_PLUGIN = "ashes of war additional attack v items.esp"
ART_SELECTOR_PLUGIN = "SpellHotbar.esp"
ART_SELECTOR_FORM_ID = "D63"
SH2_PACK_NAME = "SpellHotbar2Arts"
# Stance-default "Ashes of War Sword Neutral" is 1001002544. A reserved SH2
# band above 2e9 beats that without depending on their numbers.
PRIORITY_BASE = 2_000_000_000
FIRST_ART_ID = 2
DEFAULT_ICON = "GREATER_POWER"
DEFAULT_STAMINA = "25"
DEFAULT_COOLDOWN = "8s"
DEFAULT_GCD = "1.0"
CSV_HEADER = "ArtID\tDisplayName\tIcon\tSelector\tArtClass\tStaminaCost\tCooldown\tGlobalCooldown"
OAR_PREFIX = Path("meshes") / "actors" / "character" / "animations" / "OpenAnimationReplacer"
OAR_1H_TYPES = {1.0, 2.0, 3.0, 4.0}
OAR_2H_TYPES = {5.0, 6.0}

LogFn = Callable[[str], None]


@dataclass
class GenerateResult:
    emitted: int = 0
    missing: list[str] = field(default_factory=list)


@dataclass
class Submod:
    name: str
    rel_key: str
    config_path: Path
    overlay_rel: Path
    has_clip: bool
    art_class: str = "Generic"


def generate(
    *,
    scan_roots: Iterable[Path],
    arts_csv: Path,
    overlay_root: Path,
    previous_paths: dict[str, str] | None = None,
    log: LogFn | None = None,
) -> GenerateResult:
    write_log = log or (lambda _msg: None)
    found = _discover(scan_roots)
    found_by_name = {sub.name: sub for sub in found}
    found_keys = {sub.rel_key for sub in found}

    result = GenerateResult()
    if previous_paths:
        for name, rel in previous_paths.items():
            if name not in found_by_name and rel not in found_keys:
                msg = f"missing or renamed OAR submod: {name} ({rel})"
                result.missing.append(name)
                write_log(msg)

    _delete_user_json(overlay_root)

    if not found:
        return result

    pack_root = overlay_root / OAR_PREFIX / SH2_PACK_NAME
    if pack_root.exists():
        shutil.rmtree(pack_root)
    pack_root.mkdir(parents=True, exist_ok=True)
    (pack_root / "config.json").write_text(
        json.dumps(_pack_config(), indent=4) + "\n",
        encoding="utf-8",
    )

    rows: list[str] = [CSV_HEADER]
    art_id = FIRST_ART_ID
    for sub in found:
        if not sub.has_clip:
            write_log(f"clip missing for {sub.name} at {sub.config_path.parent}")
        dest = pack_root / sub.name / "config.json"
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_text(
            json.dumps(
                _sh2_config(
                    name=sub.name,
                    selector=art_id,
                    override_folder=_override_animations_folder(sub.overlay_rel),
                ),
                indent=4,
            )
            + "\n",
            encoding="utf-8",
        )
        rows.append(
            "\t".join(
                (
                    str(art_id),
                    sub.name,
                    DEFAULT_ICON,
                    str(art_id),
                    sub.art_class,
                    DEFAULT_STAMINA,
                    DEFAULT_COOLDOWN,
                    DEFAULT_GCD,
                )
            )
        )
        art_id += 1
        result.emitted += 1

    arts_csv.parent.mkdir(parents=True, exist_ok=True)
    arts_csv.write_text("\n".join(rows) + "\n", encoding="utf-8")
    return result


def _discover(scan_roots: Iterable[Path]) -> list[Submod]:
    roots = [Path(root) for root in scan_roots]
    found: list[Submod] = []
    seen: set[str] = set()
    for root in roots:
        if not root.exists():
            continue
        for config_path in root.rglob("config.json"):
            if not _is_oar_submod(config_path):
                continue
            try:
                config = json.loads(config_path.read_text(encoding="utf-8-sig"))
            except (OSError, json.JSONDecodeError):
                continue
            if not _is_art_pack_submod(config_path.parent, config):
                continue
            overlay_rel = _overlay_relative(config_path)
            if overlay_rel is None:
                continue
            rel_key = overlay_rel.as_posix()
            if rel_key in seen:
                continue
            seen.add(rel_key)
            name = str(config.get("name") or config_path.parent.name)
            found.append(
                Submod(
                    name=name,
                    rel_key=rel_key,
                    config_path=config_path,
                    overlay_rel=overlay_rel,
                    has_clip=_clip_in_scan_roots(overlay_rel, config, roots),
                    art_class=classify_art_class(config),
                )
            )
    found.sort(key=lambda sub: (sub.name.lower(), sub.rel_key.lower()))
    return found


def _clip_in_scan_roots(overlay_rel: Path, config: dict, roots: list[Path]) -> bool:
    for root in roots:
        if _has_aabl_clip(root / overlay_rel, config):
            return True
    return False


def _is_oar_submod(config_path: Path) -> bool:
    # .../OpenAnimationReplacer/<ModName>/<SubModName>/config.json
    parts = [p.lower() for p in config_path.parts]
    try:
        idx = parts.index("openanimationreplacer")
    except ValueError:
        return False
    return len(parts) - idx == 4 and parts[-1] == "config.json"


def _overlay_relative(config_path: Path) -> Path | None:
    parts = list(config_path.parts)
    lowered = [p.lower() for p in parts]
    try:
        idx = lowered.index("openanimationreplacer")
    except ValueError:
        return None
    # meshes/actors/character/animations/OpenAnimationReplacer/<Mod>/<Sub>
    anim_idx = None
    for i, part in enumerate(lowered):
        if part == "meshes":
            anim_idx = i
            break
    if anim_idx is None or anim_idx >= idx:
        # Fall back to the OAR tail only if meshes/ is missing from the scan tree.
        return Path(*parts[idx - 1 :]) if idx > 0 else Path(*parts[idx:])
    return Path(*parts[anim_idx:-1])


def _override_animations_folder(overlay_rel: Path) -> str:
    parts = list(overlay_rel.parts)
    lowered = [p.lower() for p in parts]
    try:
        idx = lowered.index("openanimationreplacer")
    except ValueError:
        return "../" + overlay_rel.as_posix()
    tail = parts[idx + 1 :]
    return "../" + "/".join(tail)


def _is_art_pack_submod(submod_dir: Path, config: dict) -> bool:
    if _has_aabl_clip(submod_dir, config):
        return True
    return _mentions_aow_items_plugin(config)


def _has_aabl_clip(submod_dir: Path, config: dict) -> bool:
    for path in submod_dir.rglob("*"):
        if path.is_file() and path.name.lower() == AABL_CLIP:
            return True
    override = config.get("overrideAnimationsFolder")
    if isinstance(override, str) and override.strip():
        override_path = (submod_dir / override).resolve()
        if override_path.is_dir():
            for path in override_path.rglob("*"):
                if path.is_file() and path.name.lower() == AABL_CLIP:
                    return True
    return False


def _mentions_aow_items_plugin(node) -> bool:
    if isinstance(node, dict):
        for key, value in node.items():
            if key == "pluginName" and isinstance(value, str) and value.lower() == AOW_ITEMS_PLUGIN:
                return True
            if _mentions_aow_items_plugin(value):
                return True
    elif isinstance(node, list):
        return any(_mentions_aow_items_plugin(item) for item in node)
    return False


def classify_art_class(config: dict) -> str:
    """Collapse the author's OAR IsEquippedType ORs into 1H / 2H / Dual / Generic."""
    right: set[float] = set()
    left: set[float] = set()
    _collect_equipped_types(config, right, left)
    if (right & OAR_1H_TYPES) and (left & OAR_1H_TYPES):
        return "Dual"
    melee = (right | left) & (OAR_1H_TYPES | OAR_2H_TYPES)
    if not melee:
        return "Generic"
    if melee <= OAR_1H_TYPES:
        return "1H"
    if melee <= OAR_2H_TYPES:
        return "2H"
    return "Generic"


def _collect_equipped_types(node, right: set[float], left: set[float]) -> None:
    if isinstance(node, dict):
        if node.get("condition") == "IsEquippedType":
            type_value = _equipped_type_value(node)
            if type_value is not None:
                if node.get("Left hand") is True:
                    left.add(type_value)
                else:
                    right.add(type_value)
        for value in node.values():
            _collect_equipped_types(value, right, left)
    elif isinstance(node, list):
        for item in node:
            _collect_equipped_types(item, right, left)


def _equipped_type_value(node: dict) -> float | None:
    raw = node.get("Type")
    if isinstance(raw, dict) and "value" in raw:
        try:
            return float(raw["value"])
        except (TypeError, ValueError):
            return None
    if isinstance(raw, (int, float)):
        return float(raw)
    return None


def _pack_config() -> dict:
    return {
        "name": "Spell Hotbar 2 Weapon Arts",
        "author": "Spell Hotbar 2",
        "description": "Art Selector replacements. Animation files stay in the author's folders.",
    }


def _sh2_config(*, name: str, selector: int, override_folder: str) -> dict:
    return {
        "name": name,
        "priority": PRIORITY_BASE + selector,
        "ignoreNoTriggersFlag": True,
        "overrideAnimationsFolder": override_folder,
        "conditions": [_selector_condition(float(selector)), _player_actor_base()],
    }


def _player_actor_base() -> dict:
    return {
        "condition": "IsActorBase",
        "requiredVersion": "1.0.0.0",
        "Actor base": {
            "pluginName": "Skyrim.esm",
            "formID": "7",
        },
    }


def _selector_condition(selector: float) -> dict:
    return {
        "condition": "CompareValues",
        "requiredVersion": "1.0.0.0",
        "Value A": {
            "form": {
                "pluginName": ART_SELECTOR_PLUGIN,
                "formID": ART_SELECTOR_FORM_ID,
            }
        },
        "Comparison": "==",
        "Value B": {"value": selector},
    }


def _delete_user_json(overlay_root: Path) -> None:
    if not overlay_root.exists():
        return
    for path in overlay_root.rglob("user.json"):
        if path.is_file():
            path.unlink()


def _previous_from_csv(path: Path) -> dict[str, str]:
    previous: dict[str, str] = {}
    try:
        text = path.read_text(encoding="utf-8")
    except OSError:
        return previous
    for line in text.splitlines()[1:]:
        cols = line.split("\t")
        if len(cols) >= 2 and cols[1].strip():
            previous[cols[1]] = cols[1]
    return previous


def _parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--scan",
        action="append",
        dest="scan",
        required=True,
        type=Path,
        help="Mod folder (or mods root) to scan. Repeatable.",
    )
    parser.add_argument("--arts-csv", required=True, type=Path, help="Output arts.csv path.")
    parser.add_argument(
        "--previous-csv",
        type=Path,
        help="Prior arts.csv; names missing from this scan log as renamed or gone.",
    )
    parser.add_argument(
        "--overlay",
        required=True,
        type=Path,
        help="MO2 overlay mod root that will receive SH2 OAR config.json files only.",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = _parse_args(argv)
    previous_paths = _previous_from_csv(args.previous_csv) if args.previous_csv else None
    result = generate(
        scan_roots=args.scan,
        arts_csv=args.arts_csv,
        overlay_root=args.overlay,
        previous_paths=previous_paths,
        log=lambda msg: print(msg, file=sys.stderr),
    )
    print(f"emitted {result.emitted} arts; missing {len(result.missing)}")
    return 0 if result.emitted else 1


if __name__ == "__main__":
    raise SystemExit(main())
