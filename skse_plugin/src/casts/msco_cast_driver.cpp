#include "msco_cast_driver.h"
#include <atomic>
#include "../logger/logger.h"

using namespace std::literals;

namespace SpellHotbar::casts::MscoCastDriver {

	namespace {

		// Which hand's state the current Direct Cast entered, so replay/cancel address the
		// same one. hand_mode::end doubles as "no entry live".
		hand_mode active_hand{ hand_mode::end };

		// A send waiting for graph-event dispatch context. A mod-declared event name
		// delivers when notified from inside BSAnimationGraphEvent processing -- where
		// MSCO.dll sends MSCO_start_* itself (live-verified 10:48:39) -- and returns false
		// from the Papyrus VM and input paths (live-verified seven times in the same
		// session). A send that fails directly is parked here; the animation-event hook
		// replays it from dispatch context on the next player graph event. The hand is
		// baked into the value so the hook thread never reads active_hand.
		enum class Pending : uint8_t {
			kNone = 0,
			kStartLeft,
			kStartRight,
			kStartDual,
			kEnd,
		};
		std::atomic<Pending> pending{ Pending::kNone };

		struct GraphNames {
			std::string_view variable;
			std::string_view event;
		};

		GraphNames names_for(hand_mode hand) {
			switch (hand) {
				case hand_mode::right_hand:
					return { "IsCastingRight"sv, "MSCO_start_right"sv };
				case hand_mode::dual_hand:
					return { "IsCastingDual"sv, "MSCO_start_dual"sv };
				default:
					return { "IsCastingLeft"sv, "MSCO_start_left"sv };
			}
		}

		Pending pending_for(hand_mode hand) {
			switch (hand) {
				case hand_mode::right_hand:
					return Pending::kStartRight;
				case hand_mode::dual_hand:
					return Pending::kStartDual;
				default:
					return Pending::kStartLeft;
			}
		}

		hand_mode hand_for(Pending p) {
			switch (p) {
				case Pending::kStartRight:
					return hand_mode::right_hand;
				case Pending::kStartDual:
					return hand_mode::dual_hand;
				default:
					return hand_mode::left_hand;
			}
		}

		// MSCO.dll drops all three at CastingStateExit; doing the same here covers the
		// entries that never reached a state, where that event will never come.
		void drop_variables(RE::Actor* actor) {
			actor->SetGraphVariableBool("IsCastingLeft"sv, false);
			actor->SetGraphVariableBool("IsCastingRight"sv, false);
			actor->SetGraphVariableBool("IsCastingDual"sv, false);
		}

		bool send_start(RE::Actor* actor, hand_mode hand, std::string_view context) {
			const auto names = names_for(hand);
			if (!actor->SetGraphVariableBool(names.variable, true)) {
				logger::warn("MSCO cast: could not set {}", names.variable);
			}
			const bool sent = actor->NotifyAnimationGraph(names.event);
			logger::debug("MSCO cast: notified {} from {} -> {}", names.event, context, sent);
			return sent;
		}
	}

	bool begin(RE::PlayerCharacter* pc, hand_mode hand)
	{
		if (!pc) {
			return false;
		}
		active_hand = hand;
		if (send_start(pc, hand, "the calling thread"sv)) {
			pending.store(Pending::kNone, std::memory_order_relaxed);
			return true;
		}
		// Direct delivery failed; park the entry for the hook. The controller's entry
		// grace covers the wait, and its timeout path tears down through finish().
		pending.store(pending_for(hand), std::memory_order_relaxed);
		return true;
	}

	void relay_from_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_carrier)
	{
		// Cleared before sending: the send itself dispatches events that re-enter the
		// hook, and the re-entrant call must find nothing to do.
		const Pending p = pending.exchange(Pending::kNone, std::memory_order_relaxed);
		if (p == Pending::kNone || !a_player) {
			return;
		}
		if (p == Pending::kEnd) {
			const bool sent = a_player->NotifyAnimationGraph("MCO_EndAnimation"sv);
			logger::debug("MSCO cast: notified MCO_EndAnimation from graph-event context (carrier '{}') -> {}",
				a_carrier.c_str(), sent);
			return;
		}
		const bool sent = send_start(a_player, hand_for(p), "graph-event context"sv);
		if (sent) {
			logger::debug("MSCO cast: relay carried by graph event '{}'", a_carrier.c_str());
		}
	}

	bool is_active(RE::PlayerCharacter* pc)
	{
		bool active{ false };
		if (pc) {
			pc->GetGraphVariableBool("bIsMSCO"sv, active);
		}
		return active;
	}

	bool replay(RE::PlayerCharacter* pc)
	{
		if (!pc || active_hand == hand_mode::end) {
			return false;
		}
		if (is_active(pc)) {
			return true;
		}
		if (!send_start(pc, active_hand, "the calling thread"sv)) {
			pending.store(pending_for(active_hand), std::memory_order_relaxed);
		}
		return true;
	}

	void cancel(RE::PlayerCharacter* pc)
	{
		if (pc) {
			if (is_active(pc)) {
				if (!pc->NotifyAnimationGraph("MCO_EndAnimation"sv)) {
					// The exit event is mod-declared too; park it for dispatch context.
					pending.store(Pending::kEnd, std::memory_order_relaxed);
				}
			}
			else {
				// The state never entered; a parked start must not fire later.
				pending.store(Pending::kNone, std::memory_order_relaxed);
			}
			drop_variables(pc);
		}
		else {
			pending.store(Pending::kNone, std::memory_order_relaxed);
		}
		active_hand = hand_mode::end;
	}

	void finish(RE::PlayerCharacter* pc)
	{
		pending.store(Pending::kNone, std::memory_order_relaxed);
		if (pc && !is_active(pc)) {
			drop_variables(pc);
		}
		active_hand = hand_mode::end;
	}
}
