# 06 — Give each Weapon Art a distinct icon

Every generated Ash currently stamps `GREATER_POWER`. Bound arts therefore look identical on the
bar and in the Arts tab; only the name tells them apart. Story 6 wants an icon *and* a name.

**Blocked by:** 03 (resolved)

**Status:** ready-for-agent

## You test this

Profile `Nolvus Awakening`. Binding Menu Arts tab from ticket 02.

1. Bind two different ashes to two slots. The bar shows two different icons, not the same greater-
   power glyph twice. The Arts tab matches those icons to those names.
2. A missing or unknown icon key degrades to a known fallback (today: `GREATER_POWER` / UNKNOWN),
   not a blank slot.

If every ash still shares one glyph, or if a new icon key draws nothing, it fails.

Owner art pass: sample **Crane Style**, **Disengage**, and **Blood Flurry** first. Batch the rest
of the pointer-pack names only after those three look right on the bar.

## Agent tests the rest

3. The catalogue `Icon` column is not the single placeholder for every named ash.
4. Icons load through the **extra atlas** path (`stitch_icon_atlas.py` / `icons_*.csv` `IconName`).
   `draw_art_icon` resolves extra-atlas keys, not only `DefaultIconType`. Regenerating the pack
   keeps those keys wired; it does not invent a second icon system.
5. No redistributed clip, and no unlicensed third-party art (including Elden Ring textures and AoW
   item icons), is copied into this repo to get an icon. Glyphs are original; ER ashes are a
   silhouette/language reference only. Paint and palette follow SH2; each art is one silhouette
   (hybrid).

## What this is

Per-art **identity on the bar**. The catalogue already has an `Icon` field; this ticket fills it
with distinct extra-atlas keys.

## What this is not

Not clip selection (04). Not motion (05). Not Art Class (07). Not Custom Art Folders (08). Not the
in-game editor (09). Not Nordic UI second tint.

## Notes

Today: `python_scripts/generate_art_pack.py` writes `DEFAULT_ICON = "GREATER_POWER"` on every row.
`draw_art_icon` resolves only `TextureCSVLoader::default_icon_names`. Extra atlas keys already
work for spells.

## Comments

Grill 2026-08-18: source = agent image gen; hybrid SH2 paint + ER badge consistency; extra atlas;
sample three then batch; Nordic tint later.
