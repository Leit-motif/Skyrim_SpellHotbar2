# 61 — The arming mask outlives its cast and accepts vanilla SpellFire events

**Type:** defect (DLL), low severity. Promoted out of ticket 51's secondary finding when that
ticket closed (2026-08-29).

**Status:** ready-for-agent. Small and self-contained; no live session needed to fix, one to
confirm the log goes quiet.

## The defect

`notify_spellfire` accepts a SpellFire event whenever the LAST cast's mask is still armed and no
re-arm has bumped the generation. The mask is not cleared when a cast retires, so any vanilla
release after a hotbar cast — a staff's own attack, an equipped spell — is accepted and logged
as though it belonged to a driver cast.

**Delivery is safe.** The seen latch is cleared at cast start, so no payload is double-delivered.
This is an evidence defect, not a gameplay one: it writes `SH2 cast: graph raised a … SpellFire
event` lines that did not come from a driver cast, into the exact log that every commitment
finding is read from. It is also the stale acceptance ticket 46's generation counter exists to
prevent, applied at the wrong boundary.

## Evidence

- 2026-08-26 (ticket 51's filing window): vanilla staff releases at 16:50:33, 16:50:40 and a
  16:51:57–16:52:07 stream logged `graph raised a right SpellFire event` with no driver cast,
  accepted by a mask left armed from earlier dual channels.
- 2026-08-29 14:57:03–14:57:08: a right-hand staff's own releases logged bare
  `MRh_SpellFire_Event` traces with no driver cast active and no isolation line — the same
  boundary, still open after ticket 60's work.

## The fix

Clear the arming mask at cast retire, alongside `clear_spellfire()`. One site, and it makes
"this hand is armed" mean "a live cast armed it" rather than "a cast armed it at some point."

## Acceptance

- A vanilla staff or spell release AFTER a hotbar cast logs no `graph raised a … SpellFire
  event` line and no isolation line.
- A hotbar cast still commits exactly once at its clip's own SpellFire frame, both hands and
  dual — ticket 46's rows stay green.
- The unit-level arming tests still pass (`combo_cache_test`).
