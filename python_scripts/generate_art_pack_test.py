"""Seam tests for the Art Pack generator.

The public seam is generate(): scan roots of OAR submods in, arts.csv rows and
SH2-owned config.json out. No .hkx files leave the scan tree, and no user.json
is written onto foreign Ashes of War folders.
"""

from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

from generate_art_pack import (
    CUSTOM_ART_ID_BASE,
    CUSTOM_TEMPLATE_COUNT,
    emit_custom_art_templates,
    generate,
    scan_custom_art_folders,
)


AABL = "AABL_Attack_A.hkx"
AOW_PLUGIN = "Ashes of War Additional Attack v Items.esp"
SWORD_NEUTRAL_PRIORITY = 1001002544
SH2_PACK = "SpellHotbar2Arts"


def _write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def _submod(root: Path, mod: str, name: str) -> Path:
    return (
        root
        / "meshes"
        / "actors"
        / "character"
        / "animations"
        / "OpenAnimationReplacer"
        / mod
        / name
    )


def _sh2_config_path(overlay: Path, name: str) -> Path:
    return _submod(overlay, SH2_PACK, name) / "config.json"


def _equipped_type(type_value: float, *, left_hand: bool) -> dict:
    return {
        "condition": "IsEquippedType",
        "requiredVersion": "1.0.0.0",
        "Type": {"value": type_value},
        "Left hand": left_hand,
    }


def _or_types(type_values: list[float], *, left_hand: bool) -> dict:
    return {
        "condition": "OR",
        "requiredVersion": "1.0.0.0",
        "Conditions": [_equipped_type(v, left_hand=left_hand) for v in type_values],
    }


def _worn_config(name: str, priority: int = 103900002, *, extra_conditions=None, equipped=True) -> dict:
    conditions = [
        {
            "condition": "IsActorBase",
            "requiredVersion": "1.0.0.0",
            "Actor base": {"pluginName": "Skyrim.esm", "formID": "7"},
        },
        {
            "condition": "IsWornHasKeyword",
            "requiredVersion": "1.0.0.0",
            "Keyword": {
                "form": {"pluginName": AOW_PLUGIN, "formID": "836"},
            },
        },
    ]
    if equipped:
        conditions.append(_equipped_type(1.0, left_hand=False))
    if extra_conditions:
        conditions.extend(extra_conditions)
    return {
        "name": name,
        "priority": priority,
        "ignoreNoTriggersFlag": True,
        "conditions": conditions,
    }


class GenerateArtPackTest(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        self.scan = self.root / "scan"
        self.overlay = self.root / "overlay"
        self.arts_csv = self.root / "arts_ashes.csv"

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def _generate(self, **kwargs):
        return generate(
            scan_roots=[self.scan],
            arts_csv=self.arts_csv,
            overlay_root=self.overlay,
            **kwargs,
        )

    def _assert_sh2_art(self, name: str, selector: float) -> dict:
        config_path = _sh2_config_path(self.overlay, name)
        self.assertTrue(config_path.is_file(), f"missing SH2 config for {name}")
        config = json.loads(config_path.read_text(encoding="utf-8"))
        self.assertEqual(config["name"], name)
        self.assertEqual(config["priority"], 2_000_000_000 + int(selector))
        self.assertGreater(config["priority"], SWORD_NEUTRAL_PRIORITY)
        self.assertEqual(
            config["overrideAnimationsFolder"],
            f"../AoW Pack/{name}",
        )
        kinds = [c["condition"] for c in config["conditions"]]
        self.assertEqual(kinds, ["CompareValues", "IsActorBase"])
        self.assertNotIn("IsWornHasKeyword", kinds)
        self.assertNotIn("IsEquippedType", kinds)
        selector_cond = next(c for c in config["conditions"] if c["condition"] == "CompareValues")
        # The selector is a behavior-graph variable, never an ESP form: gating on a record that
        # only exists in a hand-edited copy of upstream's plugin is what shipped a pack that
        # worked on one machine (ADR-0016).
        self.assertNotIn("form", selector_cond["Value A"])
        self.assertEqual(selector_cond["Value A"]["graphVariable"], "SH2_ArtSelector")
        self.assertEqual(selector_cond["Value A"]["graphVariableType"], "Int")
        self.assertEqual(selector_cond["Comparison"], "==")
        self.assertEqual(selector_cond["Value B"]["value"], selector)
        actor = next(c for c in config["conditions"] if c["condition"] == "IsActorBase")
        self.assertEqual(actor["Actor base"]["pluginName"], "Skyrim.esm")
        self.assertEqual(actor["Actor base"]["formID"], "7")
        return config

    def test_named_ash_with_clip_becomes_catalogue_row_and_sh2_config(self):
        sub = _submod(self.scan, "AoW Pack", "Flurry Strike")
        _write(sub / "config.json", json.dumps(_worn_config("Flurry Strike")))
        _write(sub / "animations" / AABL, "not-a-real-clip")

        result = self._generate()

        self.assertEqual(result.emitted, 1)
        text = self.arts_csv.read_text(encoding="utf-8")
        self.assertIn("Flurry Strike", text)
        self.assertTrue(
            text.startswith(
                "ArtID\tDisplayName\tIcon\tSelector\tArtClass\tStaminaCost\tCooldown\tGlobalCooldown"
            )
        )
        row = [line for line in text.splitlines() if "Flurry Strike" in line][0]
        cols = row.split("\t")
        self.assertEqual(cols[0], "2")
        self.assertEqual(cols[1], "Flurry Strike")
        self.assertEqual(cols[3], "2")
        self.assertEqual(cols[4], "1H")

        self._assert_sh2_art("Flurry Strike", 2.0)
        foreign_user = _submod(self.overlay, "AoW Pack", "Flurry Strike") / "user.json"
        self.assertFalse(foreign_user.exists())
        self.assertFalse(any(p.name.lower() == "user.json" for p in self.overlay.rglob("*") if p.is_file()))
        self.assertFalse(any(p.suffix.lower() == ".hkx" for p in self.overlay.rglob("*") if p.is_file()))

    def test_unarmed_only_ash_override_does_not_keep_weapon_gates(self):
        sub = _submod(self.scan, "AoW Pack", "Crane Style")
        _write(
            sub / "config.json",
            json.dumps(
                _worn_config(
                    "Crane Style",
                    extra_conditions=[
                        {
                            "condition": "IsEquippedType",
                            "requiredVersion": "1.0.0.0",
                            "Type": {"value": 0.0},
                            "Left hand": False,
                        },
                        {
                            "condition": "IsEquippedType",
                            "requiredVersion": "1.0.0.0",
                            "Type": {"value": 0.0},
                            "Left hand": True,
                        },
                    ],
                )
            ),
        )
        _write(sub / "animations" / AABL, "clip")

        self._generate()

        self._assert_sh2_art("Crane Style", 2.0)

    def test_keyword_only_submod_still_emits_and_logs_missing_clip(self):
        sub = _submod(self.scan, "Stance Framework", "Shadow Slash")
        _write(sub / "config.json", json.dumps(_worn_config("Shadow Slash")))
        logs: list[str] = []

        result = self._generate(log=logs.append)

        self.assertEqual(result.emitted, 1)
        self.assertTrue(any("Shadow Slash" in line and "clip" in line.lower() for line in logs))
        self.assertIn("Shadow Slash", self.arts_csv.read_text(encoding="utf-8"))
        config = json.loads(_sh2_config_path(self.overlay, "Shadow Slash").read_text(encoding="utf-8"))
        self.assertEqual(config["overrideAnimationsFolder"], "../Stance Framework/Shadow Slash")

    def test_unrelated_oar_submod_is_skipped(self):
        sub = _submod(self.scan, "Some Moveset", "Power Attack")
        _write(
            sub / "config.json",
            json.dumps(
                {
                    "name": "Power Attack",
                    "priority": 100,
                    "conditions": [
                        {
                            "condition": "IsEquippedType",
                            "requiredVersion": "1.0.0.0",
                            "Type": {"value": 1.0},
                            "Left hand": False,
                        }
                    ],
                }
            ),
        )
        _write(sub / "animations" / "attackRight.hkx", "nope")

        result = self._generate()

        self.assertEqual(result.emitted, 0)
        self.assertFalse(self.arts_csv.exists())
        self.assertFalse(_sh2_config_path(self.overlay, "Power Attack").exists())

    def test_clip_in_another_scan_root_is_not_logged_missing(self):
        configs = self.scan / "configs"
        clips = self.scan / "clips"
        sub = _submod(configs, "AoW Pack", "Flurry Strike")
        _write(sub / "config.json", json.dumps(_worn_config("Flurry Strike")))
        clip_dir = _submod(clips, "AoW Pack", "Flurry Strike")
        _write(clip_dir / AABL, "clip")
        logs: list[str] = []

        result = generate(
            scan_roots=[configs, clips],
            arts_csv=self.arts_csv,
            overlay_root=self.overlay,
            log=logs.append,
        )

        self.assertEqual(result.emitted, 1)
        self.assertFalse(any("clip missing" in line.lower() for line in logs))
        self._assert_sh2_art("Flurry Strike", 2.0)
        self.assertFalse(any(p.suffix.lower() == ".hkx" for p in self.overlay.rglob("*") if p.is_file()))

    def test_renamed_submod_logs_and_other_art_still_emits(self):
        kept = _submod(self.scan, "AoW Pack", "Double Slash")
        _write(kept / "config.json", json.dumps(_worn_config("Double Slash")))
        _write(kept / "animations" / AABL, "clip")
        previous = {
            "Flurry Strike": "AoW Pack/Flurry Strike",
            "Double Slash": "AoW Pack/Double Slash",
        }
        logs: list[str] = []

        result = self._generate(previous_paths=previous, log=logs.append)

        self.assertEqual(result.emitted, 1)
        self.assertIn("Double Slash", self.arts_csv.read_text(encoding="utf-8"))
        self.assertNotIn("Flurry Strike", self.arts_csv.read_text(encoding="utf-8"))
        self.assertTrue(any("Flurry Strike" in line for line in logs))
        self.assertTrue(any("missing" in line.lower() or "renamed" in line.lower() for line in logs))
        self._assert_sh2_art("Double Slash", 2.0)
        self.assertFalse(_sh2_config_path(self.overlay, "Flurry Strike").exists())

    def test_previous_csv_feeds_missing_log_on_regen(self):
        kept = _submod(self.scan, "AoW Pack", "Double Slash")
        _write(kept / "config.json", json.dumps(_worn_config("Double Slash")))
        _write(kept / "animations" / AABL, "clip")
        prior = self.root / "prior.csv"
        _write(
            prior,
            "ArtID\tDisplayName\tIcon\tSelector\tStaminaCost\tCooldown\tGlobalCooldown\n"
            "2\tFlurry Strike\tGREATER_POWER\t2\t25\t8s\t1.0\n"
            "3\tDouble Slash\tGREATER_POWER\t3\t25\t8s\t1.0\n",
        )
        logs: list[str] = []
        from generate_art_pack import _previous_from_csv

        result = self._generate(previous_paths=_previous_from_csv(prior), log=logs.append)

        self.assertEqual(result.emitted, 1)
        self.assertTrue(any("Flurry Strike" in line and "missing" in line.lower() for line in logs))
        self.assertIn("Double Slash", self.arts_csv.read_text(encoding="utf-8"))

    def test_regen_deletes_leftover_foreign_user_json(self):
        leftover = _submod(self.overlay, "AoW Pack", "Flurry Strike") / "user.json"
        _write(leftover, '{"priority": 1}\n')
        stale_sh2 = _sh2_config_path(self.overlay, "Old Art")
        _write(stale_sh2, '{"name": "Old Art"}\n')
        sub = _submod(self.scan, "AoW Pack", "Flurry Strike")
        _write(sub / "config.json", json.dumps(_worn_config("Flurry Strike")))
        _write(sub / "animations" / AABL, "clip")

        self._generate()

        self.assertFalse(leftover.exists())
        self.assertFalse(stale_sh2.exists())
        self._assert_sh2_art("Flurry Strike", 2.0)

    def _art_class_of(self, name: str) -> str:
        row = [line for line in self.arts_csv.read_text(encoding="utf-8").splitlines() if name in line][0]
        return row.split("\t")[4]

    def test_two_hand_type_collapses_to_2h(self):
        sub = _submod(self.scan, "AoW Pack", "Blood Flurry")
        _write(
            sub / "config.json",
            json.dumps(_worn_config("Blood Flurry", equipped=False, extra_conditions=[_equipped_type(5.0, left_hand=False)])),
        )
        _write(sub / "animations" / AABL, "clip")

        self._generate()

        self.assertEqual(self._art_class_of("Blood Flurry"), "2H")
        self._assert_sh2_art("Blood Flurry", 2.0)

    def test_mixed_one_hand_or_two_hand_collapses_to_generic(self):
        sub = _submod(self.scan, "AoW Pack", "Elegant Slash")
        _write(
            sub / "config.json",
            json.dumps(
                _worn_config(
                    "Elegant Slash",
                    equipped=False,
                    extra_conditions=[_or_types([1.0, 5.0], left_hand=False)],
                )
            ),
        )
        _write(sub / "animations" / AABL, "clip")

        self._generate()

        self.assertEqual(self._art_class_of("Elegant Slash"), "Generic")
        self._assert_sh2_art("Elegant Slash", 2.0)

    def test_dual_both_hands_collapses_to_dual(self):
        sub = _submod(self.scan, "AoW Pack", "Dual Flurry")
        _write(
            sub / "config.json",
            json.dumps(
                _worn_config(
                    "Dual Flurry",
                    equipped=False,
                    extra_conditions=[
                        _or_types([1.0, 2.0, 3.0, 4.0], left_hand=False),
                        _or_types([1.0, 2.0, 3.0, 4.0], left_hand=True),
                    ],
                )
            ),
        )
        _write(sub / "animations" / AABL, "clip")

        self._generate()

        self.assertEqual(self._art_class_of("Dual Flurry"), "Dual")
        self._assert_sh2_art("Dual Flurry", 2.0)

    def test_keyword_only_and_unarmed_punch_collapse_to_generic(self):
        keyword = _submod(self.scan, "AoW Pack", "Disengage")
        _write(keyword / "config.json", json.dumps(_worn_config("Disengage", equipped=False)))
        _write(keyword / "animations" / AABL, "clip")
        punch = _submod(self.scan, "AoW Pack", "Crane Style")
        _write(
            punch / "config.json",
            json.dumps(
                _worn_config(
                    "Crane Style",
                    equipped=False,
                    extra_conditions=[
                        _equipped_type(0.0, left_hand=False),
                        _equipped_type(0.0, left_hand=True),
                    ],
                )
            ),
        )
        _write(punch / "animations" / AABL, "clip")

        self._generate()

        self.assertEqual(self._art_class_of("Disengage"), "Generic")
        self.assertEqual(self._art_class_of("Crane Style"), "Generic")
        kinds = json.loads(_sh2_config_path(self.overlay, "Disengage").read_text(encoding="utf-8"))["conditions"]
        self.assertEqual([c["condition"] for c in kinds], ["CompareValues", "IsActorBase"])


    def test_regen_preserves_icon_by_display_name(self):
        sub = _submod(self.scan, "AoW Pack", "Disengage")
        _write(sub / "config.json", json.dumps(_worn_config("Disengage")))
        _write(sub / "animations" / AABL, "clip")
        prior = self.root / "prior.csv"
        _write(
            prior,
            "ArtID\tDisplayName\tIcon\tSelector\tArtClass\tStaminaCost\tCooldown\tGlobalCooldown\n"
            "12\tDisengage\tDESTRUCTION_FIRE_ADEPT\t12\tGeneric\t25\t8s\t1.0\n",
        )
        from generate_art_pack import _previous_from_csv, _previous_icons_from_csv

        result = self._generate(
            previous_paths=_previous_from_csv(prior),
            previous_icons=_previous_icons_from_csv(prior),
        )

        self.assertEqual(result.emitted, 1)
        row = [line for line in self.arts_csv.read_text(encoding="utf-8").splitlines() if "Disengage" in line][0]
        cols = row.split("\t")
        self.assertEqual(cols[2], "DESTRUCTION_FIRE_ADEPT")

    def test_new_ash_without_prior_icon_uses_default(self):
        sub = _submod(self.scan, "AoW Pack", "Flurry Strike")
        _write(sub / "config.json", json.dumps(_worn_config("Flurry Strike")))
        _write(sub / "animations" / AABL, "clip")

        self._generate()

        row = [line for line in self.arts_csv.read_text(encoding="utf-8").splitlines() if "Flurry Strike" in line][0]
        self.assertEqual(row.split("\t")[2], "GREATER_POWER")


class CustomArtFolderTest(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        self.core = self.root / "core"
        self.scan = self.root / "scan"
        self.overlay = self.root / "overlay"
        self.arts_csv = self.root / "arts_ashes.csv"

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def test_core_ships_twelve_templates_without_aow_or_pointer_clip(self):
        rows = emit_custom_art_templates(self.core, count=CUSTOM_TEMPLATE_COUNT)

        self.assertEqual(len(rows), 12)
        pack = _submod(self.core, SH2_PACK, "Custom_Ability_1").parent
        self.assertTrue((pack / "config.json").is_file())
        for n in range(1, 13):
            name = f"Custom_Ability_{n}"
            config = json.loads(_sh2_config_path(self.core, name).read_text(encoding="utf-8"))
            self.assertEqual(config["name"], f"Custom Ability {n}")
            self.assertNotIn("overrideAnimationsFolder", config)
            kinds = [c["condition"] for c in config["conditions"]]
            self.assertEqual(kinds, ["CompareValues", "IsActorBase"])
            blob = json.dumps(config).lower()
            self.assertNotIn("ashes of war additional attack v items.esp", blob)
            self.assertEqual(config["Value B"]["value"] if False else config["conditions"][0]["Value B"]["value"], float(CUSTOM_ART_ID_BASE + n))
            self.assertFalse(any(p.name.lower() == AABL.lower() for p in _submod(self.core, SH2_PACK, name).rglob("*")))
        self.assertEqual(rows[2]["display_name"], "Custom Ability 3")
        self.assertEqual(rows[2]["icon"], "GREATER_POWER")
        self.assertEqual(rows[2]["art_class"], "Generic")
        self.assertEqual(rows[2]["art_id"], CUSTOM_ART_ID_BASE + 3)
        self.assertFalse(rows[2]["has_clip"])

    def test_folder_files_and_extra_number_scan_into_catalogue(self):
        emit_custom_art_templates(self.core)
        extra = _submod(self.core, SH2_PACK, "Custom_Ability_13")
        extra.mkdir(parents=True)
        _write(extra / "name.txt", "Rapier Lunge\n")
        _write(extra / "icon.txt", "FIREBOLT\n")
        _write(extra / "animations" / AABL, "player-dropped-clip")
        named = _submod(self.core, SH2_PACK, "Custom_Ability_3")
        _write(named / "name.txt", "My Third Art\n")
        _write(named / "icon.txt", "FLAMES\n")

        scanned = scan_custom_art_folders([self.core])

        by_name = {row["folder"]: row for row in scanned}
        self.assertIn("Custom_Ability_3", by_name)
        self.assertIn("Custom_Ability_13", by_name)
        self.assertEqual(by_name["Custom_Ability_3"]["display_name"], "My Third Art")
        self.assertEqual(by_name["Custom_Ability_3"]["icon"], "FLAMES")
        self.assertEqual(by_name["Custom_Ability_13"]["display_name"], "Rapier Lunge")
        self.assertEqual(by_name["Custom_Ability_13"]["icon"], "FIREBOLT")
        self.assertTrue(by_name["Custom_Ability_13"]["has_clip"])
        self.assertFalse(by_name["Custom_Ability_1"]["has_clip"])
        extra_config = json.loads(_sh2_config_path(self.core, "Custom_Ability_13").read_text(encoding="utf-8"))
        self.assertNotIn("overrideAnimationsFolder", extra_config)

    def test_ash_regen_keeps_templates_and_still_points_ashes(self):
        emit_custom_art_templates(self.overlay)
        _write(_submod(self.overlay, SH2_PACK, "Custom_Ability_3") / AABL, "dummy")
        sub = _submod(self.scan, "AoW Pack", "Flurry Strike")
        _write(sub / "config.json", json.dumps(_worn_config("Flurry Strike")))
        _write(sub / "animations" / AABL, "clip")

        generate(scan_roots=[self.scan], arts_csv=self.arts_csv, overlay_root=self.overlay)

        self.assertTrue(_sh2_config_path(self.overlay, "Custom_Ability_3").is_file())
        self.assertTrue((_submod(self.overlay, SH2_PACK, "Custom_Ability_3") / AABL).is_file())
        ash = json.loads(_sh2_config_path(self.overlay, "Flurry Strike").read_text(encoding="utf-8"))
        self.assertEqual(ash["overrideAnimationsFolder"], "../AoW Pack/Flurry Strike")
        custom = json.loads(_sh2_config_path(self.overlay, "Custom_Ability_1").read_text(encoding="utf-8"))
        self.assertNotIn("overrideAnimationsFolder", custom)
        self.assertFalse(any("gild" in p.name.lower() for p in self.overlay.rglob("*.hkx")))
        text = self.arts_csv.read_text(encoding="utf-8")
        self.assertIn("Flurry Strike", text)
        self.assertNotIn("Custom_Ability_1", text)


if __name__ == "__main__":
    unittest.main()


class CoreOverlayCollisionTest(unittest.TestCase):
    """--core and --overlay pointing at one directory is the mistake that fails silently."""

    def test_same_directory_is_refused(self):
        from generate_art_pack import main

        with tempfile.TemporaryDirectory() as tmp:
            shared = Path(tmp) / "both"
            shared.mkdir()
            code = main([
                "--scan", str(Path(tmp) / "scan"),
                "--arts-csv", str(Path(tmp) / "arts.csv"),
                "--overlay", str(shared),
                "--core", str(shared),
            ])
        self.assertEqual(code, 2, "a shared core/overlay directory must be refused, not generated into")

    def test_same_directory_via_dot_segments_is_refused(self):
        from generate_art_pack import main

        with tempfile.TemporaryDirectory() as tmp:
            shared = Path(tmp) / "both"
            shared.mkdir()
            code = main([
                "--scan", str(Path(tmp) / "scan"),
                "--arts-csv", str(Path(tmp) / "arts.csv"),
                "--overlay", str(shared),
                "--core", str(shared / "." / ".." / "both"),
            ])
        self.assertEqual(code, 2, "the check resolves paths, so '.'/'..' cannot smuggle the collision through")

    def test_different_directories_pass_the_check(self):
        from generate_art_pack import _same_dir

        with tempfile.TemporaryDirectory() as tmp:
            a = Path(tmp) / "core"; a.mkdir()
            b = Path(tmp) / "overlay"; b.mkdir()
            self.assertFalse(_same_dir(a, b), "distinct directories must not trip the guard")
