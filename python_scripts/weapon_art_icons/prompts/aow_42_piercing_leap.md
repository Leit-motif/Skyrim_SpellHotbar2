# Piercing Leap

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_42_piercing_leap`

## Resolved identity

- Frozen action: A Breton silhouette is airborne behind one two-handed spear thrust along a single diagonal axis. Torso, spaced hand masses, continuous shaft, point, and wake are collinear; legs trail compactly behind the thrust.
- Outline cues: closed knight helm, compact shoulder mass.
- Weapon: one connected two-handed spear with continuous long shaft, spaced hand masses, and one point leading.
- Effect: one narrow ivory-white wake directly behind and collinear with the spear point.
- Orientation: rising diagonal thrust, lower-left to upper-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Piercing Leap. A Breton silhouette is airborne behind one two-handed spear thrust along a single diagonal axis. Torso, spaced hand masses, continuous shaft, point, and wake are collinear; legs trail compactly behind the thrust.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, compact airborne legs, spear, and thrust axis. Face is a void. Race as outline only: closed knight helm, compact shoulder mass. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected two-handed spear with continuous long shaft, spaced hand masses, and one point leading.
Causal wake: one narrow ivory-white wake directly behind and collinear with the spear point.
Scene/backdrop: dark royal-blue and charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: rising diagonal thrust, lower-left to upper-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no scenery; no floating spear; no second point; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `E80E6CC8C7A60F8D7054310201B13B673C5B3E572D8B5DBDF9A7FD7FA5E389A0`.
