# 01 — Freeze the product surface: the SMF call and the public identity

**Type:** task
**Status:** ready-for-agent — needs owner rulings, not agent work

Identity has to be settled before the archive is named and before the key art carries a title.
Nothing else in this effort waits on it — media shoots against the currently deployed UI.

## Ruling 1: SMF is post-release. SETTLED, and it stays settled.

**Owner ruling, 2026-08-29:** *"i've said this so many times. this is out of scope until after
release."* The `../skse-menu-framework/` effort ships **after** the first release. The first
release's UI is what is deployed today: the SkyUI MCM plus the current ImGui editors, bar drag and
bind menu.

Media and copy therefore shoot against the current surface, and nothing in this effort waits on
SMF. The five tickets there are marked `deferred — post-release` so a frontier scan stops
surfacing them as shippable work; that marking is the structural fix, because wording it more
strongly has already failed several times.

Do not re-open this, do not re-caveat a deliverable with it, and do not offer it as an option
again.

~~Two options: ship without SMF and shoot now, or hold media until SMF closes.~~ Struck. There was
never a decision to make here.

## Ruling 2: the public identity

Decide and record, so nothing downstream guesses.

| | |
|---|---|
| Public name | Prospective **Spell Hotbar 2 NG** (memory `sh2-thuum-repo-map`). Not settled |
| Nexus title | The name plus at most one clause. Under the Fury ruling the product is the pitch, not one feature of it |
| Version | `skse_plugin/CMakeLists.txt` reads `2.0.10` — that is **upstream's** number, inherited by our build. An overwrite that reuses the base mod's version string is a support trap: a user reading `2.0.10` in two places cannot tell whose build they are running. Pick our own scheme and stamp it |
| Category | Gameplay, matching base SH2. Not Animation — animations are a means here, not the product |
| Runtimes | Read off the built DLL's target, do not assume |
| Author identity | **Leitmotives** on Nexus, **Leit-motif** on GitHub, per the sibling repo. No real name on any public artifact |
| Source repo | Settle whether the public source is a push of this fork or a fresh repo, and whether the licence obliges publishing it at all |

## Acceptance

- [ ] The identity table is filled in with no "prospective" left.
- [ ] The five `../skse-menu-framework/` tickets read `deferred — post-release`, so they leave the
      frontier.
- [ ] The `author=` field in `nemesis/Nemesis_Engine/mod/shtb/info.ini` and `mod/shcr/info.ini`
      carries no real name, checked rather than assumed. Nemesis displays it in its own mod list,
      and this is exactly where a real name leaked in the sibling repo's release.
- [ ] The stale "publication deferred until the upstream-permission question is settled" line in
      `../mco-integration/issues/59-release-packaging-ships-no-nemesis-tree.md` is struck. The
      owner settled it 2026-08-29; leaving the line makes ticket 59 read as blocked.
