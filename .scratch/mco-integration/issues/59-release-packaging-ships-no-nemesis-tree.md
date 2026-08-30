# 59 — Build a release package for OUR mod (overwrite over base SH2)

**Type:** build (packaging). **Status:** done 2026-08-29 — archive built and verified from
the working tree; publication stays gated on the permission flag. Re-scoped earlier the same
day after the owner's distribution ruling. Originally titled "release packaging ships no
Nemesis tree" — that framing was wrong; see the correction below.

## Correction to the original lead

The first version of this ticket read the missing `Nemesis_Engine` entries in the release
scripts as a regression ("shtb never shipped"). It is not one. The entire `nemesis/` tree is
ours — first commit `207df5e` (2026-08-11, the sh2c patch later renamed shtb), every commit
Amrit's. Upstream `pWn3d1337/Skyrim_SpellHotbar2` (tip `f203cd2`, June 2025) has no Nemesis
patch at all, so its release scripts have nothing to carry and we have never run them
(`build_release_package.py` still points at `F:\Skyrim Dev\ADT`, pWn3d's machine). No user is
missing anything they once had. The gap is simply that OUR fork has no release path yet.
(Credit: session d0's history read, correcting its own earlier lead.)

## The owner's ruling: our mod is an OVERWRITE

Base Spell Hotbar 2 is a hard requirement; ours installs on top and wins the conflict. Decided
2026-08-29 against the divergence numbers: 501 files differ from upstream, 134 pure additions
(Nemesis trees, data, meshes) but 25 modifications to upstream's own C++
(`casting_controller.cpp`, `animationeventhook.cpp`, `input.cpp`, `hotbar.cpp`, `plugin.cpp`,
…) plus `SpellHotbar.psc` and `SpellHotbarMCM.psc`. Overwrite is how that ships without
pretending it is purely additive.

## Package contents

- Our built `SpellHotbar2.dll`.
- The two compiled `.pex` (`SpellHotbar`, `SpellHotbarMCM`).
- `nemesis/Nemesis_Engine/mod/shtb/` and `mod/shcr/` (repo `nemesis/` layout is already the
  install layout; check what else under `nemesis/` — e.g. `nemesis/SKSE/` — belongs).
- Our data files (`data/meshes`, `data/SKSE`, …).
- Explicitly NOT upstream's untouched assets (icons, fonts, presets) — those come from the
  base install.
- No FOMOD gate on `shcr` (owner ruling, ticket 58): commitment ships unconditionally.

Upstream's two packaging scripts are layout reference at most; do not edit them into the
build path.

## Known costs, recorded now rather than discovered later

- **Overwrite couples us to a base version.** Our DLL is compiled from upstream source at our
  fork point, so any future upstream fix is silently reverted by our overwrite until we
  rebase, and a user on a newer base SH2 runs our older DLL under their newer assets. Low risk
  today (upstream idle since June 2025), but it bites silently — pin the supported base
  version in the package docs and re-check on any upstream release.
- **Permissions.** Redistributing a modified build of pWn3d's DLL is a permissions question —
  `.scratch/mco-integration/upstream-permission-issue-draft.md` exists. The owner's call,
  outside this ticket's scope; the package must not be published anywhere until it is settled.

## Acceptance

- [x] A build script in this repo produces the archive from the working tree (the lever, not a
      hand-assembled zip), enumerating our-files-only; upstream-untouched assets provably
      absent. `python_scripts/build_mod_release.py`. Trees come from `git ls-files`, so nothing
      untracked can leak in. Absence is proved by byte-comparison against the installed base
      mod rather than by a hand-written denylist: a member identical to a base file classifies
      REDUNDANT and fails the build. Proven by injection, both directions.
- [x] The archive byte-contains the `shtb` and `shcr` patch files and both `.pex`. The verify
      pass reopens the zip and compares every member's SHA-256 against its source: 80 shtb
      files, 14 shcr files, both `info.ini`, both `.pex`.
- [x] The DLL in the archive is the current build (hash-match against `skse_plugin/build`
      output). `3c18ed8a79aeb6ae5f5fd8ea28618278fbcc8be0b3ff87dbcac8f15e86f4512d`.
- [x] Install docs: base SH2 required, install ours after/over it, run Nemesis (Launch; end
      users need no Update Engine for a selection-only change), supported base version pinned
      at `0.0.14`. `deploy/release/README.template.md` renders into the archive;
      `docs/agents/release-packaging.md` is the repo-facing manual.
- [x] Publication explicitly deferred until the upstream-permission question is settled by the
      owner. `publication_blocked` in `deploy/release/release.json`; the build prints it and
      the manifest records it.

## Comments

### 2026-08-29 — built

`build/Spell Hotbar 2 NG 0.1.0-provisional.zip`, 159 files, 2.96 MB. 155 additions, 3
overwrites (`SpellHotbar2.dll`, `SpellHotbar.pex`, `SpellHotbarMCM.pex`), 0 redundant. A
`.manifest.json` lands beside it with a SHA-256 and a classification per file.

Four things the ticket did not specify, decided here:

**No Papyrus compiler exists on this machine** — Nolvus ships no Creation Kit, and nothing on
disk answers to `PapyrusCompiler.exe`. The build imports the two `.pex` from the deployed dev
mod (`--refresh-pex`) into `papyrus/Scripts/`, now tracked, and records the SHA-256 of the
`.psc` each was compiled from in `papyrus/Scripts/compiled.json`. Every build re-checks that
hash, so an edited `.psc` fails the build instead of shipping a stale `.pex`.

**`data/SKSE/Plugins/SpellHotbar/localization/translation.txt` is excluded.** It is base's
English translation with exactly one typo fixed (`Globald Cooldown` -> `Global Cooldown`) and
a trailing newline. The base FOMOD installs the user's chosen language under that same
filename, so shipping ours forces English on every non-English user to fix one word — and the
DLL's compiled default already carries the corrected string plus the ten keys the file lacks.
The script prints the exclusion and this reason on every run.

**Both `info.ini` files read `author=Amrit Chana`.** That is the leak release ticket 01's
acceptance names, and it would have shipped in this archive, so it is fixed here:
`author=Leitmotives`, the Nexus identity from ticket 01's table. Written byte-level, CRLF
preserved. The build now scans every packaged text file for committer names read out of
`git log` and fails if it finds one — the guard, not the fix, is the durable part. The
deployed MO2 copy still carries the old string until it is redeployed; harmless, not public.

**The version pin is exact, not approximate.** `git describe upstream/master` returns `0.0.14`
at the tag, and the installed base mod is `Spell.Hotbar.2.-.0.0.14.zip`. Our fork point *is*
upstream's 0.0.14 release, so nothing upstream fixed is currently being reverted. The recorded
cost stands for the next upstream release, not for today.

Left open on purpose: identity. Release ticket 01 has not frozen the public name or the version
scheme, so `deploy/release/release.json` carries `identity_frozen: false` and the archive
filename gains `-provisional` mechanically. Ticket 02's other half — our version in the DLL's
own version resource — needs `skse_plugin/CMakeLists.txt` changed off upstream's `2.0.10` and a
rebuild, which waits on ticket 01's scheme.

One conflict to flag rather than resolve: `.scratch/release/spec.md` and release ticket 01 both
state the owner settled permissions on 2026-08-29 and call this ticket's deferral line stale.
The instruction for this build said the opposite — build but do not publish. The build is
gated on a flag in `release.json`, so settling it is a one-line change either way.
