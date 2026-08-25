# Leap Slam

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_37_leap_slam`

## Resolved identity

- Frozen action: An Argonian silhouette descends through one hooked-poleaxe slam. Both feet are airborne, knees tucked behind the fall, torso and both hand masses aligned on the connected haft, with the hooked head leading toward a compact ground impact.
- Outline cues: long snout, short crest, tail trailing opposite the fall.
- Weapon: one connected two-handed hooked poleaxe with continuous haft and compact hooked head.
- Effect: one descending steel-white wake attached to the hooked head plus one compact impact flare.
- Orientation: steep descending diagonal, upper-left to lower-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Leap Slam. An Argonian silhouette descends through one hooked-poleaxe slam. Both feet are airborne, knees tucked behind the fall, torso and both hand masses aligned on the connected haft, with the hooked head leading toward a compact ground impact.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, airborne legs, poleaxe, and descending path. Face is a void. Race as outline only: long snout, short crest, tail trailing opposite the fall. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected two-handed hooked poleaxe with continuous haft and compact hooked head.
Causal wake: one descending steel-white wake attached to the hooked head plus one compact impact flare.
Scene/backdrop: dark swamp-green and charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: steep descending diagonal, upper-left to lower-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no feathers; no painted scales; no creature portrait; no scenery; no text; no logo; no watermark; no UI border; no metallic frame
Correction priority after rejected weapon drift: one hooked poleaxe with a continuous straight haft, one compact forward axe blade, and one small rear hook on the same head. No double-bit, symmetrical crescent, double-headed fantasy axe, or detached head. Exactly one descending white wake.
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `D5459C2C71B801CE261152544FEE727C15C06A4528422F7EA0FE39D7F7FA2511`.
