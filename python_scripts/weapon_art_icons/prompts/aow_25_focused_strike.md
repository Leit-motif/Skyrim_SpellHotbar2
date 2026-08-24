# Focused Strike

**Status:** Finalized by goal-mode hard gate on 2026-08-23

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_25_focused_strike`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Focused Strike`.
  Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Focused Strike/AABL_Attack_A.HKX`;
  SHA-256 `8723BCD3FAFB392EFC84B0BE315D85D10F22166696C28F8FEECE622C802D0B0D`.
- Animation-proven: the 3.500000-second, 99-track, 1H-class clip contains three swings and hits at
  0.433316/0.566644, 0.866632/0.983294, and 1.916590/2.166580 seconds. It advances about 246 units
  with negligible rotation; the final beat carries jump-attack sound, camera shake, `$HitFog`, and
  `$ES_Paralysis`.
- Payload resolution: no active definitions were found for `$HitFog` or `$ES_Paralysis`. Their
  element, color, spell identity, and exact VFX remain unresolved and were not invented.
- Agent composition choice: the three timed hits are distilled to one defining final low lunge and
  one connected descending trail. A faceless Altmer fighter with one one-handed elven sword uses
  race-guided gold, ivory, emerald, and deep-green material cues without implying magic.
- Atlas choice: one upper-left to lower-right diagonal contrasts with Flurry Strike's ground sweep,
  Focused Cross's intersecting axes, and Enrage (M)'s upward fan.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style one-handed weapon art named "Focused Strike". This is an ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

PRIMARY GLYPH:
One—and only one—razor-focused physical sword strike dominates the square: a single narrow ivory-white descending diagonal slash traveling from upper-left toward a compact contact point at lower-right. The one trail is brightest immediately behind the sword's cutting edge and tapers backward along that same recent path. It must be visibly attached to and caused by the blade. The slash, blade, and compact contact flare occupy about 85-88% of the icon. No duplicate trails, echo slashes, secondary arcs, rings, or floating effects.

ACTION AND FIGURE:
Freeze the decisive final beat of a forward lunge: one tiny faceless Altmer fighter occupies no more than 12-15% near the upper-left/center, body low and driving diagonally down-right behind the strike, front knee deeply bent, rear leg extended, right arm reaching into the cut. The figure exists only to explain the strike and must remain a simple full-body silhouette, never a subject.
Exactly ONE practical Skyrim-style one-handed elven sword in the right hand, normal length and proportions, one grip, empty off-hand, no shield. The blade lies directly on the bright descending slash axis, with its cutting edge leading toward the lower-right contact. No second weapon and no detached blade.

SKYRIM VISUAL LANGUAGE:
Altmer identity through economical silhouette/material cues only: tall narrow proportions compressed into the lunge, closed faceless helm, simplified golden elven plate masses over deep emerald cloth and ivory accents. No face, eyes, hair, skin close-up, armor showcase, ornate character detail, or spellcasting.
The animation proves a roughly 246-unit forward sequence with three hits and a decisive final low lunge. Depict only the single defining final strike, not three hits or three trails. Named paralysis and fog payloads are unresolved, so add no magical element, aura, rune, mist spell, or colored energy.

STYLE AND COLOR:
Bold painterly graphical MMO ability icon optimized for 32x32 readability; high contrast, clean silhouette, restrained texture. One ivory-white steel slash and small pale-gold physical sparks at contact against a quiet deep emerald-black field; tiny figure in muted antique gold, ivory, and deep green. Subtle dark vignette, safe crop, edge-to-edge icon art. No border, no UI frame, no text, no letters, no numbers, no logo, no watermark.

HARD CONSTRAINTS:
exactly one tiny faceless full-body figure; exactly one one-handed sword; exactly one connected slash trail; one compact physical contact point. No portrait framing, large character, large face, second weapon, three slashes, trail echoes, floating crescent, X symbol, halo, fire, frost, lightning, purple magic, victim, gore, scenery, text, or border.
```

## Final hard-gate result

- Full-resolution result: pass. One sword, one continuous diagonal strike, and one compact contact
  dominate; there are no duplicate trails or detached effects. The Altmer is faceless and shown in
  action rather than portrait framing.
- 32 px LANCZOS reduction: pass. The ivory diagonal, sword, and gold-green source silhouette remain
  one clean ability glyph.
- Master SHA-256: `B163DE143AE9E3715FE261A68F4291D5046ED212E72F710A9255AC9E8CB68709`.
