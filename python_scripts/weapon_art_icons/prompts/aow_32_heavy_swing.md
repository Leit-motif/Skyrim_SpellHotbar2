# Heavy Swing

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_32_heavy_swing`

## Resolved identity

- Frozen action: A broad Orsimer silhouette powers one heavy two-handed battleaxe sweep. The stance is planted and counterweighted; hips and shoulders rotate behind both hand masses while the axe head leads a wide descending curve.
- Outline cues: heavy angular helm, brute shoulder mass.
- Weapon: one connected two-handed battleaxe with long haft and one dominant weighted cutting head.
- Effect: one thick steel-white heavy sweep wake attached to the axe edge.
- Orientation: descending arc, left-to-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Heavy Swing. A broad Orsimer silhouette powers one heavy two-handed battleaxe sweep. The stance is planted and counterweighted; hips and shoulders rotate behind both hand masses while the axe head leads a wide descending curve.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, battleaxe, and heavy path. Face is a void. Race as outline only: heavy angular helm, brute shoulder mass. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected two-handed battleaxe with long haft and one dominant weighted cutting head.
Causal wake: one thick steel-white heavy sweep wake attached to the axe edge.
Scene/backdrop: dark iron-gray and deep green atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: descending arc, left-to-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no rivets; no static pose; no floating head; no text; no logo; no watermark; no UI border; no metallic frame
Final weapon correction: the black axe-head silhouette is asymmetric. It has ONE broad cutting blade only on the lower-facing side of the haft, shaped like a single letter-D wedge. The opposite side is a tiny square blunt hammer poll, not a blade. Exactly one cutting edge total. No double-bit, double crescent, mirrored, symmetrical, two-blade, or fantasy axe head.
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `E6D7D22750EDE4E3DF1E51AB856CD1649EC46DD7D6C704FDA1A066067B37FC2F`.
