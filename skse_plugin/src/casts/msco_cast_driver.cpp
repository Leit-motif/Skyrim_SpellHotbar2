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
	}

	bool begin(RE::PlayerCharacter* pc, hand_mode hand)
	{
		if (!pc) {
			return false;
		}
		// Minimal slice: one state, one clip (MSCO_left1.hkx, whose SpellFire annotation
		// is the RIGHT-hand event); every hand routes into it. The entry transition lives
		// in magicbehavior's MagicRoot, so a false here means the magic stance is not
		// drawn and the controller tears the cast down through its normal failure path.
		(void)hand;
		const bool sent = pc->NotifyAnimationGraph("SH2_CastRight"sv);
		logger::debug("SH2 cast: notified SH2_CastRight -> {}", sent);
		return sent;
	}

	void relay_from_graph_event(RE::Actor*, const RE::BSFixedString& a_tag)
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
		if (pc && is_active(pc)) {
			pc->NotifyAnimationGraph("SH2_CastExit"sv);
		}
	}

	void finish(RE::PlayerCharacter* pc)
	{
		if (pc && is_active(pc)) {
			pc->NotifyAnimationGraph("SH2_CastExit"sv);
		}
	}
}
