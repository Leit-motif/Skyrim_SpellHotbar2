# Ripping Hour

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_46_ripping_hour`

## Resolved identity

- Frozen action: An Altmer silhouette completes a two-blade shear. Two short sabers travel on opposing mechanically possible paths, each arm and shoulder carrying its own blade; both wakes converge once at the central cut.
- Outline cues: pointed-ear hood, tall narrow proportion.
- Weapon: exactly two connected short sabers, one in each hand, each with a compact guard and readable cutting edge.
- Effect: one ivory-gold wake per blade, converging exactly once.
- Orientation: opposed diagonals converging at center.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Ripping Hour. An Altmer silhouette completes a two-blade shear. Two short sabers travel on opposing mechanically possible paths, each arm and shoulder carrying its own blade; both wakes converge once at the central cut.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, two short sabers, and converging paths. Face is a void. Race as outline only: pointed-ear hood, tall narrow proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: exactly two connected short sabers, one in each hand, each with a compact guard and readable cutting edge.
Causal wake: one ivory-gold wake per blade, converging exactly once.
Scene/backdrop: dark emerald and gold-charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: opposed diagonals converging at center
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no armor detail; no third trail; no long swords; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `BB0BF7E748D50AE16DDA34545837023E9B8D25A294176AA1AEB41ACBA7376CE6`.
