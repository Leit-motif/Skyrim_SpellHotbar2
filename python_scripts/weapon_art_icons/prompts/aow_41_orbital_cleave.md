# Orbital Cleave

**Status:** Finalized by owner-directed anatomical correction on 2026-08-24

**Stable icon key:** `aow_41_orbital_cleave`

## Evidence and interpretation

- Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Orbital Cleave/AABL_Attack_A.HKX`; SHA-256 `0B27223239A767BE66FB7F6152644EAB6C6F5F99A90FF68F8346C0CCCE9657DC`.
- Animation-proven: the 4.0-second, 99-track Generic clip keeps one WEAPON collision active from 0.949999 to 2.166666, makes one swing and one hit near 2.2 seconds, and advances about 131 units.
- Unresolved endurance, heavy-paralysis, and Rim-enchantment names do not establish an element or magical aura.
- Composition choice: one Redguard two-handed curved blade and one open, blade-connected orbital wake express the named action without multiplying weapons or rings.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for an Elder Scrolls weapon art named "Orbital Cleave". This is an action glyph, not a portrait, character key art, wallpaper, or scenic painting.

Show exactly one small faceless full-body Redguard two-handed swordsman at center-left completing one powerful horizontal orbiting cleave. Use a high three-quarter camera so the hips, shoulders, both hands, one weapon, and its circular path are mechanically clear. Freeze the terminal instant: knees bent, weight over the front foot, torso tightly rotated, both hands spaced on the grip, blade crossing the foreground from right toward left. Hide the face behind a dark wrapped desert veil and low hood; no visible eyes or portrait framing.

Exactly ONE connected two-handed Redguard curved greatsword: a long practical single-edged dark-steel blade with a moderate forward curve, bronze spine, compact crescent guard, indigo-wrapped long grip, and one pommel. Both hands visibly hold the same grip. The blade is neither a scimitar-sized one-hander nor a scythe.

Add exactly ONE broad continuous ivory-gold physical cutting wake tangent to and immediately behind the blade edge. It sweeps around the fighter in one near-circular orbital path, but remains open at the figure and visibly terminates at the moving blade; it is not a detached halo or perfect ring. Make the blade and this one orbiting wake the immediate 32x32 read. One compact ochre spark at the leading tip only.

The verified 4.0-second, 99-track Generic clip keeps one WEAPON collision active from 0.949999 to 2.166666, makes one swing and one hit at about 2.2 seconds, and advances roughly 131 units. Unresolved endurance, paralysis, and Rim enchantment names do not prove an element or magical aura. Depict one sustained physical cleave and one terminal contact, not multiple hits.

Use Elder Scrolls Redguard martial language: dark indigo lamellar-and-leather cuirass over sand-colored wraps, compact bronze shoulder and forearm guards, crimson sash, practical fitted boots, subtle geometric stitching. Palette: deep indigo, dark steel, warm bronze, sand, muted crimson, and one ivory-gold wake against a quiet teal-black and burnt-umber field. Crisp painterly graphical MMO hotbar icon optimized for 32x32, edge-to-edge, high contrast, safe crop.

HARD CONSTRAINTS: exactly one faceless full-body Redguard, exactly one connected two-handed curved greatsword, exactly one blade-connected orbital wake. No portrait, second weapon, floating sword, detached blade, duplicate figure, perfect halo, multiple rings, extra trails, magic storm, fire, frost, lightning, paralysis glow, victim, blood, scenery, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- The initial result failed owner review because its two-handed grip was anatomically and
  mechanically implausible. It is preserved at
  `python_scripts/weapon_art_icons/pilot/aow_41_orbital_cleave_bad_grip_superseded.png`.

## Owner-directed correction prompt

```text
Redo the supplied Orbital Cleave MMO ability icon while preserving its strongest identity: one faceless Redguard, one curved two-handed greatsword, the indigo/bronze/sand palette, and one bright circular blade-connected physical wake. Correct the entire pose and grip so the weapon physics are anatomically possible.

The sword must have one continuous long hilt behind the guard, visibly long enough for two hands. Put the lead hand fully closed around the leather-wrapped grip immediately behind the guard. Put the rear hand fully closed around the same grip near the pommel, with clear space between the hands. Both thumbs and knuckles align for the same horizontal cut; both wrists remain neutral rather than bent backward. No finger, palm, or hand touches the blade, spine, guard, empty air, or pommel knob. Both forearms connect naturally to those hands, elbows flex within normal range, shoulders rotate with the torso, and the planted legs visibly counterbalance the sword's forward mass.

Freeze one plausible terminal horizontal cleave from left toward right: hips and shoulders rotated together, front knee bent, rear foot braced, sword edge leading its path. The one ivory-gold wake must be tangent to and immediately behind the moving cutting edge, remain visibly connected to the sword tip, and sweep around the body as one open near-circle. It may not hide the corrected hands or hilt.

Keep it a crisp graphical Elder Scrolls MMO hotbar icon at 1:1, with the full-body action subordinate to the sword and one wake. Exactly one figure, one anatomically gripped connected sword, and one connected wake. No second blade, floating weapon, detached hilt, extra hand, missing finger mass, crossed wrists, hand on blade, perfect halo, extra trail, magic element, portrait, text, logo, border, or scenery.
```

## Corrected hard-gate result

- Full-resolution result: pass. Both hands close around one continuous hilt with plausible spacing;
  wrists, elbows, shoulders, hips, planted legs, blade edge, and trailing wake agree on one horizontal
  two-handed cleave.
- 32 px LANCZOS reduction: pass. The bright open orbit and the dark curved blade remain a strong
  distinct glyph with no duplicate weapon.
- Corrected master SHA-256: `3AD0FAEFE2EC4B3CEDCC6D243983FAE52F2ADB88C01CB98450FA14A370BFFFF8`.
- No autonomous regeneration beyond the explicit owner-directed correction was attempted.
