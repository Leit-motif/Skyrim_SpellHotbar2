# Point Charge

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_44_point_charge`

## Resolved identity

- Frozen action: An Orsimer silhouette drives a two-handed boar spear straight toward the viewer. The bright point dominates the foreground; spaced hand masses, continuous shaft, angular helm, shoulders, hips, and rear leg align behind the charge.
- Outline cues: heavy angular helm, brute shoulder mass.
- Weapon: one connected two-handed boar spear with continuous shaft and compact broad point.
- Effect: one bright ivory point flare with a narrow compression wake and restrained rim light on the charging mass.
- Orientation: frontal foreshortening toward the viewer.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Point Charge. An Orsimer silhouette drives a two-handed boar spear straight toward the viewer. The bright point dominates the foreground; spaced hand masses, continuous shaft, angular helm, shoulders, hips, and rear leg align behind the charge.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, spear, and compression path. Face is a void. Race as outline only: heavy angular helm, brute shoulder mass. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected two-handed boar spear with continuous shaft and compact broad point.
Causal wake: one bright ivory point flare with a narrow compression wake and restrained rim light on the charging mass.
Scene/backdrop: dark iron-gray and deep-green atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: frontal foreshortening toward the viewer
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no dark-on-dark weapon; no extra spear; no scenery; no text; no logo; no watermark; no UI border; no metallic frame
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `D7B398F54E94086419D62577A37491E98B1E80FDCA4EDA7CF05C4AD3C24418D2`.
