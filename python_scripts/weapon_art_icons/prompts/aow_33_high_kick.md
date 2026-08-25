# High Kick

**Status:** Owner-approved silhouette regeneration (2026-08-24)

**Generation path:** One new Codex built-in image-generation call with five approved icon-grammar
references; no rejected-candidate reference and no inpainting

**Stable icon key:** `aow_33_high_kick`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/High Kick`. Selected
  enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../High Kick/AABL_Attack_A.HKX`;
  SHA-256 `88CF7D1BC51F9E3A249E3ED309472B46B5A1A9184675DED75B4759ADAD065B52`.
- Animation-proven: the 1.333333-second, 97-track Generic clip advances about 363 units. Enlarged
  collision windows move from the left leg to the right leg, with hit frames at 0.499998 and
  0.733330.
- Owner-directed staging: show the finishing high right-leg roundhouse as the one dominant action;
  Khajiit outline; one foot-connected ivory crescent; plum field.
- Grammar references only: Crane Style, Aimed Blow, Blood Flurry, Champion's End, and Dragon Strike
  set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px
  readability. Their poses, facing, weapons, races, figures, and compositions are not references.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, retained as a 128x128 atlas input and designed to remain legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. Do not copy their pose, facing, weapon, race, figure, or composition.

Primary request: High Kick. A generalized Khajiit martial-artist silhouette completing one explosive high roundhouse. Planted left leg bent and supporting the center of gravity, hips rotated, torso counterleaning, right leg extended to the upper-right as the leading point. One ivory-white pressure crescent tangent to and beginning at the extended foot, fading backward along the foot's path; one compact warm-gold impact flare at the toes.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, kicking legs, and path. Face is a void. Race as outline only: pointed ears, short muzzle, tail completing the kick arc. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first, not a portrait and not character key art
Composition/framing: compact square crop; kicking leg and crescent dominate; one readable rising diagonal from lower-left to upper-right; simple large masses; clear at 32x32
Scene/backdrop: dark plum-black atmospheric field with subtle radial glow; no scenery
Color palette: deep plum-black, ivory-white kick crescent, restrained warm-gold impact
Detail budget: very low; broad painted shapes and glow
Physics: anatomically plausible roundhouse silhouette; supported planted leg, rotated hip, counterbalanced torso and tail; exactly one connected kicking leg and one foot-connected causal wake
Constraints: original art only; exactly one anonymous figure; exactly one high kick; no costume detail; no portrait framing; no extra limbs; no duplicate or echo legs; no weapon; no victim; no text; no logo; no watermark; no UI border; no metallic frame
```

## Calibration hard-gate result

- Full-resolution intermediate: pass, owner approved. One near-black Khajiit silhouette, one supported
  high roundhouse, and one ivory/gold arc caused by the leading foot; no painted costume inventory.
- 32 px LANCZOS reduction: pass, owner approved. The kick and impact arc read before the race
  outline, and the image remains a compact ability glyph.
- New 128 px atlas-input SHA-256: `D1641E46D75101D16D66FEA9EBED759D6F69C376AEBBC2BDB9DB6E2EF8008C70`.
- The retained project artifact is the 128x128 atlas input only; the generated full-size intermediate
  remains outside the project.
- The previous costume plate and its 128 px reduction were archived under
  `pilot/silhouette-drift/` before replacement.
- The owner approved this calibration and waived further review gates for the autonomous batch.
