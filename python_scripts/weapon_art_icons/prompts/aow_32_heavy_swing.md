# Heavy Swing

**Status:** Finalized by owner-directed Orsimer/battleaxe rework on 2026-08-24

**Generation path:** One Codex generation followed by one owner-directed edit using the initial
result only for action/composition continuity

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
- Initial agent composition choice: one faceless Breton with one two-handed steel claymore and one
  defining heavy backhand trail. The earlier contact was not duplicated into another visual trail.
- Final owner direction: replace the Breton/claymore with an Orsimer in Elder Scrolls-coded Orcish
  mail and one two-handed battleaxe while retaining the single heavy sweep.
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

## Initial result

- The initial Breton/claymore result was mechanically connected and readable, but the owner
  explicitly replaced its race, armor, and weapon direction. It is not the approved master.
- Superseded image SHA-256: `99FE591C585C47FD06847BC66B0E0B85D2B7786A38BBB3FE6A65543484728254`.
- Preserved ignored evidence:
  `python_scripts/weapon_art_icons/pilot/aow_32_heavy_swing_breton_superseded.png`.

## Owner-directed edit prompt

```text
Rework the supplied square Heavy Swing MMO ability icon. Preserve the basic action, camera, single lower-right-to-lower-left heavy sweep, one connected weapon trail, endpoint sparks, dark vignette, and 1:1 composition. Replace the Breton knight and claymore completely with one Elder Scrolls–coded Orsimer warrior and one two-handed battleaxe.

ORSIMER / ORCISH MAIL:
The full-body figure is a faceless Orsimer in Skyrim-style Orcish mail: dark interlinked chainmail visible at elbows, waist, and thighs beneath angular overlapping orichalcum-green/blackened-iron plates; heavy asymmetric shoulder plates with restrained hooked/tusk-like ridges; dark-green rough cloth, black-brown leather straps, rust-red sash. This must read as Tamrielic Orcish armor construction, not generic bright green skin, Warcraft armor, Roman armor, or a human knight. Keep the face entirely hidden by a closed angular Orcish helm and shadow; no facial portrait or visible tusks.

WEAPON:
Exactly ONE practical two-handed Orcish battleaxe, all parts connected: one long dark wooden haft held by BOTH hands spaced apart, and one compact heavy SINGLE-BIT orichalcum axe head at the lower-left leading end. The axe has one dominant cutting blade only, with the cutting edge facing and leading toward lower-left. No claymore, sword, hammer, double-bitted axe, second weapon, detached head, or one-handed grip.

TRAIL:
Exactly ONE broad ivory-steel physical axe wake follows immediately behind and tangent to the leading cutting edge, curving from upper-right down to lower-left. Keep one compact rust-white endpoint burst at the axe head. No extra trails, echoes, rings, or magic.

Make it a crisp painterly Elder Scrolls MMO hotbar action icon readable at 32x32. Orsimer palette: blackened iron/orichalcum dark green, chainmail steel, black, dark green cloth, rust and restrained blood-red; ivory-steel wake. No text, no border, no logo, no scenery, no victim, no gore.
```

## Final hard-gate result

- Full-resolution result: pass under explicit owner direction. Elder Scrolls-coded Orcish mail,
  one compact single-bit battleaxe head, one long haft with two clear grips, and one tangent heavy
  sweep remain physically coherent.
- 32 px LANCZOS reduction: pass. The single battleaxe sweep and dark Orsimer silhouette remain the
  immediate read.
- Master SHA-256: `A9D519A0EFCC65FF5D969CD212EC4A1272CB45F9EA7B88257A14E3014802BCE2`.
