# 14 — Consecutive hotbar casts walk the cast combo

**Type:** feature (driver + Nemesis), parent ticket 11

**What to build:** Two hotbar casts in a row play different clips and read as a combo, the way
MSCO walks `left1 → left2 → left3 → left4`. A second press during a committed cast must actually
start the next cast, not get refused because a cast is already live.

**Blocked by:** None — can start immediately. Ticket 10 already ships the cut this reuses.

**Status:** ready-for-agent

## What this is not

Not MCO's attack index (ticket 13). This ticket owns the cast index, including that an
intervening attack does not reset it. The mixed chain `attack1 → attack2 → cast1 → attack3 →
cast2` is parent 11's close-out once 13 and 14 are both green; this ticket only owns the cast-2
half.

## Behaviour

SH2 owns a cast index, independent of MCO's attack index. Consecutive Driver Casts advance it.
The index is not reset by an attack — that is a property of this counter, not a second feature.
The public hotbar path has to honour a second press while a committed cast is still live,
otherwise the clip set is unreachable from the player's hands.

Concentration stays out.

- [ ] Two casts in a row play different clips and read as a combo, not a repeat.
- [ ] Four casts in a row walk the full clip set and wrap.
- [ ] `cast1 → attack → cast2` plays clip 2, not clip 1.
- [ ] A second hotbar press during a committed cast starts the next cast through the public
      input path, not only through a test or Papyrus helper.
- [ ] Ritual consecutive casts take the same cut.
- [ ] An ordinary uninterrupted single cast is unchanged.
- [ ] Restore fixtures and close Skyrim after runtime work.
