# 59 — Release packaging ships no Nemesis tree at all

**Type:** build (packaging). **Status:** needs-triage. **Found:** 2026-08-29, checking ticket
58's "ships unconditionally" ruling (credit: session d0's read of the release scripts).

## The gap

Neither release path carries a `Nemesis_Engine` tree:

- `python_scripts/create_fomod_installer.py` enumerates its payload explicitly
  (`released_files_main_plugin_v2`, ~line 420) — no `Nemesis_Engine` entry. Verified
  2026-08-29: zero `Nemesis` references in the file (ticket 58's retire pass removed the last
  one, the `shcc` staging that was itself the only Nemesis payload ever listed).
- `build_release_package.py` — same, and it still points at `F:\Skyrim Dev\ADT` (upstream's
  machine), so it has not built a release on this machine at all.

Consequence: **`shtb` — the cast states the whole mod runs on — is not in any release either.**
The gap predates ticket 58. Local installs never noticed because `Dev - Spell Hotbar 2` carries
`Nemesis_Engine/` on disk (deployed by hand/agent), but a user installing a built release would
get a mod whose behavior states do not exist.

Caveat from the finder, still true: only those two scripts were read. If there is a manual
packaging step or another path that carries `nemesis/`, this ticket collapses to documenting
it — check for that before building.

## The ruling that makes this load-bearing

Owner, 2026-08-29 (ticket 58): no FOMOD, `shcr` ships unconditionally — "seamlessness and
commitment also applies to magic (mco type rules)". So the main install payload must carry
`nemesis/Nemesis_Engine/mod/shtb/` AND `mod/shcr/` (and `nemesis/SKSE/` — check what else under
`nemesis/` is install-layout), the way the retired `shcc` staging carried its folder: the repo
`nemesis/` layout is already the install layout.

## Acceptance

- [ ] Establish whether any other packaging path exists (a manual step, another script, a
      release archive on disk to diff against). If one ships the tree, document it here and
      close.
- [ ] Otherwise: the main install payload includes `Nemesis_Engine/mod/shtb` and `mod/shcr`
      (no FOMOD gate), plus whatever else under `nemesis/` belongs in the install.
- [ ] A freshly built release archive byte-contains the `shcr` and `shtb` patch files
      (verify the archive, not the script).
- [ ] Install docs / FOMOD description say Nemesis must be run after install (Update Engine
      not required for end users — selection change only — but Launch is).
