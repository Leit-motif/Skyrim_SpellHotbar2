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
| Public name | `deploy/release/release.json` reads **Spell Hotbar 2 NG**, provisionally. Not frozen |
| Nexus title | The name plus at most one clause. Under the Fury ruling the product is the pitch, not one feature of it |
| Version | Solved by ticket 59: `release.json` carries our own `version` (currently `0.1.0`), separate from upstream's `2.0.10` in `CMakeLists.txt`. Confirm the number and the scheme |
| Category | Gameplay, matching base SH2. Not Animation — animations are a means here, not the product |
| Runtimes | Read off the built DLL's target, do not assume |
| Author identity | **Leitmotives** on Nexus, **Leit-motif** on GitHub, per the sibling repo. No real name on any public artifact |
| Source repo | Settle whether the public source is a push of this fork or a fresh repo, and whether the licence obliges publishing it at all |

## This is now a two-field edit

Ticket 59 landed the packaging build (`5d1227b`, `e03bc50`) and it reads identity from
`deploy/release/release.json`. So freezing the surface is:

```
public_name      "Spell Hotbar 2 NG"   <- confirm or change
version          "0.1.0"               <- confirm or change
identity_frozen  false  ->  true       <- drops the "-provisional" stamp from the filename
```

Until `identity_frozen` is true the build stamps `-provisional` into the archive name, so a
verification build cannot be mistaken for an upload candidate. Nothing else in the release waits
on this except the archive filename and the key art's title lockup (ticket 06).

## Acceptance

- [ ] `public_name` and `version` in `deploy/release/release.json` are the owner's, and
      `identity_frozen` is `true`.
- [ ] Category, runtimes and the source-repo question are recorded in the table above.
- [x] **Done by ticket 59.** `nemesis/Nemesis_Engine/mod/shtb/info.ini` and `mod/shcr/info.ini`
      both read `author=Leitmotives`, verified rather than assumed, and the build now enforces it
      with a real-name guard instead of leaving it to a checklist.
- [ ] The five `../skse-menu-framework/` tickets read `deferred — post-release`, so they leave the
      frontier.
