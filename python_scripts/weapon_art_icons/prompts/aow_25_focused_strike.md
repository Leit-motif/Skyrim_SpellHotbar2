# Focused Strike

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_25_focused_strike`

## Resolved identity

- Frozen action: A tiny Altmer silhouette commits to the final descending sword lunge from upper-right toward lower-left. Tall narrow torso, forward knee, shoulder, hand mass, blade, and target line align behind the point.
- Outline cues: pointed-ear hood, tall narrow proportion.
- Weapon: one connected narrow elven straight sword with the hand mass fixed behind the guard.
- Effect: one ivory descending slash attached to the blade; the effect dominates the square.
- Orientation: descending diagonal, upper-right to lower-left.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Focused Strike. A tiny Altmer silhouette commits to the final descending sword lunge from upper-right toward lower-left. Tall narrow torso, forward knee, shoulder, hand mass, blade, and target line align behind the point.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, sword, and descending path. Face is a void. Race as outline only: pointed-ear hood, tall narrow proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected narrow elven straight sword with the hand mass fixed behind the guard.
Causal wake: one ivory descending slash attached to the blade; the effect dominates the square.
Scene/backdrop: dark emerald and charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: descending diagonal, upper-right to lower-left
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no gold armor; no halo; no second slash; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `6B2851186AD4F67EA7094FDE000382D95481FDE1E4F86C19C89E61D88C4EC091`.
