# 09 — Ability Editor

In-game ImGui over a **Custom Ability Folder**: name, atlas icon, Custom Ability Spell, Ability
Class, cooldown, GCD, and Ability Costs. The folder’s clip is the motion; PIE fires the assigned
spell at HitFrame (else 5% of duration). Pointer-pack ashes are not editable.

**Blocked by:** 08 (resolved), 06 (bar must draw extra-atlas icons the picker can assign)

**Status:** agent-done

## You test this

Profile `Nolvus Awakening`.

1. Abilities tab → Edit on **Custom Ability 3**. Own window (not Spell Editor). Set a name, an
   atlas icon, Ability Class, and bind the row. The hotbar shows that name and icon (after 06).
2. Assign a known fire-and-forget spell. Drop a real `AABL_Attack_A.hkx` if the folder is empty.
   Press the slot: the clip plays and the spell releases at HitFrame or at a tiny windup (~5%),
   not at t=0 and not at mid-clip by default.
3. Set magicka (or health) cost above zero with too little of that meter: MagFail + flash, no
   start. Stamina 25 / magicka 0 / health 0 / 8s CD / 1s GCD remain defaults if never edited.
4. Empty folder (no clip): editor still opens and saves; press still MagFails (08).
5. Ashes rows have no Edit that mutates them.

If Edit lives in Spell Editor, if an ash is rewritten, or if PIE charges magicka, it fails.

## Agent tests the rest

6. Sidecar in the Custom Ability Folder is the source of truth (ADR-0009). Emit PI
   `$custom_ability_N = @CASTSPELL|…` with **zero** resource lines. Do not use `name.txt` /
   `icon.txt` as the contract; do not put definition in `icon_edits` or the co-save.
7. Stamp `PIE.$custom_ability_N` into that folder’s `AABL_Attack_A.hkx` when missing (save/load
   or after a clip appears). Leave author payloads. Never stamp pointer-pack ashes.
8. Picker: known fire-and-forget `Spell` forms, including ritual F&F, powers, and voice spells.
   Exclude concentration, potions, scrolls, TESShout. Default assignment is vanilla Firebolt
   even if unknown. `selfTarget` from spell delivery; no extra toggle.
9. `Custom_Ability_13+` uses the same editor, sidecar, and `$custom_ability_N`.
10. `try_start_art` refuses on short magicka/health the same way as stamina. PIE does not start
    shout cooldown.
11. Fire-time slider / override is ticket 11, not this ticket.
12. Fixture restored; Skyrim closed after the drive.

## What this is

The Ability Editor. Motion is the dropped clip. Timed effect is the Custom Ability Spell via
Payload Interpreter. SH2 owns costs and cooldown.

## What this is not

Not instantiate-from-ash. Not clip picking in ImGui. Not dummy `.hkx` (08). Not MagFail gray-out
(07). Not queue into attacks (10). Not fire-time UI (11). Not Spell Editor.

## Notes

Grill 2026-08-18 parked this. Grill 2026-08-20/21 un-parked the design. Glossary: Ability Editor,
Custom Ability Spell, Ability Cost. ADR-0009.

## Comments

Grill 2026-08-18: Q7 = later enhancement. Superseded.

Grill 2026-08-21: Folder overlay only; whole package; on-disk sidecar; HitFrame else 5%;
independent costs (no seed); Firebolt placeholder; F&F including ritual; SH2 stam/magicka/health;
Class in editor; ImGui from Abilities tab; extras and empty folders in; unknown Firebolt ok.

Owner 2026-08-21: grill matches; Status → ready-for-agent.

2026-08-21: Agent 6–10. Sidecar `ability.ini` in the Custom Ability Folder (ADR-0009);
Ability Editor is its own ImGui (Abilities tab Edit, not Spell Editor); ashes have no Edit.
PI `$custom_ability_N = @CASTSPELL|…` with zero resource lines; default Firebolt; 13+ same path.
`try_start_art` MagFail+flash on short magicka/health as well as stamina. PIE stamp is HitFrame
else 5%; in-memory inject on `SH2_Art_Clip` so a dropped clip fires this session; on-disk HKX
rewrite needs `HKXC_ANNO_CLI`. `name.txt`/`icon.txt` still load if no sidecar (ticket 08 folders).
Tests: `art_data_test` green. Owner 1–5 still open. DLL deployed to Dev - Spell Hotbar 2.

