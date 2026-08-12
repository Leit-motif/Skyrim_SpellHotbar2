#pragma once
#include "../bar/hotbar.h"

namespace SpellHotbar::casts::MscoCastDriver {

	/**
	 * Enter the shtb cast state: send SH2_CastRight, whose listener this project's own
	 * Nemesis patch authored on the root state machine of magicbehavior and of
	 * 1hm_behavior. Returns the notify result — false means no active listener (no
	 * hosting drawn idle: sheathed, or mid-swing in AttackState) and the caller tears
	 * the cast down. Minimal slice: every hand routes into the one state, whose clip
	 * (MSCO_left1.hkx) raises the LEFT-hand SpellFire at 0.483s whatever hand the cast
	 * chose (ADR-0006 as amended 2026-08-12).
	 */
	bool begin(RE::PlayerCharacter* pc, hand_mode hand);

	/**
	 * Observe the player's animation-event stream for the one event that ends the state,
	 * SH2_CastExit. Called by the animation-event hook for every player graph event.
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
}
