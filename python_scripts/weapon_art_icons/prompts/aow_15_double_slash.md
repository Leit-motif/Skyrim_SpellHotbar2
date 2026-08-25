# Double Slash

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_15_double_slash`

## Resolved identity

- Frozen action: A compact Breton silhouette drives one dominant sword slash across the square, frozen at the strongest finishing beat rather than diagramming two hits. The planted stance and torso rotation support a rising lower-left to upper-right cut.
- Outline cues: closed bascinet, compact shoulder mass.
- Weapon: one connected straight one-handed sword with the hand mass fixed behind the guard.
- Effect: one broad ivory-blue blade wake owns the square and begins at the moving edge.
- Orientation: rising diagonal, lower-left to upper-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Double Slash. A compact Breton silhouette drives one dominant sword slash across the square, frozen at the strongest finishing beat rather than diagramming two hits. The planted stance and torso rotation support a rising lower-left to upper-right cut.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, sword, and path. Face is a void. Race as outline only: closed bascinet, compact shoulder mass. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected straight one-handed sword with the hand mass fixed behind the guard.
Causal wake: one broad ivory-blue blade wake owns the square and begins at the moving edge.
Scene/backdrop: dark burgundy-blue atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: rising diagonal, lower-left to upper-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no second figure; no heraldry; no duplicate blade; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `390B527770366E91B57B1A700FC6D65E59341B17A66631E302CEDE0E996AF54B`.
