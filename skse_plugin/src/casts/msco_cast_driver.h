#pragma once
#include "../bar/hotbar.h"

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
	 */
	bool begin(RE::PlayerCharacter* pc, hand_mode hand, float charge_time);

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
	 */
	void observe_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_tag);

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
	 * Re-send the entry for a sustained concentration loop if the state has exited.
	 */
	bool replay(RE::PlayerCharacter* pc);

	/**
	 * Leave the cast state early via SH2_CastExit. Sent unconditionally: the event's
	 * only listener is the state's own local transition, so it reaches nothing when
	 * the state is not live, and a missed exit cannot strand the state.
	 */
	void cancel(RE::PlayerCharacter* pc);

	/**
	 * End-of-cast cleanup; same send as cancel(), kept separate for call-site intent.
	 * The state also exits itself through an end-of-clip trigger raising SH2_CastExit.
	 */
	void finish(RE::PlayerCharacter* pc);

	void reset_session();
}
