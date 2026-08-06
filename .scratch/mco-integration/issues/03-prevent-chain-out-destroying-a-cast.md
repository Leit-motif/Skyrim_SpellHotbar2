# 03 — Let a hotbar cast chain into MCO without losing the spell

**Type:** grilling, then task (Core Fork) — and now a **cross-project design**

**What to build:** A design decision, argued and recorded, for how a Shout-Graph Cast hands off to
an MCO chain without the cut destroying the spell — followed by its implementation on this side.

**Blocked by:** 01 — Answer the open question; 02 — Make the cast release event stance-aware.

**Status:** claimed (grilling, 2026-08-05)

## The premise changed on 2026-08-05. Read this before the rest.

This ticket was written as *"stop the loss"* — a live data-loss bug to prevent. **It is not one
today.** The engine side traced the first live hotbar cast (its ticket 37) and found the engine
**never arms**: a hotbar cast does not raise `BeginCastVoice`, so no window is scheduled, the
attack press is forwarded straight to the game, and **no cut is ever sent**. `CONTEXT.md` finding
12 records this; findings 6 and 8 carry the corrections it forced.

So the two mods are safe together and **disconnected**. The work here is a **missing feature**,
not a defect — and the cost of building it is exactly the data loss the original ticket described.

**The mechanism is unchanged and still the crux.** The engine's cut is `shoutStop`, which clears
`IsShouting`. `CastingInstance::update` gates every frame on `is_anim_ok` — a raw `IsShouting`
read — and on false it calls `reset_animation_vars()` and returns true, destroying the instance
(`skse_plugin/src/casts/casting_controller.cpp:247`). If the instance dies before its timer
reaches zero, `cast_spell` is never called and the spell is gone.

**Therefore the obvious next move is the dangerous one.** Arming the engine on this mod's events
(`ShoutStart` or the exhale) without solving the cut first is **strictly worse than today**: today
there is no chain and no harm; that change gives a chain that silently eats the spell.

Also withdrawn: *"The engine cannot help — ADR-0002 forbids it from reading shout state."*
ADR-0002 governs **cooldown** state only, and the engine has read `selectedPower` since its ticket
06 and `high->currentShout` in `ReadShoutVariation`. It can tell a driver's cast from a shout, and
it is the party that has to arm first. The engine tracks its half as its ticket 38.

## Decided 2026-08-05 — and the spell-loss half has left this ticket

Grilled against the code and the engine's own window rule. The decision is
[ADR 0004](../../../docs/adr/0004-commit-a-cast-at-the-graphs-spellfire-event.md): **a cast is
committed at the graph's `Voice_SpellFire_Event`, and there is no cross-mod API.**

Both of the engine's shapes were rejected. **A** (delay the cut) needs a delay nobody can derive.
**B** (a handshake) rests on the premise that only the driver knows when its own spell fired —
true of this mod as written, and false the moment it stops ignoring the event the graph already
raises. The engine measures its window from `Voice_SpellFire_Event` and refuses to open before it
(`ShoutChainEngine.cpp:307-317`); this mod adopting the same instant makes the engine's floor and
this fork's guarantee the same instant, with nothing passed between them.

**The spell-loss work is now [ticket 07](07-commit-a-cast-at-spellfire.md)**, which has no
blockers — it corrects this mod's own lifecycle and is right with no engine present. What is left
in this ticket is the integration itself, which is still blocked.

## What remains here

- [ ] Carry the decision to the engine side as this project's answer to its ticket 38 (its
      A38.1). ADR 0004 is the artifact; **do not edit that repo**.
- [ ] The engine must arm on a hotbar cast. It arms on `BeginCastVoice`, which a hotbar cast does
      not raise, so nothing chains until that changes — and it must reach the `CombatReady_*`
      branch first, which is ticket 02.
- [ ] Verify live that a chain-out taken during a cast leaves the spell intact. **The spell
      firing at all has never been objectively confirmed** (finding 12), so this needs a real
      observation: a damaged target or a visible effect, not a magicka reading.
- [ ] Verify the negative case: a chain-out during a **real shout** must still work unchanged.
- [ ] Confirm no regression to the ordinary no-chain cast path, and that with the feature off
      behaviour is exactly today's — no arm, no cut, press forwarded.
