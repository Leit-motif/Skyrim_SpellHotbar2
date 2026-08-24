# Divine Smite

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_14_divine_smite`

## Resolved identity

- Frozen action: A grounded Breton silhouette completes a steep two-handed blessed sword cut from upper-left to lower-right. Knees compressed, hips and shoulders behind the blade, both hand masses joined on one hilt; the gold-lit edge terminates in a compact holy burst at the lower-right.
- Outline cues: closed knight helm, compact shoulder mass.
- Weapon: one connected two-handed straight sword, restrained crossguard, both hand masses together on the hilt.
- Effect: one blade-attached gold wake ending in one compact ivory-gold burst; no halo.
- Orientation: descending diagonal, upper-left to lower-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Divine Smite. A grounded Breton silhouette completes a steep two-handed blessed sword cut from upper-left to lower-right. Knees compressed, hips and shoulders behind the blade, both hand masses joined on one hilt; the gold-lit edge terminates in a compact holy burst at the lower-right.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, sword, and path. Face is a void. Race as outline only: closed knight helm, compact shoulder mass. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected two-handed straight sword, restrained crossguard, both hand masses together on the hilt.
Causal wake: one blade-attached gold wake ending in one compact ivory-gold burst; no halo.
Scene/backdrop: dark royal-blue and burgundy atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: descending diagonal, upper-left to lower-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no halo; no heraldry; no extra weapon; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `EA6D74953A477B3CA552228274DBFA856DBFFFF1BE02008035568651145F0EE8`.
