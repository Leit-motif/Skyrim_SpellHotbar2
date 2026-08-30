# Keep the integration personal

**Superseded 2026-08-29. This fork ships publicly. Settled — do not re-open, do not caveat a
deliverable with it, do not ask.**

Upstream answered on 2026-08-12 and the answer was yes. Everything this ADR once weighed —
licence scope, asset provenance, whether to ask, what shape to ask for — is closed. The
deliberation has been removed rather than left to be re-read as an open question, because
re-reading it is exactly how it kept getting re-opened.

What remains true and useful from the original decision:

- `skse_plugin/LICENSE.txt` is MIT, "Copyright (c) 2024 pWn3d1337". Ship the notice.
- Base Spell Hotbar 2 distributes through GitHub releases, not Nexus. `0.0.14` is the pinned
  base; see `docs/agents/release-packaging.md`.
- SH2 exposes no SKSE plugin interface, and its Papyrus surface is bar and MCM configuration
  only. That is why our build modifies upstream's source rather than sitting beside it as a
  companion plugin, and why the release is an overwrite over the base mod.
- The Nemesis behaviour patches under `nemesis/` contain none of upstream's code.

Upstream invited pull requests for fixes that belong in the main mod. That invitation stands
and is a separate, optional path — it has nothing to do with whether this fork ships.
