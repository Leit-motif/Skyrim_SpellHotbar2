# 01 — Freeze the product surface: the SMF call and the public identity

**Type:** task
**Status:** ready-for-agent — needs owner rulings, not agent work

Everything visual in this effort is blocked on this ticket. Nothing else here can start except
ticket 02.

## Ruling 1: does SMF ship in the first release?

`../skse-menu-framework/` has five open tickets that move the settings surface out of the SkyUI
MCM into Mod Control Panel pages, and turn the editors, bar drag and bind menu into framework
windows. Its own spec says "function of the shipped product stays 100%" and the aesthetic follows
the Mod Control Panel's Skyrim theme.

That is a full repaint of every configuration screen a screenshot would show. Two options:

**Ship without SMF, shoot now.** The gallery shows the MCM and the current ImGui editors. Media is
unblocked today. Cost: the first release's screenshots are stale the moment SMF lands, and the
page's settings section is rewritten at that point too.

**Hold media until SMF closes.** One shoot, one page, and the settings screenshots match the model
page's structure — a section per UI page, with a screenshot of that page. Cost: the release waits
on five tickets.

**Recommended: hold.** The owner's stated bar is presentation quality, and the model page's whole
structure is a section per UI page. Shipping the MCM and replacing it inside a month spends the
first-impression budget on the version being retired. A recommendation, not a decision — record
the owner's call here.

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

- [ ] The SMF ruling is recorded here in the owner's words.
- [ ] The identity table is filled in with no "prospective" left.
- [ ] The `author=` field in `nemesis/Nemesis_Engine/mod/shtb/info.ini` and `mod/shcr/info.ini`
      carries no real name, checked rather than assumed. Nemesis displays it in its own mod list,
      and this is exactly where a real name leaked in the sibling repo's release.
- [ ] The stale "publication deferred until the upstream-permission question is settled" line in
      `../mco-integration/issues/59-release-packaging-ships-no-nemesis-tree.md` is struck. The
      owner settled it 2026-08-29; leaving the line makes ticket 59 read as blocked.
