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

from generate_art_pack import generate


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


def _worn_config(name: str, priority: int = 103900002, *, extra_conditions=None) -> dict:
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
        {
            "condition": "IsEquippedType",
            "requiredVersion": "1.0.0.0",
            "Type": {"value": 1.0},
            "Left hand": False,
        },
    ]
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
        self.assertEqual(selector_cond["Value A"]["form"]["pluginName"], "SpellHotbar.esp")
        self.assertEqual(selector_cond["Value A"]["form"]["formID"], "D63")
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
        self.assertTrue(text.startswith("ArtID\tDisplayName\tIcon\tSelector\tStaminaCost\tCooldown\tGlobalCooldown"))
        row = [line for line in text.splitlines() if "Flurry Strike" in line][0]
        cols = row.split("\t")
        self.assertEqual(cols[0], "2")
        self.assertEqual(cols[1], "Flurry Strike")
        self.assertEqual(cols[3], "2")

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


if __name__ == "__main__":
    unittest.main()
