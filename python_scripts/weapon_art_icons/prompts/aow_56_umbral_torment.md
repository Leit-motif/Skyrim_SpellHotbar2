# Umbral Torment

**Status:** One-shot hard failure recorded on 2026-08-24

**Stable icon key:** `aow_56_umbral_torment`

## Evidence and interpretation

- Selected provider: `Ashes of War - Weapon Art Via Additional Attack/.../Umbral Torment/AABL_Attack_A.HKX`; SHA-256 `DB224AD82769236531CA87E398E682A84019D033EAAA1E598F9F71660B784DDA`.
- Animation-proven: the 3.0-second, 99-track 2H clip advances about 185 units and makes one hit; collision scale is 1.9, damage multiplier 3, and trail color is explicitly overridden to `(1, 0.35, 0.1, 1)`.
- Power-charge, paralysis, and hit-fog names are unresolved; no shadow, torment aura, darkness, or fog is supported.
- Composition choice: one Dunmer Redoran greatsword strike with one payload-proven ember-orange physical wake.

## Generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for an Elder Scrolls two-handed weapon art named "Umbral Torment". This is an effect-first action glyph, not a portrait, key art, wallpaper, or scene.

Make exactly ONE hot ember-orange physical weapon wake dominant: a thick compact diagonal arc upper-left to lower-right, brightest behind one greatsword edge, tapering backward, ending at blade tip in one orange-white impact. Color is explicit payload evidence, RGB approximately 1.0, 0.35, 0.1. Not fire magic, black shadow, multiple trails, ring, X, sphere, or detached crescent.

Show exactly one small faceless full-body Dunmer Redoran warrior completing one heavy two-handed diagonal strike. Front foot planted, rear leg braced, knees bent, hips and shoulders rotated. Both hands close around one continuous long hilt with spacing, lead behind guard and rear near pommel; wrists neutral, elbows bent. Hide face inside closed Redoran bonemold helm.

Exactly ONE connected two-handed Dunmer greatsword: broad dark ebony-steel blade with straight double edge, reddish-bronze fuller, compact chitin/bonemold guard, long ash-gray grip, one pommel. Both hands hold it. No second weapon, floating blade, detached hilt, axe, scythe, or one-handed grip. Lower edge leads orange wake.

The verified 3.0-second, 99-track 2H clip advances about 185 units and makes one hit. Collision scale is 1.9, damage 3, and trail color hot orange-red. Other names unresolved. Depict one enlarged high-force physical strike and proven orange trail; no shadow magic, torment aura, darkness, fog, or paralysis color.

Use Redoran construction: reddish-brown bonemold, dark mail, ash-gray wraps, brick-red sash, chitin shoulder, dark boots, bronze fittings. Palette: bonemold brown, ash gray, ebony, brick red, bronze, one hot ember-orange wake against cool slate-blue and clay. Crisp painterly graphical MMO hotbar icon optimized for 32x32.

HARD CONSTRAINTS: exactly one faceless Dunmer Redoran warrior, exactly one connected two-handed greatsword, exactly one blade-connected orange wake. Anatomically possible stance, grip, joints, leverage, balance, edge direction, and trail only. No portrait, second weapon, floating blade, detached hilt, one-handed grip, extra arm, impossible wrist, hand on blade, multiple trails, shadow, darkness, black aura, fog, paralysis glow, literal flames, blood, victim, scenery, text, logo, watermark, border, or UI frame.
```

## Hard-gate result

- Physics/anatomy: pass. Both hands, continuous hilt, shoulders, legs, sword edge, and one orange wake form a coherent heavy strike.
- Framing/effect: hard failure. Literal mountains, battlefield ground, and extensive fiery spectacle make scenic key art and read as fire magic despite the brief limiting the orange to a physical trail.
- 32 px remains legible but cannot rescue the prohibited framing/effect invention.
- Failure SHA-256: `DC46ABBB748F64A37FD982E2EEB9DF57C3CE8E221EE19EA407D7E5190FAE289C`.
- Evidence is preserved under ignored `pilot/`; no master or atlas input exists and no retry was attempted.
