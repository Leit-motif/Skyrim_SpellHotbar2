# Weapon Art icons

Original Weapon Art icon masters and atlas inputs live here.

- `source/` keeps the approved full-resolution image-generation output.
- `icons/` keeps the 128 x 128 PNG consumed by the atlas stitcher.
- `prompts/` records the approved prompt and reference roles.
- `manifest.tsv` is the handoff contract for generation, review, and later atlas wiring.
- `pilot/` contains rejected calibration outputs only. Nothing under it is atlas input.

The stable icon key is the filename stem. Do not write a key into
`data/SKSE/Plugins/SpellHotbar/artdata/arts_ashes.csv` until the generated atlas CSV contains
that key; otherwise the live catalogue would point at a missing atlas entry.

All shipped glyphs must be original. Installed SWFs and screenshots are composition references
only and are not redistributed.
