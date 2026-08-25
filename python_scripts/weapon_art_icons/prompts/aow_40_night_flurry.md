# Night Flurry

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_40_night_flurry`

## Resolved identity

- Frozen action: A Dunmer silhouette finishes one terminal shortsword cut, hips and shoulder already carried through the strike. The close hand mass and short blade remain readable while one continuous physical wake marks the completed path.
- Outline cues: close mask hood, compact chitin-like shoulder wedge.
- Weapon: one connected short chitin-or-ebony sword shorter than the torso, with hand mass directly behind its guard.
- Effect: one continuous steel-violet physical blade wake attached to the edge.
- Orientation: descending diagonal, upper-left to lower-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Night Flurry. A Dunmer silhouette finishes one terminal shortsword cut, hips and shoulder already carried through the strike. The close hand mass and short blade remain readable while one continuous physical wake marks the completed path.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, shortsword, and terminal path. Face is a void. Race as outline only: close mask hood, compact chitin-like shoulder wedge. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected short chitin-or-ebony sword shorter than the torso, with hand mass directly behind its guard.
Causal wake: one continuous steel-violet physical blade wake attached to the edge.
Scene/backdrop: dark ash-gray and violet atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: descending diagonal, upper-left to lower-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no darkness magic; no extra cuts; no copper detail; no visible face; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `94A5C77DF9A2C954F9571BFCB699FD02ADAF9974A031679D315403921FCEEEA5`.
