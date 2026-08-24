# High Kick

**Status:** Finalized by goal-mode hard gate on 2026-08-24

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_33_high_kick`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/High Kick`. Selected
  enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../High Kick/AABL_Attack_A.HKX`;
  SHA-256 `88CF7D1BC51F9E3A249E3ED309472B46B5A1A9184675DED75B4759ADAD065B52`.
- Animation-proven: the 1.333333-second, 97-track Generic clip advances about 363 units. Enlarged
  collision windows move from the left leg to the right leg, with hit frames at 0.499998 and
  0.733330.
- Payload resolution: `$ES_Paralysis`, `$RS_ImpactJD`, and `$ES_EndureArmor` were not resolved to
  authoritative visual definitions. The Dragonrend release sound at both hits does not prove a
  shout wave, color, or element.
- Composition choice: show the finishing high right-leg kick as the one dominant action. Do not
  diagram the two collision windows or invent an element.
- Race/palette choice: a faceless Khajiit in warm brown, tan, ochre, burgundy, muted gold, and deep
  purple provides deliberate race and palette rotation after the Orsimer Heavy Swing.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style martial art named "High Kick". This is a graphical action icon, not a portrait, character key art, splash illustration, wallpaper, or scenic painting.

ACTION FIGURE:
Show one faceless full-body Khajiit martial fighter delivering one explosive high roundhouse kick from lower-left toward upper-right. Freeze the finishing right-leg contact: planted left leg bent, hips fully rotated, torso leaning away for balance, right leg extended high with the boot/foot at the upper-right leading point, arms compact and guarding. The dynamic body and extended leg form the icon. Keep the face completely hidden by a low hood and shadow; use only subtle pointed-ear hood and tail silhouette cues.

ONE KICK WAKE:
Add exactly ONE short ivory-white physical pressure crescent immediately behind and tangent to the extended right foot, following the foot's upward-right arc. It must begin at the foot and fade backward toward the bent knee. One compact impact flare and a few dust flecks sit at the foot's leading edge.
No second kick trail, no echo legs, no duplicate figure, no ring, no X, no floating crescent, no weapon, and no victim.

EVIDENCE:
The verified 1.333-second clip advances roughly 363 units and opens enlarged collision windows on the left leg and then the right leg, with two hit frames and unresolved armor/paralysis/impact payload names. Represent the clean final high kick only; do not invent an element, aura, paralysis color, shout wave, or two-hit diagram.

KHAJIIT VISUAL LANGUAGE:
Economical silhouette and material cues: agile compact build, hood with subtle ear points, one curved tail balancing the kick, warm-brown leather wraps, tan cloth, ochre bindings, narrow burgundy sash, restrained muted-gold fittings and deep-purple accent. No visible face, eyes, muzzle detail, fur portrait, anatomy showcase, or ornate costume.

STYLE AND PALETTE:
Crisp painterly graphical MMO hotbar icon optimized for 32x32. The extended leg, foot, and one connected pressure crescent dominate against a quiet deep plum-black and warm charcoal field. Ivory-white physical wake, warm brown, tan, ochre, burgundy, restrained gold. Strong silhouette, high contrast, safe crop, subtle vignette, edge-to-edge art.

HARD CONSTRAINTS:
exactly one faceless full-body Khajiit, exactly one high extended right leg, exactly one short foot-connected wake. No portrait, face close-up, extra limb, duplicate leg, second trail, weapon, shield, magic, fire, frost, lightning, blood, victim, scenery, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full-resolution result: pass. One faceless Khajiit, one planted leg, one high extended leg, and one
  pressure wake attached directly behind the leading foot form a coherent action icon.
- 32 px LANCZOS reduction: pass. The raised leg, foot impact, and connected ivory crescent remain the
  immediate read; the hood and tail retain Khajiit coding without portrait framing.
- Master SHA-256: `FEA3C978381A23596621BC0959E58796D0CF5830AC2ECD60EBB1FFB5B10E1542`.
- No autonomous regeneration was attempted.
