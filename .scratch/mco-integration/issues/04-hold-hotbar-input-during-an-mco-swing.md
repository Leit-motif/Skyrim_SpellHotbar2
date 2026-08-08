# 04 — Adapt Direct Cast to ShoutMCO's cast-intent API

**Type:** task (Core Fork)

**Status:** blocked

**Blocked by:** ShoutMCO ticket 50 — Generic cast-intent driver API.

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
