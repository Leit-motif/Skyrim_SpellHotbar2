# Skyrim Weapon Art Icon Prompter forward test: Dual Flurry

Date: 2026-08-23

## Scope and verdict

Prompt-only forward test of `.agents/skills/skyrim-weapon-art-icon-prompter/SKILL.md`. No image was
generated, and `.scratch/weapon-arts/prompt-drafts/aow_17_dual_flurry-context-v1.md` was not read.

**Verdict: pass, with small evidence-routing gaps.** The skill and its required project context give
enough direction to write a materially safer prompt against the named failures: plate armor, a
static crossed-axes emblem, unrelated elemental VFX, incorrect weapon count or scale, and
portrait/key-art drift. A prompt cannot guarantee backend compliance, so the skill's post-generation
self-rejection and 32 px inspection remain necessary before any output could be accepted.

## Evidence inspected

- Manifest row: ArtID 17, `Dual Flurry`, stable key `aow_17_dual_flurry`, class `Dual`; it records
  two simultaneous paired hits, the second with knockback, and roughly 284 units of travel.
- Spell Hotbar OAR overlay:
  `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Dev - Spell Hotbar 2\meshes\actors\character\animations\OpenAnimationReplacer\SpellHotbar2Arts\Dual Flurry\config.json`.
  Its `overrideAnimationsFolder` resolves to
  `../Nolvus Ashes of War Stance Framework/Dual Flurry`.
- Installed HKX:
  `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Ashes of War - Weapon Art Via Additional Attack\meshes\actors\character\animations\OpenAnimationReplacer\Nolvus Ashes of War Stance Framework\Dual Flurry\AABL_Attack_A.hkx`
  (14,704 bytes; SHA-256
  `6BEC732875773F40770EC7AC70F0795BDF641947FF2B189D4431B875FE75CBD2`). Enabled duplicate
  providers inspected have the same hash, so provider precedence does not alter this evidence.
- `hkxc-anno-cli dump` reports a 1.666667-second clip with 40 annotations. Both hands swing at
  0.500000 and hit simultaneously at 0.583333; both swing again at 0.833333 and hit simultaneously
  at 0.883333. The second paired hit carries `HitFrame.$AoW_Knockback`. Collision is attached to
  `WEAPON` and `SHIELD` nodes, which are the main-hand and off-hand weapon paths here. Root-motion
  annotations advance from 0 to 158.837 by 0.5 seconds, 231.85 by 1.0 second, and 283.6 by 1.5
  seconds. No fire, frost, shock, or other elemental payload appears in the clip.
- Last three finalized icons: Divine Smite uses an upper-left-to-lower-right diagonal, Double Slash
  uses right-to-left effect-first action, and Dragon Strike uses frontal depth-axis foreshortening.
  The prompt below therefore uses an oblique three-quarter airborne composition whose primary read
  is the single crossed attack rather than another isolated diagonal or depth-axis thrust.

## Resulting generation prompt

```text
Use case: stylized-concept
Asset type: one original square MMO/RPG hotbar ability glyph for Skyrim Spell Hotbar 2, designed first for clear 32x32 readability.

Primary request: Dual Flurry — a leaping Nord barbarian physically completing one simultaneous cross-cut with two one-handed axes.

Animation evidence: the installed 1.666667-second dual-wield clip surges forward roughly 284 units and contains two simultaneous paired-weapon hits; the second paired hit has knockback. Freeze the decisive second paired hit and distill the two-hit flurry into one clean, forceful crossed impact.

Orientation and camera: oblique three-quarter frontal view. The barbarian leaps from lower left toward upper right, but the dominant icon shape is one compact red X formed immediately in front of the torso by the two active axe paths. Keep the entire action safely inset inside the square.

Body mechanics: both feet are visibly off the ground; knees trail behind the forward-driving hips; the torso leans into the target; shoulders rotate inward and both arms are extended through the strike. The figure is attacking, not holding a pose. The right hand drives one axe head down across the body from upper right toward lower left. The left hand drives the other axe head up across the body from lower right toward upper left. The two paths intersect once at the impact point ahead of the chest, and the two visible axe heads finish at different endpoints on opposite arms of the X.

Figure and equipment: one compact, powerful, faceless Nord barbarian silhouette in rough layered animal fur and hide, coarse weathered leather straps, wrist wraps, and lightly covered or exposed arms. Use rugged weathered brown, charcoal, and iron gray. Exactly two compact one-handed Nordic hand axes, one held in each hand: short wooden hafts and practical single bearded iron heads with broad cutting edges. Keep both axes plausible one-handed scale and fully connected to their hands.

Effect and causality: exactly two broad crimson-red physical slash afterimages, one trailing directly behind each moving axe head. The trails cross once because the arms and axes cross-cut; they are not a floating symbol behind the figure. Give the crossing point one small white-red compression spark to express the knockback. The red reads as physical ferocity and motion, not magic or blood spray.

Icon hierarchy and finish: one compact action knot fills most of the square; the airborne dark figure and both axe heads remain readable, while the two red trails provide the strongest small-size read. Use a quiet smoky steel-blue and near-black background for separation and atlas variety, with no scenery. Original hand-painted graphical fantasy MMO ability icon, bold simplified masses, restrained weathered texture, faceless and emblematic—not a portrait, costume illustration, cinematic scene, or character key art.

Constraints: exactly one airborne figure, two hands, and exactly two compact one-handed single-headed axes; no extra, ghosted, crossed-but-unheld, double-headed, oversized, or two-handed axes. Rough fur, hide, and leather only as the dominant clothing construction; no plate armor, knight armor, Stormcloak uniform, polished champion gear, or horned-helmet mascot. Exactly two weapon-connected red trails forming one clean X; no static crossed-axes badge, idle guard pose, detached X, circular aura, fire, frost, lightning, elemental particles, gore, victim, face detail, text, runes, logo, watermark, border, or promotional key-art composition. At 32x32 the read must be: airborne fur-clad barbarian + two hand axes + one causal red cross-cut.
```

## Failure-mode evaluation

| Risk | Coverage in the skill/context | Coverage in the prompt | Assessment |
| --- | --- | --- | --- |
| Plate-armor drift | Owner direction is first priority; `Nord barbarian` has explicit fur/hide vocabulary and plate drift rejection. | Positive fur/hide/leather construction, exposed arms, and explicit exclusion of plate/knight/champion gear. | Strong. |
| Static crossed-axes emblem | Causality and cross-cut guidance require arms and shoulders to create a single crossing; the gate rejects posing. | Airborne center of gravity, separate hand paths, opposite endpoints, trails behind axe heads, and X in front of the torso. | Strong. |
| Unrelated elemental VFX | Effect must come from owner or payload; race cannot choose magic; physical ferocity has a crimson vocabulary. | Red is explicitly physical motion, with only a compact compression spark; fire/frost/shock/aura are excluded. | Strong. |
| Wrong weapon scale/count | Weapon vocabulary defines a compact one-handed bearded axe; dual wielding requires hand ownership and an endpoint. | Exactly two single-headed hand axes, one per hand, with short hafts, practical scale, paths, and endpoints; extra/oversized/double-headed axes excluded. | Strong. |
| Portrait/key-art drift | Asset type, 32 px hierarchy, smallest useful figure, faceless grammar, and gate all oppose character illustration. | Square hotbar glyph, simplified faceless figure, compact action knot, no scenery, and explicit no portrait/key art. | Strong. |

## Skill gaps and suggested tightening

1. **Active animation resolution is underspecified.** The skill says to read relevant annotations,
   but not to trace `overrideAnimationsFolder` through OAR or verify the active MO2/VFS provider.
   Add an explicit step to resolve the overlay, enabled providers, file winner, and hash before
   treating an HKX as canonical.
2. **“Last three finalized manifest rows” does not itself expose their axes.** The axes live in the
   referenced prompt files or images. Say to read the prompt files named by those rows and inspect
   masters only when prose does not establish the composition.
3. **Owner/animation divergence should be recorded.** Here “leaping” is owner-directed while the
   annotations prove strong forward travel and paired hits, not airtime. The priority rule resolves
   the choice correctly, but the brief should explicitly label owner-specified staging that is not
   proven by the clip.
4. **Named payload resolution needs a bounded fallback.** The dump exposes `$AoW_Knockback`, but the
   skill does not say where to resolve project-defined `$` instructions or what to record when the
   definition is unavailable. Add a lookup order and require conservative interpretation rather
   than inventing visual effects.
5. **Prompt-only gating could be explicit.** The generation gate is excellent, but a prompt-only
   request should also require checking the written prompt against the hard acceptance fields and
   returning the prompt without calling an image tool.

## Root review and disposition

The five routing gaps above were incorporated into the skill and brief template on 2026-08-23.
Root review also narrowed the generated phrase `Nordic hand axes` to `bearded iron war axes` in the
canonical Dual Flurry draft, because Skyrim's distinct `nordicwaraxe` family could otherwise imply
Nordic Carved equipment that the owner did not request. The independent prompt is preserved above
as test evidence; `.scratch/weapon-arts/prompt-drafts/aow_17_dual_flurry-context-v1.md` is the
canonical prompt-only acceptance draft.
