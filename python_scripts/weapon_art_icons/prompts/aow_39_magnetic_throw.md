# Magnetic Throw

**Status:** Finalized by goal-mode hard gate on 2026-08-24

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_39_magnetic_throw`

## Evidence and interpretation

- Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Magnetic Throw/AABL_Attack_A.HKX`;
  SHA-256 `5CBDB6B1F1E764AB83B7C9FDE356A2F953DFA15E1AF927514E0623DC87FC15DE`.
- Animation-proven: the 2.333333-second, 97-track Generic clip begins with a left-hand sprint-power
  action, makes one early contact, retreats about 68 units, then makes seven rapid contacts on the
  same WEAPON node. One swing carries unresolved `$BlueAttackHit01`.
- Composition choice: one thrown-and-returning weapon, not eight copies. One Breton Dwemer-salvage
  skirmisher, one brass disc, and one unbroken blue-white hand-to-hub return line express the move.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style weapon art named "Magnetic Throw". This is a graphical action icon, not a portrait or key art.

Show exactly one faceless full-body Breton Dwemer-salvage skirmisher braced at lower-left, recoiling after a throw, right hand open toward upper-right. Exactly one compact spinning brass throwing disc sits at the upper-right endpoint.

The disc is one joined Dwemer-metal ring-blade with a continuous outer rim, solid inner hub, and three radial braces. Connect the open right hand to the disc with exactly one continuous narrow blue-white magnetic return ribbon beginning at the palm and ending at the hub. One compact contact glint on the rim. No second arc, orbit rings, repeated discs, projectile row, echo weapon, or lightning branches.

The verified 2.333333-second Generic clip makes one early contact, retreats about 68 units, then triggers seven rapid contacts on the same WEAPON node; one swing carries unresolved `$BlueAttackHit01`. Distill this to one thrown weapon on one return path, not eight weapons or trails. The name and blue marker support restrained magnetic-return shorthand, not an element storm.

Use practical High Rock/Dwemer salvage cues: dark fitted mail, brown leather brigandine, compact brass shoulder/forearm plates, muted cobalt cloth, cream wraps, wine-red belt accent. Crisp painterly Elder Scrolls MMO icon optimized for 32x32 against deep umber/desaturated blue.

HARD CONSTRAINTS: exactly one faceless full-body Breton, exactly one brass throwing disc, exactly one continuous hand-to-disc ribbon. No portrait, second weapon, duplicate disc, floating unrelated weapon, shield, gear cloud, projectile row, multiple trails, branching lightning, fire, frost, blood, victim, scenery, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full-resolution result: pass. One Breton source, one joined brass disc, and one continuous palm-to-
  hub return line make the separation mechanically intentional rather than a floating extra weapon.
- 32 px LANCZOS reduction: pass. The disc, blue return line, and open hand remain the immediate read.
- Master SHA-256: `95D951F286D2E277A15CB68C2ACA2D56C718085AFC597BA1F56E78D642683E7D`.
- No autonomous regeneration was attempted.
