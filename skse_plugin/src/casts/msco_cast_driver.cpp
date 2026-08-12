#include "msco_cast_driver.h"
#include <atomic>
#include "../logger/logger.h"

using namespace std::literals;

namespace SpellHotbar::casts::MscoCastDriver {

	namespace {
		// True from a consumed SH2_CastRight send until the clip's end-of-clip trigger
		// (SH2_CastExit) crosses the anim-event stream. The state's enter/exit notify
		// events cannot feed this flag: runtime-verified 2026-08-11, the sink receives
		// SH2_CastEnter only when the state EXITS (bundled with the exit batch), and
		// SH2_CastDone never arrives at all — while the clip's own annotations and its
		// trigger array deliver on schedule. The notify's true return is the entry
		// signal (a consumed transition), the clip trigger is the exit signal.
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
		// Minimal slice: one state, one clip (MSCO_left1.hkx, whose SpellFire annotation
		// is the LEFT-hand event); every hand routes into it. The entry transition lives
		// in magicbehavior's MagicRoot, so a false here means the magic stance is not
		// drawn and the controller tears the cast down through its normal failure path.
		// A true means the transition was consumed and the state is live NOW — the clip
		// starts this frame (annotations verified on schedule from the send timestamp).
		(void)hand;
		const bool sent = pc->NotifyAnimationGraph("SH2_CastRight"sv);
		state_active.store(sent, std::memory_order_relaxed);
		logger::debug("SH2 cast: notified SH2_CastRight -> {}", sent);
		return sent;
	}

	void observe_graph_event(RE::Actor*, const RE::BSFixedString& a_tag)
	{
		// SH2_CastExit reaches this sink two ways, both meaning the state is ending: the
		// clip's own end-of-clip trigger, and our cancel/finish sends echoed back. Do NOT
		// react to SH2_CastEnter here — it is delivered only in the exit batch (after
		// SH2_CastExit), so handling it would re-raise the flag on a state that just died.
		if (a_tag == "SH2_CastExit"sv) {
			state_active.store(false, std::memory_order_relaxed);
			logger::debug("SH2 cast: state exiting (clip end or cancel)");
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
		const bool sent = pc->NotifyAnimationGraph("SH2_CastRight"sv);
		state_active.store(sent, std::memory_order_relaxed);
		return sent;
	}

	void cancel(RE::PlayerCharacter* pc)
	{
		send_exit(pc);
		state_active.store(false, std::memory_order_relaxed);
	}

	void finish(RE::PlayerCharacter* pc)
	{
		send_exit(pc);
		state_active.store(false, std::memory_order_relaxed);
	}
}
