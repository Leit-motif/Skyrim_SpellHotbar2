# 12 — Custom Ability Spell assignment (enhancement)

Let the Ability Editor pick a fire-and-forget spell that Payload Interpreter fires from the
Custom Ability Folder clip, instead of the clip’s author `@CAST` / `@CASTSPELL` payloads.

**Blocked by:** 09

**Status:** deferred 2026-08-23 — owner ruling: future enhancement, not v1. Triage when re-raised.

## You test this

(Unwritten until triage.) Open Custom Ability 3, assign Ice Spike (or another known F&F), press
the bound slot: the clip plays and **that** spell releases once at HitFrame (else 5% of duration).
The clip’s original Spellscribe/Forgotten Magic CAST does not also fire. MagFail still belongs to
SH2 costs, not PIE.

## Agent tests the rest

Sidecar stores `spell_form` / `spell_plugin` / `self_target`. Emit
`Data/SKSE/PayloadInterpreter/Config/SpellHotbar2_CustomAbilities.ini` as
`$custom_ability_N = @CASTSPELL|…` with zero resource lines. Strip author `@CAST` / `@CASTSPELL`
(keep HitFrame / windows / motion). In-memory inject on `SH2_Art_Clip` so a session without
`HKXC_ANNO_CLI` matches on-disk stamp. Plant `SoundPlay.<Release editor id>` from the assigned
spell’s first MGEF; strip author whoosh co-timed with CAST; keep `SoundPlay.WPN*`. Pointer-pack
ashes never stamped. Default assignment is vanilla Firebolt (`0x12FD0` / `Skyrim.esm`) even if
unknown. Picker is a real ImGui combo (filter inside the popup, `PushID(spell)`).

## What this is

The parked product path from ticket 09: Custom Ability Spell as timed effect, clip as motion only.

## What this is not

Not Ability Editor name/icon/class/costs/CD/GCD (those shipped in 09). Not fire-time UI (11).
Not instantiate-from-ash. Not Ashes of War. Not double-fire with author CAST left in place.

## Playtest findings (2026-08-21)

Parked after owner round 2 on CS-Test (rapier, Riverwood, Ice Spike known). Profile
`Nolvus Awakening`. Live DLL at that round was the 9:30 PM Dev copy — dropdown + C++ Globald
string + first Backspace attempt. Assigned-spell SoundPlay and later inject were **not** in that
DLL (copy EBUSY).

| Claim | Result |
|---|---|
| Spell picker as a real combo | Fixed |
| Ability Editor is its own window | Fixed |
| Assigned F&F fires once at HitFrame | **No spells actually cast** |
| Author CAST left on dropped AoW/AABL clips | Many clips already have `PIE.@CAST` (sometimes two) plus matching `SoundPlay` |
| In-memory strip + `PIE.$custom_ability_N` inject | Coded; did not produce a visible cast in that round |
| PI `@CASTSPELL` → Payload Interpreter `CastSpellImmediate` | Assumed; live fire failed anyway |
| Multi-hit clips collapsed to one SH2 fire | Product rule while this path is on; not proven live |
| Empty-folder MagFail | Untested; owner does not care; automate later |

Owner decision: hide the picker, restore clip payloads as the fire, keep the tuning editor.
Stacking restored author CAST with SH2 inject would double-fire — do not ship both.

## What is already built (do not delete; do not call from product)

Leave these in tree for this ticket. 09’s park makes them dead at runtime (`inject` / `stamp` no-op).

- Sidecar + PI emit: `skse_plugin/src/game_data/custom_ability_config.{h,cpp}`
- Runtime persist / inject / hkxc stamp / Release whoosh:
  `skse_plugin/src/game_data/custom_ability_runtime.{h,cpp}`
- Loader still reads spell fields from `ability.ini`
- Tests: `art_data_test` (`ensure_custom_ability_pie_in_annotation_txt`, assignable F&F, Firebolt default)
- Editor picker (hidden in 09): `ability_editor.cpp` combo + in-popup filter

Re-enable by setting `custom_ability_spell_assignment_enabled` in `custom_ability_config.h`,
showing the picker, and **not** leaving author CAST on the clip.

## Notes

Grill 2026-08-21 originally un-parked this as ticket 09. Owner 2026-08-21 parked assignment after
live fire failed; 09 keeps the overlay editor and clip-native payloads.

Ticket 11 (fire-time slider) is an enhancement **on this inject**, not on 09’s parked editor.

If a previous session stamped HKX on disk (`HKXC_ANNO_CLI` set), recopy `AABL_Attack_A.hkx` from
the repo Custom Ability folders before retesting clip-native fire.

## Comments

2026-08-21: 09 resolved with clip-native fire. This ticket is unblocked for triage. First question:
why PI `$custom_ability_N` / in-memory CASTSPELL produced no visible cast while author `@CAST` on
the same clip did. Do not ship strip+inject until that is answered.
