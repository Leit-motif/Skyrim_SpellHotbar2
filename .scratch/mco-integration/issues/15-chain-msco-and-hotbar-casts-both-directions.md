# 15 — Chain MSCO hand casts and hotbar casts in both directions

**Type:** feature (driver + input), parent ticket 11

**What to build:** A committed hotbar cast and an MSCO hand cast chain into each other in both
directions, and one press fires one spell. The 2026-08-11 matrix had MSCO → hotbar working and
hotbar → MSCO not. Owner playtest on 2026-08-12 found a broader failure: left-hand spell + hotbar
1 fires both.

**Blocked by:** None — can start immediately. Ticket 10 already ships the attack-press cut this
mirrors.

**Status:** ready-for-agent

## What this is not

Not combo-position restore (ticket 13). Not consecutive hotbar clips (ticket 14). Not
concentration: left-hand Flames resetting the combo is expected and out of scope.

## Behaviour

A hand-cast press during a committed hotbar cast ends the hotbar state the same way an attack
press now does. An MSCO hand cast must still be able to chain into a hotbar cast. A hotbar press
while a spell is in the left hand must not also fire that left-hand spell.

- [ ] MSCO hand cast → hotbar cast chains, with no dual fire.
- [ ] Hotbar cast → MSCO hand cast chains, with no dual fire.
- [ ] Left-hand spell + hotbar slot does not cast both.
- [ ] Concentration hand casts remain out of scope and must not be treated as a counterexample.
- [ ] Restore fixtures and close Skyrim after runtime work.
