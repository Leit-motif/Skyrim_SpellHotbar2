# 06 — Give each Weapon Art a distinct icon

Every generated Ash currently stamps `GREATER_POWER`. Bound arts therefore look identical on the
bar and in the Arts tab; only the name tells them apart. Story 6 wants an icon *and* a name.

**Blocked by:** 03 (resolved)

**Status:** agent-done

## You test this

Profile `Nolvus Awakening`. Binding Menu Arts tab from ticket 02.

1. Bind two different ashes to two slots. Right-click each art in the Arts tab, pick two different
   atlas icons, Save. The bar shows those two icons (not the same greater-power glyph twice). The
   Arts tab matches those icons to those names.
2. A missing or unknown icon key degrades to `UNKNOWN`, not a blank slot. Reset in the picker
   restores the catalogue default (`GREATER_POWER` until you assign).

Owner art pass: sample **Crane Style**, **Disengage**, and **Blood Flurry** first via the picker
(any atlas glyph). Batch the rest only after those three look right on the bar. Persist picks with
the existing Save Icon Edits / MCM preset flow (`art_icons` in the icon-edits JSON).

## Agent tests the rest

3. After two picker assignments, the live catalogue is not all one placeholder icon. Fresh
   `arts_ashes.csv` may still default to `GREATER_POWER`; distinctness comes from overrides or
   regen-preserved Icon cells.
4. Icons load through the **extra atlas** path (`stitch_icon_atlas.py` / `icons_*.csv` `IconName`).
   `draw_art_icon` resolves extra-atlas keys, not only `DefaultIconType`. Regenerating the pack
   keeps those keys wired; it does not invent a second icon system.
5. No redistributed clip, and no unlicensed third-party art (including Elden Ring textures and AoW
   item icons), is copied into this repo to get an icon. Glyphs are original; ER ashes are a
   silhouette/language reference only. Paint and palette follow SH2; each art is one silhouette
   (hybrid).

## What this is

Per-art **identity on the bar**. The catalogue `Icon` field remains the pack default
(`GREATER_POWER` for generated ashes). The Binding Menu Arts tab icon picker assigns any loaded
atlas glyph per art; overrides persist in icon-edits JSON and survive pack regen (Icon preserved
by DisplayName).

## What this is not

Not clip selection (04). Not motion (05). Not Art Class (07). Not Custom Art Folders (08). Not the
in-game editor (09). Not Nordic UI second tint.

## Notes

Shipped 2026-08-21: `draw_art_icon` resolves form / extra-atlas / default / UNKNOWN;
`ArtIconEditor` in Binding Menu Arts tab (right-click row icon); `art_icons` in icon-edits JSON;
`generate_art_pack` preserves Icon by DisplayName on regen. No new PNG glyphs in repo — picker
uses existing SH2 atlases only.

Prior: `python_scripts/generate_art_pack.py` wrote `DEFAULT_ICON = "GREATER_POWER"` on every row;
`draw_art_icon` resolved only `TextureCSVLoader::default_icon_names`.

## Comments

Grill 2026-08-18: source = agent image gen; hybrid SH2 paint + ER badge consistency; extra atlas;
sample three then batch; Nordic tint later.

Agent 2026-08-21: picker-over-atlas defers bespoke glyph batch; owner assigns Crane Style /
Disengage / Blood Flurry first, then batch remainder via picker + optional CSV bake.
