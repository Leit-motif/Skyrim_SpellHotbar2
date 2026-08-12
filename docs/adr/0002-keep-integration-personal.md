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

## Superseded in part, 2026-08-12 — upstream granted a public add-on release

The author answered a direct request on his GitHub issue tracker. **The no-public-release stance
above is lifted for the add-on shape**, and the rest of this ADR stands.

Verbatim:

> For now I think the easiest way forward is to release it as addon since there is no SpellHotbar2
> page yet.
>
> For Icon permissions, I don't really care, they are AI generated. If you do the plugin way
> everything should be super clear and there should not be any issues.
>
> If you find something that should be fixed on the main mod, feel free to create a pull request.
> What you did sounds interesting.

What that settles, and what it does not:

- **An add-on release on Nexus is blessed**, in writing, publicly timestamped. It was already the
  route needing no permission; now it has explicit consent as well.
- **The icon question is closed** — he says they are AI generated and he does not care. The
  ArchAngelAries concern this ADR raised was, on his account, unfounded. Moot anyway for an add-on,
  which ships none of his assets.
- **A full package fork is NOT granted.** He steered to the add-on rather than refusing outright,
  so treat the fork route as unasked, not denied.
- **He invited pull requests.** That is the important half, because of the constraint below.

### The architectural consequence, which is the real work

"Add-on" means shipping **our own plugin**, not a modified build of his. Today it is a modified
build: 12 files under `skse_plugin/` differ from upstream, +714/-60. Most of that is our own new
files (`msco_cast_driver`, `voice_cast_driver`, ~433 lines), but the wiring edits to
`casting_controller`, `animationeventhook`, `plugin.cpp` and `papyrus_functions` are edits to his.

This ADR already recorded the obstacle: SH2 exposes no SKSE plugin interface, so a companion plugin
has no supported seam. His PR invitation is the way through — upstream a minimal seam, then consume
it from outside. Hooking `NotifyAnimationGraph` independently remains the fallback and remains
unvalidated.

**Unencumbered regardless:** the Nemesis behaviour patch under `nemesis/` (20 files) and the
ShoutMCO DLL. Neither contains any of his code.
