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

- [ ] No independent MCO event sink, `HitFrame` gate, per-frame polling, worker thread, Papyrus,
      JSON, animation-event encoding, or hot-path filesystem access is added
- [ ] Direct Cast outside an MCO attack follows its existing path with no added frame of latency
- [ ] Early, pre-hit, post-hit, and recovery-tail inputs across light and power attacks preserve
      the attack and start the cast exactly once after ShoutMCO releases it
- [ ] Release revalidates slot assignment, cooldown, resources, player state, and normal cast
      preconditions; invalid payloads discard once with no retry
- [ ] A newer vanilla shout or driver request replaces this intent and clears its payload
- [ ] Load/new game, death, gameplay-control loss, blocking menu, driver cancellation, and
      ShoutMCO watchdog abandonment leave no later ghost cast
- [ ] API-absent and incompatible-major controls fail open and show the correct read-only status
- [ ] Runtime evidence names both repositories' commits, deployed binaries, save/profile, attack
      kind, hotbar key, slot, and spell
