# 04 — Adapt Direct Cast to ShoutMCO's cast-intent API

**Type:** task (Core Fork)

**Status:** ready-for-agent

**Blocked by:** nothing. **ShoutMCO ticket 50 delivered 2026-08-12** and this is now the next step
toward comboing SH2 casts into and out of MCO attacks.

### What the provider actually ships, so you do not have to go read it

- **Export:** `ShoutMCO_GetCastIntentApi(uint32_t requestedMajor)`, undecorated, ordinal 4 in
  `ShoutMCO.dll`. A wrong major returns `nullptr` — that is the fail-open path, not an error.
- **Header:** `include/ShoutMCO_CastIntent.h` in the thuum repo, staged next to the DLL at
  `dist/include/`. Plain C, POD structs carrying `structSize` and version. Copy it; do not
  re-declare the structs by hand.
- **ABI:** v1.0. Three calls — `Request`, `Cancel`, `Status`. One callback delivering
  `SHOUTMCO_CAST_RELEASE` or `SHOUTMCO_CAST_ABANDON` **exactly once**, on the main thread.
- **`Request` returns** `PASS_THROUGH`, `DEFERRED`, or `REJECTED`, synchronously and in constant
  time.
- **Newest intent wins.** A later intent from any source — including the player's own vanilla shout
  press — displaces yours, and you get `ABANDON` with cause `REPLACED`. There is ONE global slot,
  not one per driver.
- **You may call `Request` from inside your own callback.** The slot returns its notice rather than
  dispatching under its mutex specifically so that is safe.
- **`CONTEXT_LOST` is raised on a game load and nothing else in v1.** Do not build handling for
  player death or engine-disable; they are not wired.
- **A watchdog abandons after `iShoutWaitCapMs`** (4000 ms default), so a deferred intent cannot
  hang forever on a moveset whose attack never swings.

**Provider state at handoff:** thuum `main` at `8249d43`, deployed DLL `26472DD8`, the export
confirmed live in game via `ShoutMCO.log`
(`cast-intent driver API v1.0 available as "ShoutMCO_GetCastIntentApi"`).

**Release timing is the engine's, and it is NOT "after the swing finishes".** The vanilla-side gates
were driven 2026-08-12: a shout queued mid-attack releases at the **hit frame** — `HitFrame` →
`CANCEL recovery for a queued shout -- hit landed` → `attackStop` → `inRdy` → release, with the MCO
combo position preserved across it. Your deferred cast rides that same seam, so do not add your own
timer, poll, or window.

**The seam on this side is still `MscoCastDriver::begin()`'s false return**, per ticket 08's Answer.
Trust that Answer over the in-file comments, which predate it.

**What to build:** Make Direct Cast a consumer of ShoutMCO's optional versioned C ABI. Do not
duplicate MCO animation-event tracking, `HitFrame` policy, timers, or polling in this mod.
ShoutMCO is the authority on whether an input passes through now or is deferred until a confirmed
legal state; Spell Hotbar 2 remains the authority on what the slot means and whether it can still
cast at release.

On hotbar activation, retain the normal cast payload and synchronously request a disposition:

- `pass through`: run the existing Direct Cast path unchanged;
- `deferred`: retain the payload without starting the cast;
- `release`: on the main thread, revalidate current slot assignment, cooldown, resources, player
  state, and any normal cast preconditions, then attempt exactly once;
- `abandon`: discard the retained payload without a cast or retry.

The newest valid input replaces the previous global intent. Replacement/abandon callbacks must
clear local ownership. If ShoutMCO is absent or its major API version is incompatible, fail open
through the existing Direct Cast path and report one read-only status: `active`, `unavailable`, or
`incompatible`. Log that status once at startup; no popup or repeated spam.

[ADR-0005](../../../docs/adr/0005-shoutmco-owns-release-timing-spell-hotbar-owns-cast-payload.md)
owns the boundary. ADR-0004's spellfire commitment remains unchanged: this API schedules cast
entry and does not negotiate when an already-started cast becomes committed.

## Acceptance criteria

- [x] No independent MCO event sink, `HitFrame` gate, per-frame polling, worker thread, Papyrus,
      JSON, animation-event encoding, or hot-path filesystem access is added
- [x] Direct Cast outside an MCO attack follows its existing path with no added frame of latency
- [ ] Early, pre-hit, post-hit, and recovery-tail inputs across light and power attacks preserve
      the attack and start the cast exactly once after ShoutMCO releases it
- [x] Release revalidates slot assignment, cooldown, resources, player state, and normal cast
      preconditions; invalid payloads discard once with no retry
- [x] A newer vanilla shout or driver request replaces this intent and clears its payload
- [ ] Load/new game, death, gameplay-control loss, blocking menu, driver cancellation, and
      ShoutMCO watchdog abandonment leave no later ghost cast
- [~] API-absent and incompatible-major controls fail open and show the correct read-only status
- [~] Runtime evidence names both repositories' commits, deployed binaries, save/profile, attack
      kind, hotbar key, slot, and spell

## Status: built, partially verified — one acceptance cell FAILS

**Implemented** as `skse_plugin/src/casts/cast_intent.{h,cpp}` (commits `f66bc27`, `3495cd7`), with
the seam at `MscoCastDriver::begin()`'s false return exactly as ticket 08's Answer specified. The
provider header is vendored byte-identical at `src/extern/ShoutMCO_CastIntent.h`.

### Runtime evidence (2026-08-12, 09:33–09:44)

Spell Hotbar 2 `3495cd7`, deployed `SpellHotbar2.dll` SHA256 `C97AC645…`. ShoutMCO deployed
`ShoutMCO.dll` SHA256 `26472DD8…` (the binary the ticket names), export confirmed at ordinal 4 by
`dumpbin`. Profile `Nolvus Awakening`, save `Save63…20260812064907` (Xaelle, level 17, Riverwood
area). Loadout: Iron Rapier right, Flames left. Slot 0 / hotbar key "1", driven through the
`castSlot(0)` papyrus seam; attacks driven through DevBench `input` with
`userEvent="Right Attack/Block"`. Light attacks only — see the gap below.

**What passed.**

- Negotiation: `SpellHotbar2 ShoutMCO cast-intent API: active`, logged exactly once at startup,
  matched by ShoutMCO's own `cast-intent driver API v1.0 available` line.
- Sheathed refusal (09:37:44.495): `SH2_CastRight -> false` → `not deferred (decision 0)` →
  refused. Decision 0 is `PASS_THROUGH`: ShoutMCO had nothing to wait out, and behaviour is
  byte-for-byte what it was before this change.
- Drawn idle cast (09:38:12.142): `-> true`, commit on the left SpellFire at +0.475s, and **no
  cast-intent line at all** — the success path never calls ShoutMCO, so no latency is added.
- Mid-swing defer→release→cast, first observed 09:38:41: refusal and defer in the same
  millisecond, release 72ms later, `-> true`, commit at +0.467s. This is the behaviour the ticket
  exists to create; before it, the same press cast nothing.
- Replacement (09:44:21): two rapid presses during one swing produced two deferrals and exactly
  **one** release attempt. No double cast, no ghost.
- Discard-once: every failed release produced exactly one `-> false` and no retry, no re-defer.

**What failed — the early-press cell.** Across 16 mid-swing trials the outcome depends on where in
the swing the press lands, measured as the interval from the attack input to SH2's notify:

| Press offset into swing | Release gap | Outcome |
| --- | --- | --- |
| +0.00 → +0.24s | 17–110ms | `released cast -> false` — **cast silently lost** |
| +0.27s and later | 38–117ms | `released cast -> true` — cast fires |

An early press is deferred (so ShoutMCO's engine agrees an attack is live), then released within
~100ms into a graph that still refuses the entry transition. SH2 discards once, as designed, and
the player sees only the red border. Later presses release into a ready graph and cast normally.

**This is not a regression.** Before this change *every* mid-swing press failed that way. The
change strictly adds successes; the failing cases fail exactly as they did before.

**ATTRIBUTION RESOLVED — the defect is ShoutMCO's, and it is an ordering bug.** A second traced
session (`bTrace = 1`, driver-side graph-variable logging at defer and release) settled it:
ShoutMCO releases the intent on **`preHitFrame`**, not `inRdy`, while reporting cause `READY`. At
that instant `IsAttacking=1`, so the graph is still in `AttackState` and refuses the entry.

The power-attack trace shows the ordering directly — release at `10:28:43.591`, the driver's
refused re-attempt at `.597`, and ShoutMCO's own `CANCEL fired 'attackStop' accepted=true` at
`.599`, 8ms *after* the release it was supposed to precede. Ticket 04's brief and the API header
both specify `HitFrame` → `CANCEL` → `attackStop` → `inRdy` → release; the implementation does
release → `CANCEL` → `attackStop` → `inRdy`.

This also explains the timing band exactly: a press before `preHitFrame` (~295ms into a light
attack on this loadout) is released on that `preHitFrame` and lost; a press after it waits for the
following `inRdy` and casts. Full write-up, with both logs interleaved, in
[`evidence/ticket-04-release-fires-before-attackstop.md`](../evidence/ticket-04-release-fires-before-attackstop.md).

**Spell Hotbar 2 is not changing to compensate.** No retry, no poll, no wait-a-few-frames. ADR-0005
puts release timing on ShoutMCO's side, and a timer here is the duplicated policy that ADR forbids.
The consumer behaviour on a bad release is already correct: revalidate, attempt once, discard, show
the normal refusal.

**Power attacks are now covered** and fail the same way, from the same cause: handles 4 and 5
refused with `IsAttacking=1`; handle 6, pressed late enough that the graph had returned to ready,
cast normally.

### Cells still open, named exactly

- **The early-press cell stays open until ShoutMCO's release point moves.** Nothing to do on this
  side.
- **Death, gameplay-control loss, blocking menu, and watchdog abandonment are untested at
  runtime.** Only load/new-game is covered by code (`Storage::LoadCallback` withdraws), and even
  that was not exercised live.
- **The incompatible-major control is untested.** Only the `active` path ran; `unavailable` and
  `incompatible` are unobserved.
- **Only the `kBehindAttack` hold reason was exercised**; `kBehindShout` is untested from this side.
- **No visual frame was captured**, so nothing here supports a claim about how the cast looks.

### Session trap worth keeping

A save copied to a new filename (`SH2_T04_CastIntent.ess` / `.skse`) wedges this modlist on the
Loading Menu indefinitely — reproduced twice, while the original `Save63…` loads normally every
time. Test on the latest save read-only and never save over it, rather than on a renamed copy.
