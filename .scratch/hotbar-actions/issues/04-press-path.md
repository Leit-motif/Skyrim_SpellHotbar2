# 04 — Press path: inject the Action's seam

**Type:** task

**What to build:** An Action slot mirrors its source hotbar key onto the target the Action
names: the down edge opens the target, held/repeat is mirrored, and the source key's up
closes it. Superseded 2026-09-05: this ticket previously specified a fixed "down then up"
tap, which cannot express a held control. The `castSlot` seam stays a bounded tap that
always closes its own target. An attack Action cuts a committed Driver Cast the way
physical OCPA already does (mco-integration ticket 10). Equip mode refuses.

**Blocked by:** 01, 02, 03

**Status:** superseded — rolled into 02

**Status (superseded — rolled into 02):** ready-for-agent

## You test this

Direct Cast, weapon drawn.

1. Bind an Action to the power-attack key. Press the slot. The same move as the physical
   key (or the engine chord, if 01 said OCPA cannot hear the queue).
1b. Bind an Action to the Block key (`V`). Hold the slot key: Block stays held. Release
   the slot key: Block releases. Repeated taps and repeated holds behave like the
   physical key.
1c. Open a menu or change mode mid-hold. No stuck input remains.
2. Press the slot in Equip mode. Red flash. No inject.
3. During a committed hotbar cast, past spellfire, press the Action. The cast cuts and
   the power attack starts, same as a physical OCPA press in ticket 10.
4. If 01 admitted dodge, bind Dodge and press it. A dodge plays.

Idle, a spell cast, or an Ability clip is a fail.

## Agent tests the rest

5. `castSlot(n)` on that slot injects the same way as the keybind. Log shows down and up.
6. Recursion: an Action whose target scancode is that slot's own bind does not re-enter.
   Red flash, log, no loop.
7. Ticket 41 lockout: a *costed* Action (ticket 05) refuses at its down edge while
   `current_cast` is live. A costless attack Action is the ticket-10 cut, not a new cast
   instance.
8. Only the initial down is costed. Mirrored held events and the up edge bypass admission,
   cost, cooldown, and GCD.
9. Source key events for a bound Action slot are consumed, so the original engine or mod
   binding does not fire alongside the mirror.
10. Target device, scancode, and `userEvent` are frozen at the accepted down; a rebind,
   modifier, menu, or mode change during the hold cannot redirect the up edge.

All runtime cells here are unproven.

## Notes

Reuse `InputModeCast::process_input`'s `addEvent` + `BSInputEventQueue` path, and
VoiceCastDriver's hold/release. Engine-control kind must set the control-map `userEvent`;
a scancode alone will not drive PlayerControls.

Do not start the engine-control Power Attack arm unless ticket 01's Answer says the
scancode kind failed.

An Action does not start `SH2_Art_State`, write the Ability Selector, or plant WASD.

Native SKSE only: no new Papyrus scripts and no new dependencies. Execute Hotkeys (MIT)
was inspiration only; nothing is copied from it.

## Comments

2026-09-05 — A key physically held across a game load goes dead until it is re-pressed
(finding 2 of ticket 07, accepted). `kPreLoadGame` defers the release; `retry_action_releases`
emits the target up on the first loaded frame and erases the record. A source key the player is
still physically holding then has no record: its repeats are forwarded as plain repeats and its
later up passes through as a plain up. Re-pressing the key re-arms the mirror normally. Accepted
rather than re-arming on the next repeat, because re-arming would re-run admission and re-charge
cost, cooldown, and GCD for a press the player made in the previous session.

2026-09-05 — Down and up are captured by different conditions, and the asymmetry is one-way
(finding 5 of ticket 07, traced). An Action down can only start through the `key_spells` branch,
and that branch always sets `captureEvent` for an action slot -- either through the modifier
clause or the `in_ingame_state()` clause -- so no forwarded down ever starts an Action. The
reverse does exist: a down that was captured but refused (cooldown, cost, a live cast) has no
record, so its up passes through the pre-filter unmatched and reaches the game. Harmless for the
mod hotkeys Actions mirror: an up with no matching down is what those consumers already ignore.
