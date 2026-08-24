# Leap Slam

**Status:** Finalized by goal-mode hard gate on 2026-08-24

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_37_leap_slam`

## Evidence and interpretation

- Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Leap Slam/AABL_Attack_A.HKX`;
  SHA-256 `9F7DAB66B603B3F72FD075626C684C1CB06937ADB5198632C8A88E9FEE17D612`.
- Animation-proven: the 2.833333-second, 97-track 2H clip lands an initial hit, then surges from
  near-zero motion to about 305 forward units before a second hit at 1.666665. The terminal hit
  carries unresolved AoE knockback plus explicitly named dust and camera shake.
- Composition choice: show only the terminal leap-slam landing. One Argonian, one two-handed
  poleaxe, one descending physical wake, and one compact dust impact avoid a two-hit diagram.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style two-handed weapon art named "Leap Slam". This is a graphical action icon, not a portrait, character key art, splash illustration, wallpaper, or scenic painting.

Show exactly one faceless full-body Argonian heavy skirmisher at the terminal instant of a forward leap slam, descending from upper-right toward lower-left. Both knees bend beneath the body, tail sweeps upward-right, torso folds over the weapon, and both hands are spaced clearly on one long haft driving the poleaxe head into the lower-left endpoint. Hide the reptilian face behind a closed shell-plated helm and deep shadow.

Exactly one practical two-handed Argonian poleaxe: one connected marshwood haft held by both hands and one compact single-edged bronze-darksteel axe head with a short rear hook. Add exactly one short ivory-turquoise pressure wake tangent to and behind the descending edge, running from upper-right into the connected lower-left impact. Add one compact fan-shaped ochre dust-and-stone burst at the axe head. No circular shockwave ring, second trail, echo weapon, extra slash, or detached explosion.

The verified 2.833333-second 2H clip lands an initial hit, then surges forward to roughly 305 units before a second impact carrying unresolved AoE knockback plus named dust and camera shake. Depict only the defining terminal landing. Dust and physical impact are supported; no element, spell, aura, or colored magic is proven.

Use economical Black Marsh cues: closed layered river-shell helm, dark bronze and blackened scale plates over reed-brown mail, muted turquoise wraps, coral-red sash, bone fasteners, tarnished brass, and one balancing tail. Crisp painterly Elder Scrolls MMO icon optimized for 32x32 against deep blue-green/charcoal.

HARD CONSTRAINTS: exactly one faceless full-body Argonian, exactly one connected two-handed poleaxe, exactly one downward connected wake, exactly one compact ground impact. No portrait, second weapon, floating weapon, detached axe head, hammer, extra limb, duplicate figure, multiple trails, shockwave ring, magic, fire, frost, lightning, blood, victim, scenery, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full-resolution result: pass. One airborne Argonian, one connected poleaxe with two clear grips,
  one descending wake, and one compact dust impact form a coherent terminal leap-slam action.
- 32 px LANCZOS reduction: pass. The descending figure/weapon axis and turquoise-white impact wake
  remain immediate and distinct.
- Master SHA-256: `283D5C0200CEE53A191162838717514C22A4706A7EE430D4A1161FFEC2EE8854`.
- No autonomous regeneration was attempted.
