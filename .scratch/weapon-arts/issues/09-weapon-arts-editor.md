# 09 — Ability Editor

In-game ImGui over any **Ability** catalogue row: name, atlas icon, Ability Class, cooldown, GCD, and
Ability Costs. Custom Ability persists in the folder sidecar (ADR-0009). Pointer-pack ashes persist
in the user overlay (ADR-0010). Clips fire as authored. Custom Ability Spell assignment is ticket 12.

**Blocked by:** 08 (resolved), 06 (bar must draw extra-atlas icons the picker can assign)

**Status:** resolved

## You test this

Profile `Nolvus Awakening`.

1. Abilities tab → Edit on **Custom Ability 3**. Own window titled Ability Editor (not Spell Editor).
   No Custom Ability Spell picker. Set a name, an atlas icon, Ability Class, costs / CD / GCD, and
   bind the row. The hotbar shows that name and icon (after 06). `$GLOBAL_COOLDOWN` reads
   **Global Cooldown** (not “Globald”). Backspace in the name field deletes once per tap, not a
   stuck repeat. **Pass** (owner 2026-08-21).
2. Drop a real `AABL_Attack_A.hkx` if the folder is empty. Press the slot: the clip plays and
   whatever the clip was authored to cast/whoosh, casts/whooshes. SH2 does not strip CAST or inject
   `PIE.$custom_ability_N`. **Pass** (owner 2026-08-21, Custom Ability 3).
3. Set magicka (or health) cost above zero with too little of that meter: MagFail + flash, no
   start. Stamina 25 / magicka 0 / health 0 / 8s CD / 1s GCD remain defaults if never edited.
   **Pass** (owner 2026-08-21).
4. Empty folder (no clip): editor still opens and saves; press still MagFails (08). Owner does not
   care about this cell in playtest; automate later if needed.
5. **Aimed Blow** (or any ash): Edit opens the Ability Editor, not only the icon dialog. Change a
   cost or name, Save, press: clip unchanged, SH2 costs apply. Reset restores the CSV catalogue
   values. Pointed AoW HKX is never written. **Pass** (owner 2026-08-21).

If Edit lives in Spell Editor, if an ash HKX is rewritten, if the spell picker is back, or if SH2
strips a clip’s CAST, it fails.

## Agent tests the rest

6. Custom Ability sidecar `ability.ini` is the source of truth for that folder (ADR-0009). Ash
   player tuning is the user overlay (ADR-0010). Spell fields may still round-trip unused (ticket 12).
   Do not put Custom Ability definition in `icon_edits` or the co-save.
7. Do **not** stamp or in-memory-inject Custom Ability PIE. Do **not** strip author CAST/SoundPlay.
   Pointed AoW files are never written.
8. `Custom_Ability_13+` uses the same editor and sidecar.
9. `try_start_art` refuses on short magicka/health the same way as stamina. PIE does not start
   shout cooldown (and is not the fire).
10. Spell assignment / CAST strip / `$custom_ability_N` inject / assigned whoosh is ticket 12.
11. Fire-time slider / override is ticket 11, blocked by 12.
12. Fixture restored; Skyrim left running for owner review unless they asked to tear down.

## Answer

Owner 2026-08-21: all playtest cases work (editor, clip-native fire, MagFail, ash Ability Editor).
Empty-folder MagFail skipped (owner does not care). Custom Ability Spell assignment parked to 12.

## What this is

The Ability Editor. Motion and timed effect come from the clip. SH2 owns costs and cooldown.
Custom Ability and ashes share the same ImGui; persistence differs (ADR-0009 vs ADR-0010).

## What this is not

Not instantiate-from-ash. Not clip picking in ImGui. Not dummy `.hkx` (08). Not MagFail gray-out
(07). Not queue into attacks (10). Not Custom Ability Spell assignment (12). Not fire-time UI (11).
Not Spell Editor. Not writing pointer-pack HKX.

## Notes

Grill 2026-08-18 parked this. Grill 2026-08-20/21 un-parked the design including assignment.
Owner 2026-08-21 parked assignment (ticket 12) after live fire failed. Glossary: Ability Editor,
Ability Cost. Custom Ability Spell stays in CONTEXT as the parked 12 term. ADR-0009 still names
the sidecar; spell keys in it are unused until 12.

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

2026-08-21: Owner: dropped AoW/AABL clips often already have `PIE.@CAST` (sometimes two). Product
rule while assignment is on is one spell fire — strip author `@CAST`/`@CASTSPELL`, keep a single
`PIE.$custom_ability_N` at HitFrame else 5%. That path is now ticket 12.

2026-08-21: Author `SoundPlay` co-timed with CAST is stripped when assignment is on. Inject/stamp
plants `SoundPlay.<Release editor id>` from the assigned spell's first MGEF at fire time.
Weapon swing sounds stay. Parked with ticket 12; never in a live DLL this session.

2026-08-21 owner round 2: picker combo fixed; own window fixed; `$GLOBAL_COOLDOWN` still “Globald”
(TSV override, not C++); Backspace still sticks; **no spells actually cast** from assigned F&F.
Ashes Edit: Aimed Blow should have none; Custom Ability N should. MagFail untested. Empty-folder
untested (owner does not care).

2026-08-21 owner: **park** assignment. Hide picker; restore clip payloads; keep tuning editor.
Findings + built inject/picker/PI work → ticket 12 `needs-triage`.

2026-08-21: Park landed. `custom_ability_spell_assignment_enabled = false`; picker hidden;
stamp/inject/PI emit no-op. `$GLOBAL_COOLDOWN` fixed in Dev overlay `translation.txt` (winner over
Spell Hotbar 2). ImGui keyboard is Skyrim-edge state replayed after Win32 NewFrame, with a 200ms
hold timeout so a missed key-up cannot stick Backspace. `art_data_test` ok. DLL in Dev - Spell
Hotbar 2 (10:09 PM). Game left running on CS-Test Save12, profile Nolvus Awakening, for owner 1–5.

2026-08-21 owner: **all cases work.** Status → resolved. Ashes Ability Editor included. Next:
triage 12 (assignment did not cast); 11 stays blocked on 12; 06 still agent-done (icon identity).
