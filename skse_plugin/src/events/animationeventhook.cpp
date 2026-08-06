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
			// The one event this hook exists for. Vanilla puts the magic out roughly a tenth of a
			// second into the exhale and this is the graph saying so; a cast that has heard it
			// delivers its spell even if the shout state is torn away afterwards. Casting rides
			// the shout graph (finding 1), so this is the same event a real shout raises -- the
			// flag it sets is cleared whenever a cast begins, so a shout on the vanilla key cannot
			// leave a later hotbar cast pre-committed.
			//
			// This runs on the animation thread. It sets an atomic and touches nothing else.
			if (eventHolder->IsPlayerRef() && a_event->tag == "Voice_SpellFire_Event"sv) {
				casts::CastingController::notify_spellfire();
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
