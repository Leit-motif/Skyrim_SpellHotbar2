# 06 — Key art and header banner

**Type:** task
**Status:** ready-for-agent
**Blocked by:** 01

Two separate images, and most pages ship neither. The main image is the tile every browsing user
sees before they see anything else; the header banner is the wide strip at the top of the page.
Mern sets both, which is a large part of why the page reads as made rather than uploaded.

## The main image: what the model actually does

Read off `185342` on 2026-08-29, at 2560x1440:

- **Flat saturated colour field**, not a screenshot. A blurred in-game shot sits behind it at low
  opacity and a large ghosted product glyph (a coat hanger) is tiled over it. The background's job
  is to be quiet.
- **A background-removed character render occupies the right third**, full body, angled, rim-lit,
  reading as a cut-out against the flat field.
- **Text lives in the left third on flat colour.** That is the whole trick — nothing competes with
  it, so it survives being scaled to a tile.
- **The lockup is two weights of one family.** Thin light "Fitting Room", a small stacked
  qualifier "ESO / STYLE" split by a rule, then heavy bold "TRANSMOG" in a lighter tint of the
  field colour.

## The brief for ours

**Composition.** Same skeleton, different subject. Right third: a cut-out of the player mid-cast
with the weapon still in the hand and the spell effect in the other — the whole product argument in
one silhouette. Left third: the lockup. Background: a deep arcane field, not purple (Mern owns
purple on this shelf right now and a near-copy reads as one); the ghosted glyph is the hotbar slot
frame itself.

**Put the bar in the image, enlarged.** The hotbar is the product and it is a UI element, which
means it reads at tile size only if it is drawn far larger than its in-game scale. Composite a
real captured strip across the lower third at three or four times its rendered size, icons
legible, with a live cooldown sweep on one slot. This is the one place the image is allowed to lie
about scale.

**Typography.** Two weights, one family, no third size. One heavy word carries the tile — pick it
deliberately, because at Nexus tile width it is the only word that survives. Candidates, pending
ticket 01's name ruling:

- thin `Spell Hotbar 2` over heavy `NG` — shortest, and the weakest, because "NG" means nothing to
  someone who does not already know the base mod
- thin `Spell Hotbar` + stacked qualifier over heavy `CAST` — states the verb
- thin `Spell Hotbar 2` + stacked qualifier over heavy `WITHOUT SWAPPING` — states the pitch, and
  is two words too long

Recommendation: whichever heavy word names the thing the mod does, not the version it is.

**The tile test is the acceptance, not a nicety.** Scale the finished image to 380 px wide and
look at it. If the heavy word is not instantly readable and the silhouette is not instantly
legible, the image failed, regardless of how it looks at full size.

**Stay consistent with the shipped icon art.** The mod ships a 57-key authored icon atlas built
against `python_scripts/weapon_art_icons/skyrim-visual-language.md`. The key art is the first thing
a user sees and the icons are what they look at for a hundred hours; a key art in a different
visual language makes the icons look like someone else's.

## The header banner

Mern's is a wide cropped strip of the live UI — roughly 1300x365 — with no text. Ours is the
hotbar strip in context, cropped so the slots and their icons fill the height. It is the cheapest
polish signal on the page: one crop from a ticket-07 screenshot.

## Acceptance

- [ ] Main image at 2560x1440, and it passes the 380 px tile test.
- [ ] The hotbar appears in it, enlarged, with legible icons.
- [ ] Header banner set, cropped from a real capture, no text.
- [ ] Neither image is a near-copy of the model page's colour or lockup.
- [ ] AI-generated elements, if any, are recorded here so ticket 09's disclosure is accurate.
