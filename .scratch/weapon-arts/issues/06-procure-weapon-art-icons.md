# 06 — Give each Weapon Art a distinct icon

Every generated Ash currently stamps `GREATER_POWER`. Bound arts therefore look identical on the
bar and in the Arts tab; only the name tells them apart. Story 6 wants an icon *and* a name.

**Blocked by:** 03 (resolved)

**Status:** ready-for-human

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
`ArtIconEditor` in Binding Menu Arts tab (right-click row icon). Picker Save writes
`Documents/.../SpellHotbar/icon_edits/art_icons.json` immediately; that file is loaded after
the catalogue on launch. MCM Save/Load Icon Edits remains optional export/import.

Prior: `python_scripts/generate_art_pack.py` wrote `DEFAULT_ICON = "GREATER_POWER"` on every row;
`draw_art_icon` resolved only `TextureCSVLoader::default_icon_names`.

## Comments

Grill 2026-08-18: source = agent image gen; hybrid SH2 paint + ER badge consistency; extra atlas;
sample three then batch; Nordic tint later.

Agent 2026-08-21: picker-over-atlas defers bespoke glyph batch; owner assigns Crane Style /
Disengage / Blood Flurry first, then batch remainder via picker + optional CSV bake.

Owner 2026-08-23: the picker route is exhausted — the loaded atlases do not hold enough distinct
generic glyphs to tell 57 ashes apart, so the glyphs have to be authored after all. Plan: a
separate session generates an icon atlas for the whole Ashes set from each art's name, its
description where one exists, and clues parsed out of its annotations. Expect a cluster of themes
by weapon type, then variations within a theme by colour and angle. The SH2 paint and the ER
silhouette language from the 2026-08-18 grill remain the style base, and cell 5 still binds: the
glyphs are original, no third-party art is copied in. This ticket stays open until that atlas
exists; owner assignment through the picker is blocked on it, not the other way round.

Owner 2026-08-23: **Crane Style** is the first finalized glyph. Stable key
`aow_08_crane_style`; approved master, 128 px atlas input, and exact prompt are recorded under
`python_scripts/weapon_art_icons/`. It is intentionally not written into `arts_ashes.csv` until
the atlas CSV contains the key. The approved style is a generalized, faceless MMO ability
silhouette: Khajiit lore cues, one dominant high kick, cyan-white motion crescent, restrained gold
impact, dark atmospheric field, and 32 px readability. Installed SWF frames and the supplied MMO
icon screenshot were convention references only; none are redistributed.

Owner 2026-08-23: **Aimed Blow** is finalized as `aow_02_aimed_blow`. Its animation evidence is a
committed forward lunge with one hit and `VacuumChop`; the approved icon distills that into a
silver-blue diagonal strike through a restrained gold focal point. The curved sword is accepted as
intentional Redguard shorthand. The owner's broader race/fighting-style reference is recorded in
`python_scripts/weapon_art_icons/figure-guidance.md` and guides later silhouettes without forcing a
race choice where a neutral figure is more accurate. Catalogue wiring again waits for the atlas.

Owner 2026-08-23: **Akatosh Charge** is finalized as `aow_03_akatosh_charge`. The selected glyph
is the original circular-gold candidate: an Imperial-coded spear bearer crossing four gold pulses
inside a broken divine halo. Three later attempts made the energy more literally draconic, but each
read more like a dragon illustration and less like a spell icon. The original wins on hotbar visual
grammar and 32 px readability; literal lore motifs remain subordinate to ability recognition.

Owner 2026-08-23: **Blood Flurry** is finalized as `aow_04_blood_flurry`. The greatsword-only
nine-second clip contains twelve hits and ends in a heavy paralysis slam. The approved glyph uses
an Orc-coded shock-trooper silhouette, three broad crimson arcs as shorthand for the sustained
sequence, and one lower-right terminal impact. It replaces the rejected ornate-sword pilot with a
compact spell-icon composition.

Owner 2026-08-23: **Blood Seeker** is finalized as `aow_05_blood_seeker`. The sword-only clip
delivers five rapid `DarknessAttack` hits. The approved glyph uses a lean Dunmer-coded spellsword
and narrow crimson-violet spectral cuts converging on one dark focal point, keeping it distinct
from Blood Flurry's heavy Orc-coded arcs and slam.

Owner 2026-08-23: **Blood Spiller** is finalized as `aow_06_blood_spiller`. The sword-or-dagger
clip delivers four hits with phantom-sword, shadow, and flash effects. The approved glyph uses a
Breton-coded spellsword and a tight descending cascade of physical and spectral blades into one
cutting flash, distinct from the other two Blood arts.

Owner 2026-08-23: **Champion's End** is finalized as `aow_07_champions_end`. The two-hit,
enlarged-collision clip becomes an Imperial-coded one-handed veteran behind two oversized crossing
cuts. The owner removed an unnecessary gold wreath, replaced the initial cold-blue palette with
black, bronze, ivory, and antique gold, and selected the quieter dark field over a later oxblood
haze. Backgrounds should vary across the atlas, but a colored field is not mandatory where it
weakens the icon.

Owner 2026-08-23: **Crushing Blow** is finalized as `aow_09_crushing_blow`. The heavy two-handed
sequence resolves into a Nord-coded vertical warhammer smash with a white shock ring, lifted
weathered-brown rock, dark-red cracks, and a visibly ice-blue/steel-gray field. Owner-supplied
Skyrim concept art informed only the broad horned iron-helmet language; it is not redistributed.
The final edit equalized the horns and removed two confusing spectral background hammers.
