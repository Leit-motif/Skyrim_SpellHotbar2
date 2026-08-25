# Codex Imagen handoff — silhouette regen

Updated: 2026-08-24

> **Historical completed brief — do not execute.** The required regeneration batch landed in
> `c062446`; later focused corrections landed through `4398e41`. The batch retained 128 x 128
> deliverables rather than new full-resolution masters, as recorded by blank `Master` cells in the
> manifest. Atlas/catalogue wiring was separately authorized afterward. Optional candidates for
> ArtIDs 9–12 were not owner-approved and are not shipped.

Give this file to Codex Imagen. Work in this worktree. Do not regenerate keepers. Rewrite each
required prompt under the new figure contract, then generate a new icon from scratch.

## Where

- Worktree: `C:\Nolvus\Projects\spell-hotbar-2-ticket-06-icons`
- Branch: `ticket-06-weapon-art-icons`
- Integration target: `weapon-arts`, not `main`
- Icons: `python_scripts/weapon_art_icons/icons/`
- Masters: `python_scripts/weapon_art_icons/source/`
- Prompts: `python_scripts/weapon_art_icons/prompts/`
- Manifest: `python_scripts/weapon_art_icons/manifest.tsv`
- Prompter skill: `.agents/skills/skyrim-weapon-art-icon-prompter/SKILL.md`
- Figure contract: `python_scripts/weapon_art_icons/figure-guidance.md`

## Mission

The first eight icons already speak World of Warcraft ability-icon language: a near-black faceless
mass, one saturated effect, a dark colored field. A later session still wrote "faceless" and then
described costume, so most of the atlas became character plates.

Bring the 33 required icons back to Crane Style grammar. Keep ability identity (verb, weapon,
race outline, wake color). Throw away painted fur, faces, gold plate, cloth inventory, and
full-body character framing.

## Style masters — attach as grammar only

Attach these five. Role for each: `approved icon grammar`. Do not copy pose, facing, weapon, race,
or composition.

| File | Why |
| --- | --- |
| `icons/aow_08_crane_style.png` | North star. Black Khajiit mass, one cyan crescent, gold spark. |
| `icons/aow_02_aimed_blow.png` | Faceless lunge + one thrust. |
| `icons/aow_04_blood_flurry.png` | Orc mass + three crimson arcs. |
| `icons/aow_07_champions_end.png` | Small rear mass + gold X. |
| `icons/aow_16_dragon_strike.png` | Effect-first. Figure is a backing mass. |

State in every prompt: these references set abstraction, faceless mass, limited palette, glow, and
32 px readability. They are not pose or costume references.

Do not attach the failed portraits as edit sources. Inpainting a costume plate will not become a
silhouette. Generate new.

## Do not touch

Keepers. Leave source, 128 px, prompt, and manifest row as finalized.

- `aow_08_crane_style` Crane Style
- `aow_02_aimed_blow` Aimed Blow
- `aow_03_akatosh_charge` Akatosh Charge
- `aow_04_blood_flurry` Blood Flurry
- `aow_05_blood_seeker` Blood Seeker
- `aow_06_blood_spiller` Blood Spiller
- `aow_07_champions_end` Champion's End
- `aow_16_dragon_strike` Dragon Strike

Out of scope unless the owner expands the pass: hard failures `aow_28_head_chopper`,
`aow_31_heart_strike`, `aow_34_holding_thorns`, `aow_45_reaper`, `aow_47_sacrifice_stab`,
`aow_56_umbral_torment`.

## Figure contract

Paste this block into every new prompt. Fill the brackets. Do not replace it with a clothing
construction paragraph. "Faceless" and "no portrait" are not enough without this block.

```text
FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, weapon, and
path. Face is a void. Race as outline only: [two or three cues]. No fur, hair, eyes, cloth folds,
rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and
weapon path are the subject.
```

Race outline cues live in `figure-guidance.md`. Race color lives in the field, the wake, and at
most one sash wedge.

Physics still has to be possible: supported center of gravity, plausible joints, hand masses on a
connected haft, trail starting on the moving edge. Prove that with pose, not with painted gauntlets.

Prompt order: asset and 32 px use → frozen action and camera → FIGURE MASS block → weapon as one
connected shape → one causal wake → colored atmospheric field → short constraint list.

Ban these phrases in new prompts: construction, cuirass, filigree, tabard, visible grip,
full-body character, fur rendering.

## Workflow

1. Load the prompter skill and `figure-guidance.md`.
2. Calibration shot: rewrite and generate **High Kick** (`aow_33_high_kick`) first. Stop and wait
   for owner review before the rest.
3. For each required icon: read the existing prompt only for verb, weapon, assigned race, wake
   color, and frozen action. Discard the costume paragraph. Write a new prompt. Generate new.
4. Before accepting: inspect full resolution and a 32 px reduction. Self-reject faces, fur
   painting, readable armor inventory, figure filling most of the square, or a 32 px read that
   names the costume before the verb.
5. On accept: move the old master and 128 px into `pilot/silhouette-drift/` (create the folder).
   Write the new master to `source/`, LANCZOS 128 px to `icons/`, replace the generation prompt in
   `prompts/`, set manifest status to `silhouette_regen`, and note `silhouette regen 2026-08-24
   pending owner review`.
6. Do not wire catalogue CSV or atlas keys in this pass.

## Calibration prompt — High Kick

Existing identity to keep: Khajiit, one high roundhouse, one foot-connected ivory crescent, plum
field. Existing failure: tiger-fur portrait in a tunic.

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed to remain legible at 32x32

Primary request: High Kick. A generalized Khajiit martial-artist silhouette completing one explosive high roundhouse. Planted left leg bent, hips rotated, torso counterleaning, right leg extended to the upper-right as the leading point. One ivory-white pressure crescent tangent to the extended foot, one compact warm-gold impact flare at the toes.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, kicking legs, and path. Face is a void. Race as outline only: pointed ears, short muzzle, tail completing the kick arc. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Style/medium: painted MMO ability icon; symbolic and atmospheric, not a portrait and not character key art
Composition/framing: compact square crop; kicking leg and crescent dominate; one readable diagonal; simple large masses; clear at 32x32
Scene/backdrop: dark plum-black atmospheric field with subtle radial glow; no scenery
Color palette: deep plum-black, ivory-white kick crescent, restrained warm gold impact
Detail budget: very low; broad painted shapes and glow
Orientation: rising diagonal, lower-left to upper-right
Constraints: original art only; no text; no logo; no watermark; no UI border; no metallic frame
```

Attach the five grammar masters. High Kick must sit next to Crane Style without looking like a
different game.

## Required regen (33)

Keep assigned race as outline cues. Keep weapon family and wake color. Recrop to torso / weapon /
path.

| Key | Name | Verb | Race outline | Weapon | Wake / field | Current failure |
| --- | --- | --- | --- | --- | --- | --- |
| `aow_14_divine_smite` | Divine Smite | Downward blessed cut | Breton closed helm | Two-handed sword | Gold blade + compact holy burst; no costume halo | Painted knight with halo |
| `aow_15_double_slash` | Double Slash | One dominant slash | Breton bascinet mass | One sword | One ivory-blue slash owns the square | Silver knight with tabard |
| `aow_17_dual_flurry` | Dual Flurry | Cross cut | Nord broad mass | Two one-handed axes | Two red blade trails forming one X; no hair or eyes | Barbarian portrait |
| `aow_18_earth_shatter` | Earth Shatter | Overhead slam | Orsimer angular helm | Two-handed warhammer | White compression flare, 3–5 large shards | Full-body hammer illustration |
| `aow_19_eldritch_beam` | Eldritch Beam | Horizontal beam | Neutral battlemage hood | Bare casting hand | Beam owns ~70%; white-green core, compact terminus | Robed character plate |
| `aow_23_flurry_strike` | Flurry Strike | Low finishing sweep | Orsimer brute mass | Two-handed battleaxe | One axe-connected white ground sweep | Full-body axe portrait |
| `aow_25_focused_strike` | Focused Strike | Final descending lunge | Altmer pointed-ear hood | One elven sword | One ivory descending slash; figure tiny | Gold-armored knight filling frame |
| `aow_26_furious_charge` | Furious Charge | Shield rush | Imperial closed helm, mostly hidden | One heavy shield | Shield face + ivory compression; no eagle crest | Heraldry plate |
| `aow_29_head_tap` | Head Tap | One saber cut | Altmer pointed-ear hood | One Alinor saber | One smooth ivory-gold wake; do not encode six hits | Gold plate + cape character |
| `aow_30_heart_lunge` | Heart Lunge | Thrust | Redguard wrapped head | One straight sword | One white thrust line, tiny spark | Cinematic hooded fighter |
| `aow_32_heavy_swing` | Heavy Swing | Heavy sweep | Orsimer angular helm | Two-handed battleaxe | One heavy sweep wake | Riveted plate study |
| `aow_33_high_kick` | High Kick | High roundhouse | Khajiit ears, muzzle, tail | Unarmed | Ivory foot crescent | Fur portrait of Crane Style |
| `aow_35_iai_slash` | Iai Slash | Draw-cut lunge | Imperial Blades helm | Katana + empty scabbard cue | One gold wake | Full-body swordsman |
| `aow_36_killing_blow` | Killing Blow | Finishing mace blow | Bosmer small hood | One stone mace | Compact orange impact | Spiked leather character |
| `aow_37_leap_slam` | Leap Slam | Descending poleaxe slam | Argonian snout + crest/tail outline only | Hooked poleaxe | Descending wake + compact impact; no creature portrait | Feathered Argonian painting |
| `aow_38_long_claw` | Long Claw | Rising hooked cut | Nord horned helm mass | Hooked greatsword | One rising wake | Costume Nord |
| `aow_39_magnetic_throw` | Magnetic Throw | Returning thrown blade | Altmer pointed-ear hood | One Alinor blade in flight | Bright blue palm-to-blade tether; crop to hand + loop | Dark assassin lost in the field |
| `aow_40_night_flurry` | Night Flurry | Terminal cut | Dunmer close hood | One chitin/ebony shortsword | One continuous physical wake; no darkness magic | Copper-plate assassin |
| `aow_41_orbital_cleave` | Orbital Cleave | Horizontal orbiting cut | Redguard veil/hood | One-handed saber | One circular blade wake; face is a void | Visible face under hood |
| `aow_42_piercing_leap` | Piercing Leap | Airborne spear thrust | Breton closed helm | Two-handed spear | One collinear wake; crop to weapon axis | Full-body spear illustration |
| `aow_43_pirates_slash` | Pirate's Slash | Cutlass lunge | Hooded void-face; Breton/corsair mass | Cutlass + close dagger cue | One cutlass wake only | Eyes and hair visible |
| `aow_44_point_charge` | Point Charge | Grounded spear charge | Orsimer angular helm | Two-handed boar spear | Bright point + rim-lit charging mass | Dark plate on dark field |
| `aow_46_ripping_hour` | Ripping Hour | Two-blade shear | Altmer pointed-ear hood | Two short sabers | One wake per blade, converging once | Gold-filigree armor plate |
| `aow_48_shadow_reave` | Shadow Reave | Terminal two-handed axe cut | Khajiit ear + tail outline | Two-handed battleaxe | One physical wake; no fur painting, no shadow magic | White-fur Khajiit portrait |
| `aow_49_shadow_slash` | Shadow Slash | Long low dash-cut | Imperial closed helm | Two-handed greatsword | One long low wake; no shadow magic | Costume Imperial |
| `aow_50_shoulder_slam` | Shoulder Slam | Left-shoulder body check | Female Nord helm mass | Compact bearded axe trailing, no axe wake | Shoulder-connected compression wedge | Huscarl portrait |
| `aow_51_simple_bash` | Simple Bash | Shield boss hit | Nord nasal helm, tiny behind shield | Round shield owns the square | Ivory burst on boss | Knee-up knight portrait |
| `aow_52_soul_cleaver` | Soul Cleaver | Heavy chop | Orsimer angular helm | Single-edged war cleaver | One wide chop wake; no soul VFX | Weathered plate study |
| `aow_53_soulless_swing` | Soulless Swing | Backhand mace | Redguard wrapped head | One flanged mace | Compact mace-head wake; no cloth inventory | Turban and sash outfit plate |
| `aow_54_subtle_stab` | Subtle Stab | Close underhand stab | Dunmer close hood | Dagger shorter than forearm | Tiny pale point; crop to dagger | Visible ear/face, dark-on-dark |
| `aow_55_tornado_leap` | Tornado Leap | Airborne spin | Bosmer small hood | Two short daggers | Two blade wakes forming one spiral; no leaves | Elf splash with foliage |
| `aow_57_wicked_throw` | Wicked Throw | Thrown returning blade | Altmer pointed-ear hood | One flying Alinor blade | Pale-blue out-and-return loop; void face | Blonde elf portrait |
| `aow_58_wind_slice` | Wind Slice | Passing dual-scimitar dash | Redguard wrapped head | Two scimitars | One pale pressure wake on the active blade | White tunic costume first |

Owner notes that override generic "no more Argonians" for this pass:

- Leap Slam stays Argonian as snout/crest/tail outline. Do not paint a creature.
- Shoulder Slam stays female Nord with a trailing compact bearded axe and a left-shoulder impact.
- Head Tap stays Altmer saber with one wake. "Use the figure" meant drop the six-hit diagram, not
  paint a character plate.
- Magnetic Throw and Wicked Throw stay Altmer plus a returning Alinor blade.
- Tornado Leap ships as two daggers, not the superseded handaxe.

## Optional light pass (10) — after required regen, only if asked

These already have silhouette intent. Do not full-regen unless a contrast edit fails. Rim-light
the mass in the effect color; do not add costume.

`aow_09_crushing_blow`, `aow_10_cyclone_spin`, `aow_11_dash_slam`, `aow_12_disengage`,
`aow_13_divided_strike`, `aow_20_elegant_slash`, `aow_21_enrage_f`, `aow_22_enrage_m`,
`aow_24_focused_cross`, `aow_27_furrow_strike`.

## Done when

- High Kick exists as a Crane Style sibling and the owner has accepted it.
- All 33 required icons have new prompts containing the FIGURE MASS block, new masters, and new
  128 px inputs.
- Old costume plates live under `pilot/silhouette-drift/`.
- Keepers are unchanged.
- 32 px reductions name the verb first.
- Catalogue and atlas wiring still have not been started.
