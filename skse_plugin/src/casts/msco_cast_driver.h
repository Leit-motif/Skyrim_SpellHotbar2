#pragma once
#include "../bar/hotbar.h"

namespace SpellHotbar::casts::MscoCastDriver {

	/**
	 * Enter the shtb cast state: send SH2_CastRight, whose listener this project's own
	 * Nemesis patch authored on magicbehavior's root state machine. Returns the notify
	 * result — false means no active listener (magic stance not drawn) and the caller
	 * tears the cast down. Minimal slice: every hand routes into the one state, whose
	 * clip (MSCO_left1.hkx) raises the RIGHT-hand SpellFire at 0.283s.
	 */
	bool begin(RE::PlayerCharacter* pc, hand_mode hand);

	/**
	 * Observe the player's animation-event stream: SH2_CastEnter (the state's
	 * enterNotifyEvents) raises the active flag, SH2_CastDone (exitNotifyEvents)
	 * clears it. Called by the animation-event hook for every player graph event.
	 */
	void observe_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_tag);

	/**
	 * Is the shtb cast state live right now? Reads the flag maintained by
	 * observe_graph_event; begin() resets it so a dropped SH2_CastDone cannot
	 * leak into the next cast.
	 */
	bool is_active(RE::PlayerCharacter* pc);

	/**
	 * Re-send the entry for a sustained concentration loop if the state has exited.
	 */
	bool replay(RE::PlayerCharacter* pc);

	/**
	 * Leave the cast state early via SH2_CastExit. Sent unconditionally: the event's
	 * only listener is the state's own local transition, so it reaches nothing when
	 * the state is not live, and a missed SH2_CastEnter cannot strand the state.
	 */
	void cancel(RE::PlayerCharacter* pc);

	/**
	 * End-of-cast cleanup; same send as cancel(), kept separate for call-site intent.
	 * The state also exits itself through an end-of-clip trigger raising SH2_CastExit.
	 */
	void finish(RE::PlayerCharacter* pc);
}
