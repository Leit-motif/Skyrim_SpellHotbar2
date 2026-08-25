# Dash Slam

**Status:** Finalized by owner on 2026-08-23

**Generation path:** Codex built-in image generation plus guided silhouette edits

**Stable icon key:** `aow_11_dash_slam`

## Evidence and interpretation

- `AABL_Attack_A.hkx` is 2.433333 seconds long and advances roughly 686 motion units before its
  single hit.
- The hit invokes a slam, camera shake, dust, knockback, and area knockback.
- Its OAR conditions accept one-handed swords and greatswords.
- The Orsimer-coded shock trooper and iron-gray, black, dark-green, rust, and blood-red palette
  follow the owner's figure and color guidance.
- Owner-supplied Orcish armor and helmet images guided only broad silhouette language. They are not
  copied into the repository or redistributed.

## Reference roles

- Approved Blood Flurry, Crushing Blow, and Cyclone Spin masters: project hotbar grammar, safe
  crop, action hierarchy, controlled background color, and 32 px readability only.
- Owner-supplied Orcish armor and helmet images: silhouette reference only—layered angular plates,
  tall blade-ridge helmet, flared cheek guards, and a trailing crest-tail.

## Final correction prompt

The initial generation established the diagonal dash and impact. Later edits established original
Orsimer armor and corrected the helmet geometry. The final correction added the missing crest-tail:

```text
Make ONE precise helmet-silhouette correction to the first referenced Dash Slam hotbar icon. Preserve every existing element exactly: airborne diagonal pose, body and armor, pointed pauldrons, current helmet face geometry and tall central blade-ridge, arms, hands, greatsword, speed wedge, green field, orange-red impact, debris, lighting, palette, and crop.

REFERENCE ROLE:
The second image shows the missing broad silhouette cue only. Do not copy its screenshot, exact mesh, face, texture, panel layout, or proprietary asset.

ADD THE MISSING HELMET TAIL:
From the rear/top of the existing tall central helmet ridge, add one clearly attached narrow crest-tail that follows the motion of the dash. It should:
- emerge visibly from the back of the helmet crown, with no gap;
- rise a short distance behind the blade-ridge, then trail backward toward the upper-left;
- extend roughly one helmet-height beyond the rear of the crown so it is unmistakable at 128x128;
- consist of 3 or 4 dark iron-black leather/hair strips gathered at the base and tapering to ragged points;
- form one clean readable trailing silhouette, separate from the shoulder spikes and silver dash streaks.
The tail is flexible and swept by motion, not another metal horn, spike, wing, ribbon from the body, or floating line. Keep it dark with a restrained iron-gray rim so it remains visible against the speed wedge and at 32x32.

Do not redesign the helmet again and do not alter anything outside this new attached crest-tail. No text, runes, logos, emblems, borders, or new objects.

CELL 5 CONSTRAINT: original generated icon; supplied image is silhouette reference only and is not copied or redistributed.
```
