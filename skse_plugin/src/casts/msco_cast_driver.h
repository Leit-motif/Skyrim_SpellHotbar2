#pragma once
#include "../bar/hotbar.h"

namespace SpellHotbar::casts::MscoCastDriver {

	/**
	 * Enter the sh2c cast state: send SH2_CastRight, whose listener this project's own
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
	void relay_from_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_tag);

	/**
	 * Is the sh2c cast state live right now? Reads the flag maintained by
	 * relay_from_graph_event; begin() resets it so a dropped SH2_CastDone cannot
	 * leak into the next cast.
	 */
	bool is_active(RE::PlayerCharacter* pc);

	/**
	 * Re-send the entry for a sustained concentration loop if the state has exited.
	 */
	bool replay(RE::PlayerCharacter* pc);

	/**
	 * Leave the cast state early via SH2_CastExit (no-op when the state is not live).
	 */
	void cancel(RE::PlayerCharacter* pc);

	/**
	 * End-of-cast cleanup: sends SH2_CastExit if the state is still live; the state
	 * also exits itself through an end-of-clip trigger raising the same event.
	 */
	void finish(RE::PlayerCharacter* pc);
}
