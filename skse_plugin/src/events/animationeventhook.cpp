#include "animationeventhook.h"
#include "../casts/casting_controller.h"
#include "../casts/msco_cast_driver.h"
#include "../logger/logger.h"

using namespace std::literals;

namespace SpellHotbar::events {

	namespace {
		// Every chaining question this integration has turns on the ORDER of a handful of
		// events on the player's stream -- the state's own SH2_*, the commitment point, and
		// the MCO/MSCO annotations on the clip it borrows. Two documents disagree about that
		// order, and one traced cast settles it, so the trace is kept rather than improvised
		// again next time.
		//
		// Bounded twice over, because this runs on the animation thread and the logger flushes
		// on every line: only while the cast state is live, and only for the tags this
		// integration turns on. An ordinary MCO swing raises `MCO_*` and `attack*` several times
		// a second and must never reach the file.
		bool is_traced_graph_event(const RE::BSFixedString& a_tag)
		{
			if (!casts::MscoCastDriver::is_active(RE::PlayerCharacter::GetSingleton())) {
				return false;
			}
			const char* raw = a_tag.c_str();
			if (!raw) {
				return false;
			}
			const std::string_view tag{ raw };
			return tag.starts_with("SH2_"sv) || tag.starts_with("MSCO_"sv) || tag.starts_with("MCO_"sv) ||
				   tag.starts_with("SBF_"sv) || tag.starts_with("attack"sv) || tag.contains("SpellFire"sv);
		}
	}

	void Animation_event_hook::ProcessEvent(RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	{
		{
			if (!a_event || !a_event->holder) {
				return;
			}

			auto eventHolder = const_cast<RE::TESObjectREFR*>(a_event->holder);
			//auto animationGraph = static_cast<RE::BShkbAnimationGraph*>(a_eventSource);

			// THE COMMITMENT POINT (ADR 0004, ticket 07).
			//
			// The events this hook exists for. MCBO's casting clips carry a SpellFire annotation
			// at the frame the hand throws the spell, and the graph raises it as this event; a
			// cast that has heard it delivers its spell even if the casting state is torn away
			// afterwards. Vanilla casting anims raise the same events on real casts -- the flag
			// they set is cleared whenever a cast begins, so a real cast cannot leave a later
			// hotbar cast pre-committed.
			//
			// This runs on the animation thread. It sets an atomic and touches nothing else.
			if (eventHolder->IsPlayerRef()) {
				if (is_traced_graph_event(a_event->tag)) {
					logger::trace("SH2 graph event: {}", a_event->tag.c_str());
				}

				// The driver's active flag is fed from here: SH2_CastEnter / SH2_CastDone
				// arrive on this same stream.
				casts::MscoCastDriver::observe_graph_event(eventHolder->As<RE::Actor>(), a_event->tag);

				if (a_event->tag == "MLh_SpellFire_Event"sv) {
					casts::CastingController::notify_spellfire(true);
				}
				else if (a_event->tag == "MRh_SpellFire_Event"sv) {
					casts::CastingController::notify_spellfire(false);
				}
			}

			//if (eventHolder->GetFormID() == 0x14) { //playerref
			//	logger::debug("AnimationGraphEvent: {}, {}, {}", std::string(a_event->tag), eventHolder->GetFormID(), std::string(a_event->payload));
			//}
		}
	}

	RE::BSEventNotifyControl Animation_event_hook::ProcessEvent_PC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	{
		// Chain first: every earlier handler must finish with this event before our
		// observer reads it, so another mod's cleanup for the same event cannot land
		// after our state bookkeeping.
		const auto result = _ProcessEvent_PC(a_sink, a_event, a_eventSource);
		ProcessEvent(a_event, a_eventSource);
		return result;
	}

	/*RE::BSEventNotifyControl Animation_event_hook::ProcessEvent_NPC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	{
		ProcessEvent(a_event, a_eventSource);
		return _ProcessEvent_NPC(a_sink, a_event, a_eventSource);
	}*/

	void install()
	{
		Animation_event_hook::install();
	}
}
