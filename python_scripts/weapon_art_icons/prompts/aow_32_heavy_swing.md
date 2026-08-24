# Heavy Swing

**Status:** Finalized by goal-mode hard gate on 2026-08-24

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_32_heavy_swing`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Heavy Swing`. Selected
  enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Heavy Swing/AABL_Attack_A.HKX`;
  SHA-256 `2D5979C1DF28B28E4CEA2DE800CAF63A2D93C89AF773AC3D2296848B9BD22F36`.
- Animation-proven: the 2.500000-second, 97-track, 2H-class clip advances about 183 units. It has an
  early hit frame at 0.533332, then a clear weapon swing at 1.399986 and hit at 1.600000 carrying
  `$ES_Strikefly`.
- Payload resolution: no active `$ES_Strikefly` definition was found. Its reuse across unrelated
  weapon move sets proves no element, color, or exact VFX.
- Agent composition choice: one faceless Breton with one two-handed steel claymore and one defining
  heavy backhand trail. The earlier contact is not duplicated into another visual trail.
- Atlas choice: a broad upper-right to lower-left silver sweep on burgundy-black follows the sleek
  emerald Altmer saber, horizontal Redguard thrust, and vertical Argonian hammer history.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style two-handed weapon art named "Heavy Swing". This is a graphical action icon, not a portrait, character key art, splash illustration, wallpaper, or scenic painting.

ACTION:
Show one faceless Breton heavy fighter performing one powerful two-handed claymore backhand sweep from upper-right down toward lower-left. Freeze the committed middle/end of the swing: hips and shoulders rotated together, front leg planted, rear leg driving, both hands clearly spaced on one long sword grip. The full-body figure is compact but clearly readable, about 25-30% of the square, with no visible face.

WEAPON AND ONE TRAIL:
Exactly ONE practical Skyrim-style two-handed steel claymore: one blade, one hilt, two hands on the same grip, all visibly connected, plausible proportions. The blade's cutting edge leads the lower-left travel direction.
Exactly ONE broad silver-white physical slash wake follows immediately behind and tangent to that same blade, sweeping from upper-right to lower-left. It is brightest beside the blade and fades backward. Add one compact pressure burst and a few steel sparks at the lower-left endpoint only.
No second trail, echo slash, duplicate blade, detached weapon, ring, X, or floating effect.

EVIDENCE:
The verified 2H clip advances about 183 units and contains an earlier contact followed by one clear late weapon swing/hit with an unresolved generic launch payload. Represent the defining heavy swing only. Do not invent an element, magic, color payload, or multiple-hit diagram.

BRETON VISUAL LANGUAGE:
Closed faceless bascinet-style helm; simplified mail and plate; royal-blue surcoat, burgundy sash, charcoal leather, dark steel, restrained muted-gold trim. Equipment is compact icon shorthand, not an armor showcase. No spellcasting.

STYLE AND PALETTE:
Crisp painterly graphical MMO ability icon optimized for 32x32. The connected claymore and single heavy slash dominate the silhouette; the figure explains the action. Quiet burgundy-black and charcoal field, silver-white wake, royal blue, dark steel, restrained muted gold and amber contact sparks. Strong contrast, safe crop, subtle vignette, edge-to-edge art.

HARD CONSTRAINTS:
exactly one faceless full-body Breton, exactly one connected two-handed claymore, exactly one connected slash wake. No portrait or face close-up, no hammer, mace, axe, shield, second weapon, multiple trails, magic, fire, frost, lightning, blood, victim, scenery, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full-resolution result: pass. One connected claymore, two clear grips, one broad tangent sweep,
  and one lower-left endpoint form a mechanically coherent action icon.
- 32 px LANCZOS reduction: pass. The silver sweep and claymore remain dominant; the closed-helm
  Breton reads as a compact action source rather than a portrait.
- Master SHA-256: `99FE591C585C47FD06847BC66B0E0B85D2B7786A38BBB3FE6A65543484728254`.
