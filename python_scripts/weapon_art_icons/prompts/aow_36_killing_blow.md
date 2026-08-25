# Killing Blow

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_36_killing_blow`

## Resolved identity

- Frozen action: A small Bosmer silhouette delivers one finishing stone-mace blow down and inward. The lead knee and torso compression support the shoulder, hand mass, short haft, and compact mace head at one orange impact point.
- Outline cues: small hooded hunter mass, light narrow shoulders.
- Weapon: one connected stone mace shorter than the torso, with compact head and one hand mass on the haft.
- Effect: one compact orange-white impact flare attached to the mace head.
- Orientation: short descending diagonal, upper-right to center.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Killing Blow. A small Bosmer silhouette delivers one finishing stone-mace blow down and inward. The lead knee and torso compression support the shoulder, hand mass, short haft, and compact mace head at one orange impact point.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, mace, and impact path. Face is a void. Race as outline only: small hooded hunter mass, light narrow shoulders. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected stone mace shorter than the torso, with compact head and one hand mass on the haft.
Causal wake: one compact orange-white impact flare attached to the mace head.
Scene/backdrop: dark forest-green and bark-black atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: short descending diagonal, upper-right to center
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no spikes; no leather detail; no victim; no oversized mace; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `3C1B6B6EDA616180A4D141929DC246E4A32203BA93456F3955D312F20F60D925`.
