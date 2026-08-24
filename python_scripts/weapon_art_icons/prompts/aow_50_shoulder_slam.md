# Shoulder Slam

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_50_shoulder_slam`

## Resolved identity

- Frozen action: A female Nord silhouette drives the left shoulder into one body-check impact. Bent legs and forward hips support the shoulder-connected compression wedge; a compact bearded axe trails behind with no wake and does not become the subject.
- Outline cues: nasal helm mass, broad shoulders, compact female proportion.
- Weapon: one connected compact bearded axe trailing from the rear hand mass, with short haft and no active path.
- Effect: one ivory-white compression wedge attached directly to the leading left shoulder.
- Orientation: driving diagonal, lower-right to upper-left.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Shoulder Slam. A female Nord silhouette drives the left shoulder into one body-check impact. Bent legs and forward hips support the shoulder-connected compression wedge; a compact bearded axe trails behind with no wake and does not become the subject.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, driving legs, shoulder impact, and trailing axe cue. Face is a void. Race as outline only: nasal helm mass, broad shoulders, compact female proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected compact bearded axe trailing from the rear hand mass, with short haft and no active path.
Causal wake: one ivory-white compression wedge attached directly to the leading left shoulder.
Scene/backdrop: dark cold-blue and weathered-brown atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: driving diagonal, lower-right to upper-left
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no axe wake; no shield; no portrait framing; no text; no logo; no watermark; no UI border; no metallic frame
Correction priority: no hair, ponytail, braid, or loose head shape. Female Nord coding comes only from compact female body proportion, one closed nasal helm mass, and broad shoulders. The trailing axe has exactly one compact bearded cutting blade and a tiny blunt poll, with no glow and no axe wake. The left shoulder impact remains the subject.
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `64B2D38FAD7171AD7D1EA33E378DA72441577C8320914284FFFB9E0E3BAA6E53`.
