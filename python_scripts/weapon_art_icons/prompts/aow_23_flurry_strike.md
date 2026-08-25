# Flurry Strike

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_23_flurry_strike`

## Resolved identity

- Frozen action: A low Orsimer silhouette finishes one sweeping two-handed battleaxe cut close to the ground. The hips sit behind the long haft, both hand masses remain connected, and the heavy axe head leads a right-to-left ground sweep.
- Outline cues: heavy angular helm, brute shoulder mass.
- Weapon: one connected two-handed battleaxe with a long haft and one dominant heavy cutting head.
- Effect: one axe-edge-connected steel-white ground sweep.
- Orientation: low horizontal right-to-left.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Flurry Strike. A low Orsimer silhouette finishes one sweeping two-handed battleaxe cut close to the ground. The hips sit behind the long haft, both hand masses remain connected, and the heavy axe head leads a right-to-left ground sweep.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, battleaxe, and low path. Face is a void. Race as outline only: heavy angular helm, brute shoulder mass. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected two-handed battleaxe with a long haft and one dominant heavy cutting head.
Causal wake: one axe-edge-connected steel-white ground sweep.
Scene/backdrop: dark iron-gray and blood-red atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: low horizontal right-to-left
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no multiple strikes; no floating axe; no scenery; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `CD65C3A60E3EA4278D77D6F3DB0522AFB68F6B77C848BD0494BCF5F240A28F91`.
