# Shadow Reave

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_48_shadow_reave`

## Resolved identity

- Frozen action: A Khajiit silhouette completes one terminal two-handed battleaxe cut. Hips and shoulders have carried both hand masses through the long haft, and the heavy head finishes low with the tail counterbalancing the motion.
- Outline cues: ear points, short muzzle, tail counter-arc.
- Weapon: one connected two-handed battleaxe with long haft and one heavy cutting head.
- Effect: one steel-white physical wake attached to the axe edge.
- Orientation: descending diagonal, upper-right to lower-left.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Shadow Reave. A Khajiit silhouette completes one terminal two-handed battleaxe cut. Hips and shoulders have carried both hand masses through the long haft, and the heavy head finishes low with the tail counterbalancing the motion.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, battleaxe, tail cue, and terminal path. Face is a void. Race as outline only: ear points, short muzzle, tail counter-arc. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected two-handed battleaxe with long haft and one heavy cutting head.
Causal wake: one steel-white physical wake attached to the axe edge.
Scene/backdrop: dark plum and charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: descending diagonal, upper-right to lower-left
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no painted fur; no shadow magic; no extra axe; no text; no logo; no watermark; no UI border; no metallic frame
Correction priority: the battleaxe head is asymmetric with exactly one practical single-bit cutting blade on one side and a tiny blunt poll on the other. No double-bit, double crescent, mirrored, or symmetrical axe. Keep the Khajiit as a pure black outline with ear, short muzzle, and tail cues only.
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `B133A8FDA9D090E83778CD00B8A234F7AC7801009436702306CB3C86FB313049`.
