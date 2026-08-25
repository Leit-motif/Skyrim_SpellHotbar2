# Simple Bash

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_51_simple_bash`

## Resolved identity

- Frozen action: A tiny Nord silhouette braces directly behind one round shield and drives the central boss toward the viewer. Shoulder, forearm mass, shield plane, and impact align; the shield owns the square while the figure remains secondary.
- Outline cues: nasal helm mass, broad shoulders.
- Weapon: one connected round shield with a central boss and the bracing arm mass directly behind it.
- Effect: one compact ivory-white burst centered on the shield boss.
- Orientation: frontal foreshortening toward the viewer.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Simple Bash. A tiny Nord silhouette braces directly behind one round shield and drives the central boss toward the viewer. Shoulder, forearm mass, shield plane, and impact align; the shield owns the square while the figure remains secondary.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, round shield, and boss impact. Face is a void. Race as outline only: nasal helm mass, broad shoulders. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected round shield with a central boss and the bracing arm mass directly behind it.
Causal wake: one compact ivory-white burst centered on the shield boss.
Scene/backdrop: dark cold-blue and charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: frontal foreshortening toward the viewer
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no sword; no raised knee; no heraldry; no second shield; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `844280614746796745D282C97C8043E373D4E79C9756CB5B67794707E3B59755`.
