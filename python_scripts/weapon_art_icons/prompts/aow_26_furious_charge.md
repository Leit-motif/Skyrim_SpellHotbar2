# Furious Charge

**Status:** Finalized by goal-mode hard gate on 2026-08-23

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_26_furious_charge`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Furious Charge`.
  Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Furious Charge/AABL_Attack_A.HKX`;
  SHA-256 `ECC2D44A60A18FF1545D11EF38B8D9CAA32469CF1D8D725D6641F090D15432CD`.
- Animation-proven: the 5.066667-second, 99-track clip advances about 693 units. It opens four
  collision windows on the `SHIELD` node: three 2.5-scale, 0.3-damage contacts during the run and
  one final 4.0-scale, full-damage contact with a hit frame at 3.000000 seconds. Repeated gravel
  footfalls and small camera shakes mark the charge. It contains no weapon swing event or elemental
  payload.
- Catalogue mismatch: ArtClass is `2H`, but active animation evidence explicitly collides on the
  shield node. The icon follows the active clip and depicts no two-handed weapon.
- Agent composition choice: one Imperial heavy shield, one mostly hidden faceless legionary, one
  continuous charge wake, and one terminal physical impact. Imperial crimson, blackened iron,
  bronze, gold, and ivory-gray follow project race/palette guidance without inventing magic.
- Atlas choice: a foreshortened lower-left to upper-right shield wedge contrasts with the preceding
  sword diagonal, cross impact, and low axe sweep.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style shield art named "Furious Charge". This is an ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

PRIMARY GLYPH:
A single battered Imperial heavy shield and the compressed physical charge-impact in front of it dominate 88-90% of the square. Use a low frontal three-quarter view: the shield drives from lower-left toward upper-right, strongly foreshortened, with its bronze rim leading into one compact ivory-white compression burst near the upper-right edge. Behind the shield, exactly ONE continuous tapering rush wake of dust and pressure traces the same straight charge axis back toward lower-left. This is a body-and-shield rush, not a weapon slash. No multiple trails, echo shields, rings, or floating effects.

ACTION AND CAUSALITY:
One tiny faceless Imperial legionary is almost entirely hidden immediately behind the shield, no more than 10-12% of the icon: only a simplified closed helmet, braced shoulder, bent driving leg, and rear boot are needed to connect body to shield. The shield is strapped to the left forearm and visibly pushed by the shoulder. Exactly one practical broad Imperial heavy shield with plausible riveted bronze-and-iron construction. No sword, axe, mace, spear, bow, staff, or second shield.
The verified animation is a long roughly 693-unit forward charge using repeated SHIELD-node collisions and a much larger final shield collision. Depict one defining terminal shield impact, not repeated hits or repeated trails.

SKYRIM VISUAL LANGUAGE AND COLOR:
Imperial identity through economical material cues only: muted crimson cloth, blackened iron, weathered bronze, one restrained gold accent. Quiet charcoal-crimson smoky field. The physical rush wake is ivory-gray dust with a few bronze-orange contact sparks and small stone fragments at the leading shield rim. No magic, no elemental aura, no holy glow, no rune.
Bold painterly graphical MMO ability icon optimized for 32x32 readability; compact masses, high contrast, restrained texture, safe crop, dark vignette, edge-to-edge art. The shield silhouette and single forward impact must read instantly when tiny.

HARD CONSTRAINTS:
exactly one shield, one mostly hidden faceless figure, one continuous charge wake, one terminal physical impact. No portrait, face, large character, weapon, multiple slash trails, duplicate shield, floating shield, ring, X symbol, fire, frost, lightning, spell, victim, gore, scenery, text, letters, numbers, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full-resolution result: pass. The foreshortened shield, its continuous wake, and the terminal rim
  impact dominate; the legionary is faceless, largely hidden, and mechanically subordinate.
- 32 px LANCZOS reduction: pass. The red-black-gold shield wedge, trailing dust, and bright leading
  contact remain one immediate charge glyph.
- Master SHA-256: `42D9CAC79398645B8CD5610A11F6C9D9078F5A75A1959CB4E7BED6D1E0C4339B`.
