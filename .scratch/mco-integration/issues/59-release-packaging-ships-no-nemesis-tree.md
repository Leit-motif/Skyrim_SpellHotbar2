# 59 — Build a release package for OUR mod (overwrite over base SH2)

**Type:** build (packaging). **Status:** ready-for-agent, re-scoped 2026-08-29 after the
owner's distribution ruling. Originally titled "release packaging ships no Nemesis tree" —
that framing was wrong; see the correction below.

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

- [ ] A build script in this repo produces the archive from the working tree (the lever, not a
      hand-assembled zip), enumerating our-files-only; upstream-untouched assets provably
      absent.
- [ ] The archive byte-contains the `shtb` and `shcr` patch files and both `.pex`.
- [ ] The DLL in the archive is the current build (hash-match against `skse_plugin/build`
      output).
- [ ] Install docs: base SH2 required, install ours after/over it, run Nemesis (Launch;
      end users need no Update Engine for a selection-only change), supported base version
      pinned.
- [ ] Publication explicitly deferred until the upstream-permission question is settled by
      the owner.
