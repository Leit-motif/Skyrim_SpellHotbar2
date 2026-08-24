# Eldritch Beam

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_19_eldritch_beam`

## Resolved identity

- Frozen action: A small neutral battlemage silhouette at the far left braces and extends one bare casting hand horizontally right. The shoulder, elbow, wrist, palm, and beam share one straight force axis; the beam occupies about seventy percent of the square.
- Outline cues: close void hood, narrow shoulder mass.
- Weapon: one open casting-hand mass at the beam origin; no held weapon.
- Effect: one white-green beam with a tight bright core and one compact terminus.
- Orientation: horizontal left-to-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Eldritch Beam. A small neutral battlemage silhouette at the far left braces and extends one bare casting hand horizontally right. The shoulder, elbow, wrist, palm, and beam share one straight force axis; the beam occupies about seventy percent of the square.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, casting hand, and beam path. Face is a void. Race as outline only: close void hood, narrow shoulder mass. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one open casting-hand mass at the beam origin; no held weapon.
Causal wake: one white-green beam with a tight bright core and one compact terminus.
Scene/backdrop: dark charcoal and muted green atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: horizontal left-to-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no robes detail; no staff; no lightning branches; no scenery; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `D824C0B0011BF9E5A831C0904A94FC27B490811278F21C162E79434707AC49D0`.
