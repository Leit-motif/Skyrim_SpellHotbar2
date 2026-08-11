#include "msco_cast_driver.h"
#include "../logger/logger.h"

using namespace std::literals;

namespace SpellHotbar::casts::MscoCastDriver {

	namespace {

		// Which hand's state the current Direct Cast entered, so replay/cancel address the
		// same one. hand_mode::end doubles as "no entry live".
		hand_mode active_hand{ hand_mode::end };

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

		// MSCO.dll drops all three at CastingStateExit; doing the same here covers the
		// entries that never reached a state, where that event will never come.
		void drop_variables(RE::PlayerCharacter* pc) {
			pc->SetGraphVariableBool("IsCastingLeft"sv, false);
			pc->SetGraphVariableBool("IsCastingRight"sv, false);
			pc->SetGraphVariableBool("IsCastingDual"sv, false);
		}

		bool send_start(RE::PlayerCharacter* pc, hand_mode hand) {
			const auto names = names_for(hand);
			if (!pc->SetGraphVariableBool(names.variable, true)) {
				logger::warn("MSCO cast: could not set {}", names.variable);
			}
			const bool sent = pc->NotifyAnimationGraph(names.event);
			logger::debug("MSCO cast: notified {} -> {}", names.event, sent);
			if (!sent) {
				drop_variables(pc);
			}
			return sent;
		}
	}

	bool begin(RE::PlayerCharacter* pc, hand_mode hand)
	{
		if (!pc) {
			return false;
		}
		if (send_start(pc, hand)) {
			active_hand = hand;
			return true;
		}
		active_hand = hand_mode::end;
		return false;
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
		return send_start(pc, active_hand);
	}

	void cancel(RE::PlayerCharacter* pc)
	{
		if (pc) {
			if (is_active(pc)) {
				const bool sent = pc->NotifyAnimationGraph("MCO_EndAnimation"sv);
				logger::debug("MSCO cast: notified MCO_EndAnimation -> {}", sent);
			}
			drop_variables(pc);
		}
		active_hand = hand_mode::end;
	}

	void finish(RE::PlayerCharacter* pc)
	{
		if (pc && !is_active(pc)) {
			drop_variables(pc);
		}
		active_hand = hand_mode::end;
	}
}
