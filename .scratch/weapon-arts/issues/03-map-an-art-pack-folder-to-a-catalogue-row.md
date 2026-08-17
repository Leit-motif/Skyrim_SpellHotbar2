# 03 — Map a clip folder to a Weapon Art catalogue row

Players will want each special MCO attack in a folder — Ashes of War's tree is the obvious pile —
to show up as its own named art. Ticket 01 has one Test Art row and an inert `AABL_Attack_A`
placeholder. This ticket grows the catalogue from folders of real clips.

**Status:** ready-for-agent

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

ADR-0009: a Weapon Art is any MCO-annotated attack clip. SH2 / PIE own the machinery. Ashes of
War is a content source, not AABL-hotkey / worn-keyword machinery. Do not require the filename
`AABL_Attack_A.hkx`. Do not redistribute clips.

`ArtDefinition` needs a clip path (or equivalent) so a row names a file. How that file reaches
`SH2_Art_State`'s clip generator (OAR onto a SH2-owned placeholder vs registered clip names) is
this ticket's implementation choice — not one Nemesis generator per art unless that is proven
necessary.

Custom spell registration is FormID-keyed overrides of existing spells. An art is not a spell
form. Folder → catalogue row that points at an existing HKX is the registration path.
