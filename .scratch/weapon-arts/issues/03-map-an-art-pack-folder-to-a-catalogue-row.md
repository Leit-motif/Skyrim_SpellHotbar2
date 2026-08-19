# 03 — Map an Art Pack folder to a Weapon Art catalogue row

Players will want each Ashes of War (or any OAR folder that replaces `AABL_Attack_A.hkx`) to show
up as its own named art in Spell Hotbar 2. Ticket 01 has one hardcoded Test Art in `arts.csv`.
This ticket grows the catalogue from folders, without copying animation files.

**Status:** resolved — catalogue + bound-art selection passed; pack *shape* is ticket 04

**Blocked by:** 01

## You test this

With Ashes of War present in the load order (Nolvus), after the generated pack is installed:

1. The bind menu Arts list (ticket 02) — or `arts.csv` if 02 is not landed yet — names more than
   Test Art. At least one real art name from the pack is visible (e.g. a named Ash).
2. Bind that art, press the slot with a weapon drawn. The clip that plays is that art, not the
   inert `AABL_Attack_A` placeholder and not a different Ash.
3. Bind a second art to another slot. Pressing each slot plays a different clip.
4. Unequip any slot-55 art clothing. The bound art still plays (selector, not worn keyword).

If every slot plays the same placeholder, or if the worn item still picks the clip, it fails.

## Agent tests the rest

5. Art Selector is 0 when no art is live. Worn-item Ashes of War behaviour still works on the
   AABL hotkey path (spec story 18).
6. Regenerating the pack against the installed Ashes of War does not copy `.hkx` files into this
   repo or the compatibility package.
7. A missing folder or renamed OAR submod logs loudly (spec story 29); the other arts still bind.

## Notes

ADR-0007: the graph always plays `Animations\AABL_Attack_A.hkx`. ADR-0008: which replacement wins
is the Art Selector global, via OAR conditions at higher priority than worn-item conditions.

Do **not** add one Nemesis path per art. Do **not** redistribute Ashes of War clips.

Authoring-time script: read installed OAR submods, emit (1) catalogue rows and (2) OAR
selector conditions. 03’s emit was `user.json` shadows on their folders. That shape is
wrong long-term (it breaks story 18 at selector 0). Ticket 04 owns the replacement.

Custom spell registration is FormID-keyed overrides of existing spells. An art is not a spell
form. Mapping a folder → catalogue row + selector + OAR override is the registration path.
An in-game “add custom art” dialog can wait until this generator has proved the data shape.

## Comments

2026-08-17: Generator lives at `python_scripts/generate_art_pack.py`. Fixture tests in
`python_scripts/generate_art_pack_test.py` (5 passing). Scan of Nolvus Stance Framework + AoW
items pack emitted 57 named ashes (stance-default `Ashes of War *` folders skipped — they have
no items-plugin keyword). Overlay: `Dev - Spell Hotbar 2 - Art Pack` (`user.json` only, 0 `.hkx`).
Catalogue also in `data/SKSE/Plugins/SpellHotbar/artdata/arts_ashes.csv`. Live: SH2ArtBind04
loaded 58 arts (Test Art + 57); `getArtSelector` is 0 at rest; slot 0 bound Elegant Slash (id 20),
slot 1 Heart Strike (id 31); `SH2_ArtStart`/`SH2_ArtExit` fired with no slot-55 AoW clothing
equipped.

2026-08-17: Clip selection is the 03 baseline (priority bump + stripped weapon gates). Remaining
pack-shape work — SH2-owned `config.json` instead of `user.json` shadows on their folders — is
ticket 04. Rooted-in-place Disengage is ticket 05, not a 03 miss.

Owner later the same day: after those two generator fixes, bound ashes played their own clips
(Elegant Slash 20, Heart Strike 31, Crane Style 8, Disengage 12) rather than Sword Neutral /
Dante. Arts list named the pack. Cell 5’s worn-item path at selector 0 was not re-proven and
is ticket 04’s reason to exist.

## Answer

A folder that replaces `AABL_Attack_A.hkx` becomes a named catalogue row. Binding it sets the
Art Selector and plays that Ash without slot-55 clothing and without copying `.hkx`. The emit
that does this is still `user.json` on their folders; replacing that emit is ticket 04.
