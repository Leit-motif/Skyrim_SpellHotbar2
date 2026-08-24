# Magnetic Throw

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_39_magnetic_throw`

## Resolved identity

- Frozen action: A small Altmer silhouette at lower-left reaches one palm toward a single airborne Alinor blade at upper-right. The blade follows one readable out-and-return loop, and a bright blue tether directly connects palm to blade.
- Outline cues: pointed-ear hood, tall narrow proportion.
- Weapon: one connected narrow flying Alinor-style blade with a gently curved edge; no blade held by the figure.
- Effect: one bright blue palm-to-blade tether plus one compact pale-blue return loop.
- Orientation: looping diagonal, lower-left to upper-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Magnetic Throw. A small Altmer silhouette at lower-left reaches one palm toward a single airborne Alinor blade at upper-right. The blade follows one readable out-and-return loop, and a bright blue tether directly connects palm to blade.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to casting torso, hand, flying blade, and return loop. Face is a void. Race as outline only: pointed-ear hood, tall narrow proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected narrow flying Alinor-style blade with a gently curved edge; no blade held by the figure.
Causal wake: one bright blue palm-to-blade tether plus one compact pale-blue return loop.
Scene/backdrop: dark emerald-charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: looping diagonal, lower-left to upper-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no second blade; no darkness magic; no assassin portrait; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `50466D5AC182B8D063C10D92413F4E88666E497144EE878FA15C169C3952B0E5`.
