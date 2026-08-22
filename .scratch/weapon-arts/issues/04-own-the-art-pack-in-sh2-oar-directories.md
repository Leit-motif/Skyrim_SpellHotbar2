# 04 — Own the Art Pack in Spell Hotbar 2 OAR directories

Ticket 03 maps each named Ash to a catalogue row and can pick the right clip. It does that by
writing `user.json` onto **their** OAR folders. That fully shadows their `config.json`, so
selector 0 can no longer use the original worn-keyword Crane Style (and friends). This ticket
moves selection into Spell Hotbar 2's own Art Pack.

**Blocked by:** 03 (resolved)

**Status:** resolved

## You test this

Profile `Nolvus Awakening`, save `SH2ArtBind04` (Prisoner, QASmoke, Noble Rapier). Binding Menu
Arts tab from ticket 02.

1. Bind Crane Style and Disengage to two slots. Press each with the rapier drawn, no slot-55
   art clothing. Each plays its own clip, not Sword Neutral and not each other.
2. Clear the Art Selector (end the art, or unequip the slots). Use Ashes of War the old way —
   worn keyword / AABL hotkey. Crane Style (or Neutral, depending what is worn) still works as
   it did before this fork.

If selector 0 still cannot reach a worn Ash, or if a bound art still depends on a `user.json`
sitting in an Ashes of War folder, it fails.

## Agent tests the rest

3. Regenerating the pack writes `config.json` under a Spell Hotbar 2 OAR tree
   (`OpenAnimationReplacer/<this fork's pack>/<Art>/`). It does not write `user.json` into
   Ashes of War (or Additional Attack) folders, and it copies no `.hkx`.
4. Each SH2 submod points at the existing Ash clip (`overrideAnimationsFolder` or equivalent)
   so the file on disk stays in the author's mod.
5. Conditions are Art Selector `CompareValues` plus player-only. Worn keywords and weapon-type
   gates stay on **their** configs, not on ours. Priority is a reserved SH2 band that beats
   Sword Neutral (`1001002544`) without depending on their numbers.
6. Art Selector is 0 at rest. A missing or renamed source folder logs loudly; other arts still
   bind.

## What this is

Clip **selection**. ADR-0007 still names `Animations\AABL_Attack_A.hkx`. ADR-0008 still
chooses the replacement with the Art Selector. The Art Pack is **this fork’s** OAR submods,
generated at authoring time from installed Ashes of War — not a runtime call into that mod.

## What this is not

Not motion. Disengage still jumping in place is ticket 05. Own directories do not make AMR
apply; they only decide which file plays.

Not a Nemesis path per art. Not redistributing clips. Not notifying Additional Attack, MSCO,
or any other mod.

## Notes

Baseline was ticket 03’s `user.json` shadows. This ticket replaced that emit.

Target shape: one SH2 submod per named ash, `config.json` only, `overrideAnimationsFolder`
aimed at the scanned Ash folder, priority `2000000000 + selector`, conditions = selector +
`IsActorBase` player. After a regen, delete leftover `user.json` from the overlay so VFS
cannot keep serving the old shadows.

Seam tests cover scan → catalogue + SH2 `config.json` with zero `.hkx` and no foreign
`user.json`. `--previous-csv` logs missing/renamed names on regen.

## Comments

2026-08-18: Generator emit is now SH2-owned `OpenAnimationReplacer/SpellHotbar2Arts/<Art>/config.json`.
Live regen of Stance Framework + AoW items pack: 57 arts, 0 missing, 0 `.hkx`, 0 leftover
`user.json` in `Dev - Spell Hotbar 2` and `Dev - Spell Hotbar 2 - Art Pack`. Crane Style
selector 8 / priority 2000000008, Disengage 12 / 2000000012, both
`overrideAnimationsFolder` → `../Nolvus Ashes of War Stance Framework/<Art>`. Conditions are
CompareValues + IsActorBase player. Catalogue: `data/SKSE/Plugins/SpellHotbar/artdata/arts_ashes.csv`.
Seam tests: 8 passing (`python_scripts/generate_art_pack_test.py`).

## Answer

Clip selection now lives in this fork’s Art Pack. Regenerating writes `config.json` under
`SpellHotbar2Arts`, points `overrideAnimationsFolder` at the author’s folder, and does not
shadow their `config.json` with `user.json`.

Owner 2026-08-20: cells 1–2 are not leftover work. Bound ashes already play without wearing
anything. Slot-55 clothing / AABL hotkey was Ashes of War’s old identity, not SH2’s. Proving
that path at selector 0 was a regression check for ticket 03’s `user.json` shadows; it is not
Ability product acceptance. Ticket closed.
