# 12 — Amend ADR-0005: combo position is not release timing

**Type:** decision (ADR), parent ticket 11

**What to build:** ADR-0005 keeps a hard separation of concerns. ShoutMCO still owns *when a
deferred cast may begin*. Spell Hotbar still owns the payload. Combo-position continuity across a
Driver Cast is a different concern and must be named as one, not smuggled in as a second MCO
timing policy.

**Blocked by:** None — can start immediately.

**Status:** resolved

## Why this is its own ticket

ADR-0005 forbids Spell Hotbar from independently observing MCO events. That sentence is about
**release timing**: do not duplicate ShoutMCO's HitFrame / window policy, do not poll ShoutMCO, do
not own a second "may this cast start now" state machine.

Ticket 11's restore path observes attack and ready tags and writes `MCO_nextattack` /
`MCO_nextpowerattack`. That is combo-position continuity, which the 2026-08-12 ownership question
already assigned to this repo. It is not release timing. Shipping it without amending the ADR
makes the next session treat the observation as a licence to grow a second timing cache — the
exact duplicated-state risk ADR-0005 rejected.

## The contract this ticket writes

- **Unchanged:** ShoutMCO owns release or abandonment of a deferred Cast Intent. Spell Hotbar
  does not poll ShoutMCO, does not observe MCO events to decide when a cast may begin, and does
  not keep an independent MCO timing cache for that purpose.
- **Named exception:** for a Driver Cast this mod itself started, Spell Hotbar may sample
  attack/ready tags and write `MCO_nextattack` / `MCO_nextpowerattack` so the swing that follows
  continues the combo. Preserve, never derive. Gated to a cast this mod started.
- **Still forbidden:** using those same tags as a HitFrame buffer, a chain window, or any other
  "when may this press start" policy. Combo restore and release timing do not share state,
  callbacks, or a cache.
- **No ShoutMCO call for this.** The ownership question closed that: ShoutMCO's API answers
  whether a press may start during someone else's animation, not where a cast sits in the combo.

No player-visible change. Ticket 13 implements against this contract and must not start before
the ADR says so.

- [x] ADR-0005 restates the original release-timing decision unchanged.
- [x] ADR-0005 names the combo-position exception, its trigger, and the preserve-never-derive rule.
- [x] ADR-0005 states what the exception must not become.
- [x] CONTEXT.md's Cast Driver / Driver Cast language still matches the ADR.

## Answer

ADR-0005's original Decision and Consequences are unchanged. A 2026-08-12 scope amendment names
combo-position continuity as a distinct concern, not a second release-timing cache.

Contract, as written in the ADR:

- ShoutMCO still owns release or abandonment of a deferred Cast Intent. Spell Hotbar still owns
  the payload. No polling, no MCO-event observation to decide when a cast may begin, no independent
  timing cache for that purpose.
- **Named exception:** for a Driver Cast this mod itself started, sample attack/ready tags and
  write `MCO_nextattack` / `MCO_nextpowerattack`. Preserve, never derive. Gated to a cast this mod
  started. No ShoutMCO call — that API is still "may this press start", not combo position.
- **Forbidden:** using those tags as a HitFrame buffer, a chain window, or any other "when may
  this press start" policy. Combo restore and release timing do not share state, callbacks, or a
  cache.

CONTEXT.md: Cast Driver and Driver Cast now point at that exception; finding 10's "one authority
for MCO state" is scoped to release timing so ticket 13 cannot read it as a ban on the rolling
sample.

Ticket 13 is unblocked. No player-visible change.

## Comments

**2026-08-12 — resolved on `ticket-11-combo-chain`.** Paper only. The blob's restore path in
`msco_cast_driver.cpp` is still ticket 13's work; this ticket only makes that path an ADR
exception rather than a silent ADR-0005 breach.
