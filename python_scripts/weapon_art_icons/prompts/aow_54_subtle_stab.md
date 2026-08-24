# Subtle Stab

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_54_subtle_stab`

## Resolved identity

- Frozen action: A close-cropped Dunmer silhouette completes one underhand dagger stab. The elbow stays bent, hand remains near the target, and the blade is visibly shorter than the forearm; the tiny point leads upward.
- Outline cues: close mask hood, compact chitin-like shoulder wedge.
- Weapon: one connected dagger shorter than the forearm, held underhand with the hand mass directly behind the short hilt.
- Effect: one tiny pale-violet point flare attached to the dagger tip.
- Orientation: short rising diagonal, lower-right to center.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Subtle Stab. A close-cropped Dunmer silhouette completes one underhand dagger stab. The elbow stays bent, hand remains near the target, and the blade is visibly shorter than the forearm; the tiny point leads upward.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, forearm, dagger, and tiny path. Face is a void. Race as outline only: close mask hood, compact chitin-like shoulder wedge. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected dagger shorter than the forearm, held underhand with the hand mass directly behind the short hilt.
Causal wake: one tiny pale-violet point flare attached to the dagger tip.
Scene/backdrop: dark ash-gray and muted violet atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: short rising diagonal, lower-right to center
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no visible face; no long sword; no large trail; no second hand weapon; no text; no logo; no watermark; no UI border; no metallic frame
Correction priority: exactly one Dunmer attacker silhouette and no other person, opponent, victim, target body, face, or shadow figure. The short underhand dagger points into empty dark field with one tiny pale-violet point flare attached to its tip.
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `219A8AEC157CA7140840093D0CED5039D9FC5C90FD34B079A6C9D1B18534705D`.
