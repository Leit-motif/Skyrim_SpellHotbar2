# Pirate's Slash

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_43_pirates_slash`

## Resolved identity

- Frozen action: A hooded Breton-corsair silhouette lunges through one cutlass slash. The forward knee and torso support the active arm; a close dagger remains only a tiny off-hand cue while the cutlass and its single wake own the square.
- Outline cues: void hood, compact corsair shoulder mass.
- Weapon: one connected curved cutlass in the active hand plus one dagger kept close to the off-hand mass.
- Effect: one ivory cutlass wake attached to the active blade; no dagger wake.
- Orientation: rising diagonal lunge, right-to-left.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Pirate's Slash. A hooded Breton-corsair silhouette lunges through one cutlass slash. The forward knee and torso support the active arm; a close dagger remains only a tiny off-hand cue while the cutlass and its single wake own the square.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, cutlass, close dagger cue, and path. Face is a void. Race as outline only: void hood, compact corsair shoulder mass. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected curved cutlass in the active hand plus one dagger kept close to the off-hand mass.
Causal wake: one ivory cutlass wake attached to the active blade; no dagger wake.
Scene/backdrop: dark burgundy and sea-charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: rising diagonal lunge, right-to-left
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no eyes; no hair; no pirate scenery; no second wake; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `978E0B394ED031B21F307BEA85AB07A485EEACE5DD003CBA2D5465A3AB3946A3`.
