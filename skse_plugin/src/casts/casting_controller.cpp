#include "casting_controller.h"
#include <atomic>
#include "../logger/logger.h"
#include "../game_data/game_data.h"
#include "../game_data/cast_anim_ids.h"
#include "../input/keybinds.h"
#include "../rendering/render_manager.h"
#include "spell_proc.h"
#include "msco_cast_driver.h"
#include "art_driver.h"
#include "combo_cache.h"
#include "cast_intent.h"
#include "unpaused_clock.h"
#include "../game_data/custom_ability_config.h"

namespace SpellHotbar::casts::CastingController {

	std::unique_ptr<BaseCastingInstance> current_cast = nullptr;

	// THE DEFERRED selectedPower WRITE-BACK (thuum ticket 62).
	//
	// A hotbar shout swaps its own form into the player's selectedPower so the injected "Shout"
	// button press fires OUR shout instead of the equipped one. The cast instance dies at GCD
	// expiry, which is ~1.8s BEFORE the graph leaves the shout (measured 2026-08-22: restore at
	// 55.114, shoutStop at 56.959), and the old code restored there. Words two and three of a
	// three-word shout echo after that point, and everything that reads selectedPower then sees
	// the equipped shout: SKSE's kVoiceFire reported the wrong form to our own bookkeeping, and
	// OAR re-selected the exhale clip against it. That is the misroute.
	//
	// The window we need therefore outlives the instance, so the hold lives here on the
	// controller. The instance hands over its pair of forms and dies on schedule; nothing about
	// set_casted, the GCD, or cooldown timing moves.
	struct DeferredPowerRestore {
		RE::TESForm* swapped{ nullptr };   // what we wrote into selectedPower
		RE::TESForm* old_form{ nullptr };  // what belongs there
		float age{ 0.0f };
		bool active{ false };
	};
	DeferredPowerRestore deferred_restore{};

	// Belt and braces. If IsShouting never falls — graph wedged, actor yanked out of the clip,
	// a mod holding the variable — the swap must not outlive the shout by much. The longest
	// vanilla three-word shout is comfortably inside this.
	constexpr float deferred_restore_timeout{ 8.0f };

	// Put the old power back, but only if the slot is still holding what we put there: the
	// player may have equipped something else in the meantime, and that choice wins.
	void write_back_power(RE::TESForm* swapped, RE::TESForm* old_form, const char* why)
	{
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (!pc) {
			return;
		}
		auto& dat = pc->GetActorRuntimeData();
		const auto cur_id = dat.selectedPower ? dat.selectedPower->GetFormID() : 0u;
		const bool applied = dat.selectedPower == swapped;
		if (applied) {
			dat.selectedPower = old_form;
		}
		logger::debug("SH2 power: restored selectedPower {:08X} -> {:08X} (applied={}, {})", cur_id,
			old_form ? old_form->GetFormID() : 0u, applied, why);
	}

	void flush_deferred_power_restore()
	{
		if (!deferred_restore.active) {
			return;
		}
		write_back_power(deferred_restore.swapped, deferred_restore.old_form, "flush");
		deferred_restore = {};
	}

	// Forget a pending write-back without performing it. Used on a game load, where the save
	// being read owns selectedPower and a pre-load form must never be written over it.
	void discard_deferred_power_restore()
	{
		if (!deferred_restore.active) {
			return;
		}
		logger::debug("SH2 power: dropping deferred selectedPower restore to {:08X}",
			deferred_restore.old_form ? deferred_restore.old_form->GetFormID() : 0u);
		deferred_restore = {};
	}

	void defer_power_restore(RE::TESForm* swapped, RE::TESForm* old_form)
	{
		flush_deferred_power_restore();  // never stack two holds
		deferred_restore.swapped = swapped;
		deferred_restore.old_form = old_form;
		deferred_restore.age = 0.0f;
		deferred_restore.active = true;
		logger::debug("SH2 power: holding selectedPower {:08X} until IsShouting falls (restores {:08X})",
			swapped ? swapped->GetFormID() : 0u, old_form ? old_form->GetFormID() : 0u);
	}

	void update_deferred_power_restore(float delta, bool is_shouting)
	{
		if (!deferred_restore.active) {
			return;
		}
		deferred_restore.age += delta;
		if (!is_shouting) {
			write_back_power(deferred_restore.swapped, deferred_restore.old_form, "IsShouting fell");
			deferred_restore = {};
		}
		else if (deferred_restore.age > deferred_restore_timeout) {
			write_back_power(deferred_restore.swapped, deferred_restore.old_form, "timeout");
			deferred_restore = {};
		}
	}

	// THE COMMITMENT POINT (ADR 0004 as amended by ADR 0006, ticket 07).
	//
	// The clip our shtb state plays (MSCO_left1) carries an `MLh_SpellFire_Event` annotation at
	// the exact frame the hand throws the spell (0.483s in, observed live at +0.46s), and the
	// graph raises it as an event when the clip crosses that frame. That instant is the commitment point: the magic is out,
	// so anything that ends the animation afterwards costs nothing. Before it, losing the
	// casting state cancels the cast and costs nothing either, since the magicka is only
	// deducted once `cast_spell` succeeds.
	//
	// The annotation leads, the authored cast time is the floor (ADR 0006): if the event never
	// arrives — clip replaced by an override without the annotation, graph rebuilt wrong — the
	// cast still delivers when its timer expires *and the clip has ended*, instead of silently
	// delivering nothing. Timer expiry while the clip is still playing is not that fallback
	// (clip 4's SpellFire is at ~0.92s, past a 0.5s floor).
	//
	// File-scope and atomic rather than a member, because the animation-event hook runs on the
	// game's animation thread while casts update on the game loop. The hook sets a flag; it never
	// touches an instance the loop may be destroying underneath it.
	//
	// The armed hands, the latch, and the cast they belong to share ONE word, because the three
	// have to move together. Bits 0-1 are the armed mask (bit 0 = left, bit 1 = right); a vanilla
	// cast of an equipped spell raises the same events, so an event from a hand this cast is not
	// using must not commit it. Bit 2 is the latch. The high 32 bits are a per-cast generation
	// (ticket 46, Codex review finding 4): a clip that raises a second SpellFire after a chain cut
	// would otherwise satisfy the NEXT cast's freshly armed mask, so the hook commits with a
	// compare-exchange against the generation it read for that event, and an arming that landed in
	// between drops the stale event instead.
	constexpr uint64_t spellfire_mask_bits{ 0x3ULL };
	constexpr uint64_t spellfire_seen_bit{ 0x4ULL };
	constexpr int spellfire_generation_shift{ 32 };
	std::atomic<uint64_t> spellfire_state{ 0 };

	[[nodiscard]] constexpr uint8_t spellfire_state_mask(uint64_t state) noexcept
	{
		return static_cast<uint8_t>(state & spellfire_mask_bits);
	}

	[[nodiscard]] constexpr uint32_t spellfire_state_generation(uint64_t state) noexcept
	{
		return static_cast<uint32_t>(state >> spellfire_generation_shift);
	}

	// Arm the commitment point for one cast: forget stale fires and accept only the hand(s)
	// this cast throws with. Called right before the state entry is sent.
	void arm_spellfire(hand_mode used_hand) {
		// Ticket 44 spike: the per-hand clip matrix arrives, so the mask follows the RESOLVED
		// hand instead of the borrowed clip's left-only annotation. Left arms left, right arms
		// right, and a dual cast arms both because either authored event is that cast's own.
		//
		// Arming both for dual does NOT deliver twice: `notify_spellfire` only raises the
		// `spellfire_seen` latch, and ticket 43's delivery path reads that latch once per cast.
		// Two armed events on one dual cast set the same flag twice and deliver once.
		//
		// The mask is what keeps an UNRELATED equipped-hand cast from committing this one, so a
		// hand that is not this cast's own must stay unarmed. `used_hand` is always resolved by
		// `set_weapon_dependent_casting_source` before this call, so auto/voice cannot reach
		// here; if one ever does, `spellfire_arm_mask` falls back to left (the borrowed clip's
		// own annotation) rather than arming nothing and losing the commitment point.
		const uint8_t mask = spellfire_arm_mask(static_cast<int>(used_hand));
		if (mask == spellfire_hand_bit(SpellFireHand::left) && used_hand != hand_mode::left_hand) {
			logger::debug("SH2 cast: arm_spellfire got an unresolved hand ({}), arming left",
				static_cast<int>(used_hand));
		}
		// The generation moves on every arming, so an event the hook read under the previous
		// cast's arming can no longer commit this one. It is only ever compared for equality;
		// wrapping after 2^32 casts is harmless.
		const uint32_t generation =
			spellfire_state_generation(spellfire_state.load(std::memory_order_relaxed)) + 1U;
		spellfire_state.store(
			(static_cast<uint64_t>(generation) << spellfire_generation_shift) | mask,
			std::memory_order_relaxed);
	}

	SpellFireArming spellfire_arming() {
		const uint64_t state = spellfire_state.load(std::memory_order_relaxed);
		return { spellfire_state_mask(state), spellfire_state_generation(state) };
	}

	void notify_spellfire(SpellFireHand hand, uint32_t generation, bool driver_cast_active) {
		uint64_t state = spellfire_state.load(std::memory_order_relaxed);
		// A dual cast raises both authored events and each one takes this path; setting an
		// already-set latch twice still delivers once (ticket 43), and both are logged because
		// the pair is what proves a dual clip's contract live.
		//
		// Ticket 61: `spellfire_event_commits_the_cast` rather than the mask alone. The mask says
		// which hands the last cast threw with and stays armed past that cast's own clip, so
		// vanilla releases in an armed hand were accepted here and logged as driver-cast events.
		// The driver term is the same one isolation has always used, read from the same snapshot.
		while (spellfire_state_generation(state) == generation &&
			   spellfire_event_commits_the_cast(
				   driver_cast_active, hand, spellfire_state_mask(state))) {
			if (spellfire_state.compare_exchange_weak(state, state | spellfire_seen_bit,
					std::memory_order_relaxed)) {
				logger::debug("SH2 cast: graph raised a {} SpellFire event",
					hand == SpellFireHand::left ? "left" : "right");
				return;
			}
		}
	}

	// Cleared when a cast starts, not only when one ends: a shout pressed on the vanilla key
	// raises spellfire with no cast instance live at all, and that must not leave the next hotbar
	// cast committed before it has begun.
	void clear_spellfire() {
		// Mask AND latch. Every caller is a teardown site (the instance is reset on the same
		// lines), and arm_spellfire rebuilds the whole word at the next cast start — leaving
		// the mask armed here let post-cast VANILLA SpellFire events keep setting the latch
		// (the "graph raised a ..." stream with no cast live, ticket 51 / Codex finding 4).
		// The generation survives, so an in-flight event's stale snapshot still compares.
		spellfire_state.fetch_and(~(spellfire_mask_bits | spellfire_seen_bit),
			std::memory_order_relaxed);
	}

	bool is_cast_committed() {
		return (spellfire_state.load(std::memory_order_relaxed) & spellfire_seen_bit) != 0ULL;
	}

	bool is_committed_cast_holding_graph() {
		return current_cast && current_cast->has_cuttable_cast_state() && is_cast_committed() &&
			   MscoCastDriver::is_active();
	}

	// Ticket 45. The other half of the cut gate: the instance is gone (retired at GCD expiry) but
	// the clip is still playing on a live shtb state, so there is still something to end. A
	// charging cast still has `current_cast` and is refused here, staying protected by the
	// committed gate above.
	bool is_cuttable_follow_through() {
		return graph_is_in_cuttable_follow_through(current_cast != nullptr, MscoCastDriver::is_active());
	}

	// Ticket 41: no callers. It reports the pre-revert admission rule -- a press inside the
	// SpellFire-to-WinClose window is a combo step rather than a refusal -- which the stock gate
	// in InputModeCast::process_input now shadows for every hotbar press. Kept because pruning
	// was out of the ticket's scope; do NOT wire it back into an input path without reopening
	// ticket 41's decision, and read `classify_hotbar_cast_press`'s comment in the same light.
	bool can_accept_hotbar_cast() {
		return classify_hotbar_cast_press(current_cast != nullptr, is_committed_cast_holding_graph(),
				   MscoCastDriver::combo_window_open()) != HotbarCastPress::refuse;
	}

	bool is_live_concentration()
	{
		return current_cast && dynamic_cast<CastingInstanceSpellConcentration*>(current_cast.get());
	}

	CastingInstanceSpellConcentration* live_channel()
	{
		if (!current_cast) {
			return nullptr;
		}
		auto* channel = dynamic_cast<CastingInstanceSpellConcentration*>(current_cast.get());
		return (channel && !channel->casted()) ? channel : nullptr;
	}

	bool is_channel_chainable()
	{
		auto* channel = live_channel();
		return channel_chain_window_open(channel != nullptr, channel && channel->is_streaming());
	}

	void cut_channel_for_attack(RE::PlayerCharacter* pc)
	{
		if (auto* channel = live_channel()) {
			logger::debug("SH2 cast: attack pressed on a streaming channel; ending the channel");
			channel->end_channel(pc);
		}
	}

	bool our_latch_is_closed()
	{
		return CastIntent::should_retain_now();
	}

	/**
	* TICKET 43. A cast retired at GCD expiry that had not yet delivered. The lockout is over and
	* the button is free, but the payload is still owed: our payload IS the animation event, so
	* releasing the button must never eat the cast. The instance is kept alive here, un-torn-down,
	* until whichever of its three exits comes first (`classify_armed_delivery`, plus the cut).
	*
	* At most one exists: it is only ever created when `current_cast` is dropped, and every path
	* that starts a new action flushes it first.
	*/
	std::unique_ptr<CastingInstance> armed_cast;

	/**
	* Deliver the armed payload now, from wherever `reason` says. Safe to call with nothing armed.
	*
	* RISK 2 (ticket 43, per-hand since ticket 44): the event path isolates the caster the
	* SpellFire event names immediately before vanilla sees it (`animationeventhook.cpp`
	* ProcessEvent_PC) so an equipped spell cannot fire alongside ours. A delivery that does NOT
	* come through that path -- the cut, the clip-end fallback -- has to run the same preparation
	* itself, or the payload leaves the wrong caster; it silences both equipped hands.
	*
	* RISK 1 is `deliver_payload`'s own `m_spell_started` latch: one delivery per instance, whichever
	* path reaches it first. The instance is dropped here regardless, so no second path can even try.
	*/
	void deliver_armed_payload(std::string_view reason)
	{
		if (!armed_cast) {
			return;
		}
		if (auto* pc = RE::PlayerCharacter::GetSingleton()) {
			MscoCastDriver::interrupt_equipped_casters_if_spell(pc);
			logger::debug("SH2 cast: armed payload delivered at {} ({:.2f}s on the cast clock)", reason,
				armed_cast->get_lockout_elapsed());
			armed_cast->deliver_payload(pc);
		}
		armed_cast->on_reset_keep_graph();
		armed_cast.reset();
		clear_spellfire();
	}

	void yield_shtb_for_non_chain_start()
	{
		// The cut, as the ticket names it. Anything about to take the graph away from a clip that
		// still owes a payload pays it out first.
		deliver_armed_payload("the cut"sv);
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (ArtDriver::is_active()) {
			ArtDriver::cancel(pc);
		}
		if (MscoCastDriver::is_active()) {
			MscoCastDriver::cancel(pc);
		}
		MscoCastDriver::interrupt_equipped_casters_if_spell(pc);
		if (current_cast) {
			current_cast->on_reset();
			current_cast.reset();
			clear_spellfire();
		}
	}

	void yield_if_our_latch_is_open()
	{
		if (should_yield_shtb_before_hotbar_shout(ArtDriver::is_active(), ArtDriver::latch_open()) ||
			should_yield_shtb_before_hotbar_shout(
				MscoCastDriver::is_active(), MscoCastDriver::combo_window_open())) {
			yield_shtb_for_non_chain_start();
		}
	}

	void reset_cast() {
		current_cast->on_reset();
		current_cast.reset();
		clear_spellfire();
	}

	// TICKET 42. A cast retired on its own lockout leaves its clip playing, and the animation
	// globals -- animation type, art selector, casting source -- are what OAR picked that clip
	// with. Clearing them under a live clip would be a change of the selection mid-play, so the
	// reset is deferred to the frame the graph is finally quiet on. Idempotent, and a new cast
	// rewrites all of them from its own start path regardless.
	bool animation_vars_reset_pending{ false };

	void poll_pending_animation_var_reset()
	{
		// An armed cast counts as a live cast here: the globals are what its still-playing clip was
		// selected with, and its payload has not left yet.
		if (!animation_vars_reset_pending || current_cast || armed_cast || MscoCastDriver::is_active()) {
			return;
		}
		animation_vars_reset_pending = false;
		GameData::reset_animation_vars();
	}

	/**
	* Retire a cuttable cast without leaving the shtb state: the clip carries on as follow-through
	* and the next press enters the next clip from inside the live state, the same seam a ticket-14
	* chain press uses. `MscoCastDriver::finish` is deliberately NOT called -- sending SH2_CastExit
	* here would cut the very presentation this ticket is trying to keep. The clip's own end raises
	* that exit, and the state watchdog covers a graph that never does.
	*
	* TICKET 43: retirement no longer waits for delivery, so an instance that still owes its payload
	* is moved to `armed_cast` intact rather than torn down. Its teardown runs at delivery instead.
	* The SpellFire arming is deliberately left standing for it -- the clip that is still playing is
	* the armed cast's own, and its event is the normal way that payload leaves.
	*/
	void retire_cuttable_cast()
	{
		auto* spell_cast = dynamic_cast<CastingInstance*>(current_cast.get());
		if (retired_cast_stays_armed(spell_cast != nullptr, current_cast->casted())) {
			armed_cast.reset(spell_cast);
			static_cast<void>(current_cast.release());
		} else {
			if (spell_cast) {
				spell_cast->on_reset_keep_graph();
			} else {
				current_cast->on_reset();
			}
			current_cast.reset();
			clear_spellfire();
		}
		animation_vars_reset_pending = true;
		poll_pending_animation_var_reset();
	}

	void drop_live_cast()
	{
		// Dropped for a game load: the save being read owns selectedPower, so a hold from the
		// session being left behind is forgotten rather than written over it.
		discard_deferred_power_restore();
		current_cast.reset();
		// RISK 3 (ticket 43): an armed payload belongs to the session being left behind. Discarded,
		// never delivered into the save being read.
		armed_cast.reset();
		clear_spellfire();
		// The globals belong to the session being left behind; the load resets them itself.
		animation_vars_reset_pending = false;
		ArtDriver::reset_session();
		MscoCastDriver::reset_session();
		CastIntent::cancel();
		logger::info("SH2: dropped live cast for game load");
	}

	// Ticket 14: a follow-up hotbar press during a committed cast is a combo step. Drop the
	// live instance without CastExit so begin() can notify the next clip from inside the
	// current state. Concentration is excluded by the cuttable gate.
	void cut_committed_cast_for_combo(RE::PlayerCharacter*)
	{
		if (!is_committed_cast_holding_graph()) {
			return;
		}
		logger::debug("SH2 cast: follow-up cast on a committed cast; chaining the next clip");
		if (auto* spell_cast = dynamic_cast<CastingInstance*>(current_cast.get())) {
			spell_cast->on_reset_keep_graph();
		} else {
			current_cast->on_reset();
		}
		current_cast.reset();
		clear_spellfire();
	}

	//Play sound on actor and return soundhandle
	RE::BSSoundHandle playSound(RE::Actor* a, RE::BGSSoundDescriptorForm* a_descriptor)
	{
		RE::BSSoundHandle handle;
		handle.soundID = static_cast<uint32_t>(-1);
		handle.assumeSuccess = false;
		handle.state = RE::BSSoundHandle::AssumedState::kInitialized;

		auto am = RE::BSAudioManager::GetSingleton();
		if (am) {
			am->BuildSoundDataFromDescriptor(handle, a_descriptor, 16); // 16 used by https://github.com/D7ry/EldenParry
			if (handle.SetPosition(a->data.location)) {
				handle.SetObjectToFollow(a->Get3D());
				handle.Play();
			}
		}
		return handle;
	}

	BaseCastingInstance::BaseCastingInstance(RE::TESForm* form, float casttime) : m_form(form),
		m_cast_timer(casttime),
		m_total_casttime(casttime),
		m_gcd(1.5f),
		m_press_elapsed(0.0f),
		m_casted(false)
	{
	}

	CastingInstance::CastingInstance(RE::SpellItem* spell, float casttime, float manacost, hand_mode used_hand, uint16_t casteffect, bool is_spell_proc) : BaseCastingInstance(spell, casttime),
		m_charge_sound(nullptr),
		m_release_sound(nullptr),
		m_cast_loop_sound(nullptr),
		m_charge_sound_instance(),
		m_loop_sound_instance(),
		m_played_release(false),
		m_release_anim_time(0.0f),
		m_played_pre_release(false),
		m_pre_release_anim_time(0.0f),
		m_manacost(manacost),
		m_used_hand(used_hand),
		m_equip_ability(nullptr),
		m_casteffect(casteffect),
		m_spell_proc(is_spell_proc),
		m_spell_started(false),
		m_entry_grace(1.5f),
		m_last_anim_ok(false)
	{
		if (m_form && (m_form->GetFormType() == RE::FormType::Spell || m_form->GetFormType() == RE::FormType::Scroll)) {
			uint32_t size = get_spell()->effects.size();
			for (uint32_t i = 0U; i < size; ++i) {
				const auto& eff = get_spell()->effects[i];

				if (eff->baseEffect) {
					for (const auto& effectSound : eff->baseEffect->effectSounds) {

						if (effectSound.id == RE::MagicSystem::SoundID::kCharge && !m_charge_sound) {
							m_charge_sound = effectSound.sound;
						}

						if (effectSound.id == RE::MagicSystem::SoundID::kRelease && !m_release_sound) {
							m_release_sound = effectSound.sound;
						}

						if (effectSound.id == RE::MagicSystem::SoundID::kCastLoop && !m_cast_loop_sound) {
							m_cast_loop_sound = effectSound.sound;
						}

					}
				}
			}

			if (size > 0 && get_spell()->effects[0]->baseEffect) {
				m_equip_ability = spell->effects[0]->baseEffect->data.equipAbility;
			}
		}

		m_charge_sound_instance.soundID = static_cast<uint32_t>(-1);
		m_charge_sound_instance.assumeSuccess = false;
		m_charge_sound_instance.state = RE::BSSoundHandle::AssumedState::kInitialized;

		m_loop_sound_instance.soundID = static_cast<uint32_t>(-1);
		m_loop_sound_instance.assumeSuccess = false;
		m_loop_sound_instance.state = RE::BSSoundHandle::AssumedState::kInitialized;
	}

	bool CastingInstance::is_first_time_update() const
	{
		return m_cast_timer >= m_total_casttime;
	}

	bool BaseCastingInstance::is_gcd_expired() const
	{
		// TICKET 42: the lockout is measured from the press, not from cast completion. The old
		// `m_cast_timer <= -m_gcd` made occupancy casttime + gcd, so raising `m_gcd` alone moved
		// nothing that the player feels.
		return m_press_elapsed >= m_gcd;
	}

	bool CastingInstance::should_play_release_anim()
	{
		return !m_played_release && (m_cast_timer <= m_release_anim_time);
	}

	bool BaseCastingInstance::advance_time(float delta)
	{
		// The press clock. Kept separate from the cast timer, which still counts the authored
		// cast down and past zero exactly as it always did -- delivery, the release anim, and the
		// first-update edge all read it and none of them are on the lockout's business.
		m_press_elapsed += delta;
		if (m_cast_timer >= -m_gcd) {
			m_cast_timer -= delta;
		}

		//logger::info("Cast Progres: {}%", 100.0f * (1.0f - (m_cast_timer / m_total_casttime)));
		return m_cast_timer <= 0.0f;
	}

	float BaseCastingInstance::get_current_casttime() const
	{
		return m_total_casttime - std::clamp(m_cast_timer, 0.0f, m_total_casttime);
	}

	float BaseCastingInstance::get_current_gcd_progress() const
	{
		// Same shape as before -- 0 at the press, 1 at expiry -- over the press-anchored lockout,
		// so the HUD sweep draws the real 1.5s rather than casttime + gcd (ticket 42).
		if (m_gcd <= 0.0f) {
			return 0.0f;
		}
		return std::clamp(m_press_elapsed / m_gcd, 0.0f, 1.0f);
	}

	float BaseCastingInstance::get_current_gcd_duration() const
	{
		return m_gcd;
	}

	const std::string_view CastingInstance::get_end_anim() const
	{
		return "MT_BreathExhaleShort"sv;
	}

	const std::string_view BaseCastingInstance::get_start_anim() const
	{
		return "ShoutStart"sv;
	}

	const std::string_view CastingInstance::get_cancel_anim() const
	{
		return "ShoutStop"sv;
	}

	void CastingInstance::apply_cast_start_spell(RE::PlayerCharacter* pc)
	{
		if (m_equip_ability) {
			//if (!pc->HasSpell(m_equip_ability)) { //hasSpell seems to not work on equip abilities from current spells
			if (!player_has_equip_ability(m_equip_ability)) {
				pc->AddSpell(m_equip_ability);
			}
			else {
				//do not remove it
				m_equip_ability = nullptr;
			}
		}

		if (GameData::spellhotbar_castfx_spell) {
			GameData::update_spell_casting_art_and_time(m_casteffect, static_cast<uint32_t>(std::ceil(m_total_casttime)), m_used_hand);
			//cast_spell_on_player(GameData::spellhotbar_castfx_spell, 1.0f);
			pc->AddSpell(GameData::spellhotbar_castfx_spell);
		}
	}

	void CastingInstance::on_reset_keep_graph()
	{
		BaseCastingInstance::on_reset();
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc) {
			pc->RemoveSpell(GameData::spellhotbar_castfx_spell);
		}
		if (m_form->GetFormType() == RE::FormType::Spell || m_form->GetFormType() == RE::FormType::Scroll) {
			RE::SpellItem* spell = m_form->As<RE::SpellItem>();
			GameData::post_cast_mod_callback(spell);
		}
	}

	void CastingInstance::on_reset()
	{
		on_reset_keep_graph();
		MscoCastDriver::finish(RE::PlayerCharacter::GetSingleton());
	}

	bool CastingInstance::is_anim_ok(RE::PlayerCharacter*) const
	{
		return MscoCastDriver::is_active();
	}

	RE::BSSoundHandle _play_sound_if_exists(RE::BGSSoundDescriptorForm* snd)
	{
		RE::BSSoundHandle handle;
		handle.soundID = static_cast<uint32_t>(-1);
		handle.assumeSuccess = false;
		handle.state = RE::BSSoundHandle::AssumedState::kInitialized;

		if (snd) {
			const auto id = snd->GetFormEditorID();
			if (id) {
				auto* pc = RE::PlayerCharacter::GetSingleton();
				if (pc) {
					handle = playSound(pc, snd);
				}
			}
		}
		return handle;
	}

	bool BaseCastingInstance::has_cuttable_cast_state() const
	{
		return false;
	}

	bool CastingInstance::has_cuttable_cast_state() const
	{
		return true;
	}

	void BaseCastingInstance::on_reset()
	{
		//called before cast instance is deleted
	}

	void BaseCastingInstance::consume_items()
	{
		if (m_form && m_form->GetFormType() == RE::FormType::Scroll) {
			//consume the scroll
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
				RE::FormID id = m_form->GetFormID();
				auto refs = pc->GetInventory([id](const RE::TESBoundObject& object) { return object.formID == id; });
				for (auto& [k, v] : refs) {
					pc->RemoveItem(k, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
				}
			}
		}
	}

	void BaseCastingInstance::apply_cooldown()
	{
		auto data = GameData::get_spell_data(m_form, true, true);
		if (data.cooldown > 0.0f) {
			SpellHotbar::GameData::add_gametime_cooldown_with_timescale(m_form->GetFormID(), data.cooldown, true);
		}
	}

	void CastingInstance::deliver_payload(RE::PlayerCharacter* pc)
	{
		if (m_spell_started) {
			return;
		}
		if (!is_cast_committed()) {
			// Ticket 43 added a second way to arrive here without the event: the cut. Either way
			// the payload is owed and leaves now rather than being eaten.
			logger::warn("SH2 cast: no SpellFire event; delivering the payload anyway");
		}
		m_spell_started = true;
		stop_charge_sound();
		play_release_sound();
		if (cast_spell(get_spell(), m_used_hand == hand_mode::dual_hand, m_spell_proc)) {
			if (m_spell_proc) {
				casts::SpellProc::consume_proc();
			}
			apply_cooldown();
			pc->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kMagicka, -m_manacost);
			consume_items();
		}
		set_casted();
	}

	bool CastingInstance::update(RE::PlayerCharacter* pc, float delta)
	{
		const bool anim_ok = is_anim_ok(pc);
		if (anim_ok != m_last_anim_ok) {
			logger::debug("SH2 cast: casting state active became {} ({}s on the cast timer)", anim_ok, m_cast_timer);
			m_last_anim_ok = anim_ok;
		}

		// Past spellfire the cast is committed and delivers regardless of the casting state
		// (ADR 0004). The annotation leads (ADR 0006); the authored cast time is only the
		// missing-annotation fallback, not an early trigger. Clip 4's SpellFire is past
		// that authored time, so a live clip waits for the pose instead of the windup.
		if (anim_ok || is_cast_committed()) {
			// The grace only bridges the send frame: the driver's flag is set from the
			// notify's own return, so a live state reads active immediately and losing
			// it afterwards cancels immediately.
			m_entry_grace = 0.0f;
			if (is_first_time_update()) {
				play_charge_sound();
				apply_cast_start_spell(pc);
				GameData::start_cast_timer();
			}

			const bool timer_expired = advance_time(delta);
			GameData::advance_cast_timer(delta);
			if (classify_cast_delivery(m_spell_started, timer_expired, is_cast_committed(), anim_ok) ==
				CastDelivery::deliver) {
				deliver_payload(pc);
			}
		}
		else if (m_entry_grace > 0.0f) {
			// SH2_CastRight is sent when the instance is created; the grace covers the
			// frames between instance creation and the send landing, so a cast is not
			// torn down before its state had a chance to go live.
			m_entry_grace -= delta;
		}
		else {
			const bool timer_expired = m_cast_timer <= 0.0f;
			if (classify_cast_delivery(m_spell_started, timer_expired, is_cast_committed(), anim_ok) ==
				CastDelivery::deliver) {
				deliver_payload(pc);
			}
			GameData::reset_animation_vars();
			return true;
		}
		return false;
	}

	void CastingInstance::play_charge_sound()
	{
		m_charge_sound_instance = _play_sound_if_exists(m_charge_sound);
	}

	void CastingInstance::stop_charge_sound()
	{
		if (m_charge_sound_instance.IsValid() && m_charge_sound_instance.IsPlaying()) {
			m_charge_sound_instance.FadeOutAndRelease(25); //fadeout in ms
		}
	}

	void CastingInstance::play_release_sound() const
	{
		_play_sound_if_exists(m_release_sound);
	}

	void CastingInstance::stop_cast_loop_sound()
	{
		if (m_loop_sound_instance.IsValid()) { //&& m_loop_sound_instance.IsPlaying()) {
			m_loop_sound_instance.FadeOutAndRelease(25); //fadeout in ms
		}
	}

	void CastingInstance::play_cast_loop_sound()
	{
		m_loop_sound_instance = _play_sound_if_exists(m_cast_loop_sound);
	}

	CastingInstanceSpell::CastingInstanceSpell(RE::SpellItem* spell, float casttime, float manacost, hand_mode used_hand, uint16_t casteffect, bool spell_proc) : CastingInstance(spell, casttime, manacost, used_hand, casteffect, spell_proc)
	{
		// TICKET 43: an action costs one number, and that number is the whole lockout -- the clip's
		// remaining length is presentation and nothing about it gates the button. Read at
		// construction so the MCM slider takes effect on the very next press, with no rebuild.
		m_gcd = GameData::spell_gcd;
	}

	CastingInstanceRitual::CastingInstanceRitual(RE::SpellItem* spell, float casttime, float manacost, hand_mode used_hand, uint16_t casteffect, bool spell_proc) : CastingInstance(spell, casttime, manacost, used_hand, casteffect, spell_proc)
	{
		m_release_anim_time = 0.25f;
		// Same action-class number as a fire-and-forget spell, measured from the press (ticket 42),
		// and tunable with it (ticket 43).
		m_gcd = GameData::spell_gcd;
	}

	CastingInstanceSpellConcentration::CastingInstanceSpellConcentration(RE::SpellItem* spell, float casttime, float manacost, hand_mode used_hand, uint16_t casteffect, bool spell_proc, const Input::KeyBind& keybind, int slot)
		: CastingInstance(spell, casttime, manacost, used_hand, casteffect, spell_proc),
		m_keybind(keybind),
		m_slot(slot)
	{
		m_cast_timer = 0;
		m_total_casttime = spell->data.castDuration;
		m_release_anim_time = casttime;
		m_gcd = 0.25f;
	}

	void CastingInstanceSpellConcentration::apply_cast_start_spell(RE::PlayerCharacter* pc)
	{
		if(m_equip_ability) {
			//if (!pc->HasSpell(m_equip_ability)) { //hasSpell seems to not work on equip abilities from current spells
			if (!player_has_equip_ability(m_equip_ability)) {
				pc->AddSpell(m_equip_ability);
			}
			else {
				//do not remove it
				m_equip_ability = nullptr;
			}
		}
	}

	void CastingInstanceSpellConcentration::on_reset()
	{
		CastingInstance::on_reset();
		RE::PlayerCharacter* pc = RE::PlayerCharacter::GetSingleton();
		if (pc && m_equip_ability) {
			pc->RemoveSpell(m_equip_ability);
		}
	}

	bool CastingInstanceSpellConcentration::advance_time(float delta)
	{
		m_cast_timer += delta;
		if (casted()) {
			m_gcd -= delta;
		}
		//logger::info("CastTimer: {}, gcd: {}", m_cast_timer, m_gcd);
		return false;
	}

	bool CastingInstanceSpellConcentration::is_first_time_update() const
	{
		return m_cast_timer <= 0;
	}

	bool CastingInstanceSpellConcentration::update(RE::PlayerCharacter* pc, float delta)
	{
		//duration spells cast for duration without holding key
		bool keydown = has_duration() || m_keybind.isDown();
		bool cancel{ false };

		float timer_old = m_cast_timer;
		advance_time(delta);
		const bool anim_ok = is_anim_ok(pc);
		if (anim_ok) {
			// Entry confirmed: the grace only bridges the frames between the SH2_CastRight
			// send and the state raising SH2_CastEnter.
			m_entry_grace = 0.0f;
		}
		else {
			m_entry_grace = std::max(0.0f, m_entry_grace - delta);
		}

		if (timer_old == 0) {
			//startup
			//stop if key not down directly at start; SH2_CastRight was only just sent, so
			//the casting state cannot be required here -- the grace check in the charge
			//loop covers the entry window.
			if (!keydown) {
				MscoCastDriver::cancel(pc);
				GameData::reset_animation_vars();
				cancel = true;
			}
			else {
				//start the chargeup
				play_charge_sound();
				apply_cast_start_spell(pc);
				GameData::start_cast_timer();
			}
		}
		else if (!is_cast_committed() && m_cast_timer < m_release_anim_time) {
			//during charge loop
			// The clip's spellfire annotation commits the cast on its own schedule (ADR
			// 0004); the authored charge time is the floor (ADR 0006), so the charge also
			// completes when the timer passes it. A released key still stops the cast --
			// that is the player asking, not an interruption.
			if (!keydown || (!anim_ok && !is_cast_committed() && m_entry_grace <= 0.0f)) {
				//trigger gcd and stop cast
				set_casted();
				stop_charge_sound();
				GameData::reset_animation_vars();
				MscoCastDriver::cancel(pc);
			}
		}
		else {
			//charge finished

			constexpr float loop_timer = 0.5f;

			if (!m_spell_started) {
				//first cast update
				m_spell_started = true;
				cast_spell(get_spell(), m_used_hand == hand_mode::dual_hand, m_spell_proc, m_manacost);
				if (m_spell_proc) {
					casts::SpellProc::consume_proc();
				}
				stop_charge_sound();
				play_cast_loop_sound();
				GameData::global_casting_conc_spell->value = 1.0f;
			}
			else if (static_cast<int>(timer_old / loop_timer) < static_cast<int>(m_cast_timer / loop_timer)) {
				//Per-loop callbacks. The animation is not re-sent here: the start clip ends the
				//shtb state on its own and the hold is sustained by the OAR idle loop, which
				//every conc submod builds by replacing the idle and locomotion set while
				//SpellHotbar_isCastingConcSpell is raised (ADR-0013). Re-notifying the entry
				//restarted the single-play start clip every half second and held the actor out
				//of the idle that owns the loop.

				if (keydown) {
					RenderManager::highlight_skill_slot(m_slot, loop_timer*2.0f, false);
					auto spell = m_form->As<RE::SpellItem>();
					if (spell != nullptr) {
						GameData::concentration_cast_mod_callback(spell, m_spell_proc);
					}
				}
			}

			//if ((m_manacost*0.5f) > pc->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMagicka)) {
			if (pc->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMagicka)<=0.0f) {
				//cancel if out of magicka
				keydown = false;
				RE::HUDMenu::FlashMeter(RE::ActorValue::kMagicka);
				RE::PlaySound(Input::sound_MagFail);
			}

			if (has_duration() && m_cast_timer > m_release_anim_time + m_total_casttime) {
				//abort a channel spell with duration
				keydown = false;
			}

			if (!keydown) {
				end_channel(pc);
			}

		}

		return cancel;
	}

	void CastingInstanceSpellConcentration::end_channel(RE::PlayerCharacter* pc)
	{
		if (m_casted) {
			//Already ended -- an attack cut it and this is the update tick behind it.
			return;
		}
		//trigger gcd and stop cast
		set_casted();
		stop_cast_loop_sound();
		stop_charge_sound();
		RE::MagicSystem::CastingSource src = static_cast<RE::MagicSystem::CastingSource>(std::clamp(static_cast<int>(SpellHotbar::GameData::global_casting_source->value), 0, 3));
		auto playerCaster = pc->GetMagicCaster(src);
		if (playerCaster) {
			playerCaster->InterruptCast(false);
		}
		GameData::reset_animation_vars();
		if (channel_end_arms_combo_restore(m_spell_started)) {
			MscoCastDriver::end_channel(pc);
		}
		else {
			MscoCastDriver::cancel(pc);
		}
		play_release_sound();
		apply_cooldown();
		consume_items();
	}

	bool CastingInstanceSpellConcentration::is_gcd_expired() const
	{
		return m_gcd <= 0;
	}

	bool CastingInstanceSpellConcentration::has_cuttable_cast_state() const
	{
		return false;
	}

	bool CastingInstanceSpellConcentration::has_duration() const
	{
		return m_total_casttime > 0.0f;
	}

	float CastingInstanceSpellConcentration::get_current_gcd_progress() const
	{
		if (has_duration()) {
			return m_cast_timer / (m_release_anim_time + m_total_casttime);
		}
		return 0.0f;
	}

	float CastingInstanceSpellConcentration::get_current_gcd_duration() const
	{
		return m_release_anim_time + m_total_casttime;
	}

	void update_cast(float delta)
	{
		// TICKET 37. First thing in the frame, before anything below reads a deadline off it.
		// This is the only advance: the caller runs this whole function only while the game is
		// unpaused, which is what makes the counter gameplay time rather than wall time.
		UnpausedClock::advance(delta);
		// One bool graph read per unpaused frame on the player: the shout's own liveness, which
		// is what a deferred selectedPower write-back is waiting on.
		if (auto* player = RE::PlayerCharacter::GetSingleton()) {
			bool is_shouting{ false };
			player->GetGraphVariableBool("IsShouting"sv, is_shouting);
			update_deferred_power_restore(delta, is_shouting);
			// Before the latch poll, not after: clearing a wedged state here lets the same
			// frame's poll attempt a press that is still inside its cap. The attempt can still
			// be refused -- the wedged cast's own instance is torn down later this frame, and a
			// live instance refuses a new press -- but a loud refusal is the contract; what must
			// not happen is the press waiting out its cap behind a state already known dead.
			MscoCastDriver::poll_watchdog(player);
		}
		CastIntent::poll_local_release();
		if (current_cast) {
			bool cleared{ false };
			if (!current_cast->casted()) {
				auto pc = RE::PlayerCharacter::GetSingleton();
				if (pc) {

					if (current_cast->update(pc, delta)) {
						reset_cast();
						cleared = true;
					}

				}
			}
			else {
				current_cast->advance_time(delta);
			}
			if (!cleared) {
				if (current_cast->has_cuttable_cast_state()) {
					// TICKET 43. The GCD is the WHOLE lockout: no animation gates the button, so
					// this is the press-anchored clock and nothing else. Ticket 42's `&&
					// is_cast_committed()` floor is gone -- a cast retired before its SpellFire
					// keeps its payload owed as `armed_cast` rather than losing it, which is what
					// the floor was really protecting. Delivery is now a separate clock entirely.
					//
					// Note this branch is reached whether or not the instance has delivered; under
					// ticket 42 only a delivered one could ever get here.
					if (current_cast->is_gcd_expired()) {
						logger::debug(
							// The elapsed figure is the instance's own clock, which starts when the
							// cast state goes live -- a frame or two after the press itself.
							"SH2 cast: lockout over at {:.2f}s on the cast clock (payload {}); clip {} playing",
							current_cast->get_lockout_elapsed(),
							current_cast->casted() ? "already delivered" : "still owed, staying armed",
							MscoCastDriver::is_active() ? "still" : "no longer");
						retire_cuttable_cast();
					}
				} else if (current_cast->casted() && current_cast->is_gcd_expired()) {
					GameData::reset_animation_vars();
					reset_cast();
				}
			}
		}
		if (armed_cast) {
			// TICKET 43: the two polled exits of an owed payload. The third, the cut, is not polled
			// -- the press that cuts the clip delivers directly, before it starts its own cast.
			switch (classify_armed_delivery(
				armed_cast->casted(), is_cast_committed(), MscoCastDriver::is_active())) {
			case ArmedDelivery::on_spellfire:
				deliver_armed_payload("its own SpellFire"sv);
				break;
			case ArmedDelivery::on_clip_end:
				deliver_armed_payload("clip end, no SpellFire"sv);
				break;
			case ArmedDelivery::hold:
			default:
				break;
			}
		}
		poll_pending_animation_var_reset();
	}

	// Why a start attempt produced no live cast. `graph_refused` is the one failure ShoutMCO can
	// wait out: MscoCastDriver::begin() returned false because the animation graph is not in a
	// state that hosts the cast entry — mid-MCO-swing the root state machine sits in AttackState
	// (ticket 08). Every other failure is final and refuses exactly as it always did.
	enum class start_result { started, graph_refused, failed };

	start_result start_cast(CastingInstanceSpellData& cast_info)
	{
		auto pc = RE::PlayerCharacter::GetSingleton();
		const bool chaining = is_committed_cast_holding_graph();
		cut_committed_cast_for_combo(pc);
		if (!current_cast) {
			if (pc) {

				int anim = cast_info.m_animation;
				if (anim < 0) {
					anim = GameData::chose_default_anim_for_spell(cast_info.m_spell, -1, false);
				}
				GameData::set_animtype_global(anim);

				hand_mode used_hand = GameData::set_weapon_dependent_casting_source(cast_info.m_hand, cast_info.m_dual_cast);
				current_cast = std::make_unique<CastingInstanceSpell>(cast_info.m_spell, cast_info.m_casttime, cast_info.m_manacost, used_hand, cast_info.m_casteffect, cast_info.m_spellproc);
				arm_spellfire(used_hand);
				if (MscoCastDriver::begin(pc, used_hand, cast_info.m_casttime, CastShape::fire_and_forget)) {
					return start_result::started;
				}
				current_cast.reset();
				GameData::reset_animation_vars();
				if (chaining) {
					MscoCastDriver::cancel(pc);
					return start_result::failed;
				}
				return start_result::graph_refused;
			}
		}
		return start_result::failed;
	}

	start_result start_conc_cast(CastingInstanceSpellData& cast_info, const Input::KeyBind& keybind, size_t slot)
	{
		if (!current_cast) {
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {

				// Same Animation2 trap as start_ritual_cast: the per-spell column is not the dual
				// id. Dual concentration only ever worked because conc rows leave it at -1; ask
				// the family instead so a configured row cannot break the channel's dual pose.
				int anim;
				if (cast_info.m_dual_cast) {
					anim = GameData::is_dual_family_id(cast_info.m_animation2)
						? cast_info.m_animation2
						: GameData::chose_default_anim_for_spell(cast_info.m_spell, -1, true);
				}
				else {
					anim = cast_info.m_animation;
					if (anim < 0) {
						anim = GameData::chose_default_anim_for_spell(cast_info.m_spell, -1, false);
					}
				}
				GameData::set_animtype_global(anim);

				hand_mode used_hand = GameData::set_weapon_dependent_casting_source(cast_info.m_hand, cast_info.m_dual_cast);
				current_cast = std::make_unique<CastingInstanceSpellConcentration>(cast_info.m_spell, cast_info.m_casttime, cast_info.m_manacost, used_hand, cast_info.m_casteffect, cast_info.m_spellproc, keybind, static_cast<int>(slot));

				arm_spellfire(used_hand);
				if (MscoCastDriver::begin(pc, used_hand, cast_info.m_casttime, CastShape::channel)) {
					return start_result::started;
				}
				current_cast.reset();
				GameData::reset_animation_vars();
				return start_result::graph_refused;
			}
		}
		return start_result::failed;
	}

	start_result start_ritual_conc_cast(CastingInstanceSpellData & cast_info, const Input::KeyBind& keybind, size_t slot)
	{
		if (!current_cast) {
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
				
				bool is_fast_anim = cast_info.m_casttime <= fast_cast_threshold;

				int anim = is_fast_anim ? cast_info.m_animation2 : cast_info.m_animation;
				float pre_release_anim = GameData::Spell_cast_data::get_ritual_conc_anim_prerelease_time(anim);

				GameData::set_animtype_global(anim);

				hand_mode used_hand = GameData::set_weapon_dependent_casting_source(cast_info.m_hand, cast_info.m_dual_cast);
				current_cast = std::make_unique<CastingInstanceSpellRitualConcentration>(cast_info.m_spell, cast_info.m_casttime, cast_info.m_manacost, used_hand, cast_info.m_casteffect, cast_info.m_spellproc, keybind, static_cast<int>(slot), pre_release_anim);

				arm_spellfire(used_hand);
				if (MscoCastDriver::begin(pc, used_hand, cast_info.m_casttime, CastShape::channel)) {
					return start_result::started;
				}
				current_cast.reset();
				GameData::reset_animation_vars();
				return start_result::graph_refused;
			}
		}
		return start_result::failed;
	}

	start_result start_ritual_cast(CastingInstanceSpellData& cast_info)
	{
		auto pc = RE::PlayerCharacter::GetSingleton();
		const bool chaining = is_committed_cast_holding_graph();
		cut_committed_cast_for_combo(pc);
		if (!current_cast) {
			if (pc) {
				
				const bool is_fast_cast = cast_info.m_casttime <= fast_cast_threshold;
				const bool dual_1h = cast_info.m_dual_cast && !cast_info.m_spell->IsTwoHanded();
				const bool use_variant =
					GameData::ritual_cast_slot(cast_info.m_spell->IsTwoHanded(),
						cast_info.m_dual_cast, is_fast_cast) == GameData::CastAnimSlot::variant;

				// Upstream's per-spell Animation2 column is NOT the dual id — vanilla aimed rows
				// carry 10001 (its first-person anim) and mod rows a 100xx variant, so reading it
				// here presented every configured spell's dual cast as single-hand (owner-observed
				// 2026-08-26). Dual ids are structural per family (10016/10017); ask the family.
				int anim;
				if (dual_1h) {
					anim = GameData::is_dual_family_id(cast_info.m_animation2)
						? cast_info.m_animation2
						: GameData::chose_default_anim_for_spell(cast_info.m_spell, -1, true);
				}
				else {
					anim = use_variant ? cast_info.m_animation2 : cast_info.m_animation;
					if (anim < 0) {
						anim = GameData::chose_default_anim_for_spell(cast_info.m_spell, -1, use_variant);
					}
				}
				GameData::set_animtype_global(anim);

				hand_mode used_hand = GameData::set_weapon_dependent_casting_source(cast_info.m_hand, cast_info.m_dual_cast);
				current_cast = std::make_unique<CastingInstanceRitual>(cast_info.m_spell, cast_info.m_casttime, cast_info.m_manacost, used_hand, cast_info.m_casteffect, cast_info.m_spellproc);
				arm_spellfire(used_hand);
				if (MscoCastDriver::begin(pc, used_hand, cast_info.m_casttime, CastShape::fire_and_forget)) {
					return start_result::started;
				}
				current_cast.reset();
				GameData::reset_animation_vars();
				if (chaining) {
					MscoCastDriver::cancel(pc);
					return start_result::failed;
				}
				return start_result::graph_refused;
			}
		}
		return start_result::failed;
	}

	/**
	 * Turn a start attempt into the press's answer. A graph refusal is offered to ShoutMCO, which
	 * owns release timing (ADR-0005): if it takes the intent, the press is accepted and this mod
	 * re-attempts it once from the release callback. Anything else refuses as it always did.
	 */
	bool resolve_start(start_result result, const Input::KeyBind& keybind, size_t slot)
	{
		if (result == start_result::started) {
			return true;
		}
		if (result == start_result::graph_refused) {
			// The armed hand belongs to the cast that never started; a deferred intent must not
			// leave it waiting to commit something.
			clear_spellfire();
			return CastIntent::offer(slot, keybind);
		}
		return false;
	}

	/**
	 * Ticket 41: from the ORDINARY hotbar bar (InputModeCast), this is only ever reached with no
	 * cast instance live -- that mode's stock gate refuses the press otherwise. So the
	 * latch-closed offer below and `classify_hotbar_cast_press`'s chain arm no longer answer an
	 * ordinary hotbar press. They are not dead. Two callers still reach them with a live
	 * instance possible:
	 *
	 *   - InputModeVampireLord::process_input, which sends potions straight here with no
	 *     can_start_new_cast() check of its own. Stock SH2 did not gate that mode either, so
	 *     ticket 41 left it alone.
	 *   - CastIntent::fire_payload, for a deferred SPELL or potion payload. Note it dispatches
	 *     weapon arts to try_start_art instead, so an art release never passes through here.
	 */
	bool try_start_cast(RE::TESForm* form, const Input::KeyBind& keybind, size_t slot, hand_mode hand)
	{
		if (our_latch_is_closed()) {
			return CastIntent::offer(slot, keybind);
		}
		if (ArtDriver::is_active() && ArtDriver::latch_open()) {
			yield_shtb_for_non_chain_start();
		}
		const auto press = classify_hotbar_cast_press(
			current_cast != nullptr, is_committed_cast_holding_graph(),
			MscoCastDriver::combo_window_open());
		if (press != HotbarCastPress::refuse) {
			// Ticket 43: THE CUT. This press is about to enter a new clip; a previous cast that
			// retired on its GCD without having delivered pays out here, at the instant of the cut,
			// before anything re-arms SpellFire or starts a new instance. A REFUSED press never
			// reaches this -- it cuts nothing, so the owed payload keeps waiting for its own event.
			deliver_armed_payload("the cut"sv);
			// Ticket 14: a chain press still needs the commitment bit when start_cast
			// runs the cut. Idle starts still drop leftover shout spellfire here.
			if (!keep_commitment_until_cut(press)) {
				clear_spellfire();
			}
			// This press supersedes any intent still waiting from an earlier one, whether or not
			// it ends up deferred itself. Withdrawing here keeps a stale payload from surfacing
			// as a cast the player no longer asked for.
			CastIntent::cancel();
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (form && pc) {
				if (form->GetFormType() == RE::FormType::Spell || form->GetFormType() == RE::FormType::Scroll) {
					RE::SpellItem* spell = form->As<RE::SpellItem>();

					//check if spell is still known/enough scrolls in inv
					bool spell_allowed{true};
					if (form->GetFormType() == RE::FormType::Spell && !pc->HasSpell(spell)) {
						spell_allowed = false;
						RE::DebugNotification("Spell is no longer known!");
					}
					else if (form->GetFormType() == RE::FormType::Spell && !GameData::can_cast_spell_mod_compat(spell)) {
						spell_allowed = false;
					}
					else if (form->GetFormType() == RE::FormType::Scroll && GameData::count_item_in_inv(form->GetFormID()) <= 0) {
						spell_allowed = false;
						RE::DebugNotification("No more scrolls left!");
					}
					
					if (spell_allowed) {

						float manacost{ 0.0f };
						bool is_spell_proc{ false };

						if (form->GetFormType() == RE::FormType::Spell) {
							manacost = spell->CalculateMagickaCost(pc);

							auto spell_proc = SpellProc::has_matching_proc(spell);
							if (spell_proc.has_value()) {
								manacost *= spell_proc.value();
								is_spell_proc = true;
							}
						}
						bool dual_cast{ false };
						if (!spell->GetNoDualCastModifications() && ((hand == auto_hand && GameData::should_dual_cast()) || hand == dual_hand) && GameData::player_can_dualcast_spell(spell)) {

							RE::GameSettingCollection* game_settings = RE::GameSettingCollection::GetSingleton();
							if (game_settings) {
								auto setting = game_settings->GetSetting("fMagicDualCastingCostMult");
								if (setting) {
									manacost *= setting->GetFloat();
									dual_cast = true;
								}
							}
						}
						else {
							if (!spell->IsTwoHanded() && hand == dual_hand && !dual_cast) {
								// Silent until ticket 48's diagnosis: a level-3 fixture without
								// the school's dual-cast perk spent a session reading as "the
								// dual submods never select".
								logger::debug(
									"SH2 cast: dual requested but spell is not dual-castable here (perk/spell); downgrading to auto");
								hand = auto_hand;
							}
						}

						if (manacost <= pc->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMagicka) 
							|| (spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration)) { 

							//set casttime

							auto spell_data = GameData::get_spell_data(spell);

							float casttime = spell_data.casttime;

							if (is_spell_proc) {
								casttime = SpellProc::adjust_casttime(casttime, spell);
							}
							CastingInstanceSpellData cast_info{ spell, casttime, manacost, hand, dual_cast, spell_data.animation, spell_data.animation2, spell_data.casteffectid, is_spell_proc};

							start_result result{ start_result::failed };
							if (spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration) {
								if (spell->IsTwoHanded()) {
									result = start_ritual_conc_cast(cast_info, keybind, slot);
								}
								else
								{
									result = start_conc_cast(cast_info, keybind, slot);
								}
							}
							else {
								if (spell->IsTwoHanded() || dual_cast) {
									result = start_ritual_cast(cast_info);
								}
								else
								{
									result = start_cast(cast_info);
								}
							}
							return resolve_start(result, keybind, slot);
						}
						else {
							RE::HUDMenu::FlashMeter(RE::ActorValue::kMagicka);
							RE::PlaySound(Input::sound_MagFail);
						}
					}
					else {
						RE::PlaySound(Input::sound_MagFail);
					}
				}
				else if (form->GetFormType() == RE::FormType::AlchemyItem) {
					return start_potion_use(form);
				}
			}
		}

		return false;
	}

	bool start_potion_use(RE::TESForm* alch_item)
	{
		if (!current_cast) {
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
				if (GameData::count_item_in_inv(alch_item->GetFormID()) > 0) {
					// Ticket 43: a potion press ends the lockout-free window too.
					deliver_armed_payload("the cut (potion)"sv);
					current_cast = std::make_unique<CastingInstancePotionUse>(alch_item);
					return true;
				}
				else
				{
					RE::DebugNotification("No more potions left!");
					return false;
				}
			}
		}
		return false;
	}

	bool try_cast_power(RE::TESForm* form, const Input::KeyBind& keybind, size_t slot, hand_mode hand)
	{
		auto pc = RE::PlayerCharacter::GetSingleton();
		bool is_shouting{ false };
		if (pc) {
			pc->GetGraphVariableBool("IsShouting"sv, is_shouting);
		}
		const bool is_shout = form && form->GetFormType() == RE::FormType::Shout;
		// Ticket 41: no longer reachable from an ORDINARY hotbar press -- InputModeCast's stock
		// gate refuses anything arriving while our latch is closed. Nor from the release path:
		// fire_payload runs under `attempting_release`, so is_firing() is true and this arm's own
		// guard skips it. The caller that does still reach it is InputModeVampireLord, which
		// calls try_cast_power with no gate of its own (stock did not gate that mode either).
		if (is_shout && !CastIntent::is_firing() && (our_latch_is_closed() || is_shouting)) {
			return CastIntent::offer(slot, keybind);
		}
		if (is_shout) {
			yield_if_our_latch_is_open();
		}
		if (can_start_new_cast()) {
			// Ticket 43: a shout or power press cuts a cast clip too.
			deliver_armed_payload("the cut (power/shout)"sv);
			clear_spellfire();
			// A power or shout press is a newer input too, so it supersedes a waiting intent. A
			// real shout also reaches ShoutMCO's own hook and would displace it there, but a
			// voice-slot power never does — nothing on that path touches the driver at all.
			CastIntent::cancel();
			if (form && pc) {
				if (form->GetFormType() == RE::FormType::Shout) {

					if (GameData::individual_shout_cooldowns || pc->GetVoiceRecoveryTime() <= 0.0f) {

						current_cast = std::make_unique<CastingInstanceShout>(form);
						return true;
					}
					else {
						RE::HUDMenu::FlashMeter(RE::ActorValue::kShoutRecoveryMult);
						RE::PlaySound(Input::sound_UIMenuCancel);
					}
				}
				else if (!is_shouting && form->GetFormType() == RE::FormType::Spell) {
					RE::SpellItem* spell = form->As<RE::SpellItem>();

					if (spell->GetEquipSlot() == GameData::equip_slot_voice) {
						//logger::info("Start Power Cast");
						current_cast = std::make_unique<CastingInstancePower>(form);
					
						return true;
					}
				
				}

			}
		}
		return false;
	}

	bool is_currently_using_power()
	{
		if (current_cast) {
			CastingInstancePower* curr_shout = dynamic_cast<CastingInstancePower*>(current_cast.get());
			if (curr_shout) {
				return true;
			}
		}
		return false;
	}

	bool is_currently_using_procced_spell()
	{
		if (current_cast) {
			CastingInstance* curr_cast = dynamic_cast<CastingInstance*>(current_cast.get());
			if (curr_cast) {
				return curr_cast->is_procced();
			}
		}
		return false;
	}

	float get_current_casttime()
	{
		if (current_cast) {
			return current_cast->get_current_casttime();
		}
		return 0.0f;
	}

	bool can_start_new_cast()
	{
		return current_cast == nullptr;
	}

	void try_finish_power_cast(RE::FormID formID)
	{
		if (current_cast && current_cast->get_form() && current_cast->get_form()->formID == formID)
		{
			current_cast->set_casted();
			current_cast->apply_cooldown();
		}
	}

	void try_finish_shout_cast(RE::FormID formID)
	{
		if (current_cast && current_cast->get_form() && current_cast->get_form()->formID == formID)
		{
			current_cast->set_casted();
			//update gcd
			current_cast->updateGCD(0.5f);
		}
	}

	float get_current_gcd_progress()
	{
		if (current_cast) {
			return current_cast->get_current_gcd_progress();
		}
		return 0.0f;
	}
	float get_current_gcd_duration()
	{
		if (current_cast) {
			return current_cast->get_current_gcd_duration();
		}
		return 0.0f;
	}

	void cast_spell_on_player(RE::SpellItem* spell, float magnitude, bool no_art) {
		if (!spell) return;

		auto pc = RE::PlayerCharacter::GetSingleton();
		if (!pc) return;

		auto playerMagicCaster = pc->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
		if (!playerMagicCaster) return;

		playerMagicCaster->CastSpellImmediate(spell, no_art, pc, 1.0f, false, magnitude, nullptr);
	}

	inline bool check_slot_for_equip_ability(RE::PlayerCharacter* pc, RE::SpellItem* equip_ability, RE::BGSEquipSlot* slot) {
		auto form = pc->GetEquippedObjectInSlot(slot);
		if (form && form->GetFormType() == RE::FormType::Spell) {
			auto spell = form->As<RE::SpellItem>();
			if (spell && !spell->effects.empty() && spell->effects[0]->baseEffect) {
				auto eq_ab = spell->effects[0]->baseEffect->data.equipAbility;
				if (eq_ab == equip_ability) {
					return true;
				}
			}
		}
		return false;
	}

	bool player_has_equip_ability(RE::SpellItem* equip_ability)
	{
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc && GameData::equip_slot_left_hand && GameData::equip_slot_right_hand) {

			return check_slot_for_equip_ability(pc, equip_ability, GameData::equip_slot_left_hand) ||
				check_slot_for_equip_ability(pc, equip_ability, GameData::equip_slot_right_hand) ||
				check_slot_for_equip_ability(pc, equip_ability, GameData::equip_slot_both_hand);
		
		}

		return false;
	}

	bool cast_spell(RE::SpellItem* spell, bool dual_cast, bool spell_proc, std::optional<float> concentration_manacost)
	{
		//Credits to https://github.com/ArcEarth/DynamicAnimationCasting/
		if (!spell) return false;

		auto pc = RE::PlayerCharacter::GetSingleton();
		if (!pc) return false;

		RE::MagicSystem::CastingSource castsource = SpellHotbar::GameData::global_casting_source ? static_cast<RE::MagicSystem::CastingSource>(std::clamp(static_cast<int>(SpellHotbar::GameData::global_casting_source->value), 0, 3)) : RE::MagicSystem::CastingSource::kOther;

		auto playerMagicCaster = pc->GetMagicCaster(castsource);
		if (!playerMagicCaster) return false;

		bool targetSelf = spell->GetDelivery() == RE::MagicSystem::Delivery::kSelf;
		RE::Actor* target = targetSelf ? pc : pc->GetActorRuntimeData().currentCombatTarget.get().get();

		if (!spell->GetNoDualCastModifications() && dual_cast) {
			playerMagicCaster->SetDualCasting(true);
		}

		if (concentration_manacost.has_value()) {
			playerMagicCaster->currentSpellCost = concentration_manacost.value();
		}

		GameData::pre_cast_mod_callback(spell);

		playerMagicCaster->CastSpellImmediate(spell, false, target, 1.0f, false, 0.0f, targetSelf ? nullptr : pc);

		//This is to fix failed target location spells
		if (spell->GetDelivery() == RE::MagicSystem::Delivery::kTargetLocation && spell->GetCastingType() != RE::MagicSystem::CastingType::kConcentration) {
			//if spell cast fails, this keeps hanging
			if (pc->IsCasting(spell)) {
				pc->InterruptCast(false);
				RE::PlaySound(Input::sound_MagFail);
				return false;
			}
		}

		//mod support Ordinator: Vancian magic, Energy Roil, Spellblade
		GameData::casted_spell_mod_callback(spell, dual_cast, spell_proc);

		return true;
	}

	CastingInstanceSpellRitualConcentration::CastingInstanceSpellRitualConcentration(RE::SpellItem* spell, float casttime, float manacost, hand_mode used_hand, uint16_t casteffect, bool spell_proc, const Input::KeyBind& keybind, int slot, float pre_release_anim_time)
		: CastingInstanceSpellConcentration(spell, casttime, manacost, used_hand, casteffect, spell_proc, keybind, slot)
	{
		m_pre_release_anim_time = pre_release_anim_time;
	}

	CastingInstancePower::CastingInstancePower(RE::TESForm* form) : BaseCastingInstance(form, 0.0f), m_old_form(nullptr), m_reequiped(false)
	{
		m_gcd = 0.5f;
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc) {
			// A previous hotbar shout may still be holding its write-back (thuum ticket 62).
			// Settle it first, or this cast would capture that shout as the power to restore
			// and the player's real one would be lost.
			flush_deferred_power_restore();
			auto& dat = pc->GetActorRuntimeData();
			m_old_form = dat.selectedPower;
			dat.selectedPower = form;
		}
	}

	void CastingInstancePower::on_reset()
	{
		BaseCastingInstance::on_reset();
		reequip_old_power();
	}

	bool CastingInstancePower::reequip_old_power()
	{
		if (!m_reequiped) {
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (pc) {
				// The write-back either happens now (powers, and a shout whose graph is
				// already done) or is handed to the controller to finish after the clip
				// (thuum ticket 62). Either way this instance is finished with it, so the
				// bookkeeping below — m_reequiped, consume_items, the caller's shout
				// cooldowns — keeps its existing timing.
				if (defer_power_write_back(pc)) {
					defer_power_restore(m_form, m_old_form);
				}
				else {
					write_back_power(m_form, m_old_form, "immediate");
				}
				m_reequiped = true;
				consume_items();
				return true;
			}
		}
		return false;
	}

	bool CastingInstancePower::update(RE::PlayerCharacter* pc, float delta)
	{
		advance_time(delta);
		if (casted() && !m_reequiped) {
			reequip_old_power();
		}
		return is_gcd_expired();
	}

	CastingInstanceShout::CastingInstanceShout(RE::TESForm* form) : CastingInstancePower(form)
	{
		m_gcd = 1.5f;
		if (GameData::individual_shout_cooldowns) {
			//If cast is allowed to start, individual shout CD must be 0
			if (m_old_form == nullptr || m_form->GetFormID() != m_old_form->GetFormID()) {
				GameData::reset_shout_cd();
			}
		}
	}

	bool CastingInstanceShout::defer_power_write_back(RE::PlayerCharacter* pc) const
	{
		if (!pc) {
			return false;
		}
		bool is_shouting{ false };
		pc->GetGraphVariableBool("IsShouting"sv, is_shouting);
		// Only hold it if the graph is actually shouting. A cast that died before the graph
		// ever entered — refused, interrupted, cut for a combo — restores here and now, exactly
		// as it did before, and never waits for an edge that will not come.
		return is_shouting;
	}

	bool CastingInstanceShout::reequip_old_power()
	{
		if(CastingInstancePower::reequip_old_power() && GameData::individual_shout_cooldowns) {
			if (m_form != m_old_form) {
				GameData::reset_shout_cd();

				//apply new shout CD if shout
				if (m_old_form != nullptr && m_old_form->GetFormType() == RE::FormType::Shout) {
					GameData::apply_cd_for_shout(m_old_form->GetFormID());
				}
			}
			return true;
		}
		return false;
	}

	CastingInstanceSpellData::CastingInstanceSpellData(RE::SpellItem* spell, float casttime, float manacost, hand_mode hand, bool dual_cast, int animation, int animation2, uint16_t casteffect, bool is_spell_proc) :
		m_spell(spell), m_casttime(casttime), m_manacost(manacost), m_hand(hand), m_dual_cast(dual_cast), m_animation(animation), m_animation2(animation2), m_casteffect(casteffect), m_spellproc(is_spell_proc)
	{
	}

	CastingInstancePotionUse::CastingInstancePotionUse(RE::TESForm* form): BaseCastingInstance(form, 0.0f)
	{
		m_gcd = 1.0f;
		if (form->GetFormType() == RE::FormType::AlchemyItem) {
			auto alch = form->As<RE::AlchemyItem>();
			if (!alch->IsFood() && !alch->IsPoison()) {
				//potion
				m_gcd = GameData::potion_gcd;
			}
		}
	}

	bool CastingInstancePotionUse::update(RE::PlayerCharacter* pc, float delta)
	{
		if (!casted()) {
			RE::ActorEquipManager::GetSingleton()->EquipObject(pc, m_form->As<RE::AlchemyItem>());
			set_casted();
		}
		advance_time(delta);
		return is_gcd_expired();
	}

	CastingInstanceWeaponArt::CastingInstanceWeaponArt(uint32_t art_id, float gcd)
		: BaseCastingInstance(nullptr, 0.0f), m_art_id(art_id)
	{
		// Ticket 42: an art with no usable number falls back to the action class, matching the
		// catalogue defaults rather than the old 1.0.
		m_gcd = gcd > 0.0f ? gcd : 1.5f;
	}

	bool CastingInstanceWeaponArt::update(RE::PlayerCharacter* pc, float delta)
	{
		advance_time(delta);
		if (pc && !pc->AsActorState()->IsWeaponDrawn()) {
			ArtDriver::cancel(pc);
			return true;
		}
		if (ArtDriver::is_active()) {
			if (m_cast_timer > 8.0f) {
				ArtDriver::cancel(pc);
				return true;
			}
			return false;
		}
		return is_gcd_expired();
	}

	void CastingInstanceWeaponArt::on_reset()
	{
		ArtDriver::finish(RE::PlayerCharacter::GetSingleton());
		BaseCastingInstance::on_reset();
	}

	bool try_start_art(uint32_t art_id, size_t slot, const Input::KeyBind& keybind)
	{
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (!pc) {
			logger::warn("SH2 art: no player");
			return false;
		}
		const ArtDefinition* art = GameData::get_art(art_id);
		if (!art) {
			logger::warn("SH2 art: unknown art id {}", art_id);
			return false;
		}
		if (!art->has_clip) {
			logger::warn("SH2 art: empty Custom Ability '{}' (no AABL_Attack_A.hkx)", art->display_name);
			RE::PlaySound(Input::sound_MagFail);
			return false;
		}
		if (!pc->AsActorState()->IsWeaponDrawn()) {
			logger::info("SH2 art: refused, weapon sheathed");
			RE::PlaySound(Input::sound_MagFail);
			return false;
		}
		if (!art_class_is_live(art->art_class, GameData::getPlayerEquipmentType())) {
			logger::info("SH2 art: art {} refused, Art Class mismatch", art_id);
			RE::PlaySound(Input::sound_MagFail);
			return false;
		}
		if (GameData::is_art_on_cd(art_id)) {
			logger::info("SH2 art: art {} on cooldown", art_id);
			return false;
		}
		auto* av = pc->AsActorValueOwner();
		const float have_stamina = av ? av->GetActorValue(RE::ActorValue::kStamina) : 0.0f;
		const float have_magicka = av ? av->GetActorValue(RE::ActorValue::kMagicka) : 0.0f;
		const float have_health = av ? av->GetActorValue(RE::ActorValue::kHealth) : 0.0f;
		const auto short_meter = unaffordable_art_meter(art->stamina_cost, art->magicka_cost, art->health_cost,
			have_stamina, have_magicka, have_health);
		if (short_meter != ArtMeter::None) {
			RE::ActorValue flash = RE::ActorValue::kStamina;
			if (short_meter == ArtMeter::Magicka) {
				flash = RE::ActorValue::kMagicka;
			} else if (short_meter == ArtMeter::Health) {
				flash = RE::ActorValue::kHealth;
			}
			logger::info("SH2 art: unaffordable (stam {} mag {} hp {})", art->stamina_cost, art->magicka_cost,
				art->health_cost);
			RE::HUDMenu::FlashMeter(flash);
			RE::PlaySound(Input::sound_MagFail);
			return false;
		}

		if (our_latch_is_closed()) {
			return CastIntent::offer(slot, keybind);
		}
		yield_if_our_latch_is_open();

		// Ticket 43: an art swing takes the graph away from any cast clip still owing a payload.
		deliver_armed_payload("the cut (weapon art)"sv);
		CastIntent::cancel();
		GameData::set_art_selector(art->selector);
		current_cast = std::make_unique<CastingInstanceWeaponArt>(art_id, art->gcd);
		if (!ArtDriver::begin(pc)) {
			logger::info("SH2 art: SH2_ArtStart not consumed (mid-swing or patch missing)");
			current_cast.reset();
			GameData::reset_art_selector();
			if (CastIntent::is_firing()) {
				return false;
			}
			return CastIntent::offer(slot, keybind);
		}
		if (art->stamina_cost > 0.0f && av) {
			av->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina, -art->stamina_cost);
		}
		if (art->magicka_cost > 0.0f && av) {
			av->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kMagicka, -art->magicka_cost);
		}
		if (art->health_cost > 0.0f && av) {
			av->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth, -art->health_cost);
		}
		if (art->cooldown_days > 0.0f) {
			GameData::add_art_cooldown(art_id, art->cooldown_days);
		}
		return true;
	}

}
