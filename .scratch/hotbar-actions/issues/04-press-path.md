# 04 — Press path: inject the Action's seam

**Type:** task

**What to build:** Pressing an Action slot injects on the kind ticket 01 admitted. Down
then up. `castSlot` drives it. An attack Action cuts a committed Driver Cast the way
physical OCPA already does (mco-integration ticket 10). Equip mode refuses.

**Blocked by:** 01, 02, 03

**Status:** superseded — rolled into 02

**Status (superseded — rolled into 02):** ready-for-agent

## You test this

Direct Cast, weapon drawn.

1. Bind Power Attack. Press the slot. The same move as physical `B` (or the engine
   chord, if 01 said OCPA cannot hear the queue).
2. Press the slot in Equip mode. Red flash. No inject.
3. During a committed hotbar cast, past spellfire, press the Action. The cast cuts and
   the power attack starts, same as a physical OCPA press in ticket 10.
4. If 01 admitted dodge, bind Dodge and press it. A dodge plays.

Idle, a spell cast, or an Ability clip is a fail.

## Agent tests the rest

5. `castSlot(n)` on that slot injects the same way as the keybind. Log shows down and up.
6. Recursion: an Action whose target scancode is that slot's own bind does not re-enter.
   Red flash, log, no loop.
7. Ticket 41 lockout: a *costed* Action (ticket 05) refuses while `current_cast` is live.
   A costless attack Action is the ticket-10 cut, not a new cast instance.

## Notes

Reuse `InputModeCast::process_input`'s `addEvent` + `BSInputEventQueue` path, and
VoiceCastDriver's hold/release. Engine-control kind must set the control-map `userEvent`;
a scancode alone will not drive PlayerControls.

Do not start the engine-control Power Attack arm unless ticket 01's Answer says the
scancode kind failed.

An Action does not start `SH2_Art_State`, write the Ability Selector, or plant WASD.
