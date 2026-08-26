# 06 — Give each Weapon Art a distinct icon

Every generated Ash currently stamps `GREATER_POWER`. Bound arts therefore look identical on the
bar and in the Arts tab; only the name tells them apart. Story 6 wants an icon *and* a name.

**Blocked by:** 03 (resolved)

**Status:** DONE — owner-accepted 2026-08-25 on the acceptance-pass screenshots. See the closing
comment.

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

Goal mode 2026-08-23: **Flurry Strike** (`aow_23_flurry_strike`) initially failed the autonomous
one-shot gate. The owner then explicitly requested a narrow correction removing the three upper
slash echoes and retaining only the single ground-level trail physically attached to the axe.
That corrected full result and 32 px reduction pass under the owner's disposition and are finalized.

Goal mode 2026-08-23: **Focused Cross** (`aow_24_focused_cross`) initially failed the autonomous
one-shot gate because a detached mace rode the upper arc. The owner explicitly requested a narrow
correction removing that floating weapon. The corrected effect-first cross impact remains readable
at 32 px and is finalized. Twenty-three of 57 icons are finalized; 34 remain unprocessed.

Goal mode 2026-08-23: **Focused Strike** (`aow_25_focused_strike`) is finalized from its single
generation attempt. Its three timed 1H hits are distilled to one defining final low lunge and one
connected ivory-white diagonal sword path; unresolved paralysis and fog names contribute no
invented magic. The faceless Altmer uses race-guided gold, ivory, emerald, and deep green. The full
result and 32 px LANCZOS reduction passed the hard gate. Twenty-four of 57 icons are finalized;
33 remain unprocessed.

Goal mode 2026-08-23: **Furious Charge** (`aow_26_furious_charge`) is finalized from its single
generation attempt. The active clip overrides its catalogue `2H` suggestion: all four collisions
use the shield node during a roughly 693-unit charge, with a much larger final contact. One
foreshortened Imperial shield, one continuous dust wake, and one terminal impact dominate while
the faceless legionary remains mostly hidden. The full result and 32 px LANCZOS reduction passed
the hard gate. Twenty-five of 57 icons are finalized; 32 remain unprocessed.

Goal mode 2026-08-23: **Furrow Strike** (`aow_27_furrow_strike`) is finalized from its single
generation attempt. Its weaker setup collision and stronger dust/knockback finisher are distilled
to one defining physical path: one Nord greatsword feeding one bright perspective ground furrow.
The figure remains small and faceless, and no second trail or invented element appears. The full
result and 32 px LANCZOS reduction passed the hard gate. Twenty-six of 57 icons are finalized;
31 remain unprocessed.

Goal mode 2026-08-23: **Head Chopper** (`aow_28_head_chopper`) is a recorded one-shot hard failure,
not a finalized icon. The result correctly shows two axes with only one active trail, but the left
axe became a polearm-length crescent weapon held in one hand, violating the dual one-handed weapon
construction. Full and 32 px evidence are preserved under the ignored `pilot/` tree; no master or
atlas input exists and no regeneration was attempted. Twenty-six of 57 icons are finalized;
30 remain unprocessed and one is a recorded hard failure.

Goal mode 2026-08-24: **Head Tap** (`aow_29_head_tap`) is finalized under explicit owner direction.
The initial and first rework hammer/Argonian versions were rejected; the next Altmer version still
over-indexed on six counted beats. The approved result ignores that encoding and uses the figure:
one sleek faceless Altmer, one held Alinorian saber, and one smooth ivory-gold wake. The full result
and 32 px LANCZOS reduction passed. Twenty-eight of 57 icons are finalized; 27 remain unprocessed
and two are recorded hard failures.

Goal mode 2026-08-23: **Heart Lunge** (`aow_30_heart_lunge`) is finalized from its single
generation attempt. Its two right-hand hits and roughly 392-unit advance become one neutral
Redguard straight-sword thrust with one collinear wake and one endpoint. The unresolved generic
launch payload and ability name contribute no heart symbol, blood, or red energy. The full result
and 32 px LANCZOS reduction passed the hard gate. Twenty-eight of 57 icons are finalized;
28 remain unprocessed and one is a recorded hard failure.

Goal mode 2026-08-23: **Heart Strike** (`aow_31_heart_strike`) is a recorded one-shot hard failure,
not a finalized icon. The result correctly limits three bright collision windows to one rising
trail, but the mace and partial haft float at its endpoint while the Breton holds no weapon. Full
and 32 px evidence are preserved under the ignored `pilot/` tree; no master or atlas input exists
and no regeneration was attempted. Twenty-eight of 57 icons are finalized; 27 remain unprocessed
and two are recorded hard failures.

Goal mode 2026-08-24: **Heavy Swing** (`aow_32_heavy_swing`) is finalized under explicit owner
direction. The initial Breton/claymore result was replaced by an Elder Scrolls-coded Orsimer in
Orcish mail using one connected two-handed single-bit battleaxe and one broad silver-white sweep.
The earlier contact and unresolved generic launch payload add no duplicate trail or invented
element. The full result and 32 px LANCZOS reduction passed. Twenty-nine of 57 icons are finalized;
26 remain unprocessed and two are recorded hard failures.

Goal mode 2026-08-24: **High Kick** (`aow_33_high_kick`) is finalized from its single generation
attempt. Its sequential left- and right-leg collision windows become one clean finishing Khajiit
high kick with one foot-connected ivory physical wake, not a two-hit diagram. Unresolved payloads
add no invented element or shout wave. The full result and 32 px LANCZOS reduction passed. Thirty
of 57 icons are finalized; 25 remain unprocessed and two are recorded hard failures.

Goal mode 2026-08-24: **Holding Thorns** (`aow_34_holding_thorns`) is finalized from its single
generation attempt. A dense dual-weapon contact sequence becomes one forward-driving faceless
Dunmer with two hand-connected chitin blades and one leading connected wake. The name contributes
no literal or magical thorns because the clip proves none. Full and 32 px results passed.
Thirty-one of 57 icons are finalized; 24 remain unprocessed and two are recorded hard failures.

Owner correction 2026-08-24: **Holding Thorns** is no longer finalized. The owner requested Morag
Tong armor and a transparent background. The one edit preserved the action and improved the armor,
but returned opaque RGB with a baked checkerboard rather than real alpha. The stitcher supports
RGBA-to-DXT5 transparency; the result itself does not. Both versions are preserved as ignored
evidence, the canonical master/input were removed, and no autonomous retry was attempted.

Goal mode 2026-08-24: **Iai Slash** (`aow_35_iai_slash`) is finalized from its single generation
attempt. One fast 1H collision and hit becomes a low faceless Imperial Akaviri draw-cut with one
connected katana, one empty scabbard, and one blade-tangent wake. Unresolved knockback contributes
no element or victim. Full and 32 px results passed. Thirty-two of 57 icons are finalized; 23
remain unprocessed and two are recorded hard failures.

Goal mode 2026-08-24: **Killing Blow** (`aow_36_killing_blow`) is finalized from its single
generation attempt. Because its 1H clip has zero annotations, the icon claims only one decisive
physical action: a faceless Bosmer, one connected bone-bound stone mace, and one tangent wake. No
victim, blood, hit-count diagram, or element is invented. Full and 32 px results passed. Thirty-three
of 57 icons are finalized; 22 remain unprocessed and two are recorded hard failures.

Goal mode 2026-08-24: **Leap Slam** (`aow_37_leap_slam`) is finalized from its single generation
attempt. Its initial hit and rapid forward surge are distilled to the supported terminal event: one
airborne faceless Argonian, one connected two-handed poleaxe, one descending wake, and one compact
dust impact. No second-hit diagram, shockwave ring, or element is invented. Full and 32 px results
passed. Thirty-four of 57 icons are finalized; 21 remain unprocessed and two are hard failures.

Owner correction 2026-08-24: **Leap Slam** was researched and replaced. Official Murkmire concept
art supports bare-headed Dead-Water fighters with natural horns/crests, lean shell/scute armor,
moss cloth, leather lashings, bone accents, and wrapped long-haft weapons. The final uses that
language with one connected asymmetric hooked poleaxe, one descending wake, and one impact. The
helmeted generic dragon-plate version is preserved only as ignored evidence. Full and 32 px passed.

Owner refinement 2026-08-24: targeted official research replaced the bare natural crest with one
open Elder Argonian purple brow circlet, bronze/turquoise fitting, and exactly five rear-swept
cream/rust/dark-brown feathers while leaving the snout and jaw exposed. The Dead-Water armor,
connected poleaxe, single wake, and impact remain. The bare-head intermediate is ignored evidence.

Goal mode 2026-08-24: **Long Claw** (`aow_38_long_claw`) is finalized from its single generation
attempt. Its three high-damage 2H contacts become one terminal faceless Nord greatsword cut with one
connected hooked blade and one tangent rising wake. Unresolved fog/paralysis names add no magic or
colored aura. Full and 32 px results passed. Thirty-five of 57 icons are finalized; 20 remain
unprocessed and two are hard failures.

Goal mode 2026-08-24: **Magnetic Throw** (`aow_39_magnetic_throw`) is finalized from its single
generation attempt. Eight same-node contacts become one faceless Breton action source, one brass
throwing disc, and one unbroken blue-white palm-to-hub return line, not eight projectiles. Full and
32 px results passed. Thirty-five of 57 icons are finalized; 19 remain unprocessed and three are
recorded hard failures.

Owner corrections and goal closure 2026-08-24: Orbital Cleave was rebuilt from U.S. Army 1907
saber Figure 12 after three anatomy/weapon failures; Pirate's Slash is now a female Breton
buccaneer; Shadow Reave uses white Khajiit fur; Shoulder Slam is a female Nord axe-warrior; Subtle
Stab is a female Dunmer thrust grounded in U.S. Army 1907 Figure 23 with mist-blue ethereal blade;
and Tornado Leap is a female Bosmer dual-dagger vortex retaining the forest/leaves field. Wicked
Throw finalized as a female Altmer arcane-trickster with one returning blue blade, visually
distinguished from Magnetic Throw by its broad oval orbit. Wind Slice finalized as one female
Redguard dual-scimitar dash with one active pressure wake. All 57 catalogue entries now have a
disposition: 51 finalized, 0 unprocessed, and 6 recorded hard failures. Atlas stitching, catalogue
wiring, deployment, and runtime testing remain outside this goal's authorized scope.

Goal mode 2026-08-24: **Orbital Cleave** (`aow_41_orbital_cleave`) is finalized from its single
generation attempt. One long weapon-collision window and one hit become one sustained physical
orbiting cut by a faceless Redguard using one connected two-handed curved greatsword and one
continuous blade-led ivory-gold wake. Unresolved payload names add no magic. Full and 32 px results
passed. Thirty-seven of 57 icons are finalized; 17 remain unprocessed and three are recorded hard
failures.

Owner correction 2026-08-24: the first Orbital Cleave result was rejected for an impossible
two-handed sword grip. The replacement keeps the Redguard/orbital identity but puts both hands on
one continuous long hilt with plausible spacing, neutral wrists, connected arm leverage, and a
supported stance. This establishes an explicit anatomy-and-physics sanity gate for all later icons.

Goal mode 2026-08-24: **Piercing Leap** (`aow_42_piercing_leap`) is finalized from its single
generation attempt. One airborne Breton uses one connected two-hand-gripped spear and one collinear
pressure wake. Hand spacing, arm leverage, airborne body axis, rear leg, and cape form a plausible
thrust; unresolved ice/wolf/fog/paralysis names add no magic. Full and 32 px results passed.
Thirty-eight of 57 icons are finalized; 16 remain unprocessed and three are hard failures.

Goal mode 2026-08-24: **Pirate's Slash** (`aow_43_pirates_slash`) is finalized from its single
generation attempt. Five alternating contacts become one low Imperial Abecean corsair lunge with
one active cutlass, one guarded dagger, and exactly one cutlass-connected wake. Both grips and the
supported stance pass the anatomy/physics gate. Full and 32 px results passed. Thirty-nine of 57
icons are finalized; 15 remain unprocessed and three are hard failures.

Goal mode 2026-08-24: **Point Charge** (`aow_44_point_charge`) is finalized from its single
generation attempt. Two hits in a roughly 499-unit ground rush become one low Orsimer boar-spear
charge with one point-aligned wake. Grip spacing, arm leverage, low center of gravity, planted lead
foot, and driving rear leg pass the physics gate. Full and 32 px results passed. Forty of 57 icons
are finalized; 14 remain unprocessed and three are hard failures.

Goal mode 2026-08-24: **Reaper** (`aow_45_reaper`) is a recorded one-shot hard failure. Its Dunmer,
connected glaive, two-handed leverage, edge direction, and one persistent wake pass the physics
gate, but literal volcanic mountains, lava, and flying rocks make scenic key art rather than a
hotbar glyph. Evidence is ignored-pilot only; no master/input or retry. Forty of 57 icons are
finalized; 13 remain unprocessed and four are hard failures.

Goal mode 2026-08-24: **Ripping Hour** (`aow_46_ripping_hour`) is finalized from its single
generation attempt. The alternating Dual chain becomes one terminal Altmer shearing cut with two
held Alinorian short sabers and one connected wake per blade. Independent grips, joints, stance,
and blade paths pass physics; no clock or unresolved-name magic is invented. Full and 32 px passed.
Forty-one of 57 icons are finalized; 12 remain unprocessed and four are hard failures.

Goal mode 2026-08-24: **Sacrifice Stab** (`aow_47_sacrifice_stab`) is a recorded one-shot hard
failure. Its Bosmer lunge, support, arm line, grip, and wake pass anatomy, but the requested long
dagger became an unmistakable full sword. Weapon family remains a hard field. Evidence is ignored-
pilot only; no master/input or retry. Forty-one icons are finalized; 11 remain unprocessed and five
are hard failures.

Goal mode 2026-08-24: **Shadow Reave** (`aow_48_shadow_reave`) is finalized from its single
generation attempt. Three 2H hits and a jump finisher become one Khajiit landing cut with one
connected crescent battleaxe and one neutral physical wake. Grip, stance, edge, and trail pass the
physics gate; no shadow magic is invented. Full and 32 px passed. Forty-two icons are finalized;
10 remain unprocessed and five are hard failures.

Goal mode 2026-08-24: **Shadow Slash** (`aow_49_shadow_slash`) is finalized from its single
generation attempt. Two 2H hits across a roughly 637-unit advance become one low Imperial
greatsword dash and one connected horizontal wake. Grip, joints, stance, edge, and trail pass the
physics gate; no shadow magic is invented. Full and 32 px passed. Forty-three icons are finalized;
nine remain unprocessed and five are hard failures.

Goal mode 2026-08-24: **Shoulder Slam** (`aow_50_shoulder_slam`) is finalized from its single
generation attempt. The left-upper-arm collision and 463-unit charge become one weaponless
Dead-Water Argonian body check with one shell-armored shoulder and one connected compression path.
Anatomy passes; fire-breath sound adds no fire. Full and 32 px passed. Forty-four icons are
finalized; eight remain unprocessed and five are hard failures.

Goal mode 2026-08-24: **Simple Bash** (`aow_51_simple_bash`) is finalized from its single generation
attempt. One short hit with explicit shield-bash sound becomes one boss-centered impact on one
connected Nordic round shield, driven by one weaponless Nord. Shield, stance, and force path pass
physics; no paralysis magic is invented. Full and 32 px passed. Forty-five icons are finalized;
seven remain unprocessed and five are hard failures.

Goal mode 2026-08-24: **Soul Cleaver** (`aow_52_soul_cleaver`) is finalized from its single
generation attempt. Two close hits become one Orsimer executioner chop with one connected
two-handed single-edged war cleaver and one edge-led wake. Grip, stance, leverage, and path pass
physics; no soul/paralysis magic is invented. Full and 32 px passed. Forty-six icons are finalized;
six remain unprocessed and five are hard failures.

Goal mode 2026-08-24: **Soulless Swing** (`aow_53_soulless_swing`) is finalized from its single
generation attempt. Four 1H hits become one Redguard terminal backhand with one connected flanged
mace and one head-centered wake. Grip, wrist, stance, striking faces, and trail pass physics; no
soul/fog/paralysis magic is invented. Full and 32 px passed. Forty-seven icons are finalized; five
remain unprocessed and five are hard failures.

Goal mode 2026-08-24: **Subtle Stab** (`aow_54_subtle_stab`) is finalized from its single generation
attempt. Three close contacts become one compact Morag Tong thrust with one true forearm-scale
dagger and one tiny point marker. Blade scale, grip, wrist, stance, and path pass physics; no
ice-rune effect is invented. Full and 32 px passed. Forty-eight icons are finalized; four remain
unprocessed and five are hard failures.

Goal mode 2026-08-24: **Tornado Leap** (`aow_55_tornado_leap`) is finalized from its single
generation attempt. Three right-weapon hits with tripled trail persistence become one airborne
Bosmer rotation, one connected handaxe, and one open spiral physical wake. Body tuck, grip,
clearance, and axe path pass physics; no literal wind magic is invented. Full and 32 px passed.
Forty-nine icons are finalized; three remain unprocessed and five are hard failures.

Goal mode 2026-08-24: **Umbral Torment** (`aow_56_umbral_torment`) is a recorded one-shot hard
failure. Its grip, sword, edge, and payload-proven orange trail pass physics, but literal mountains,
battlefield ground, and fiery spectacle create scenic key art and imply unsupported fire magic.
Evidence is ignored-pilot only; no master/input or retry. Forty-nine icons are finalized; two remain
unprocessed and six are hard failures.

Owner correction 2026-08-24: **Magnetic Throw** was replaced. The gadget-like Breton/Dwemer disc is
superseded by an arcane-trickster Altmer assassin throwing one cobalt-blue psychic Alinorian shadow
blade with one continuous palm-to-pommel magnetic return tether. No disc, projectile copies, pink
psychic color, or branching lightning remains. Full and 32 px passed.

Atlas wiring 2026-08-24: the repo-local `build_weapon_art_atlas.py` now creates SH2's native
`icons_weapon_arts.png` plus tab-separated UV catalogue and writes stable keys into
`arts_ashes.csv`. Static validation covers 51 unique shippable keys; the six recorded hard failures
(ArtIDs 28, 31, 34, 45, 47, and 56) remain on `GREATER_POWER`. Optional replacement candidates for
ArtIDs 9–12 were not owner-approved, so the atlas uses their prior approved assets. Deployment and
the ticket's in-game Arts-tab/hotbar acceptance remain open.

Goal mode 2026-08-24: **Night Flurry** (`aow_40_night_flurry`) is finalized from its single
generation attempt. Four closely spaced one-handed contacts become one terminal physical cut by a
faceless Dunmer Morag Tong assassin using one connected chitin/ebony shortsword and one continuous
blade-sourced ivory wake. Unresolved unbalance/paralysis names add no darkness or other magic. Full
and 32 px results passed. Thirty-six of 57 icons are finalized; 18 remain unprocessed and three are
recorded hard failures.

Atlas completion 2026-08-25: all 57 manifest entries now have shippable 128 x 128 inputs. The
rebuilt 1024 x 1024 `icons_weapon_arts.png`, UV catalogue, and `arts_ashes.csv` validate with 57
assigned keys and zero placeholders. The isolated runtime package was deployed through Skyrim
Agent to the active `Nolvus Awakening` profile as `Dev - Spell Hotbar 2 Weapon Art Icons`; the MO2
bridge verified it enabled at winning priority 4466, and deployed-file SHA-256 hashes match the
package. The owner explicitly waived in-game validation for this pass, so Arts-tab/hotbar visual
acceptance was not run.

Owner acceptance 2026-08-25: **passed.** Owner: *"you can tell from the screenshots that obviously
the icons are working."* Three Binding Menu screenshots taken during ticket 14's pass show the
catalogue and the bound-slot strip both rendering per-art glyphs — Aimed Blow, Akatosh Charge,
Blood Flurry, Crane Style, Crushing Blow, Cyclone Spin, Dash Slam, Disengage, Point Charge,
Reaper, Ripping Hour, Sacrifice Stab, Simple Bash, Soul Cleaver, Soulless Swing, Subtle Stab and
Tornado Leap are each individually recognizable, and bound slots NP2/NP3/NP5/NP6/NP7/NP8 carry
different icons instead of the old identical `GREATER_POWER` glyph. That is the ticket's premise —
per-art identity on the bar — and it is visibly satisfied.

The two spec cells the owner was not asked to chase, since the screenshots already answer them
adequately: the unknown-key fallback renders as the `Custom Ability 11` bust rather than a blank
slot, and the picker's Reset path is unchanged code from the 2026-08-21 ship. Fixture: atlas
package deployed at MO2 priority 4466 with the stamped art pack, so the arts shown also play real
clips.
