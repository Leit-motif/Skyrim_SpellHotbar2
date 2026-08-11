#pragma once
#include "../bar/hotbar.h"

namespace SpellHotbar::casts::MscoCastDriver {

	/**
	 * Enter MCBO's own casting state for one Direct Cast: raise the hand's IsCasting*
	 * graph variable and send the matching MSCO_start_* event, the same recipe MSCO.dll
	 * runs when it intercepts BeginCastLeft/Right on a real cast.
	 */
	bool begin(RE::PlayerCharacter* pc, hand_mode hand);

	/**
	 * Is an MSCO casting state active on the graph right now? bIsMSCO is written by an
	 * is-active modifier inside those states, so it doubles as "our entry was accepted".
	 */
	bool is_active(RE::PlayerCharacter* pc);

	/**
	 * Re-enter the casting state for a sustained concentration loop if it has exited.
	 */
	bool replay(RE::PlayerCharacter* pc);

	/**
	 * Leave the casting state early and drop the IsCasting* variables.
	 */
	void cancel(RE::PlayerCharacter* pc);

	/**
	 * End-of-cast cleanup once the instance is done: never disturbs a still-playing
	 * recovery, only clears leftovers when the state is already gone.
	 */
	void finish(RE::PlayerCharacter* pc);
}
