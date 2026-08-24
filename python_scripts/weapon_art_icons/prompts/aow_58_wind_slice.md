# Wind Slice

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_58_wind_slice`

## Resolved identity

- Frozen action: A Redguard silhouette passes through one dual-scimitar dash, but only the leading scimitar is actively cutting. The torso leans into travel, one blade leads, and the second remains a close trailing weapon cue without its own wake.
- Outline cues: wrapped head mass, lean duelist proportion.
- Weapon: exactly two connected curved scimitars, one active at the leading edge and one trailing close to the body.
- Effect: one pale ivory pressure wake attached only to the active scimitar edge.
- Orientation: passing horizontal left-to-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Wind Slice. A Redguard silhouette passes through one dual-scimitar dash, but only the leading scimitar is actively cutting. The torso leans into travel, one blade leads, and the second remains a close trailing weapon cue without its own wake.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, two scimitars, and passing path. Face is a void. Race as outline only: wrapped head mass, lean duelist proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: exactly two connected curved scimitars, one active at the leading edge and one trailing close to the body.
Causal wake: one pale ivory pressure wake attached only to the active scimitar edge.
Scene/backdrop: dark deep-red and sand-charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: passing horizontal left-to-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no second wake; no white costume; no scenery; no extra blade; no text; no logo; no watermark; no UI border; no metallic frame
No correction history. Enforce exactly two scimitars but exactly one active pale pressure wake on the leading blade; the trailing scimitar remains close to the body with no glow.
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `FBB959CB1E2B286ECD0C084FC92E54C1094C4E44A12112E191082EBD39259095`.
