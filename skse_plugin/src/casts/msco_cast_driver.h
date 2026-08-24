#pragma once
#include "../bar/hotbar.h"
#include "combo_cache.h"

namespace SpellHotbar::casts::MscoCastDriver {

	/**
	 * Enter the shtb cast state. The event is SH2_CastRight for combo step 1 and
	 * SH2_Cast2/3/4 for the later clips (MSCO_left2/3/4.hkx). Returns the notify
	 * result — false means no active listener (no hosting drawn idle: sheathed, or
	 * mid-swing in AttackState) and the caller tears the cast down.
	 *
	 * Every clip in the set raises a LEFT-hand SpellFire (OAR Base-default variants:
	 * 0.48s / 0.30s / 0.35s / 0.92s), so the driver still arms the left bit only.
	 *
	 * `charge_time` is written to `MSCO_attackspeed` before the notify so the clip
	 * plays at MSCO's charge-scaled pace (ticket 18). WASD capture during the state
	 * is ticket 19; bAnimationDriven comes from the shtb graph wrap (ticket 21).
	 *
	 * `shape` picks the entry and what the state is for. A fire-and-forget press enters
	 * the clip set above. A channel enters SH2_CastChannel instead, a state of its own
	 * holding a MODE_LOOPING clip on the shout-inhale path, and stays there for the whole
	 * hold until end_channel sends SH2_CastExit. It walks no clip index and opens no
	 * follow-up window; OAR still picks the per-family clip from the animation-type
	 * global, exactly as it does for the throw set (ADR-0013, ticket 28).
	 */
	bool begin(RE::PlayerCharacter* pc, hand_mode hand, float charge_time, CastShape shape);

	/**
	 * Load MSCO.ini's charge-to-speed curve. Called at DataLoaded and again at each
	 * begin() so a saved MSCO.ini is picked up without a restart. Missing file keeps
	 * the shipped exponential defaults. Does not read MSCO.dll or Menu Framework.
	 */
	void load_charge_curve();

	/**
	 * Is the current Driver Cast inside its SpellFire-to-WinClose combo window? A
	 * follow-up hotbar press chains only while this is true.
	 */
	bool combo_window_open();

	/**
	 * Observe the player's animation-event stream. Ends the state on SH2_CastExit,
	 * records MCO combo position at attack-time events, and writes that position
	 * back once after a Driver Cast's ready-state reset (ADR-0005 named exception).
	 *
	 * `a_payload` is the event's own payload member, and it is not optional decoration:
	 * a clip annotation `PIE.@SGVI|MCO_nextattack|3` reaches the sink split at the first
	 * `.` into the event NAME `PIE` and the payload `@SGVI|MCO_nextattack|3`, so MCO's
	 * combo advance is carried there rather than in the tag (ticket 29). Both fields are
	 * run through the same SGVI parser: a pack that writes the annotation without a `.`
	 * puts it in the tag instead, and neither form should be the one that works.
	 *
	 * It also tracks which swing is open (opened at MCO_AttackInitiate /
	 * MCO_PowerAttackInitiate for real swings, closed at attackStop / MCO_EndAnimation or
	 * when the initiate belongs to our own borrowed clip), so a cast that interrupts a
	 * swing can tell a pre-advance sample -- which would replay the interrupted swing --
	 * from the successor it is supposed to hand on.
	 */
	void observe_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_tag,
		const RE::BSFixedString& a_payload);

	/**
	 * Is the shtb cast state live right now? Raised from the entry notify's own return and
	 * cleared on the state ending, so a dropped event cannot leak into the next cast.
	 */
	bool is_active();

	/**
	 * Should the player's graph events be traced right now?
	 *
	 * True while the state is live, and then for a bounded burst of events after a cut. The
	 * burst is the point: the events that say whether a cut actually handed off to an attack
	 * all arrive *after* the state is gone, so a trace that stopped at the cut would go dark
	 * exactly where the only interesting question is. Bounded by a count rather than a clock
	 * because this is read on the animation thread.
	 */
	bool should_trace_graph_events();

	/**
	 * End a concentration channel: hand its combo position on and make sure the state is
	 * gone. Sent whether or not the state is still live -- a channel normally leaves it at
	 * the end of the start clip, so by release there is usually nothing left to exit.
	 */
	void end_channel(RE::PlayerCharacter* pc);

	/**
	 * Leave the cast state early via SH2_CastExit. Sent unconditionally: the event's
	 * only listener is the state's own local transition, so it reaches nothing when
	 * the state is not live, and a missed exit cannot strand the state.
	 */
	void cancel(RE::PlayerCharacter* pc);

	/**
	 * Arm the last sampled MCO combo so the next ready/PIE write restores it.
	 * Abilities reuse this (ADR-0005 named exception); they must not write
	 * MCO_nextattack=1 on entry.
	 */
	void arm_combo_restore();

	/**
	 * End-of-cast cleanup; same send as cancel(), kept separate for call-site intent.
	 * The state also exits itself through an end-of-clip trigger raising SH2_CastExit.
	 */
	void finish(RE::PlayerCharacter* pc);

	void reset_session();

	/**
	 * Read-only view of the armed combo restore, for the ticket-28 probe. Throwaway: it
	 * exists so the input thread can report what the driver is holding at the moment of an
	 * attack press without reaching into the cache or changing its one-shot semantics.
	 */
	bool combo_restore_pending();
	std::optional<McoCombo> combo_restore_peek();

	/**
	 * Interrupt the left MagicCaster when that hand holds a spell. Driver Cast
	 * begin, Ability begin, and yielding our shtb clip all use this so a cancelled
	 * MSCO left charge cannot stick IsCasting until sheathe.
	 */
	void interrupt_left_caster_if_spell(RE::PlayerCharacter* pc);
}
