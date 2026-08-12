#include "cast_intent.h"
#include "casting_controller.h"
#include "../extern/ShoutMCO_CastIntent.h"
#include "../game_data/game_data.h"
#include "../logger/logger.h"
#include "../rendering/render_manager.h"

namespace SpellHotbar::casts::CastIntent {

	namespace {
		// What the pressed slot meant at the moment of the press. Held by value, and by FormID
		// rather than by pointer, so nothing here can dangle while the intent waits.
		struct Payload {
			size_t slot;
			RE::FormID formID;
			slot_type type;
			hand_mode hand;
			// Points into Input::key_spells / key_oblivion_*, which are globals: safe to keep.
			const Input::KeyBind* keybind;
		};

		const ShoutMCO_CastIntentApi* api{ nullptr };
		Status status{ Status::unavailable };

		// The one intent this mod can own. `live_handle` is invalid exactly when no payload is
		// retained; the two are always cleared together.
		ShoutMCO_CastHandle live_handle{ SHOUTMCO_CAST_HANDLE_INVALID };
		Payload payload{};
		bool payload_retained{ false };

		// True only while the release callback re-attempts the press. The seam reads it and
		// declines to offer again, which is what makes a release attempt exactly one attempt
		// instead of a defer/refuse ping-pong.
		bool attempting_release{ false };

		void clear_payload()
		{
			live_handle = SHOUTMCO_CAST_HANDLE_INVALID;
			payload_retained = false;
			payload = {};
		}

		const char* cause_name(uint32_t a_cause)
		{
			// Diagnostic only -- never branch cast behaviour on the cause. Unknown values are
			// generic by contract, because a minor version may append to the enum.
			switch (a_cause) {
			case SHOUTMCO_CAUSE_READY:        return "ready";
			case SHOUTMCO_CAUSE_REPLACED:     return "replaced";
			case SHOUTMCO_CAUSE_CANCELLED:    return "cancelled";
			case SHOUTMCO_CAUSE_CONTEXT_LOST: return "context lost";
			case SHOUTMCO_CAUSE_WATCHDOG:     return "watchdog";
			default:                          return "unspecified";
			}
		}

		/**
		 * The release attempt. The intent may have waited hundreds of milliseconds, so every
		 * precondition the original press passed is checked again here, and a payload that no
		 * longer holds is discarded once with no retry.
		 */
		void attempt_release(const Payload& p)
		{
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (!pc || pc->IsDead() || !Input::in_ingame_state()) {
				logger::debug("SH2 cast intent: released into a state that cannot cast, discarded");
				return;
			}

			// Slot assignment: the bar may have switched (sneak) or the slot been rebound.
			auto skill = GameData::get_current_spell_info_in_slot(p.slot);
			if (skill.formID != p.formID || skill.type != p.type || skill.hand != p.hand) {
				logger::debug("SH2 cast intent: slot {} no longer holds the pressed skill, discarded", p.slot);
				return;
			}

			// The same gates InputModeCast applies to a live keypress: cooldown, 3D, controls,
			// furniture, mount, no cast already running, and the cast-only restrictions.
			if (!Input::allowed_to_instantcast(skill.formID) ||
				!CastingController::can_start_new_cast() ||
				!Input::allowed_to_cast(skill.formID)) {
				logger::debug("SH2 cast intent: preconditions no longer met on release, discarded");
				RenderManager::highlight_skill_slot(static_cast<int>(p.slot), 0.5f, true);
				return;
			}

			auto form = RE::TESForm::LookupByID(skill.formID);
			if (!form) {
				return;
			}

			attempting_release = true;
			const bool success = CastingController::try_start_cast(form, *p.keybind, p.slot, skill.hand);
			attempting_release = false;

			logger::debug("SH2 cast intent: released cast on slot {} -> {}", p.slot, success);
			RenderManager::highlight_skill_slot(static_cast<int>(p.slot), 0.5f, !success);
		}

		/**
		 * ShoutMCO's single callback, fired exactly once per deferred intent, on the main thread.
		 *
		 * It is never re-entrant: ShoutMCO retires the intent under its own mutex but only queues
		 * the callback as a task, so this always arrives on a later drain — after the `Request` or
		 * `Cancel` that retired it has long returned. The handle check is what makes that safe.
		 * A callback for an intent this mod has already replaced or withdrawn names a handle that
		 * is no longer the live one, and must not touch the payload that took its place.
		 */
		void on_intent(ShoutMCO_CastHandle a_handle, ShoutMCO_CastOutcome a_outcome, uint32_t a_cause, void*)
		{
			if (!payload_retained || a_handle != live_handle) {
				// An intent this mod already retired. Nothing of ours is left to clear.
				return;
			}

			const Payload p = payload;
			clear_payload();

			if (a_outcome == SHOUTMCO_CAST_RELEASE) {
				attempt_release(p);
			}
			else {
				logger::debug("SH2 cast intent: abandoned ({}), payload on slot {} dropped",
					cause_name(a_cause), p.slot);
			}
		}
	}

	void negotiate()
	{
		if (auto dll = GetModuleHandleA("ShoutMCO.dll")) {
			auto get_api = reinterpret_cast<ShoutMCO_GetCastIntentApi_t>(
				GetProcAddress(dll, SHOUTMCO_CAST_INTENT_EXPORT));
			if (get_api) {
				// A null return is the incompatible case, not an error: ShoutMCO refuses a major
				// version it does not implement, and we fall back to native behaviour.
				api = get_api(SHOUTMCO_CAST_INTENT_VERSION_MAJOR);
				status = api ? Status::active : Status::incompatible;
			}
		}
		logger::info("SpellHotbar2 ShoutMCO cast-intent API: {}", status_name());
	}

	Status get_status()
	{
		return status;
	}

	const char* status_name()
	{
		switch (status) {
		case Status::active:       return "active";
		case Status::incompatible: return "incompatible";
		default:                   return "unavailable";
		}
	}

	bool offer(size_t slot, const Input::KeyBind& keybind)
	{
		if (!api || attempting_release) {
			return false;
		}

		// Snapshot the slot as the press saw it, before handing the intent over.
		const auto skill = GameData::get_current_spell_info_in_slot(slot);

		ShoutMCO_CastRequest req{};
		req.structSize = sizeof(req);
		req.versionMajor = SHOUTMCO_CAST_INTENT_VERSION_MAJOR;
		req.versionMinor = SHOUTMCO_CAST_INTENT_VERSION_MINOR;
		req.flags = 0u;
		req.callback = &on_intent;
		req.context = nullptr;

		// A deferred request replaces whatever intent was pending, whoever owned it. The displaced
		// owner's abandon callback is queued, not called here, so if it was one of ours it lands
		// after this returns and finds a handle that no longer matches -- which is exactly what
		// keeps it from clearing the payload taken below.
		ShoutMCO_CastHandle handle{ SHOUTMCO_CAST_HANDLE_INVALID };
		const auto decision = api->Request(&req, &handle);
		if (decision != SHOUTMCO_CAST_DEFERRED) {
			logger::debug("SH2 cast intent: not deferred (decision {}), refusing as before",
				static_cast<int>(decision));
			return false;
		}

		live_handle = handle;
		payload = Payload{ slot, skill.formID, skill.type, skill.hand, &keybind };
		payload_retained = true;

		logger::debug("SH2 cast intent: slot {} deferred to ShoutMCO", slot);
		return true;
	}

	void cancel()
	{
		if (!api || live_handle == SHOUTMCO_CAST_HANDLE_INVALID) {
			return;
		}
		api->Cancel(live_handle);
		// Drop local ownership now rather than waiting for the abandon callback, which ShoutMCO
		// queues onto a later drain. Waiting would leave a withdrawn payload looking pending for
		// the rest of the frame — and on a game load, past the point the bar it names still
		// exists. The queued callback then finds a stale handle and does nothing, as intended.
		clear_payload();
	}
}
