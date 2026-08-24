# Holding Thorns

**Status:** Owner-directed Morag Tong/transparent-background edit hard failure on 2026-08-24

**Generation path:** One initial Codex generation, followed by one owner-directed edit using the
initial result as the sole image reference

**Stable icon key:** `aow_34_holding_thorns`

## Evidence and interpretation

- Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Holding Thorns/AABL_Attack_A.HKX`;
  SHA-256 `C6B0AABC9F7FF871710A40869BA3BEC0CFEE77DF841B3FB3E00699289CC74711`.
- Animation-proven: the 4.000000-second, 99-track clip advances about 252 units. It begins with
  simultaneous right- and left-hand swings/hits, then alternates a dense sequence of contacts from
  both weapons through 3.033333 seconds.
- Payload resolution: the clip contains only a one-point cost marker and light camera shake. No
  thorn, plant, poison, spell, element, or colored-trail payload is present.
- Composition choice: distill the sequence to one connected Dunmer dual-blade finishing action,
  not a hit-count diagram. The physical hooked chitin blade shapes carry the name without literal
  magical thorns.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style dual-weapon art named "Holding Thorns". This is a graphical action icon, not a portrait, character key art, splash illustration, wallpaper, or scenic painting.

ACTION FIGURE:
Show exactly one faceless full-body Dunmer skirmisher driving forward from upper-left toward lower-right in one compact, low dual-blade cross-cut. Freeze one coherent finishing instant: front knee deeply bent, rear foot pushing, torso turned, shoulders compressed, both arms visibly engaged. The right hand cuts one short hooked chitin saber downward-right as the dominant leading strike; the left hand holds one shorter chitin dagger close across the torso in a tight crossing guard/cut. Both weapons must visibly connect to their own hands. The figure and action fill the square; face entirely hidden by a narrow closed Bonemold helm and deep shadow.

WEAPONS AND ONE ACTION READ:
Exactly TWO practical one-handed Dunmer chitin blades, one in each hand: one short hooked saber and one compact dagger, both with tan-brown insect-shell construction, dark edges, wrapped grips, and small practical guards. The weapon silhouettes may evoke thorn points through their hooked physical geometry, but there are NO literal plant thorns, vines, spell growths, floating spikes, or magic.
Add exactly ONE dominant short ivory-amber physical pressure wake attached directly behind the leading right-hand saber, curving only a short distance toward the center. The left dagger has NO separate luminous trail; its physical crossing pose supplies the secondary beat. One compact ochre spark cluster at the leading saber tip. No disconnected arc, no multiple slashes, no fan of trails, no X made from floating light.

EVIDENCE:
The verified 4.0-second clip advances about 252 units and alternates many right- and left-hand weapon swings and hits, beginning with both hands together and ending in rapid alternating contacts. Distill the full sequence to one connected dual-weapon finishing action; do not diagram the hit count. The clip contains no elemental, plant, poison, thorn, or colored-VFX payload, so the ability name does not authorize magical thorns.

DUNMER / ELDER SCROLLS VISUAL LANGUAGE:
Economical Morrowind-coded cues: narrow closed Bonemold helm, ash-gray cloth at joints, layered tan Bonemold/chitin cuirass and bracers with restrained insect-shell ridges, dark teal undercloth, rust-red waist wrap, aged bronze fittings. Keep the silhouette lean and severe. No visible face, red eyes, skin portrait, ornate costume, generic ninja pajamas, or oversized fantasy pauldrons.

STYLE AND PALETTE:
Crisp painterly graphical Elder Scrolls MMO hotbar icon optimized for 32x32. Strong compact silhouette, high contrast, safe crop, edge-to-edge art, subtle vignette. Palette: tan bone and chitin amber, ash gray, dark teal, rust red, aged bronze, narrow ivory-amber physical wake, against a quiet desaturated blue-gray volcanic haze field with no scenery.

HARD CONSTRAINTS:
exactly one faceless full-body Dunmer, exactly two connected one-handed chitin weapons, exactly one luminous wake attached to the leading saber. No portrait, face close-up, extra weapon, floating weapon, detached blade, third hand, extra limb, duplicate figure, multiple trails, literal thorns, vines, plants, poison, magic, fire, frost, lightning, blood, victim, scenery, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full-resolution result: pass. One faceless Dunmer, two distinct hand-connected chitin blades, one
  compact forward-driving action, and one wake terminating at the leading saber tip are coherent.
- 32 px LANCZOS reduction: pass. The dual-blade silhouette and one amber cutting wake remain clear;
  Bonemold/chitin color and shape cues survive without portrait framing.
- Master SHA-256: `0CE069B80C44A5FEB6D649E566C60D1B8C8E31289951F6C8F7D9AA656E0242FB`.
- This initial Bonemold result was superseded by the owner's request for unmistakable Morag Tong
  armor and a clear/transparent background. It is preserved only as ignored evidence at
  `python_scripts/weapon_art_icons/pilot/aow_34_holding_thorns_bonemold_superseded.png`.

## Owner-directed edit and hard failure

- Requested change: retain the action, two connected chitin blades, and one leading wake; replace
  the armor with Dunmer Morag Tong assassin armor and remove the background to true transparency.
- Pipeline validation: transparent input is supported. `stitch_icon_atlas.py` reads RGBA, preserves
  each icon alpha channel, multiplies it by the project alpha mask, and writes DXT5 DDS.
- Edit result: the Morag Tong armor and weapon construction passed visually, but the provider baked
  a gray checkerboard into an opaque RGB image instead of returning alpha.
- Provider result mode: `RGB`; alpha channel: absent; corner pixel: `(235, 235, 234, 255)` after
  RGBA conversion. This is fake transparency and cannot be accepted as an atlas input.
- Failed result preserved at
  `python_scripts/weapon_art_icons/pilot/aow_34_holding_thorns_morag_tong_fake_transparency.png`.
- No canonical master or 128 px atlas input remains. No autonomous retry was attempted.
