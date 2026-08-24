# Leap Slam

**Status:** Finalized by owner-directed researched Elder Argonian headdress correction on 2026-08-24

**Generation path:** One initial Codex generation, followed by one owner-directed edit using the
initial result as the sole image reference

**Stable icon key:** `aow_37_leap_slam`

## Evidence and interpretation

- Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Leap Slam/AABL_Attack_A.HKX`;
  SHA-256 `9F7DAB66B603B3F72FD075626C684C1CB06937ADB5198632C8A88E9FEE17D612`.
- Animation-proven: the 2.833333-second, 97-track 2H clip lands an initial hit, then surges from
  near-zero motion to about 305 forward units before a second hit at 1.666665. The terminal hit
  carries unresolved AoE knockback plus explicitly named dust and camera shake.
- Initial composition choice: show only the terminal leap-slam landing. One Argonian, one
  two-handed poleaxe, one descending physical wake, and one compact dust impact avoid a two-hit
  diagram.
- Owner correction: research Argonian armor and weapon styling, remove the helmet, and replace the
  generic dragon-plate language with an uncovered Dead-Water/Murkmire Argonian.
- Owner refinement: use a researched Argonian feathered headdress rather than a fully bare natural
  crest. A targeted official-source pass resolved the compact Elder Argonian open-circlet form.
- Research record: `.scratch/weapon-arts/research/argonian-armor-weapon-language.md`, using official
  ESO Murkmire concept art and official ZOS material references.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style two-handed weapon art named "Leap Slam". This is a graphical action icon, not a portrait, character key art, splash illustration, wallpaper, or scenic painting.

Show exactly one faceless full-body Argonian heavy skirmisher at the terminal instant of a forward leap slam, descending from upper-right toward lower-left. Both knees bend beneath the body, tail sweeps upward-right, torso folds over the weapon, and both hands are spaced clearly on one long haft driving the poleaxe head into the lower-left endpoint. Hide the reptilian face behind a closed shell-plated helm and deep shadow.

Exactly one practical two-handed Argonian poleaxe: one connected marshwood haft held by both hands and one compact single-edged bronze-darksteel axe head with a short rear hook. Add exactly one short ivory-turquoise pressure wake tangent to and behind the descending edge, running from upper-right into the connected lower-left impact. Add one compact fan-shaped ochre dust-and-stone burst at the axe head. No circular shockwave ring, second trail, echo weapon, extra slash, or detached explosion.

The verified 2.833333-second 2H clip lands an initial hit, then surges forward to roughly 305 units before a second impact carrying unresolved AoE knockback plus named dust and camera shake. Depict only the defining terminal landing. Dust and physical impact are supported; no element, spell, aura, or colored magic is proven.

Use economical Black Marsh cues: closed layered river-shell helm, dark bronze and blackened scale plates over reed-brown mail, muted turquoise wraps, coral-red sash, bone fasteners, tarnished brass, and one balancing tail. Crisp painterly Elder Scrolls MMO icon optimized for 32x32 against deep blue-green/charcoal.

HARD CONSTRAINTS: exactly one faceless full-body Argonian, exactly one connected two-handed poleaxe, exactly one downward connected wake, exactly one compact ground impact. No portrait, second weapon, floating weapon, detached axe head, hammer, extra limb, duplicate figure, multiple trails, shockwave ring, magic, fire, frost, lightning, blood, victim, scenery, text, logo, watermark, border, or UI frame.
```

## Initial result

- The initial helmeted generic-plate version was superseded by explicit owner direction. It is
  preserved as ignored evidence at
  `python_scripts/weapon_art_icons/pilot/aow_37_leap_slam_helmeted_superseded.png`.

## Owner-directed researched edits

The edit preserved the leap, camera, one poleaxe, one descending wake, and one compact impact while
applying these researched constraints:

The first researched edit established the Dead-Water armor/weapon construction but left the head
fully bare. The owner then specified a feathered headdress and required another research pass. That
bare-head intermediate is preserved at
`python_scripts/weapon_art_icons/pilot/aow_37_leap_slam_bare_head_superseded.png`.

Final model-facing headdress block from the official-source research:

```text
No helmet. One open Elder Argonian brow circlet: a narrow deep-purple band above the eyes with one small aged-bronze forehead fitting and one restrained turquoise cabochon; exactly five medium cream, rust, and dark-brown feathers fixed along the rear half of the band and swept backward/upward by the leap. Long reptilian snout, mouth, jaw, throat, and scaled side profile fully exposed; only one or two short natural side horns visible, no second crest, skullcap, mask, hood, radial halo, or towering ceremonial plume.

Preserve the researched Dead-Water heavy skirmisher armor, exactly one connected two-handed wrapped-marshwood hooked poleaxe with two grips, one upper-right-to-lower-left leap-slam, one weapon-connected ivory-teal wake, and one compact ochre impact.
```

## Final hard-gate result

- Full-resolution result: pass. The exposed snout/jaw, open purple brow circlet, bronze/turquoise
  fitting, five rear-swept feathers, lean shell/scute armor, moss cloth, bone shoulder, wrapped
  marshwood haft, and asymmetric hooked poleaxe form a researched Argonian synthesis.
- Exactly one connected poleaxe remains in two clear grips, with one descending wake and one compact
  impact; no helmet or extra weapon appears.
- 32 px LANCZOS reduction: pass. The feather wedge, exposed head, descending poleaxe, and
  turquoise-white wake remain distinct.
- Master SHA-256: `6BF6AF23B18197999D2411F30FA49203E351FABE7B6180C331E7B679586BEF7D`.
- No autonomous regeneration beyond the owner-directed edit was attempted.
