#include "voice_cast_driver.h"
#include "../game_data/game_data.h"
#include "../input/input.h"
#include "../logger/logger.h"

namespace SpellHotbar::casts::VoiceCastDriver {

	namespace {
		RE::SpellItem* proxy_power{ nullptr };
		RE::TESForm* previous_power{ nullptr };
		bool active{ false };
		bool proxy_was_known{ false };

		bool process_voice_button(float value, float held_duration)
		{
			// Direct ShoutHandler::ProcessButton was accepted but never transitioned the graph
			// (2026-08-08, two canaries). Real hotbar shout casts work because their button
			// event travels the full input dispatch, so the synthetic press must too: push a
			// fully-formed event onto the game's own input queue and let next frame's dispatch
			// deliver it exactly like a physical keypress.
			auto queue = RE::BSInputEventQueue::GetSingleton();
			auto user_events = RE::UserEvents::GetSingleton();
			if (!queue || !user_events) {
				logger::error("Voice cast driver could not resolve BSInputEventQueue/UserEvents");
				return false;
			}

			auto [device, key] = Input::get_shout_key_and_device();
			auto event = RE::ButtonEvent::Create(device, user_events->shout, key, value, held_duration);
			if (!event) {
				logger::error("Voice cast driver could not allocate a shout button event");
				return false;
			}

			// The queue owns the event from here; the dispatcher consumes it next frame.
			queue->PushOntoInputQueue(event);
			logger::debug("Voice cast driver queued shout button (value={}, held={})", value, held_duration);
			return true;
		}
	}

	bool initialize()
	{
		if (proxy_power) {
			return true;
		}

		if (!GameData::spellhotbar_unbind_slot || !GameData::equip_slot_voice) {
			logger::error("Voice cast driver requires SpellHotbar_unbind and the Voice equip slot");
			return false;
		}

		if (GameData::spellhotbar_unbind_slot->GetSpellType() != RE::MagicSystem::SpellType::kLesserPower ||
			GameData::spellhotbar_unbind_slot->GetEquipSlot() != GameData::equip_slot_voice) {
			logger::error("SpellHotbar_unbind is not a Voice-slot Lesser Power");
			return false;
		}

		// This shipped form has one MagicEffect specifically authored as a zero-magnitude,
		// zero-duration dummy. Reusing it gives ShoutHandler a registered power with no payload.
		proxy_power = GameData::spellhotbar_unbind_slot;
		logger::info("Voice cast driver initialized with proxy {:08X}", proxy_power->GetFormID());
		return true;
	}

	bool begin(RE::PlayerCharacter* pc)
	{
		if (!pc || active || (!proxy_power && !initialize())) {
			return false;
		}
		auto equip_manager = RE::ActorEquipManager::GetSingleton();
		if (!equip_manager) {
			logger::error("Voice cast driver could not resolve ActorEquipManager");
			return false;
		}

		// ShoutHandler rejects a selected lesser power the actor does not know. Keep the proxy
		// session-only, but add and equip it for exactly the lifetime of this Direct Cast. Going
		// through ActorEquipManager is significant: assigning selectedPower alone does not build
		// the voice equipment state consumed by ShoutHandler.
		proxy_was_known = pc->HasSpell(proxy_power);
		if (!proxy_was_known && !pc->AddSpell(proxy_power)) {
			logger::error("Voice cast driver could not add proxy {:08X} to the player", proxy_power->GetFormID());
			return false;
		}

		auto& actor_data = pc->GetActorRuntimeData();
		previous_power = actor_data.selectedPower;
		equip_manager->EquipSpell(pc, proxy_power, GameData::equip_slot_voice);
		if (actor_data.selectedPower != proxy_power) {
			logger::error("Voice cast driver could not equip proxy {:08X}", proxy_power->GetFormID());
			if (!proxy_was_known && pc->HasSpell(proxy_power)) {
				pc->RemoveSpell(proxy_power);
			}
			previous_power = nullptr;
			proxy_was_known = false;
			return false;
		}
		active = true;
		logger::debug("Voice cast driver selected proxy {:08X}", proxy_power->GetFormID());
		return true;
	}

	bool press()
	{
		return active && process_voice_button(1.0f, 0.0f);
	}

	bool release(float held_duration)
	{
		return active && process_voice_button(0.0f, std::max(held_duration, 0.001f));
	}

	bool replay(float held_duration)
	{
		if (!active || !process_voice_button(1.0f, 0.0f)) {
			return false;
		}
		return process_voice_button(0.0f, std::max(held_duration, 0.001f));
	}

	void restore(RE::PlayerCharacter* pc)
	{
		if (!active) {
			return;
		}

		if (!pc) {
			pc = RE::PlayerCharacter::GetSingleton();
		}
		if (pc) {
			auto& actor_data = pc->GetActorRuntimeData();
			if (actor_data.selectedPower == proxy_power) {
				auto equip_manager = RE::ActorEquipManager::GetSingleton();
				if (equip_manager && previous_power && previous_power->GetFormType() == RE::FormType::Shout) {
					equip_manager->EquipShout(pc, previous_power->As<RE::TESShout>());
				}
				else if (equip_manager && previous_power && previous_power->GetFormType() == RE::FormType::Spell) {
					equip_manager->EquipSpell(pc, previous_power->As<RE::SpellItem>(), GameData::equip_slot_voice);
				}
				else {
					actor_data.selectedPower = previous_power;
				}
			}
			if (proxy_power && !proxy_was_known && pc->HasSpell(proxy_power)) {
				pc->RemoveSpell(proxy_power);
			}
		}

		previous_power = nullptr;
		proxy_was_known = false;
		active = false;
	}

	void cancel(RE::PlayerCharacter* pc)
	{
		if (active && pc) {
			pc->NotifyAnimationGraph("ShoutStop"sv);
		}
		restore(pc);
	}

	bool is_active()
	{
		return active;
	}
}
