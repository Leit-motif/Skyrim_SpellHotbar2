# Soulless Swing

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_53_soulless_swing`

## Resolved identity

- Frozen action: A Redguard silhouette rotates through one compact backhand mace swing. Feet and hips turn beneath the shoulder; the hand mass, short haft, and flanged head finish together on the outside line.
- Outline cues: wrapped head mass, lean duelist proportion.
- Weapon: one connected one-handed flanged mace with short haft and compact head.
- Effect: one compact steel-white mace-head wake attached to the striking head.
- Orientation: short horizontal backhand, left-to-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Soulless Swing. A Redguard silhouette rotates through one compact backhand mace swing. Feet and hips turn beneath the shoulder; the hand mass, short haft, and flanged head finish together on the outside line.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, flanged mace, and compact path. Face is a void. Race as outline only: wrapped head mass, lean duelist proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected one-handed flanged mace with short haft and compact head.
Causal wake: one compact steel-white mace-head wake attached to the striking head.
Scene/backdrop: dark deep-red and sand-charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: short horizontal backhand, left-to-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no soul effect; no sash detail; no oversized mace; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `190F1C93B9E05CD5FFA492EDD69C3BE4E43921A88A92A6E093A0FBC151CE5431`.
