# Head Tap

**Status:** Finalized by owner-directed Altmer/Alinorian figure rework on 2026-08-24

**Generation path:** One initial Codex generation, two owner-directed full reworks, and one
owner-directed simplifying edit using only the preceding Altmer result as its source

**Stable icon key:** `aow_29_head_tap`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Head Tap`. Selected
  enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Head Tap/AABL_Attack_A.HKX`;
  SHA-256 `B989B53C54D14856AD62A1C65B88A8A4B6B446F8A31C4C5352281C570D245ED2`.
- Animation-proven: the 3.333333-second, 99-track, 2H-class clip advances about 232 units and
  contains six weapon swings/hits from 0.166666 through 1.766666 seconds. It then triggers
  `$ES_HeavyStrikefly` at 1.866666; `$CostWarAshPoint1` occurs at the start.
- Payload resolution: no active definition was found for either named payload. Installed Argonian
  warhammer, battleaxe, and greatsword move-set annotations reuse `$ES_HeavyStrikefly`, supporting
  a heavy physical launch interpretation but proving no element, color, or exact VFX.
- Agent composition choice: one Argonian with one two-handed rectangular steel warhammer, one short
  vertical trail, and one final downward contact. Six hit events are not multiplied into trails.
- Atlas choice: a frontal vertical foreshortened hammer and swamp-green/teal/dark-brown palette
  follow the failed high axe arc, perspective furrow, and Imperial diagonal shield wedge.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style two-handed weapon art named "Head Tap". This is an ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

PRIMARY GLYPH:
One—and only one—compact downward warhammer strike dominates 85-88% of the square. The practical rectangular steel hammer head is the largest crisp object near the lower-center, shown at the instant it delivers one precise downward contact into a tiny ivory-white compression point. A single short vertical steel-white motion trail extends directly upward behind that same hammer head, brightest immediately behind it and tapering toward the top. One restrained circular puff of physical dust and a few orange steel sparks spread from the exact contact point.
No second trail, no repeated hammer echoes, no rings, no floating effect, no target head, no victim, and no gore.

FIGURE, GRIP, AND CAMERA:
Low frontal three-quarter camera looking slightly upward along the hammer path. One tiny faceless Argonian two-handed fighter occupies no more than 10-12% near the upper-right source end, subordinate to the hammer head and vertical strike. Freeze the final compact follow-through: body crouched and leaning down, shoulders stacked over the haft, both hands clearly spaced on one long grip, elbows bent, hammer axis vertical into the contact.
Exactly ONE practical Skyrim-style two-handed warhammer: one simple rectangular iron/steel head, one plausible long wooden haft, two hands on the same haft. No axe blades, pick spike, sword, shield, second weapon, detached head, or oversized fantasy ornament.

EVIDENCE AND SKYRIM VISUAL LANGUAGE:
The verified 3.333-second 2H clip advances roughly 232 units and contains six timed swings/hits before an unresolved heavy-strike launch payload. Depict only the defining final downward hit and one motion trail, not six hits or six trails. The named payloads do not prove an element or color.
Argonian identity through economical full-body silhouette/material cues only: small closed hood/helm with a subtle backward crest outline, narrow tail balancing the crouch, layered dark-brown leather and swamp-green cloth, restrained teal stitching and one muted orange sash. Face fully hidden; no eyes, scales, snout detail, skin close-up, or costume showcase.

STYLE AND COLOR:
Bold painterly graphical MMO ability icon optimized for 32x32 readability; compact graphic masses, high contrast, restrained texture. Dominant ivory-white/steel vertical strike against a quiet deep swamp-green and charcoal-black field; tiny source figure in dark brown, swamp green, restrained teal, and muted orange. No magical glow, fire, frost, lightning, poison, or rune. Dark vignette, safe crop, edge-to-edge art.

HARD CONSTRAINTS:
exactly one tiny faceless full-body Argonian, exactly one two-handed warhammer with two hands on one haft, exactly one short connected vertical trail, one physical contact. No portrait, face, large character, multiple trails, repeated hammer, giant ornate weapon, victim, head, blood, gore, spell, scenery, text, logo, watermark, border, or UI frame.
```

## Initial result and owner rejection

- The initial full-resolution result was mechanically connected and readable at 32 px, but the
  owner rejected it because a single planted hammer did not communicate the animation-proven
  six-hit combo. It is not an approved master.
- Rejected image SHA-256: `F331BC6BFF317E7365CC85A245894FB79A6D13061DBA6A2D893D4590C6F53DC4`.
- Preserved ignored evidence: `python_scripts/weapon_art_icons/pilot/aow_29_head_tap_owner_rejected.png`.

## Owner-directed full rework prompt

```text
Create ONE completely new square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style six-hit two-handed combo named "Head Tap". Do not preserve or imitate any prior composition. This is an effect-first ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

DEFINING SIX-HIT COMBO GLYPH:
The ability must instantly read as a rapid SIX-CONTACT advancing hammer combo.
Show exactly SIX compact physical impact beats arranged along ONE continuous rising zigzag/S-curve that advances from lower-left toward upper-right:
- five smaller ivory-white compression bursts behind,
- one sixth and largest terminal burst at upper-right.
Each of the six beats is a tight physical contact star/puff with a little dust and 1-2 stone chips, not a weapon trail, not a floating rune, and not a separate slash.
Connect the six beats with one thin broken steel-gray motion path so they read as successive timing marks of one rapid combo. The connected six-beat pattern occupies 85-90% of the square and is the icon.

ONE REAL WEAPON, FINAL BEAT:
At the sixth/largest upper-right contact, show exactly ONE real practical two-handed warhammer head physically touching that final burst. One long wooden haft runs back from that head to both hands of the source figure. The hammer has one compact rectangular steel head, one haft, two hands; all parts visibly connected.
The five earlier impact beats contain NO extra hammer heads, no weapon afterimages, and no duplicate weapons. They are only fading contact dents left by the preceding hits.

TINY SOURCE FIGURE:
One tiny faceless Argonian fighter occupies no more than 10-12% at lower-left, subordinate to the six impact beats. Low advancing stance, body leaning up-right, both hands spaced on the single hammer haft driving the final strike. Full body reduced to silhouette cues: closed dark helm/hood with subtle crest, small balancing tail, dark-brown leather, swamp-green cloth, restrained teal binding and muted orange sash. No face, eyes, scales, snout detail, anatomy display, or costume showcase.

EVIDENCE AND COLOR:
The verified 3.333-second clip advances about 232 units and delivers six timed weapon hits before an unresolved heavy-launch payload. Therefore show exactly six contact beats and no invented element. Nonmagical ivory-white compression, steel gray, ochre dust, charcoal stone, restrained orange sparks; deep swamp-green/black background with subtle vignette. No fire, frost, lightning, poison, blood, magic aura, or rune.

STYLE:
Bold painterly graphical MMO hotbar icon optimized for 32x32 readability; compact graphic masses, high contrast, clean negative space, restrained texture, edge-to-edge art, safe crop. The six-beat cadence and larger final impact must survive reduction.

HARD CONSTRAINTS:
exactly six impact beats; exactly one actual connected two-handed warhammer at the sixth beat; exactly one tiny faceless full-body Argonian; no extra weapons, no floating hammer, no separate slash trails, no large character, no portrait, no victim, no head target, no blood, no gore, no scenery, no text, logo, watermark, border, or UI frame.
```

## Rework hard-gate failure

- Full-resolution result: fail. It communicates six ascending contact beats, but the terminal
  hammer consists of a head and short wrapped stub floating at the sixth burst. No continuous haft
  reaches the Argonian's hands, so weapon and action causality are broken.
- 32 px reduction: the six-beat cadence remains visible, but small-size readability does not waive
  the detached terminal hammer.
- Preserved ignored evidence:
  `python_scripts/weapon_art_icons/pilot/aow_29_head_tap_rework_hard_failure.png` and
  `aow_29_head_tap_rework_hard_failure_32.png`.
- Rework image SHA-256: `D7EB76F7D8571951D981FCF569BD80F3BC079AABD17077F7AF63CEF9584F720F`.
- At that stage no master or atlas input remained; the owner then supplied the Altmer directions
  recorded below.

## Second owner direction

The owner rejected the hammer/Argonian direction entirely and directed a High Elf (Altmer) using
an Alinorian saber, with a sleek, smooth visual treatment. This owner direction overrides the
catalogue `2H` suggestion and the agent's prior heavy-weapon composition choice; the six-hit clip
remains the mechanical source.

## Superseded Altmer/Alinorian six-beat prompt

```text
Create ONE completely new square 1:1 raster image: a sleek polished MMO/RPG hotbar ABILITY ICON for a six-hit Altmer saber combo named "Head Tap". This is an effect-first ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

OWNER-DIRECTED IDENTITY:
A High Elf (Altmer) Alinorian saber technique: elegant, fast, controlled, smooth. No hammer, no mace, no axe, no Argonian, no bulky armor, no ground smash, no huge rocks.

PRIMARY SIX-HIT GLYPH:
ONE single continuous silk-smooth saber motion ribbon sweeps from lower-left, curves gracefully through the center, and rises to upper-right in an elegant elongated S-curve. The ribbon is one narrow ivory-white stroke with pale-gold edges, never multiple parallel slashes.
Embed exactly SIX small bright contact beats along that same continuous ribbon: five restrained pearl-like compression glints spaced along the earlier path and one sixth slightly brighter final glint at upper-right. The six beats are subtle timing accents within one smooth combo line, not six explosions, not six starbursts, and not floating runes.
The single S-ribbon and its six glints occupy 85-90% of the square and must remain readable at 32x32.

ONE CONNECTED SABER AND TINY ALTMER:
At the sixth/final upper-right glint, show exactly ONE real Alinorian saber physically held in the right hand of one tiny faceless Altmer duelist. The narrow slightly curved ivory-steel blade with restrained gold Alinorian filigree lies tangent to the ribbon and leads the final cut. Its hilt is visibly inside the duelist's closed right hand; blade, hilt, hand, arm, and body are all connected.
The Altmer occupies no more than 10-12% near upper-right: tall narrow silhouette compressed into a smooth forward finishing step, left arm extended back for balance, face fully hidden by a sleek closed gold-and-ivory helm. No second weapon, shield, staff, spellcasting, or detached blade.

ALTMER MATERIAL AND PALETTE:
Economical Alinorian visual cues: slim articulated antique-gold armor masses, ivory cloth, deep emerald sash, pale-yellow edge accents. Radiant aristocratic palette but no holy magic and no elemental spell. Quiet deep emerald-black background with soft gold vignette; only a few tiny physical pale-gold sparks at the sixth contact. No dust explosion, rubble field, fire, frost, lightning, blood, heart, or gore.

EVIDENCE:
The verified clip delivers six timed hits during a forward advance. Preserve that identity through six subtle beats on one continuous saber path. Do not turn it into one generic strike, six separate trails, or multiple weapons.

STYLE:
Crisp painterly graphical MMO hotbar icon, compact elegant masses, smooth calligraphic motion, high contrast, restrained texture, safe crop, edge-to-edge art. Ability/effect first; tiny faceless source figure only explains causality.

HARD CONSTRAINTS:
exactly one continuous S-shaped ribbon; exactly six subtle glints on that one ribbon; exactly one connected Alinorian saber; exactly one tiny faceless Altmer. No hammer, mace, axe, Argonian, extra weapon, floating saber, parallel slash trails, large character, portrait, face, explosions, giant rocks, victim, blood, text, logo, watermark, border, or UI frame.
```

## Superseded Altmer six-beat result

- This result is mechanically connected and readable at 32 px, but the owner rejected its long
  six-beat S path as over-indexing on the combo count. It is not the approved master.
- Superseded image SHA-256: `01E5BF69634C7B0354B6798D94E4B4D4353EBE578AA8AF0480141B65E887D328`.
- Preserved ignored evidence:
  `python_scripts/weapon_art_icons/pilot/aow_29_head_tap_altmer_six_beat_superseded.png` and
  `aow_29_head_tap_altmer_final_32.png`.

## Final owner direction: use the figure

The owner directed that the six-hit encoding be ignored and that the Altmer figure itself carry
the icon. The final edit retains one faceless Altmer, one held Alinorian saber, and one smooth
connected saber wake, with no counted beats.

## Final simplifying edit prompt

```text
Rework the supplied square MMO ability icon by using the existing upper-right Altmer/High Elf Alinorian saber figure as the sole character and action source. Ignore and remove the six-hit concept completely. Delete the entire long S-shaped ribbon, every pearl-like glint/contact beat, the large lower-left flare, and all extra decorative gold ribbons. Recompose the same sleek faceless Altmer duelist and held Alinorian saber into a clean dynamic action-bar icon: place the full-body figure around the right-center, still compact and subordinate, in the same elegant forward saber cut. Add only ONE smooth short ivory-gold saber wake directly behind and tangent to the single held blade, sweeping from lower-left toward the blade at upper-right. No counted beats, no dots, no multiple trails, no floating blade, no second weapon. Preserve the slim antique-gold armor, ivory cloth, deep emerald sash, closed faceless helm, and deep emerald-black palette. The saber hilt must remain visibly inside the right hand and blade, hilt, hand, arm, and body must remain connected. Crisp painterly MMO hotbar glyph, sleek and smooth, readable at 32x32, no portrait framing, no text, no border, no hammer, no Argonian, no debris explosions.
```

## Final hard-gate result

- Full-resolution result: pass under explicit owner direction. One sleek faceless Altmer figure,
  one held Alinorian saber, and one smooth ivory-gold wake form the complete icon. No hammer,
  Argonian, counted glints, duplicate trail, or floating weapon remains.
- 32 px LANCZOS reduction: pass. The gold-green leaping saber silhouette and single curved wake
  remain clean and readable.
- Master SHA-256: `49E74530AA16D941009BF180A1675587D6F5D3B0DD9A1A4F24F2AEC226E15A61`.
