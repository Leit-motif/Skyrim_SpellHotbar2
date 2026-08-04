# 03 — Stop a chain-out from destroying a live cast

**Type:** grilling, then task (Core Fork)

**What to build:** A design decision, argued and recorded, for preventing the spell loss that
occurs when an MCO chain-out cuts a live cast — followed by its implementation.

**Blocked by:** 01 — Answer the open question; 02 — Make the cast release event stance-aware.

**Status:** ready-for-agent

This is the one collision that loses player data. The engine's chain cuts the shout with
`shoutStop`, which clears `IsShouting`; this mod tears the cast instance down the moment
`IsShouting` goes false, and it fires the spell itself on its own timer. So a chain taken
during a cast destroys the cast **before the timer fires and the spell never goes off**.

**Chaining out of a Shout-Graph Cast is not a feature to enable. It is a bug to prevent.**

The engine cannot help: its ADR-0002 forbids it from reading shout state, so it cannot tell a
cast from a real shout. The distinction has to be made here.

Grill the design before writing code — `CONTEXT.md` names two families and they are not
equivalent:

- **Fire the spell before the cut.** Removes the loss but changes cast timing, and interacts
  with whatever ticket 01 found about the liveness check.
- **Suppress chaining while a cast is live.** Preserves timing but requires this side to
  detect and block a chain the engine owns, and decides on the player's behalf that a cast
  outranks a chain.

- [ ] Grill both families, plus any third the grilling surfaces, against what ticket 01
      actually found. Record the decision and its rejected alternatives as an ADR.
- [ ] Implement the chosen design in the Core Fork.
- [ ] Verify live that a chain-out attempted during a cast no longer loses the spell.
- [ ] Verify the negative case: a chain-out during a **real shout** must still work
      unchanged. Vanilla places `Voice_SpellFire_Event` 0.1 s into the exhale, so the cut is
      safe there, and this ticket must not break it.
- [ ] Confirm no regression to the ordinary no-chain cast path.
