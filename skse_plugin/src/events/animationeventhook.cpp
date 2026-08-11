#include "animationeventhook.h"
#include "../casts/casting_controller.h"

using namespace std::literals;

namespace SpellHotbar::events {

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
				if (a_event->tag == "MLh_SpellFire_Event"sv) {
					casts::CastingController::notify_spellfire(true);
				}
				else if (a_event->tag == "MRh_SpellFire_Event"sv) {
					casts::CastingController::notify_spellfire(false);
				}
			}

			//if (eventHolder->GetFormID() == 0x14) { //playerref
			//	logger::info("AnimationGraphEvent: {}, {}, {}", std::string(a_event->tag), eventHolder->GetFormID(), std::string(a_event->payload));
			//}
		}
	}

	RE::BSEventNotifyControl Animation_event_hook::ProcessEvent_PC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	{
		ProcessEvent(a_event, a_eventSource);
		return _ProcessEvent_PC(a_sink, a_event, a_eventSource);	
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
