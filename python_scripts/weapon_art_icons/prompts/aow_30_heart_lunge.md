# Heart Lunge

**Status:** Finalized by goal-mode hard gate on 2026-08-23

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_30_heart_lunge`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Heart Lunge`. Selected
  enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Heart Lunge/AABL_Attack_A.HKX`;
  SHA-256 `D58D5558F008A07C5F62BFFD33D63D05009FC2D4D06278E8F167F13F9FA7DF30`.
- Animation-proven: the 2.166667-second, 97-track clip advances about 392 units, traces the right
  weapon from 0.200000 to 1.000000 seconds, and lands right-hand hits at 0.500000 and 0.899999.
  The second hit carries `$ES_Strikefly`.
- Payload resolution: no active `$ES_Strikefly` definition was found. The name is reused across
  unrelated Argonian sword, hammer, axe, and greatsword annotations, so it proves a generic launch
  behavior rather than a heart, blood, element, color, or exact VFX.
- Agent composition choice: one faceless Redguard duelist, one straight one-handed sword, one
  horizontal thrust wake, and one physical endpoint. The two hits are distilled to the terminal
  lunge rather than doubled.
- Atlas choice: a right-to-left horizontal thrust on midnight teal/sand follows the vertical
  Argonian hammer, failed high Khajiit arc, and deep perspective Nord furrow.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style weapon art named "Heart Lunge". This is an ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

PRIMARY GLYPH:
One—and only one—precise physical sword thrust dominates 85-88% of the square. A narrow straight steel blade drives horizontally from right toward a compact contact point at left-center. One continuous ivory-white compression wake lies directly behind and collinear with that same blade, brightest immediately behind the point and tapering toward the right. The sword point, one straight wake, and one tiny steel-spark contact are the entire glyph.
No heart shape, anatomical heart, blood, red energy, second trail, echo blade, ring, X, crescent, or floating effect.

FIGURE AND WEAPON:
One tiny faceless Redguard duelist occupies no more than 10-12% at the far-right source end, almost entirely hidden behind the thrust axis. Freeze the terminal forward lunge: front knee deeply bent, rear leg extended, torso low, right shoulder and arm locked behind the blade, empty left hand drawn back for balance.
Exactly ONE practical one-handed straight steel sword in the right hand, normal proportions, one narrow point, one cutting blade, no ornate fantasy enlargement. The blade is physically connected to the grip and lies exactly on the one compression wake. No scimitar curve, dagger, shield, second weapon, or detached blade.

EVIDENCE AND SKYRIM VISUAL LANGUAGE:
The verified clip advances roughly 392 units, makes two right-hand swings/hits, and ends the second hit with an unresolved generic strike-launch payload. Depict only the defining terminal lunge and one wake, not two attacks or two trails. The ability name and payload prove no blood, heart imagery, element, or color.
Redguard identity through economical full-body silhouette/material cues only: wrapped lower-face scarf and low hood/closed helm, layered sand-brown leather and deep-red cloth, restrained turquoise binding, muted gold fittings. Face fully hidden; no eyes, skin close-up, portrait, or costume showcase.

STYLE AND COLOR:
Bold painterly graphical MMO ability icon optimized for 32x32 readability; compact graphic masses, strong negative space, high contrast, restrained texture. Dominant ivory-white/steel thrust on a quiet midnight-teal and sand-charcoal field; tiny source accents in deep red, warm brown, muted gold, and restrained turquoise. A few physical amber sparks only at the left contact. No magical glow, fire, frost, lightning, poison, or rune. Dark vignette, safe crop, edge-to-edge art.

HARD CONSTRAINTS:
exactly one tiny faceless full-body Redguard, exactly one one-handed straight sword, exactly one connected horizontal wake, one physical contact. No portrait, face, large character, second trail, repeated blade, heart symbol, blood, gore, victim, spell, scenery, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full-resolution result: pass. One straight sword, one collinear wake, and one left endpoint remain
  causally connected; no heart, blood, extra trail, or detached effect was invented.
- 32 px LANCZOS reduction: pass. The leftward ivory thrust and bright endpoint dominate; the
  faceless Redguard only anchors the source.
- Master SHA-256: `C4E275DD44DFBD6C08F38CAAF6170FBAB7356CDC8A2D84E4FB9DF8B0983AE65A`.
