# 05 — Resolve the SYHO animation conflict

**Type:** task (likely Compatibility Package)

**What to build:** Casting animations that actually play in this load order, instead of being
shadowed by another mod's shout clips.

**Blocked by:** None. Independent of the graph work and can proceed in parallel.

**Status:** ready-for-agent

Reproduced live 2026-08-03 during Baseline Adoption ticket 02, and attributed without needing
to read anyone's conditions:

| Mod | OAR submod priorities |
| --- | --- |
| `SYHO - Shout Your Heart Out` | `99999990`–`99999996` (8 submods) |
| Spell Hotbar 2 | max `99901002` (56 submods) |

SYHO's *lowest* priority outranks this mod's *highest*, so wherever both sets of conditions
pass SYHO wins **every** contested cast. This is not intermittent and not a tuning accident.
It confirms `CONTEXT.md` finding 11, which had raised the hazard and explicitly left it
unconfirmed.

**The part that constrains the fix: SYHO's clip does not loop.** A fire-and-forget borrowing
the wrong animation looks wrong; a concentration spell channelling for an arbitrary duration
against a one-shot clip has no animation that can represent it. So the remedy cannot be
cosmetic — this mod's own concentration clips have to win, or sustained casts stay visibly
broken.

- [ ] Read SYHO's own OAR conditions. They have still never been read; the priority
      comparison alone settled attribution, but the fix needs to know when SYHO actually
      claims a cast.
- [ ] Choose a resolution and record why: renumber this mod's submods above SYHO, add
      conditions excluding SYHO during a hotbar cast, or a load-order-local override.
- [ ] Keep it in the **Compatibility Package** unless the fix turns out to be generally
      applicable. SYHO's presence is a property of this load order, not of the mod.
- [ ] Do not renumber blindly. Whatever wins must still lose to anything that should
      legitimately outrank a cast.
- [ ] **Capture frames** for a fire-and-forget and a sustained concentration cast. This claim
      is entirely visual; an OAR priority table does not prove what played.
- [ ] Confirm real shouts still play SYHO's animations. This ticket must not fix casts by
      breaking the shout overhaul the player installed deliberately.
