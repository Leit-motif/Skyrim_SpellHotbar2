# Magnetic Throw

**Status:** Finalized by owner-directed Altmer arcane-trickster replacement on 2026-08-24

**Generation path:** One initial Codex generation, followed by one owner-directed edit using the
initial result as the sole image reference

**Stable icon key:** `aow_39_magnetic_throw`

## Evidence and interpretation

- Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Magnetic Throw/AABL_Attack_A.HKX`;
  SHA-256 `5CBDB6B1F1E764AB83B7C9FDE356A2F953DFA15E1AF927514E0623DC87FC15DE`.
- Animation-proven: the 2.333333-second, 97-track Generic clip begins with a left-hand sprint-power
  action, makes one early contact, retreats about 68 units, then makes seven rapid contacts on the
  same WEAPON node. One swing carries unresolved `$BlueAttackHit01`.
- Initial composition choice: one Breton Dwemer-salvage skirmisher, one brass disc, and one
  unbroken blue-white hand-to-hub return line.
- Owner correction: the disc/gadget concept looked wrong. Replace it with an arcane-trickster
  Altmer assassin throwing one blue psychic/telekinetic shadow blade that returns magnetically.

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

- The initial Breton/Dwemer-disc result was superseded by explicit owner direction. It is preserved
  at `python_scripts/weapon_art_icons/pilot/aow_39_magnetic_throw_dwemer_disc_superseded.png`.

## Owner-directed edit prompt

```text
Replace the armored Breton/Dwemer-disc concept with one original Altmer arcane assassin at lower-left telekinetically throwing exactly one magical Alinorian shadow blade toward upper-right. Use fitted midnight-indigo Alinor assassin leather/cloth with blackened-gold geometric plates, muted teal sash, angular hood with pointed-ear silhouette, and a fully shadowed face.

Exactly one forearm-length single-edged Alinorian assassin blade: dark metal, shallow curve, pale-gold spine, compact crescent guard, wrapped grip, one pommel. Wrap it in restrained cobalt-blue psychic energy. Connect the open casting palm to the blade pommel with exactly one smooth continuous icy-blue/cobalt magnetic return tether. One projectile and one return path represent all repeated same-node contacts.

No brass disc, chakram, ring weapon, second blade, blade swarm, duplicate projectile, extra trail, pink, magenta, violet psychic energy, branching lightning, portrait, victim, or scenery. Make the blue blade and palm-to-pommel tether the immediate 32x32 read.
```

## Final hard-gate result

- Full-resolution result: pass. One faceless Altmer action source, one connected physical/magical
  blade, and one continuous cobalt palm-to-pommel tether form a coherent throw-and-recall ability.
- 32 px LANCZOS reduction: pass. The blue blade, one return line, open hand, and crouched assassin
  remain distinct; no gadget-disc read survives.
- Master SHA-256: `CC476264C957CDFE16786C5002CD0512632B24E5970DE9928D109643C53C8992`.
- No autonomous regeneration beyond the owner-directed edit was attempted.
