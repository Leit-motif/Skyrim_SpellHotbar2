# 19 — Root the player during a Driver Cast

**Type:** defect (driver / graph)

**What to build:** A Driver Cast roots the player for the duration of the cast. Currently the
player can move while the animation plays.

**Blocked by:** None — can start immediately.

**Status:** ready-for-agent

## How this showed up

Owner playtest, 2026-08-12, during ticket 11. Not combo membership. Not a child of ticket 11.
An SH2 cast is supposed to root; it does not.

- [ ] The player cannot translate during a Driver Cast, owner-verified in game.
- [ ] Rooting ends when the cast state ends, including a ticket-10 cut into an attack.
- [ ] Restore fixtures and close Skyrim after runtime work.
