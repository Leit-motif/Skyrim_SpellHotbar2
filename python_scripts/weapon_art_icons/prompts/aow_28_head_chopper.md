# Head Chopper

**Status:** Hard failure after the one permitted generation attempt on 2026-08-23; not finalized

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_28_head_chopper`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Head Chopper`.
  Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Head Chopper/AABL_Attack_A.HKX`;
  SHA-256 `D6FC0A16BB1F8E41FE63F9C184FEE2A2625EDF6ECEFFEEB59F11910407293366`.
- Animation-proven: the 2.166667-second, 97-track, Dual-class clip advances about 348 units. It
  begins with simultaneous right/left swings and paired hits, then lands separate right- and
  left-hand follow-ups. No payload, collision modifier, element, or trail color is present.
- Agent composition choice: exactly two one-handed axes on a faceless Khajiit, with only the final
  left-hand chop receiving a trail. The right-hand axe remains visibly held but untrailed so four
  hit frames do not become four visual echoes.
- Atlas intent: one high right-to-left arc and a warm-brown/plum/ochre Khajiit palette followed the
  perspective ground furrow, Imperial shield wedge, and Altmer sword diagonal.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style dual-wield weapon art named "Head Chopper". This is an ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

PRIMARY GLYPH:
One—and only one—broad physical axe-cut trail dominates 82-85% of the square: a near-horizontal, slightly descending ivory-steel arc traveling from upper-right toward upper-left at head height. The active LEFT-hand axe head is at the upper-left leading end of that trail, its single cutting edge facing and leading leftward. The trail is brightest immediately behind that cutting edge and tapers back toward upper-right. One compact steel-spark compression nick appears at the leading end only. No head, target, blood, or gore.
There must be NO second trail, echo slash, X, ring, floating crescent, or detached effect.

FIGURE AND TWO WEAPONS:
One tiny faceless Khajiit dual-wielder occupies no more than 12-15% at the lower-right source end, subordinate to the single upper cut. Freeze a low forward follow-through after a rapid advance: torso twisted left, left arm extended across the body into the active chop, right arm drawn low and back.
Exactly TWO practical one-handed single-bit axes:
1. the LEFT-hand axe is the active chopper at upper-left and physically attached to the one trail;
2. the RIGHT-hand axe is clearly held low beside the figure with no trail and no effect.
Both axes have compact single cutting heads, short plausible hafts, one hand per axe. No double-bitted heads, no extra axe, no sword, shield, mace, or two-handed grip.

EVIDENCE AND SKYRIM VISUAL LANGUAGE:
The verified dual clip advances roughly 348 units, begins with simultaneous right/left swings, then performs separate right and left follow-ups. Distill the sequence to the defining final left-hand chop while keeping both required axes visible. Do not turn four hit frames into four trails.
Khajiit identity through economical full-body silhouette cues only: a small hooded head with subtle pointed-ear outline, one curved tail balancing the lunge, layered warm-brown leather and tan cloth, narrow burgundy sash, restrained antique-gold fittings. Face fully hidden in shadow; no eyes, muzzle detail, fur portrait, or costume showcase.

STYLE AND COLOR:
Bold painterly graphical MMO ability icon optimized for 32x32 readability; compact masses, strong silhouette, restrained texture. Single ivory-steel trail against a quiet deep plum-black and smoky warm-brown field; tiny figure in tan, ochre, burgundy, warm brown, and muted gold. A few physical amber sparks only at the cutting edge. No magic or elemental color. Dark vignette, safe crop, edge-to-edge art.

HARD CONSTRAINTS:
exactly one tiny faceless full-body Khajiit, exactly two one-handed single-bit axes, exactly one connected trail from the left axe, right axe untrailed. No portrait, large face, large character, second trail, crossed energy X, double-bitted axe, detached weapon, victim, severed head, blood, gore, fire, frost, lightning, rune, scenery, text, logo, watermark, border, or UI frame.
```

## Hard-gate failure

- Full-resolution result: fail. It correctly shows two axes and one trail, but the active left axe
  has a polearm-length haft and oversized crescent head while being held in one hand. That is not a
  plausible one-handed dual-wield weapon and violates the hard weapon construction field.
- 32 px reduction: the single high arc remains readable, but small-size clarity does not waive the
  incorrect left-axe scale and grip.
- Preserved ignored evidence:
  `python_scripts/weapon_art_icons/pilot/aow_28_head_chopper_hard_failure.png` and
  `aow_28_head_chopper_hard_failure_32.png`.
- Failure image SHA-256: `0CB61A03EA4DC0FC851698E8F7246499824D31581EF62A54600AC3F2C2F2FFF5`.
- Per the one-shot goal, no regeneration was attempted and no master or atlas input was created.
