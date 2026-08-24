# Dual Flurry

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_17_dual_flurry`

## Resolved identity

- Frozen action: A broad Nord silhouette completes a physically possible cross-cut with two compact one-handed axes. Bent knees and rotated shoulders carry the arms through one crossing point; the axe heads finish on opposite sides of the X.
- Outline cues: nasal helm mass, broad shoulders.
- Weapon: exactly two connected compact one-handed axes, one in each hand, each with a short haft and one practical cutting head.
- Effect: two crimson axe-edge trails cross exactly once to form one readable X.
- Orientation: centered crossing diagonals.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Dual Flurry. A broad Nord silhouette completes a physically possible cross-cut with two compact one-handed axes. Bent knees and rotated shoulders carry the arms through one crossing point; the axe heads finish on opposite sides of the X.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, two axes, and crossed path. Face is a void. Race as outline only: nasal helm mass, broad shoulders. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: exactly two connected compact one-handed axes, one in each hand, each with a short haft and one practical cutting head.
Causal wake: two crimson axe-edge trails cross exactly once to form one readable X.
Scene/backdrop: dark charcoal and cold blue field with restrained red glow; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: centered crossing diagonals
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no hair; no eyes; no third trail; no floating axe; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `D46C004A456F616C5CBE862E1AA746F5B0F0A9679E9953CE0CF72DBCF3CCEFDB`.
