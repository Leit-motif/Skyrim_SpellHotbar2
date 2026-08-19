# 08 — Ship Custom Art Folder templates

Players who customize (the Rapier M workflow) drop a clip into an SH2-owned folder. Players who
do not customize still get the Ashes of War pointer pack when that mod is installed. Stories 22,
31, 32.

**Blocked by:** 04 (pack shape)

**Status:** ready-for-agent

## You test this

Profile `Nolvus Awakening`.

1. The Arts tab lists `Weapon_Art_1` … `Weapon_Art_12` as catalogue rows **and** the pointer-pack
   ashes when AoW is present. Binding `Weapon_Art_3` onto slot 7 is allowed (folder number is not
   a key index).
2. Drop a real `AABL_Attack_A.hkx` (and optional name / icon files) into `Weapon_Art_3`. After
   regen or reload, that row’s name/icon update; pressing the bound slot plays that clip, not
   Test Art and not an Ash.
3. Empty templates still appear; pressing an empty one degrades loudly (story 29), not a silent
   dead button.

If `Weapon_Art_1` is forced onto hotbar slot 1, or if templates replace the AoW list, it fails.

## Agent tests the rest

4. Core overlay / data ships twelve template submods under
   `OpenAnimationReplacer/SpellHotbar2Arts/Weapon_Art_1` … `Weapon_Art_12`, each with SH2
   `config.json` (selector + player). `N` = `max_bar_size` (12). Extra numbered folders still
   scan into the catalogue.
5. Default Art Class is Generic. Display name and icon come from files in the folder when
   present; otherwise the folder name and `GREATER_POWER` (until 06). No in-game rename UI
   (ticket 09).
6. Templates own their `.hkx` (dummy or player-dropped). Pointer-pack ashes still use
   `overrideAnimationsFolder` and copy no Gild clips. Generator / loader does not require the
   AoW items plugin for the twelve rows.
7. If a dummy clip is shipped, a PIE / `SH2_ArtEffect` placeholder annotation is allowed when it
   is cheap. Do not annotate pointed AoW files.

## What this is

The **drop-in authoring shape** for this fork’s arts. Same idea as overlaying Sword Neutral, new
path: SH2-owned, selector-keyed, bindable.

## What this is not

Not slot = folder. Not copying AoW into these folders. Not the PIE/spell editor (09). Not
promoting stance-default `Ashes of War *` folders into the catalogue.

## Notes

Installer: templates always; AoW pointer group gated on `Ashes of War Additional Attack v Items.esp`
(existing story 20).

## Comments

Grill 2026-08-18: both baseline pointer pack and numbered custom folders. Folder-only name/icon
for v1.
