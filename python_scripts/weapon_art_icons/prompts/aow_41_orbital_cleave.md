# Orbital Cleave

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_41_orbital_cleave`

## Resolved identity

- Frozen action: A Redguard silhouette rotates through one horizontal saber cleave. Bent knees and centered hips support the torso turn; the saber edge leads one circular orbit while the figure remains a small pivot mass.
- Outline cues: wrapped veil hood, lean duelist proportion.
- Weapon: one connected one-handed saber with a curved cutting edge and hand mass behind the guard.
- Effect: one circular ivory-gold blade wake attached to the saber edge.
- Orientation: centered horizontal orbit.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Orbital Cleave. A Redguard silhouette rotates through one horizontal saber cleave. Bent knees and centered hips support the torso turn; the saber edge leads one circular orbit while the figure remains a small pivot mass.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, saber, and circular path. Face is a void. Race as outline only: wrapped veil hood, lean duelist proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected one-handed saber with a curved cutting edge and hand mass behind the guard.
Causal wake: one circular ivory-gold blade wake attached to the saber edge.
Scene/backdrop: dark deep-red and sand-charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: centered horizontal orbit
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no visible face; no second blade; no floating ring; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `875F899577A6407CC70F6194369A5D66F8796E171FF1ECF80D216FF2B43438F4`.
