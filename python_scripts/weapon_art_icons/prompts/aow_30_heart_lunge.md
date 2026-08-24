# Heart Lunge

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_30_heart_lunge`

## Resolved identity

- Frozen action: A Redguard silhouette commits to one straight sword thrust from left to right. Bent lead knee, forward hip, shoulder, hand mass, blade, and point share one line; the rear mass counterbalances the lunge.
- Outline cues: wrapped head mass, lean duelist proportion.
- Weapon: one connected straight one-handed sword, point leading, hand mass behind the guard.
- Effect: one thin white thrust line attached to the point with one tiny terminal spark.
- Orientation: horizontal left-to-right thrust.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Heart Lunge. A Redguard silhouette commits to one straight sword thrust from left to right. Bent lead knee, forward hip, shoulder, hand mass, blade, and point share one line; the rear mass counterbalances the lunge.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, sword, and thrust path. Face is a void. Race as outline only: wrapped head mass, lean duelist proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected straight one-handed sword, point leading, hand mass behind the guard.
Causal wake: one thin white thrust line attached to the point with one tiny terminal spark.
Scene/backdrop: dark sand-red and charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: horizontal left-to-right thrust
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no heart symbol; no blood; no curved blade; no second line; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `92755EC2E0C708FCD23943A3A40E7BC5B72811DA33D462E71F8A45A8B1FB7EE4`.
