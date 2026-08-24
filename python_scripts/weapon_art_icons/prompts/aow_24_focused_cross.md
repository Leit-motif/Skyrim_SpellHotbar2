# Focused Cross

**Status:** Hard failure after the one permitted generation attempt on 2026-08-23; not finalized

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_24_focused_cross`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Focused Cross`.
  Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Focused Cross/AABL_Attack_A.HKX`;
  SHA-256 `11F5F72265B5D70973FC78AE2C3B7A2019C45742A5A46456E6A05F13045FC148`.
- Animation-proven: the 2.333333-second, 99-track clip contains one `weaponSwing` at 0.500000
  seconds and one `HitFrame` at 0.700000 seconds. It advances about 313 units with negligible
  lateral displacement. No payload, colored trail, or collision annotation is present.
- Agent composition choice: a tiny Dunmer bonemold soldier with one practical one-handed mace.
  The name was represented by one descending swing crossing the straight physical dash wake at
  their shared contact point, not by two weapons or an independent X emblem.
- Atlas intent: a descending upper-right mace path crossing a horizontal rush axis followed rising
  Elegant Slash, rotational Enrage (F), and upward-frontal Enrage (M). Charcoal, ash, bonemold
  brown, muted red, and steel-white broadened the recent palette without inventing an element.

## Final generation prompt

```text
Create ONE square 1:1 raster image: a polished MMO action-bar ability icon for a Skyrim-style weapon art named "Focused Cross". This is an ABILITY ICON, not a portrait, not character key art, not a character illustration.

ICON MECHANIC AND COMPOSITION:
The dominant readable subject is a concentrated physical impact shaped by the intersection of TWO CONNECTED MOTION CUES: (1) one narrow, straight horizontal dash-wake driving from left toward a contact point slightly right of center, and (2) one short descending mace-swing arc cutting from upper-right toward that exact same contact point. Their physical intersection forms a focused cross-like burst at impact. It must read as a forward rush plus ONE weapon blow, not as a floating X emblem, crossed weapons, holy magic, or a spell.
Make the intersecting dash wake, mace arc, compressed dust, and hard steel sparks occupy 80-85% of the icon. The burst at their shared contact point is the focal point. Keep the outer silhouette bold and readable at 32x32 pixels.

FIGURE:
A tiny, faceless Dunmer bonemold soldier occupies only about 15-20% of the icon near the lower-left/center, subordinate to the motion effect. Athletic crouched forward-release pose, seen at three-quarter distance, face fully hidden by angle, shadow, and helmet. Ash-gray Dunmer identity should be a subtle silhouette/costume cue, never a facial close-up.
Exactly ONE practical one-handed flanged steel mace, held in the right hand, one dominant mace head, normal proportions, clearly connected to the descending swing arc. No second weapon, no axe, no sword, no shield, no staff, no oversized fantasy weapon.

STYLE AND COLOR:
High-contrast painterly MMO ability icon, compact graphic masses, crisp silhouette, dramatic but materially physical. Deep charcoal-black and volcanic ash background; bonemold/chitin brown and muted dark red accents on the tiny Dunmer; ivory-white compressed dash wake, cold steel-gray swing arc, restrained orange steel sparks only at contact. No elemental magic, no fire aura, no lightning, no runes.
Lighting and visual hierarchy prioritize the intersecting physical trails and impact, not the character. Edge-to-edge icon art with subtle dark vignette. No border, no UI frame, no text, no letters, no numbers, no logo, no watermark.

HARD NEGATIVES:
not a portrait; not splash art; not a full-body hero poster; no large face; no beauty shot; no centered character; no two weapons; no crossed weapons; no floating X symbol; no disconnected effects; no giant mace; no ornate staff; no elemental spell; no gore; no scenery; no readable text.
```

## Hard-gate failure

- Full-resolution result: fail. The figure holds a long haft that reaches the central impact while
  a separate mace head appears on the upper descending arc. The two pieces do not form one weapon,
  so the generated result depicts a disconnected/impossible mace construction despite otherwise
  strong effect-first framing.
- 32 px reduction: the intersecting physical axes remain highly readable, but small-size
  legibility does not waive the disconnected weapon or broken causality.
- Preserved ignored evidence:
  `python_scripts/weapon_art_icons/pilot/aow_24_focused_cross_hard_failure.png` and
  `aow_24_focused_cross_hard_failure_32.png`.
- Failure image SHA-256: `1C3313082EE18FA9141A201F71CC16388C91307DECF898EBF7ED9F4E8B6265DA`.
- Per the one-shot goal, no regeneration was attempted and no master or atlas input was created.
