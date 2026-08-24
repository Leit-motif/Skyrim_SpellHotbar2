# Head Tap

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_29_head_tap`

## Resolved identity

- Frozen action: An Altmer silhouette completes one precise saber cut with a compact shoulder turn and supported stance. The narrow blade finishes high while one smooth arc records the single active cut; do not diagram repeated hits.
- Outline cues: pointed-ear hood, tall narrow proportion.
- Weapon: one connected narrow Alinor-style saber with a gently curved edge and compact guard.
- Effect: one smooth ivory-gold blade wake attached to the saber.
- Orientation: shallow rising sweep, right-to-left.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Head Tap. An Altmer silhouette completes one precise saber cut with a compact shoulder turn and supported stance. The narrow blade finishes high while one smooth arc records the single active cut; do not diagram repeated hits.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, saber, and path. Face is a void. Race as outline only: pointed-ear hood, tall narrow proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected narrow Alinor-style saber with a gently curved edge and compact guard.
Causal wake: one smooth ivory-gold blade wake attached to the saber.
Scene/backdrop: dark emerald-black atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: shallow rising sweep, right-to-left
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no six-hit diagram; no cape; no extra blade; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `E86FD8C92A6E3AF9C60EC2A319891770FDDA15BEEA9DB75928E833C99922282A`.
