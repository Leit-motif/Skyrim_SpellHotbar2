# Soul Cleaver

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_52_soul_cleaver`

## Resolved identity

- Frozen action: An Orsimer silhouette delivers one heavy war-cleaver chop. The planted stance, hips, shoulders, both hand masses, long hilt, and single-edged head align behind a wide descending cut.
- Outline cues: heavy angular helm, brute shoulder mass.
- Weapon: one connected two-handed single-edged war cleaver with a heavy forward cutting edge.
- Effect: one wide steel-white physical chop wake attached to the edge.
- Orientation: descending diagonal, upper-left to lower-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Soul Cleaver. An Orsimer silhouette delivers one heavy war-cleaver chop. The planted stance, hips, shoulders, both hand masses, long hilt, and single-edged head align behind a wide descending cut.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, war cleaver, and chop path. Face is a void. Race as outline only: heavy angular helm, brute shoulder mass. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected two-handed single-edged war cleaver with a heavy forward cutting edge.
Causal wake: one wide steel-white physical chop wake attached to the edge.
Scene/backdrop: dark iron-gray and rust atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: descending diagonal, upper-left to lower-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no soul effect; no armor detail; no second edge; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `C26111D3624B08F6988142D5C4085C83E249AB2BC3577AA2B389C7C1104EC0AD`.
