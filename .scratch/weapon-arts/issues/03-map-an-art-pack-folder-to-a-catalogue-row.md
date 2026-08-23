# 03 — Map a clip folder to a Weapon Art catalogue row

Players will want each special MCO attack in a folder — Ashes of War's tree is the obvious pile —
to show up as its own named art. Ticket 01 has one Test Art row and an inert `AABL_Attack_A`
placeholder. This ticket grows the catalogue from folders of real clips.

**Status:** resolved — catalogue + bound-art selection passed; pack *shape* is ticket 04

**Blocked by:** 01

## You test this

After catalogue rows exist for more than Test Art (generated from an installed folder of MCO
attack HKX, Ashes of War or otherwise):

1. The Arts list (ticket 02, or `arts.csv` + `slotArt` if 02 is not landed) names more than Test
   Art.
2. Bind one row, press with a weapon drawn. The clip that plays is that row's HKX, not the inert
   placeholder and not a different row's clip.
3. Bind a second art to another slot. Each slot plays a different clip.
4. No slot-55 art clothing is required. Unequipping such an item does not change the bound art.

If every slot plays the same placeholder, or if a worn Ashes of War item still picks the clip, it
fails.

## Agent tests the rest

5. Art Selector is 0 when no SH2 art is live.
6. Generating rows does not copy `.hkx` into this repo; files stay in the load-order VFS.
7. A missing file logs loudly (spec story 29); other arts still bind.

## Notes

ADR-0011: a Weapon Art is any MCO-annotated attack clip. SH2 / PIE own the machinery. Ashes of
War is a content source, not AABL-hotkey / worn-keyword machinery. Do not require the filename
`AABL_Attack_A.hkx`. Do not redistribute clips.

`ArtDefinition` needs a clip path (or equivalent) so a row names a file. How that file reaches
`SH2_Art_State`'s clip generator (OAR onto a SH2-owned placeholder vs registered clip names) is
this ticket's implementation choice — not one Nemesis generator per art unless that is proven
necessary.

Do **not** add one Nemesis path per art. Do **not** redistribute Ashes of War clips.

Authoring-time script: read installed OAR submods, emit (1) catalogue rows and (2) OAR
selector conditions. 03’s emit was `user.json` shadows on their folders. That shape is
wrong long-term (it breaks story 18 at selector 0). Ticket 04 owns the replacement.

Custom spell registration is FormID-keyed overrides of existing spells. An ability is not a spell
form. Folder → catalogue row that points at an existing HKX is the registration path.

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
Ability Selector and plays that Ash without slot-55 clothing and without copying `.hkx`. The emit
that does this is still `user.json` on their folders; replacing that emit is ticket 04.
