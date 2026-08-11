#include "msco_cast_driver.h"
#include <atomic>
#include "../logger/logger.h"

using namespace std::literals;

namespace SpellHotbar::casts::MscoCastDriver {

	namespace {
		// Set when the graph raises SH2_CastEnter (our state's enterNotifyEvents), cleared
		// on SH2_CastDone (exitNotifyEvents). This rides the same anim-event stream
		// MSCO.dll uses to hear CastingStateExit, so it is the authoritative "our casting
		// state is live" signal and needs no graph-variable polling.
		std::atomic<bool> state_active{ false };

		// SH2_CastExit is this mod's own event: the only listener is the state-local
		// transition inside SH2_CastRight_State, so sending it while the state is not
		// live reaches nothing. Sending unconditionally covers the case where entry
		// happened but SH2_CastEnter never reached the hook.
		void send_exit(RE::PlayerCharacter* pc)
		{
			if (pc) {
				pc->NotifyAnimationGraph("SH2_CastExit"sv);
			}
		}
	}

	bool begin(RE::PlayerCharacter* pc, hand_mode hand)
	{
		if (!pc) {
			return false;
		}
		// A stale flag must not close this cast's entry grace: if the graph tore down
		// without delivering SH2_CastDone (load, teardown mid-state), the flag would
		// still read true here. Each cast starts from false; its own SH2_CastEnter
		// raises it again.
		state_active.store(false, std::memory_order_relaxed);
		// Minimal slice: one state, one clip (MSCO_left1.hkx, whose SpellFire annotation
		// is the RIGHT-hand event); every hand routes into it. The entry transition lives
		// in magicbehavior's MagicRoot, so a false here means the magic stance is not
		// drawn and the controller tears the cast down through its normal failure path.
		(void)hand;
		const bool sent = pc->NotifyAnimationGraph("SH2_CastRight"sv);
		logger::debug("SH2 cast: notified SH2_CastRight -> {}", sent);
		return sent;
	}

	void observe_graph_event(RE::Actor*, const RE::BSFixedString& a_tag)
	{
		if (a_tag == "SH2_CastEnter"sv) {
			state_active.store(true, std::memory_order_relaxed);
			logger::debug("SH2 cast: state entered");
		}
		else if (a_tag == "SH2_CastDone"sv) {
			state_active.store(false, std::memory_order_relaxed);
			logger::debug("SH2 cast: state exited");
		}
	}

	bool is_active(RE::PlayerCharacter*)
	{
		return state_active.load(std::memory_order_relaxed);
	}

	bool replay(RE::PlayerCharacter* pc)
	{
		if (!pc) {
			return false;
		}
		if (is_active(pc)) {
			return true;
		}
		return pc->NotifyAnimationGraph("SH2_CastRight"sv);
	}

	void cancel(RE::PlayerCharacter* pc)
	{
		send_exit(pc);
	}

	void finish(RE::PlayerCharacter* pc)
	{
		send_exit(pc);
	}
}
