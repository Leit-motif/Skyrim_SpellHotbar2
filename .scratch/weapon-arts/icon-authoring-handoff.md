# Weapon Art icon authoring handoff

Updated: 2026-08-23

## Resume here

- Worktree: `C:\Nolvus\Projects\spell-hotbar-2-ticket-06-icons`
- Branch: `ticket-06-weapon-art-icons`
- Integration target: `weapon-arts`, not `main`
- Active art: **47 — Sacrifice Stab** (`aow_47_sacrifice_stab`)
- Status: **not started**
- Inventory: **41 of 57 finalized; 12 unprocessed and 4 recorded hard failures remain**

Ongoing owner direction: deliberately vary race/archetype, race-guided palette, and weapon family
across the remaining atlas. Use `figure-guidance.md` and `color-guidance.md`; include supported
Orsimer, Altmer, Dunmer, axes, maces, and other compatible silhouettes instead of defaulting to a
neutral sword fighter. Animation and payload evidence still win, and race never invents an element.

Mandatory owner physics gate from 2026-08-24: before finalizing any generation, verify that anatomy
and physics are possible. Check supported center of gravity, joint ranges, hand placement and grip
continuity, wrist/elbow/shoulder force path, weapon leverage and counterbalance, striking-edge
direction, and trail causality. A polished image with an impossible grip or action is a hard failure.

Do not restart the set and do not regenerate Crane Style through Disengage. Their approved assets,
prompts, and notes are already recorded in `python_scripts/weapon_art_icons/manifest.tsv`.

## Divided Strike disposition

The owner directed that `divided_strike_cross_slash_v8.png` ship under time constraint on
2026-08-23. It is usable but explicitly not the visual result the owner wanted. The approved master,
128 px atlas input, prompt, and this compromise are recorded under `python_scripts/weapon_art_icons/`.

## Divine Smite disposition

The owner approved the attached original two-handed-grip Divine Smite render on 2026-08-23, not
the later one-handed correction. The exact attachment is the approved master. Its 128 px atlas
input, prompt, animation/effect evidence, and catalogue-class mismatch note are recorded under
`python_scripts/weapon_art_icons/`.

## Double Slash disposition

The owner approved `double_slash_breton_knight_candidate_v5.png` on 2026-08-23. The final
effect-first MMO glyph intentionally retains one dominant slash despite two swing events and the
ability name. Its source figure is a small French-medieval knight-coded Breton rather than a
portrait or character illustration. The approved master, 128 px atlas input, prompt, and evidence
are recorded under `python_scripts/weapon_art_icons/`.

## Dragon Strike disposition

The owner approved `dragon_strike_candidate_v1.png` on 2026-08-23. The effect-first glyph uses a
frontal depth-axis composition: one white-hot flaming sword and its foreground impact dominate the
square, with a small faceless warrior behind them. Symmetrical flame curls suggest dragon jaws or
swept horns without depicting a literal dragon. The approved master, 128 px atlas input, prompt,
animation evidence, and Flaming Strike payload interpretation are recorded under
`python_scripts/weapon_art_icons/`.

## Earth Shatter disposition

Goal-mode one-shot finalized on 2026-08-23. The verified 2.533333-second clip raises both arms,
advances about 101 units, and lands one downward hit at 0.916666 seconds without an elemental
payload. The final glyph uses a vertical Orsimer warhammer slam with one hammer-sourced white
compression flare, ochre-gray dust, and stone fragments. The full image and its 32 px LANCZOS
reduction passed the hard gate. The master, 128 px atlas input, prompt, provider hash, evidence,
and model-decision provenance are recorded under `python_scripts/weapon_art_icons/`.

## Eldritch Beam disposition

Goal-mode one-shot finalized on 2026-08-23. The verified payload stages three caster effects and
then fires a true 2000-unit beam from the right magic node at 1.666600 seconds. Live records and
winning NIF structure prove a green-lit laser/flare/cloud/strip beam with endpoint sparkles plus a
secondary fire-linked cone and explosion. The final horizontal glyph keeps one small braced caster,
one continuous white-green beam, and one compact terminal flare; the warm fringe remains confined
to that payload-proven endpoint. The full image and 32 px LANCZOS reduction passed the hard gate.
The master, 128 px atlas input, prompt, hashes, and evidence are recorded under
`python_scripts/weapon_art_icons/`.

## Elegant Slash disposition

Goal-mode one-shot finalized on 2026-08-23. The verified clip is a single-weapon, roughly 421-unit
forward burst with one enlarged collision and one terminal hit. Its `$ES_Paralysis` definition was
unresolved, so the final glyph adds no paralysis element or invented magic. One broad ivory-white
rising blade path remains the icon; a tiny faceless lunging duelist and one sword explain its
source. The full image and 32 px LANCZOS reduction passed the hard gate. The master, 128 px atlas
input, prompt, hash, and evidence are recorded under `python_scripts/weapon_art_icons/`.

## Enrage (F) disposition

Goal-mode one-shot finalized on 2026-08-23. The verified clip contains one swing and one hit and
explicitly sets a deep-crimson weapon trail at 2.5 times intensity. The open crimson crescent is
the ability glyph; the faceless female fighter is only a small black source marker, with no face or
costume showcase. The full image and 32 px LANCZOS reduction passed the hard gate. The master,
128 px atlas input, prompt, hash, and evidence are recorded under `python_scripts/weapon_art_icons/`.

## Enrage (M) disposition

Goal-mode one-shot finalized on 2026-08-23. This is not a male reskin of Enrage (F): the verified
five-second clip has no weapon and stages a blast, shout, heavy camera shake, and final self effect.
Because all `$FZ*` definitions are unresolved, the glyph uses neutral physical force rather than an
invented element. Three broken ivory-white pressure bands dominate above a tiny faceless weaponless
figure. The full image and 32 px LANCZOS reduction passed the hard gate. The master, 128 px atlas
input, prompt, hash, and evidence are recorded under `python_scripts/weapon_art_icons/`.

## Flurry Strike disposition

The single autonomous generation failed the hard gate on 2026-08-23. The owner then explicitly
requested one narrow correction: delete the three upper trail echoes and retain only the low sweep
physically attached to the axe head. That edit is final. The full image and 32 px LANCZOS reduction
pass under the owner's disposition; the original failure evidence remains in ignored `pilot/` and
the initial prompt, failure, correction prompt, provenance, and hashes remain auditable in
`prompts/aow_23_flurry_strike.md`.

## Focused Cross disposition

The single autonomous generation failed the hard gate on 2026-08-23 because a detached mace rode
the upper arc. The owner explicitly requested one narrow correction removing that floating weapon.
That edit is final. The effect-first dash/swing intersection remains readable at 32 px with one
held weapon and no detached mace. Original failure evidence remains in ignored `pilot/`; the
initial prompt, failure, correction prompt, provenance, and hashes remain auditable in
`prompts/aow_24_focused_cross.md`.

## Focused Strike disposition

Goal-mode one-shot finalized on 2026-08-23. The verified 1H clip advances about 246 units and
contains three timed hits, with unresolved `$HitFog` and `$ES_Paralysis` names on the final beat.
The icon deliberately depicts one defining final strike rather than multiplying those hits into
echo trails: one ivory-white descending diagonal remains physically attached to one Altmer elven
sword. The full image and 32 px LANCZOS reduction passed the hard gate. The master, 128 px atlas
input, prompt, provider hash, evidence, and model-decision provenance are recorded under
`python_scripts/weapon_art_icons/`.

## Furious Charge disposition

Goal-mode one-shot finalized on 2026-08-23. Despite catalogue ArtClass `2H`, the verified active
clip advances about 693 units and opens every collision on the `SHIELD` node, ending with a
four-times-scale full-damage shield contact. The icon therefore uses one foreshortened Imperial
heavy shield, one continuous physical charge wake, and one terminal rim impact with the legionary
mostly hidden. The full image and 32 px LANCZOS reduction passed the hard gate. The master, 128 px
atlas input, prompt, provider hash, evidence, and mismatch note are recorded under
`python_scripts/weapon_art_icons/`.

## Furrow Strike disposition

Goal-mode one-shot finalized on 2026-08-23. The verified 2H clip advances about 208 units, opens a
weaker setup collision, then lands one 1.5-damage final collision with extended bright trail,
`$AoW_Dust`, `$AoW_Knockback`, and camera shake. The named payload definitions remain unresolved,
so the icon uses only physical soil, stone, and steel: one small Nord, one two-handed greatsword,
and one perspective ground furrow. The full image and 32 px LANCZOS reduction passed the hard gate.
The master, 128 px atlas input, prompt, provider hash, evidence, and payload uncertainty are recorded
under `python_scripts/weapon_art_icons/`.

## Head Chopper exception

The single goal-mode generation attempt failed the hard gate on 2026-08-23. It correctly limited
the dual-wield sequence to two axes and one visible left-hand trail, but the active axe became a
polearm-length crescent weapon held in one hand. It is not finalized and has no master or atlas
input. Full and 32 px evidence remain in the ignored `pilot/` tree; the evidence, final prompt,
hash, and failure reasons are recorded in `prompts/aow_28_head_chopper.md`. No regeneration was
attempted.

## Head Tap disposition

The owner rejected the initial single-impact hammer, then rejected the hammer/Argonian direction
after its six-beat rework detached the terminal weapon. A subsequent Altmer/Alinorian saber version
was mechanically clean but still over-indexed on six counted beats. Final owner direction was to
ignore the six-hit encoding and use the figure. The approved icon therefore uses one sleek faceless
Altmer, one connected Alinorian saber, and one smooth ivory-gold wake. Full and 32 px results passed.
All superseded versions remain auditable under ignored `pilot/`; prompts, hashes, dispositions, and
the final master are recorded in `prompts/aow_29_head_tap.md`.

## Heart Lunge disposition

Goal-mode one-shot finalized on 2026-08-23. The verified right-hand clip advances about 392 units,
lands two hits, and carries an unresolved generic strike-launch payload on the second. Neither the
name nor payload proves blood or literal heart imagery, so the icon uses one neutral physical
Redguard straight-sword thrust, one collinear wake, and one endpoint. The full image and 32 px
LANCZOS reduction passed the hard gate. The master, 128 px atlas input, prompt, provider hash,
evidence, and negative inference are recorded under `python_scripts/weapon_art_icons/`.

## Heart Strike exception

The single goal-mode generation attempt failed the hard gate on 2026-08-23. It correctly used one
bright trail rather than tripling the clip's hits, but the mace and partial haft float at the far
endpoint while the Breton's hands hold no weapon. It is not finalized and has no master or atlas
input. Full and 32 px evidence remain in the ignored `pilot/` tree; the evidence, final prompt,
hash, and failure reasons are recorded in `prompts/aow_31_heart_strike.md`. No regeneration was
attempted.

## Heavy Swing disposition

Finalized under explicit owner direction on 2026-08-24. The verified 2H clip advances about 183
units and has one clear late weapon swing/hit after an earlier contact; its generic launch payload
is unresolved. The initial Breton/claymore result was superseded. The approved icon uses one
closed-helm Orsimer in Elder Scrolls-coded Orcish mail, one connected two-handed single-bit
battleaxe, and one broad silver-white sweep with no duplicate trail or invented element. Full and
32 px results passed. The master, atlas input, prompt, provider hash, provenance, and superseded
result are recorded under `python_scripts/weapon_art_icons/`.

## High Kick disposition

Goal-mode one-shot finalized on 2026-08-24. The verified Generic clip advances about 363 units and
opens sequential left- and right-leg collision windows, but its named payloads do not prove a
visual element. The icon uses one faceless Khajiit finishing one high right-leg kick with one
foot-connected ivory pressure wake. The two collision windows are not diagrammed. Full and 32 px
results passed. The master, atlas input, prompt, provider hash, evidence, and uncertainty are
recorded under `python_scripts/weapon_art_icons/`.

## Holding Thorns disposition

Owner-directed edit is a recorded hard failure on 2026-08-24. The initial Bonemold/chitin Dunmer
result passed mechanically, then the owner requested unmistakable Morag Tong armor and a transparent
background. The single edit delivered the requested armor and retained two connected blades plus
one leading wake, but baked a checkerboard into an opaque RGB image with no alpha channel. The atlas
pipeline itself does support transparency through RGBA and DXT5; this provider result does not.
Neither version is canonical. Both are preserved under the ignored `pilot/` tree, and no autonomous
retry was attempted.

## Iai Slash disposition

Goal-mode one-shot finalized on 2026-08-24. The verified 1H clip pauses, accelerates to about 285
forward units, and lands one enlarged/brightened weapon hit with unresolved knockback. The icon uses
one faceless Imperial in Elder Scrolls Akaviri/Blades armor, one connected katana, one empty
scabbard, and one smooth blade-connected draw wake. Full and 32 px results passed; provenance and
payload uncertainty are recorded under `python_scripts/weapon_art_icons/`.

## Killing Blow disposition

Goal-mode one-shot finalized on 2026-08-24. The selected 1H clip has zero annotations, so neither a
hit pattern nor an element is claimed. The icon uses one faceless Bosmer delivering one decisive
downward blow with one connected bone-bound stone mace and one tangent physical wake. No victim,
blood, or magical death effect is invented. Full and 32 px results passed; the evidence boundary is
recorded under `python_scripts/weapon_art_icons/`.

## Leap Slam disposition

Finalized under owner-directed researched correction on 2026-08-24. The verified 2H clip lands an
initial hit, then surges about 305 units into a terminal hit with AoE knockback, dust, and shake.
The initial helmeted generic-plate Argonian and a bare-head intermediate were superseded. The
approved icon uses official Murkmire/Dead-Water armor and Elder Argonian headwear language: exposed
snout/jaw beneath an open purple brow circlet with bronze/turquoise fitting and five rear-swept
feathers, lean slate shell/scute armor, moss cloth, leather lashings, pale bone accents, and one
wrapped-marshwood hooked poleaxe. The action retains one descending wake and one compact impact.
Full and 32 px passed; research and provenance are recorded under `.scratch/weapon-arts/research/`
and `python_scripts/weapon_art_icons/`.

## Long Claw disposition

Goal-mode one-shot finalized on 2026-08-24. The verified 2H clip advances about 334 units and makes
three high-damage contacts, ending with unresolved heavy-paralysis/smash-fog names and shake. The
icon distills that sequence to one faceless Nord, one connected long hooked greatsword, and one
tangent rising wake. No three-hit diagram, fog magic, or paralysis color is invented. Full and 32
px results passed.

## Magnetic Throw disposition

Finalized under owner-directed replacement on 2026-08-24. The Generic clip makes one early contact,
retreats about 68 units, then rapidly contacts seven times on the same weapon node. The rejected
Breton/Dwemer disc was replaced by one faceless Altmer arcane assassin, one cobalt-blue psychic
Alinorian shadow blade, and one continuous magnetic return tether from open palm to blade pommel.
No projectile copies, gadget disc, pink psychic color, or lightning web remains. Full and 32 px
results passed.

## Night Flurry disposition

Goal-mode one-shot finalized on 2026-08-24. The verified one-handed clip advances only about 60
units and makes four closely spaced weapon contacts. The icon distills those beats to one terminal
physical cut: one faceless Dunmer Morag Tong assassin, one connected chitin/ebony shortsword, and
one continuous ivory-gray blade wake. Unresolved unbalance/paralysis names add no darkness,
paralysis color, or other magic. Full and 32 px results passed.

## Orbital Cleave disposition

Goal-mode one-shot finalized on 2026-08-24. One long WEAPON collision window and one terminal hit
become one sustained physical orbiting cut: one faceless Redguard, one connected two-handed curved
greatsword, and one continuous blade-led ivory-gold wake. Unresolved endurance, paralysis, and
enchantment names add no magic. Full and 32 px results passed.

Owner correction: the first otherwise-polished result had an impossible two-handed sword grip and
was superseded. The corrected master places both closed hands on one continuous long hilt with
plausible spacing, neutral wrists, connected forearms, supported stance, and a blade-led wake. This
correction established the mandatory anatomy-and-physics gate for every subsequent generation.

## Piercing Leap disposition

Goal-mode one-shot finalized on 2026-08-24. The forward airborne clip and two near-simultaneous hit
frames become one piercing action: one faceless Breton polearm knight, one connected two-hand-gripped
spear, and one collinear pressure wake. The shoulders, elbows, airborne body axis, rear leg, and cape
plausibly counterbalance the point. Unresolved ice/wolf/fog/paralysis names add no magic. Full and
32 px results passed.

## Pirate's Slash disposition

Goal-mode one-shot finalized on 2026-08-24. Five alternating dual-weapon contacts become one low
Imperial Abecean corsair lunge with one active cutlass wake and one held no-trail parrying dagger.
Both grips, arms, hips, and planted legs form a plausible action; the cutlass edge leads its single
wake. Full and 32 px results passed.

## Point Charge disposition

Goal-mode one-shot finalized on 2026-08-24. A roughly 499-unit ground rush and two hits become one
low Orsimer boar-spear charge with one point-aligned pressure wake. Wide hand spacing, neutral
wrists, low hips, planted lead foot, and driving rear leg pass the physics gate. Full and 32 px
results passed.

## Reaper disposition

Recorded one-shot hard failure on 2026-08-24. The Dunmer, connected glaive, two-handed leverage,
edge direction, and one persistent wake are mechanically coherent, but the model turned the quiet
volcanic color field into literal mountains, lava, and a scenic panorama. That key-art framing
violates the icon brief. Evidence is preserved under ignored `pilot/`; no master/input or retry.

## Ripping Hour disposition

Goal-mode one-shot finalized on 2026-08-24. The long alternating Dual chain becomes one terminal
Altmer shearing cut with two independently held Alinorian short sabers and exactly one short wake
per blade. The grips, joints, stance, and converging paths pass physics; unresolved storm/fog/time/
paralysis implications add no magic. Full and 32 px results passed.

## Dual Flurry context-layer pivot

Final disposition: the owner supplied and approved an externally generated Gemini final cut on
2026-08-23 after the Codex generation attempts repeatedly failed to keep the axe heads, cutting
directions, and energy trails physically coherent. The exact uploaded PNG is canonical. It shows a
leaping blond Nord barbarian whose two inward-facing axes generate the crossed crimson trails along
their curved lines of motion. The approved master, 128 px atlas input, provenance record, and
animation evidence are stored under `python_scripts/weapon_art_icons/`.

The prompt-layer history below remains useful as a record of the failure and the context work, but
its draft is superseded and must not be used to regenerate or replace the approved final cut.

Several unapproved candidates exposed a prompt-authoring problem rather than an image-backend
problem: race labels were treated as costumes, prior rejected candidates carried visual content
forward, effect geometry overruled body action, and generic fantasy armor displaced the requested
Skyrim archetype. The owner paused generation on 2026-08-23 to build a durable context layer.

Canonical context now begins at:

- `python_scripts/weapon_art_icons/skyrim-visual-language.md`
- `python_scripts/weapon_art_icons/prompt-brief-template.md`
- `.agents/skills/skyrim-weapon-art-icon-prompter/SKILL.md`

The superseded prompt-only forward test is
`.scratch/weapon-arts/prompt-drafts/aow_17_dual_flurry-context-v1.md`. It resolves Dual Flurry as a
leaping Nord barbarian in rough fur and hide, using two compact one-handed axes to physically create
two red trails that cross once. Do not use it to replace the approved owner-supplied final.

## Earlier Divided Strike stopping point

The animation evidence supports a three-beat dual-weapon sequence: off-hand collision first,
main-hand collision second, then both weapon nodes collide in the terminal beat. The intended icon
is a figure visibly moving through that attack and ending in a cross-cutting X strike. The pose must
create the attack; luminous trails cannot be pasted over an idle or weapon-display stance.

The closest candidate before pausing is locally preserved at:

`python_scripts/weapon_art_icons/pilot/source/divided_strike_closest_v5.png`

Owner assessment: “better,” with the advancing motion accepted as progress, but the X still needed
to be **convex**, and the dagger might need to change to make the physical trajectory coherent.

The most recent candidate is locally preserved at:

`python_scripts/weapon_art_icons/pilot/source/divided_strike_rejected_convex_v6.png`

That candidate is explicitly **wrong**. It interpreted convexity as two large continuous opposing
C-curves, producing an hourglass/parenthesis shape rather than the owner's intended convex X. Do
not continue from its trail geometry and do not call it approved.

Before another generation, establish the intended convex-X geometry with the owner in plain shape
language or a quick owner-supplied sketch/reference. Preserve the moving lunge from the closest v5
candidate, then change the weapon and trail geometry together. Do not guess at “convex” again.

## Locked project direction

- Original generated art only; no third-party artwork or Bethesda assets are copied or shipped.
- MMO/RPG hotbar convention: generalized faceless figure, one dominant action, readable at 32 px.
- Elder Scrolls race and palette shorthand live in `figure-guidance.md` and `color-guidance.md`.
- Backgrounds should vary across the atlas without defaulting every icon to black or adding haze.
- Figure facing and action axes must rotate across the atlas; every new prompt names an explicit
  orientation and avoids repeating the recent upper-left-to-lower-right default unless the clip
  requires it. See `composition-guidance.md`.
- Catalogue wiring waits until the atlas contains the approved stable keys.
