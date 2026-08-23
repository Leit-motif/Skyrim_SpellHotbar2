# Divided Strike

**Status:** Shipped by owner under time constraint on 2026-08-23; accepted as usable, not as the
preferred visual result

**Generation path:** Codex built-in image generation plus guided pose and trail edits

**Stable icon key:** `aow_13_divided_strike`

## Evidence and interpretation

- The animation is a three-beat dual-weapon sequence: off-hand collision, main-hand collision,
  then both weapon nodes collide in the terminal beat.
- The shipped glyph uses a forward dual-wield lunge and an ordinary luminous X cross-slash.
- The owner explicitly said the result was not what they wanted but chose to ship it to avoid
  spending more time. Preserve that distinction in future review.

## Reference roles

- Approved Weapon Art masters: project hotbar grammar, safe crop, action hierarchy, controlled
  background color, and 32 px readability only.
- Earlier Divided Strike candidates: edit targets used to develop the moving lunge and cross-slash.
  No third-party artwork is stored or redistributed.

## Shipped prompt

```text
Use case: precise-object-edit
Asset type: square fantasy game hotbar icon
Input images: Image 1 is the edit target.
Primary request: Keep the same armored warrior lunging forward, but make the attack an ordinary
dual-weapon cross-slash.
Action geometry: two clean diagonal slashes crossing once to form a normal, unmistakable X, like
the multiplication symbol x. One slash runs upper-left to lower-right; the other runs upper-right
to lower-left. The strokes should be nearly straight with only the slight natural arc of a sword
swing. No S-curves, C-curves, loops, parentheses, hourglass, butterfly, or decorative trail shapes.
Physical coherence: one sword creates one slash and one dagger creates the other. Each blade and
arm must visibly follow its slash direction; the warrior is actively completing both cuts.
Style: preserve Image 1's dark-fantasy painted icon style, faceless figure, dark armor, red-magenta
trails, bright central crossing, background, lighting, framing, and forward motion.
Constraints: exactly two weapons, exactly two slash trails, exactly one crossing. Change only the
weapons, arms where necessary, and trails. No text, runes, border, logo, watermark, extra limbs,
extra weapons, or floating trails.
```
