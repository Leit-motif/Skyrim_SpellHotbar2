# Iai Slash

**Status:** Silhouette regeneration, autonomous AFK batch (2026-08-24)

**Generation path:** New Codex built-in generation with five approved grammar-only references; no prior candidate reference and no inpainting

**Stable icon key:** `aow_35_iai_slash`

## Resolved identity

- Frozen action: An Imperial Blades silhouette explodes from a low draw-cut lunge, traveling left-to-right. The scabbard remains a small empty cue at the hip; both hips and shoulder rotation drive the single long gold cut.
- Outline cues: flared Blades helm, compact guarded shoulders.
- Weapon: one connected narrow gently curved single-edged sword plus one small empty scabbard cue.
- Effect: one gold blade wake beginning at the cutting edge and fading behind the draw.
- Orientation: low horizontal left-to-right.
- Retained project artifact: 128x128 atlas input only; the generated full-size intermediate is not kept in the project.

## Generation prompt

```text
Use case: stylized-concept
Asset type: one 1:1 MMO-style RPG ability icon for Skyrim Spell Hotbar 2, designed for a final retained 128x128 atlas input and legible at 32x32

Input images: Images 1-5 are approved icon grammar references only. They set abstraction, faceless mass, limited palette, glow, effect-first hierarchy, and 32 px readability. They are not pose, facing, weapon, race, figure, or costume references.

Primary request: Iai Slash. An Imperial Blades silhouette explodes from a low draw-cut lunge, traveling left-to-right. The scabbard remains a small empty cue at the hip; both hips and shoulder rotation drive the single long gold cut.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, katana-like blade, empty scabbard cue, and path. Face is a void. Race as outline only: flared Blades helm, compact guarded shoulders. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the subject.

Weapon geometry: one connected narrow gently curved single-edged sword plus one small empty scabbard cue.
Causal wake: one gold blade wake beginning at the cutting edge and fading behind the draw.
Scene/backdrop: dark crimson-charcoal atmospheric field; no scenery
Style/medium: painted MMO ability icon; symbolic, atmospheric, effect-first; simple large masses and strong glow; not a portrait or character key art
Composition/framing: compact square crop; effect and weapon path dominate; the figure remains 25-45%; clear at 32x32
Orientation: low horizontal left-to-right
Physics: supported center of gravity, plausible joints, connected hand masses and weapon geometry, with every dominant trail beginning on the moving edge
Constraints: original art only; no second sword; no emblem; no ornate warrior portrait; no text; no logo; no watermark; no UI border; no metallic frame
Correction priority after rejected trail drift: exactly one gold draw-cut wake attached to the blade. No second arc, lower arc, ring, duplicate trail, echo line, or surrounding circular streak. The empty scabbard is only a small dark cue.
```

## Hard gate

Passed autonomously. The full-resolution intermediate and exact 32 px LANCZOS reduction preserve
the action-first read, connected physics, and figure-mass hierarchy. The retained project artifact
is the 128x128 atlas input only; the generated full-size intermediate remains outside the project.

- 128 px SHA-256: `0F60036EBEE37869D0C6159265DF74353E1138887F3D73BE563F77B2B566FAD8`.
