# 02 — Package the archive

**Type:** build
**Status:** resolved 2026-08-29 — delivered by `../mco-integration/issues/59-release-packaging-ships-no-nemesis-tree.md` in `5d1227b` and `e03bc50`, on main.

**Do not start work here.** A parallel session built this while the release plan was being written.
This ticket exists now only to record what landed and what it changed for the rest of the effort.

## What shipped

- `python_scripts/build_mod_release.py` — the lever. Output goes to gitignored `build/`: the `.zip`
  plus a `.manifest.json` listing every file with its SHA-256 and its classification against the
  base mod. 159 files, 155 additions, three overwrites (`SpellHotbar2.dll`, `SpellHotbar.pex`,
  `SpellHotbarMCM.pex`).
- `deploy/release/release.json` — the single identity and path config point.
- `deploy/release/README.template.md` and `docs/agents/release-packaging.md` — the operating manual.
- `papyrus/Scripts/*.pex` committed, with `DESKTOP-AMRIT` stripped out of them.
- A real-name guard, with `public_identities` naming what it must *not* flag.

## What it did to the rest of this effort

**It closed two of ticket 01's acceptance cells.** `nemesis/Nemesis_Engine/mod/shtb/info.ini` and
`mod/shcr/info.ini` both read `author=Leitmotives` — verified, not assumed. And the build now
enforces it rather than leaving it to a checklist.

**It solved ticket 02's own addition better than this ticket asked.** Name and version are
parameters in `release.json`, and `identity_frozen: false` stamps `-provisional` into the archive
filename so a verification build cannot be mistaken for an upload candidate. Ticket 01's naming
decision is now a two-field edit rather than a hunt.

**It pinned the base version.** `base_mod.supported_version` is `0.0.14`, upstream commit
`f203cd2`. Ticket 08's known-limits paragraph and ticket 09's day-one sticky both need that number
and can now read it instead of deriving it.
