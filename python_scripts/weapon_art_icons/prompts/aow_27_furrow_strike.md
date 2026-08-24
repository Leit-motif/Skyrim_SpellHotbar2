# Furrow Strike

**Status:** Finalized by goal-mode hard gate on 2026-08-23

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_27_furrow_strike`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Furrow Strike`.
  Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Furrow Strike/AABL_Attack_A.HKX`;
  SHA-256 `248B72C7256AC0D2F2F009050230ADE9E60C829E9893CA76C9C9F928B53A6839`.
- Animation-proven: the 3.000000-second, 99-track, 2H-class clip advances about 208 units. A weaker
  0.5-damage weapon collision begins at 1.333333 seconds; the defining 1.5-damage collision begins
  at 1.750000, triples trail lifetime, doubles trail brightness, triggers `$AoW_Dust`, then lands
  at 1.833333 with `$AoW_Knockback` and camera shake.
- Payload resolution: no active definitions were found for `$AoW_Dust` or `$AoW_Knockback`.
  Physical dust, displaced soil, stone, and impact force follow the animation names and collision;
  no element or magical color was inferred.
- Agent composition choice: one tiny faceless Nord with one two-handed steel greatsword carves one
  continuous perspective furrow. The weaker setup collision is not depicted as another trail.
- Atlas choice: the bright cut narrows from the lower-left foreground toward its upper-right source,
  contrasting with the preceding foreshortened shield wedge and flat diagonals.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style two-handed weapon art named "Furrow Strike". This is an ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

PRIMARY GLYPH AND CAMERA:
A single freshly carved ground furrow dominates 85-88% of the square in dramatic depth perspective. Camera low and slightly overhead along the cut: the furrow begins broad in the lower-left foreground, with two dark displaced-earth banks and one bright ivory-gray compression seam, then narrows toward the greatsword contact point near the upper-right/center. The furrow is ONE continuous physical path caused by ONE low two-handed blade cut. It is not a separate magic beam or decorative trail.
At the narrow source end, one practical steel greatsword cutting edge is visibly embedded just above the ground and tangent to the same furrow axis. A compact burst of ochre dust, small stones, and restrained steel sparks erupts exactly where blade meets earth. No extra slash arcs, echo trails, rings, cracks radiating in other directions, or floating effects.

FIGURE AND WEAPON:
One tiny faceless Nord two-handed warrior occupies no more than 12-15% at the upper-right source end, subordinate to the foreground furrow. Freeze the final low follow-through: broad compact silhouette leaning forward, front knee bent, rear leg braced, both hands clearly spaced on one long greatsword grip, shoulders and blade aligned with the ground cut. Closed shadowed helmet; no visible face.
Exactly ONE practical Skyrim-style two-handed steel greatsword, one blade, two hands on one grip, plausible proportions. No axe, hammer, mace, shield, second sword, or detached blade.

EVIDENCE AND VISUAL LANGUAGE:
The verified clip advances roughly 208 units, uses a weaker setup weapon collision, then one stronger final weapon collision with extended bright trail, dust payload, camera shake, and knockback. Depict only the defining final strike and one furrow, not two collisions or multiple trails. The exact dust and knockback payload definitions are unresolved, so use only nonmagical physical soil, stone, and steel.
Nord identity through economical silhouette and material cues: simplified steel-gray armor, weathered brown leather, a narrow dark-red cloth accent, and restrained cold blue-gray edging. No anatomy detail, face, costume showcase, or scenic landscape.

STYLE:
Bold painterly graphical MMO ability icon optimized for 32x32 readability; compact masses, strong negative space, high contrast, restrained texture. Palette: ivory-gray compression seam, steel gray, cold blue-gray, dark earth brown, ochre dust, charcoal, tiny rust-orange sparks. Quiet dark field and vignette. Edge-to-edge icon art, safe crop.

HARD CONSTRAINTS:
exactly one tiny faceless full-body figure, exactly one two-handed greatsword, exactly one connected ground furrow, one contact burst. No portrait, large character, face, multiple slashes, multiple furrows, energy beam, magic, fire, frost, lightning, rune, victim, gore, scenery, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full-resolution result: pass. One greatsword enters one continuous perspective furrow at one
  compact dust-and-stone contact; the source warrior is small, distant, and faceless.
- 32 px LANCZOS reduction: pass. The sword and narrowing ivory ground seam remain a single readable
  glyph rather than resolving as a landscape.
- Master SHA-256: `DBE93DBE68478F595E7C374AC1E3AFE7F39B93E20300299ABED8C2660D01DF4F`.
