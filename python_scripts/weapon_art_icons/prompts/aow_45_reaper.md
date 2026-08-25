# Reaper

**Status:** One-shot hard failure recorded on 2026-08-24

**Stable icon key:** `aow_45_reaper`

## Evidence and interpretation

- Selected provider: `Ashes of War - Weapon Art Via Additional Attack/.../Reaper/AABL_Attack_A.HKX`; SHA-256 `816FF37D56C5A9D16BF29EFF6C4629CD0777A904A0B0E8F5352529B9AB890174`.
- Animation-proven: the 1.916667-second, 97-track Generic clip makes three weapon contacts; each explicitly triples trail lifetime and base-color intensity, and the terminal hit carries knockback, shake, and AoE knockback.
- The name does not prove death, soul, shadow, or necromancy magic.
- Composition choice: one Dunmer Ashlander, one connected crescent glaive, and one persistent physical wake.

## Generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for an Elder Scrolls weapon art named "Reaper". This is an effect-first action glyph, not a portrait, character key art, wallpaper, or scene.

Make exactly ONE long bright physical cutting wake the dominant icon shape: a broad open crescent sweeping from lower-left upward and around toward upper-right, brightest immediately behind one moving blade and tapering backward. It is one continuous wake, not three trails, a ring, skull, soul, or detached crescent.

At lower-center show exactly one small faceless full-body Dunmer Ashlander completing the terminal two-handed reap from left toward right. Feet wide and planted, front knee bent, rear leg braced, hips and shoulders rotating together; lead hand closed near the glaive head and rear hand near the butt with wide spacing; neutral wrists and bent elbows transmit force. Hide the face with a close ash-cloth hood and chitin half-mask.

Exactly ONE connected two-handed Dunmer chitin crescent glaive: long dark marshwood shaft, one practical forward-facing crescent chitin blade with dark metal cutting edge, copper lashings, ash-gray grip wraps, one butt cap. Both hands hold the same shaft. The cutting edge leads the wake and blade tip meets its bright endpoint.

The verified 1.916667-second, 97-track Generic clip makes three contacts. Every collision triples trail lifetime and base-color intensity; the terminal hit carries knockback, shake, and AoE knockback. Distill it to one persistent physical wake and one terminal impact. The name proves no death magic, darkness, souls, or necromancy.

Use Elder Scrolls Dunmer Ashlander construction: layered ash-gray and dark umber wraps, lean reddish-brown chitin, muted rust sash, copper clasps, dark boots. Palette: ash gray, chitin brown, dark umber, muted rust, copper, one bright ivory-gray wake against a subdued ochre-violet volcanic field. Crisp painterly graphical MMO hotbar icon optimized for 32x32.

HARD CONSTRAINTS: exactly one small faceless Dunmer, exactly one connected two-handed crescent glaive, exactly one blade-connected persistent wake. Anatomically possible grip, leverage, stance, and cutting direction only. No portrait, second weapon, floating scythe, detached head, double-ended weapon, extra arm, impossible wrist, hand on blade, multiple trails, perfect ring, skull, soul, ghost, darkness magic, blood, victim, scenery, text, logo, watermark, border, or UI frame.
```

## Hard-gate result

- Physics/anatomy: pass. Both hands, shaft, blade, stance, edge, and one wake form a coherent reap.
- Full-resolution framing: hard failure. The requested quiet color field became literal volcanic mountains, lava, flying rocks, and a scenic panorama, pushing the result into fantasy key-art framing expressly forbidden by the brief.
- 32 px result remains visually legible but cannot rescue the framing violation.
- Failure SHA-256: `6545A69FC0D74A50AE56DFB841F72589605F3EB2080213031EC6B50C83A12198`.
- Full and 32 px evidence are preserved under the ignored `pilot/` tree. No master or atlas input exists, and no autonomous regeneration was attempted.
