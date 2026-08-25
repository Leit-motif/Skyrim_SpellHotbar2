# 02 — Make the cast release event stance-aware

**Type:** task (Core Fork)

**What to build:** A cast that fires the exhale event matching the player's actual stance, so
that a weapon-drawn cast lands on the `CombatReady_*` branch instead of the sheathed one.

**Blocked by:** None. Independent of ticket 01 — this is true regardless of how the open
question resolves.

**Status:** superseded 2026-08-23 — do not implement this file. Audit against the current
source, on the owner's ruling that the pre-08 MCO tickets are out of date: the premise died
with ticket 08. A hotbar cast no longer enters the shout graph at all — every spell start path
(`start_cast`, `start_conc_cast`, `start_ritual_conc_cast`, `start_ritual_cast`) goes through
`MscoCastDriver::begin()` into the shtb states distributed into the weapon behaviour graphs,
so no cast can land on the sheathed `MT_*` branch, drawn or not. The unconditional
`MT_BreathExhaleShort` this ticket targets survives only as dead code:
`CastingInstance::get_end_anim()` (`casting_controller.cpp:379`) has no caller. Stance-correct
animation is what ticket 08 shipped and the owner accepted; the `CombatReady_*` branch is
ShoutMCO's concern for real shouts, which this mod does not route.

This is the precondition for any chaining integration. MCO chaining only ever concerns the
`CombatReady_*` branch (nested states 1, 12 and 13 of the inhale's transition array `#0244`).
The mod fires `MT_BreathExhaleShort` unconditionally, so **every cast currently lands in the
one branch the chain engine does not serve**. Ticket 03 cannot work until this does.

It is also an animation defect on its own terms: a cast with a weapon drawn plays the
sheathed shout clip.

- [ ] Select the exhale event from the player's actual stance rather than emitting
      `MT_BreathExhaleShort` unconditionally.
- [ ] Keep the change in the Core Fork — it is generally applicable and carries no Nolvus
      assumption.
- [ ] Confirm the mod still reaches only the short exhale per stance. The release event never
      varies by word count, so the animation surface stays small; this ticket must not grow
      into the twelve-name shout animation API.
- [ ] Verify live that a weapon-drawn cast reaches the `CombatReady_*` branch and a sheathed
      cast still reaches `MT_*`.
- [ ] Capture a frame for each stance. The animation claim is visual and is not proven by an
      event trace.
- [ ] Note that ticket 05's SYHO conflict may mask the visual result entirely. Either
      sequence this after 05 or disable SYHO for the observation and say which was done.
