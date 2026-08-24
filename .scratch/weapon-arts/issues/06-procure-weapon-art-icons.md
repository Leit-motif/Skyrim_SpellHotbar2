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

Owner 2026-08-23: **Cyclone Spin** is finalized as `aow_10_cyclone_spin`. Two huge mirrored-radius
hits and a knockback finisher become a Redguard-coded duelist inside a two-part turquoise-gold
cyclone. Its sand/ochre field, warm-brown edges, and deep-red center establish deliberate background
and palette variety while retaining 32 px clarity.

Owner 2026-08-23: **Dash Slam** is finalized as `aow_11_dash_slam`. Its roughly 686-unit dash and
single area-knockback hit become an Orsimer-armored greatsword shock trooper crossing a forge-green
field into a rust-orange impact. Owner-supplied armor references guided only the original layered
plate silhouette. Helmet corrections added the tall blade-ridge, flared cheek guards, and attached
trailing crest-tail; no source reference is redistributed.

Owner 2026-08-23: **Disengage** is finalized as `aow_12_disengage`. Its backward acrobatic motion
becomes an inverted Bosmer-coded dual-dagger leap carried by one moss-green rotation crescent. The
final correction aligns the whole body to one coherent three-quarter plane and relaxes the arms so
the glyph reads as retreating movement, not a strike.

Session pause 2026-08-23: **Divided Strike** (`aow_13_divided_strike`) is in progress and has no
approved candidate. Eleven of 57 icons are finalized, leaving 46. The moving-lunge candidate was
the closest, but its X geometry was not convex as the owner intended; the following outward-C-curve
interpretation was explicitly rejected. Resume from `.scratch/weapon-arts/icon-authoring-handoff.md`
and clarify the intended convex-X shape before generating again.

Owner 2026-08-23: **Divided Strike** is shipped as `aow_13_divided_strike` under an explicit time
constraint. The selected glyph is a forward dual-wield lunge with an ordinary red-magenta X
cross-slash. The owner said it was not the result they wanted but chose to ship it rather than spend
more time; this is a pragmatic acceptance, not an endorsement of the abandoned convex-X direction.
Twelve of 57 icons are finalized, leaving 45.

Owner 2026-08-23: **Divine Smite** is finalized as `aow_14_divine_smite`. The selected glyph uses
a Breton-coded dark spellsword, radiant diagonal gold-white sword smite, terminal explosion,
circular divine halo, and pale-blue enchant wisps. The owner selected the original two-handed-grip
render over the later one-handed correction; the exact attached PNG is canonical despite the
catalogue's `1H` class. Thirteen of 57 icons are finalized, leaving 44.

Owner 2026-08-23: **Double Slash** is finalized as `aow_15_double_slash`. Its two swing events and
roughly 151-unit advance are distilled into one dominant silver-blue slash and amber endpoint for
hotbar clarity. A small French-medieval knight-coded Breton in closed bascinet, mail, plate, and
blue/burgundy surcoat remains subordinate to the effect. Fourteen of 57 icons are finalized,
leaving 43.

Owner 2026-08-23: **Dragon Strike** is finalized as `aow_16_dragon_strike`. Its early Flaming
Strike setup, pre-swing sword ignition, roughly 152-unit advance, and terminal hit become one
frontal, foreshortened white-hot flaming sword impact. Abstract symmetrical flame curls suggest
dragon jaws without depicting a literal creature. Fifteen of 57 icons are finalized, leaving 42.

Session 2026-08-23: **Dual Flurry** (`aow_17_dual_flurry`) is in progress. The 1.666667-second
dual-wield clip advances roughly 284 units and delivers two simultaneous paired-weapon hits; the
second pair carries knockback. The first candidate is pending.

Historical context-layer pivot 2026-08-23: Dual Flurry generation was paused after repeated semantic drift.
The project now separates race, archetype or faction, equipment family, physical action, and effect
instead of allowing a race palette or prior candidate to choose the rest. Skyrim visual vocabulary,
a semantic prompt brief, and the project-local prompt-authoring skill live under
`python_scripts/weapon_art_icons/` and `.agents/skills/skyrim-weapon-art-icon-prompter/`. The first
prompt-only forward test is `.scratch/weapon-arts/prompt-drafts/aow_17_dual_flurry-context-v1.md`;
that draft is superseded by the final disposition below.

Owner 2026-08-23: **Dual Flurry** is finalized as `aow_17_dual_flurry` from the exact 1024 x 1024
Gemini-generated PNG supplied by the owner. It depicts a leaping blond Nord barbarian whose two
inward-facing one-handed axes physically generate the crossed crimson trails along their curved
lines of motion. Earlier Codex candidates remain rejected calibration output. Sixteen of 57 icons
are finalized, leaving 41.

Goal mode 2026-08-23: **Earth Shatter** is finalized as `aow_18_earth_shatter` from its single
generation attempt. The verified clip advances about 101 units from a high two-handed wind-up into
one downward hit. With no elemental payload proven, the glyph uses an Orsimer-coded warhammer slam
whose white compression flare, dust, and stone originate at the hammer-ground contact. The full
result and 32 px LANCZOS reduction passed the hard gate. Seventeen of 57 icons are finalized,
leaving 40.

Goal mode 2026-08-23: **Eldritch Beam** is finalized as `aow_19_eldritch_beam` from its single
generation attempt. The resolved payload stages three casting effects and then fires a green-lit
2000-unit beam from the right magic node, with a secondary fire-linked terminal component. The
glyph uses one small braced caster, one continuous horizontal white-green beam, and one compact
endpoint whose warm fringe stays subordinate. The full result and 32 px LANCZOS reduction passed
the hard gate. Eighteen of 57 icons are finalized, leaving 39.

Goal mode 2026-08-23: **Elegant Slash** is finalized as `aow_20_elegant_slash` from its single
generation attempt. Its roughly 421-unit single-weapon lunge becomes one broad rising ivory-white
blade path with a tiny faceless duelist only to show causality. The unresolved `$ES_Paralysis`
payload contributes no invented element or color. The full result and 32 px LANCZOS reduction
passed the hard gate. Nineteen of 57 icons are finalized, leaving 38.

Goal mode 2026-08-23: **Enrage (F)** is finalized as `aow_21_enrage_f` from its single generation
attempt. Its one-hit clip explicitly requests a 2.5-intensity deep-crimson weapon trail, which
becomes the open crescent icon while a small faceless female fighter only marks the source. The
full result and 32 px LANCZOS reduction passed the hard gate. Twenty of 57 icons are finalized,
leaving 37.

Goal mode 2026-08-23: **Enrage (M)** is finalized as `aow_22_enrage_m` from its single generation
attempt. Its weaponless five-second clip stages a blast and heavy battle shout; unresolved `$FZ*`
payloads authorize no element. Broken ivory-white pressure bands form the icon above a tiny
faceless source figure. The full result and 32 px LANCZOS reduction passed the hard gate. Twenty-one
of 57 icons are finalized, leaving 36.

Goal mode 2026-08-23: **Flurry Strike** (`aow_23_flurry_strike`) is a recorded one-shot hard
failure, not a finalized icon. The three repeated trails survived at 32 px, but the generated
Orsimer was character-key-art scale and the battleaxe became double-bitted despite the practical
single-head brief. Full and 32 px evidence are preserved under the ignored `pilot/` tree; no master
or atlas input exists and no regeneration was attempted. Twenty-one of 57 icons are finalized;
35 remain unprocessed and one is a recorded hard failure.
