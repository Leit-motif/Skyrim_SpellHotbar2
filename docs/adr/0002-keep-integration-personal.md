# Keep the integration personal

The maintained fork and its runtime artifacts are for the user's personal Nolvus installation, not a public mod release. Modified binaries will not be distributed while upstream modification and redistribution rights remain unclear; the GitHub fork serves as tracked development history rather than a supported release channel.

## Rights, as investigated 2026-08-03

The decision stands unchanged. This records what was checked, so a later public transition
starts from evidence rather than from scratch.

- `skse_plugin/LICENSE.txt` is MIT, "Copyright (c) 2024 pWn3d1337". It grants modification and
  redistribution outright, conditional only on shipping the notice. It sits inside
  `skse_plugin/`, not at the repository root, and GitHub detects no repository-level licence —
  so the defensible scope is the C++ plugin source, not the shipped package.
- Assets in the distributable are not covered by it: shout animations derived from Thu'um
  (README calls these open permission), icons credited to ArchAngelAries, SkyUI assets.
- Spell Hotbar 2 has no Nexus page; distribution is GitHub releases (0.0.14 at time of check).
  There is therefore no SH2 permissions block. Spell Hotbar 1 (Nexus 110763), same author,
  states: modification allowed with credit, assets usable with credit, no selling, and
  "You are not allowed to upload this file to other sites under any circumstances."
- Distributing the fork's own DLL against a user-installed upstream SH2 redistributes none of
  his files and needs no permission. A full package re-upload would need his consent.
- SH2 exposes no SKSE plugin interface, and its Papyrus surface is bar and MCM configuration
  only, so a companion plugin has no supported seam and would have to hook
  `NotifyAnimationGraph`. That option is open but unvalidated; see `CONTEXT.md` findings 8-10.
