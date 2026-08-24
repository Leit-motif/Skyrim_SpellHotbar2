# Wicked Throw

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_57_wicked_throw`

## Resolved identity

- Frozen action: An Altmer silhouette releases one Alinor blade into an out-and-return flight loop. The throwing hand points from a small lower-right figure mass toward the airborne blade; the blade remains the only projectile.
- Outline cues: pointed-ear hood, tall narrow proportion.
- Weapon: one connected narrow flying Alinor-style blade with gently curved edge; no second blade.
- Effect: one pale-blue out-and-return loop attached to the flying blade.
- Orientation: looping diagonal, lower-right to upper-left.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Wicked Throw. An Altmer silhouette releases one Alinor blade into an out-and-return flight loop. The throwing hand points from a small lower-right figure mass toward the airborne blade; the blade remains the only projectile.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to throwing torso, hand, flying blade, and return loop. Face is a void. Race as outline only: pointed-ear hood, tall narrow proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected narrow flying Alinor-style blade with gently curved edge; no second blade.
Causal wake: one pale-blue out-and-return loop attached to the flying blade.
Scene/backdrop: dark emerald-black atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: looping diagonal, lower-right to upper-left
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no blonde hair; no visible face; no second blade; no portrait; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `4ACDEC3E94FA83B16BB9B88313ED4A5A2154A74BA49F028BFA4752D2837AB289`.
