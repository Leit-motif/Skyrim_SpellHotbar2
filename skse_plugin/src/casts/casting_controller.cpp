#include "casting_controller.h"
#include <atomic>
#include "../logger/logger.h"
#include "../game_data/game_data.h"
#include "../input/keybinds.h"
#include "../rendering/render_manager.h"
#include "spell_proc.h"
#include "msco_cast_driver.h"
#include "art_driver.h"
#include "combo_cache.h"
#include "cast_intent.h"

namespace SpellHotbar::casts::CastingController {

	std::unique_ptr<BaseCastingInstance> current_cast = nullptr;

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
	std::atomic<bool> spellfire_seen{ false };

	// Which hand's SpellFire event may commit the current cast (bit 0 = left, bit 1 = right).
	// A vanilla cast of an equipped spell raises the same events, so an event from a hand this
	// cast is not using must not commit it.
	constexpr uint8_t fire_left{ 1 };
	constexpr uint8_t fire_right{ 2 };
	std::atomic<uint8_t> spellfire_mask{ 0 };

	// Arm the commitment point for one cast: forget stale fires and accept only the hand(s)
	// this cast throws with. Called right before the state entry is sent.
	void arm_spellfire(hand_mode used_hand) {
		// Minimal slice: the single SH2 state plays MSCO_left1, whose annotation is the
		// LEFT-hand SpellFire regardless of the hand this cast chose (runtime-verified
		// 2026-08-11: MLh_SpellFire_Event at +0.46s, every run), so only the left bit
		// arms — a right event can only come from an unrelated equipped cast and must
		// not commit this one. Per-hand masking returns with the per-hand clip matrix.
		(void)used_hand;
		uint8_t mask = fire_left;
		spellfire_mask.store(mask, std::memory_order_relaxed);
		spellfire_seen.store(false, std::memory_order_relaxed);
	}

	void notify_spellfire(bool left_hand) {
		const uint8_t bit = left_hand ? fire_left : fire_right;
		if (spellfire_mask.load(std::memory_order_relaxed) & bit) {
			logger::debug("SH2 cast: graph raised a {} SpellFire event", left_hand ? "left" : "right");
			spellfire_seen.store(true, std::memory_order_relaxed);
		}
	}

	// Cleared when a cast starts, not only when one ends: a shout pressed on the vanilla key
	// raises spellfire with no cast instance live at all, and that must not leave the next hotbar
	// cast committed before it has begun.
	void clear_spellfire() {
		spellfire_seen.store(false, std::memory_order_relaxed);
	}

	bool is_cast_committed() {
		return spellfire_seen.load(std::memory_order_relaxed);
	}

	bool is_committed_cast_holding_graph() {
		return current_cast && current_cast->has_cuttable_cast_state() && is_cast_committed() &&
			   MscoCastDriver::is_active();
	}

	bool can_accept_hotbar_cast() {
		return classify_hotbar_cast_press(current_cast != nullptr, is_committed_cast_holding_graph(),
				   MscoCastDriver::combo_window_open()) != HotbarCastPress::refuse;
	}

	void reset_cast() {
		current_cast->on_reset();
		current_cast.reset();
		clear_spellfire();
	}

	void drop_live_cast()
	{
		current_cast.reset();
		clear_spellfire();
		ArtDriver::reset_session();
		MscoCastDriver::reset_session();
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
		return m_cast_timer <= -m_gcd;
	}

	bool CastingInstance::should_play_release_anim()
	{
		return !m_played_release && (m_cast_timer <= m_release_anim_time);
	}

	bool BaseCastingInstance::advance_time(float delta)
	{
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
		float f = (m_cast_timer + m_gcd) / (m_total_casttime + m_gcd);

		return std::clamp(1.0f - f, 0.0f, 1.0f);
	}

	float BaseCastingInstance::get_current_gcd_duration() const
	{
		return m_total_casttime + m_gcd;
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

	bool BaseCastingInstance::blocks_movement() const
	{
		return false;
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
			logger::warn("SH2 cast: no SpellFire event; delivering after the clip ended past the authored cast time");
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
		m_gcd = 0.0f;
	}

	CastingInstanceRitual::CastingInstanceRitual(RE::SpellItem* spell, float casttime, float manacost, hand_mode used_hand, uint16_t casteffect, bool spell_proc) : CastingInstance(spell, casttime, manacost, used_hand, casteffect, spell_proc)
	{
		m_release_anim_time = 0.25f;
		m_gcd = 1.5f;
	}

	bool CastingInstanceRitual::blocks_movement() const
	{
		return m_cast_timer >= -0.5f; //!m_casted &&
	}

	CastingInstanceSpellConcentration::CastingInstanceSpellConcentration(RE::SpellItem* spell, float casttime, float manacost, hand_mode used_hand, uint16_t casteffect, bool spell_proc, const Input::KeyBind& keybind, int slot, bool blocksMovement)
		: CastingInstance(spell, casttime, manacost, used_hand, casteffect, spell_proc),
		m_keybind(keybind),
		m_slot(slot),
		m_blocks_movement(blocksMovement)
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
				//Check for anim reloop & do loop callbacks

				if (keydown) {
					MscoCastDriver::replay(pc);
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
				MscoCastDriver::cancel(pc);
				play_release_sound();
				apply_cooldown();
				consume_items();
			}

		}

		return cancel;
	}

	bool CastingInstanceSpellConcentration::is_gcd_expired() const
	{
		return m_gcd <= 0;
	}

	bool CastingInstanceSpellConcentration::blocks_movement() const
	{
		return m_blocks_movement && !m_casted;
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

	void update_cast(float delta)
	{
		if (current_cast) {
			if (!current_cast->casted()) {
				auto pc = RE::PlayerCharacter::GetSingleton();
				if (pc) {

					if (current_cast->update(pc, delta)) {
						reset_cast();
					}

				}
			}
			else {
				// FNF Driver Casts live until the clip ends (ticket 18): no leftover 1.0s/1.5s
				// tail after CastExit. Potions, shouts, and powers still use their own GCD.
				if (current_cast->has_cuttable_cast_state()) {
					if (!MscoCastDriver::is_active()) {
						GameData::reset_animation_vars();
						reset_cast();
					} else {
						current_cast->advance_time(delta);
					}
				} else {
					current_cast->advance_time(delta);
					if (current_cast->is_gcd_expired()) {
						GameData::reset_animation_vars();
						reset_cast();
					}
				}
			}
		}
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
				if (MscoCastDriver::begin(pc, used_hand, cast_info.m_casttime)) {
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

				int anim = cast_info.m_dual_cast ? cast_info.m_animation2 : cast_info.m_animation;
				if (anim < 0) {
					anim = GameData::chose_default_anim_for_spell(cast_info.m_spell, -1, cast_info.m_dual_cast);
				}
				GameData::set_animtype_global(anim);

				hand_mode used_hand = GameData::set_weapon_dependent_casting_source(cast_info.m_hand, cast_info.m_dual_cast);
				current_cast = std::make_unique<CastingInstanceSpellConcentration>(cast_info.m_spell, cast_info.m_casttime, cast_info.m_manacost, used_hand, cast_info.m_casteffect, cast_info.m_spellproc, keybind, static_cast<int>(slot), cast_info.m_dual_cast);

				arm_spellfire(used_hand);
				if (MscoCastDriver::begin(pc, used_hand, cast_info.m_casttime)) {
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
				if (MscoCastDriver::begin(pc, used_hand, cast_info.m_casttime)) {
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
				
				bool is_fast_cast = cast_info.m_casttime <= fast_cast_threshold;

				int anim = is_fast_cast ? cast_info.m_animation2 : cast_info.m_animation;
				if (anim < 0) {
					anim = GameData::chose_default_anim_for_spell(cast_info.m_spell, -1, is_fast_cast);
				}
				GameData::set_animtype_global(anim);

				hand_mode used_hand = GameData::set_weapon_dependent_casting_source(cast_info.m_hand, cast_info.m_dual_cast);
				current_cast = std::make_unique<CastingInstanceRitual>(cast_info.m_spell, cast_info.m_casttime, cast_info.m_manacost, used_hand, cast_info.m_casteffect, cast_info.m_spellproc);
				arm_spellfire(used_hand);
				if (MscoCastDriver::begin(pc, used_hand, cast_info.m_casttime)) {
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

	bool try_start_cast(RE::TESForm* form, const Input::KeyBind& keybind, size_t slot, hand_mode hand)
	{
		const auto press = classify_hotbar_cast_press(
			current_cast != nullptr, is_committed_cast_holding_graph(),
			MscoCastDriver::combo_window_open());
		if (press != HotbarCastPress::refuse) {
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
				bool is_shouting{ false };
				pc->GetGraphVariableBool("IsShouting"sv, is_shouting);

				if (!is_shouting && (form->GetFormType() == RE::FormType::Spell || form->GetFormType() == RE::FormType::Scroll)) {
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
				else if (!is_shouting && (form->GetFormType() == RE::FormType::AlchemyItem)) {
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
		if (can_start_new_cast()) {
			clear_spellfire();
			// A power or shout press is a newer input too, so it supersedes a waiting intent. A
			// real shout also reaches ShoutMCO's own hook and would displace it there, but a
			// voice-slot power never does — nothing on that path touches the driver at all.
			CastIntent::cancel();
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (form && pc) {
				bool is_shouting{ false };
				pc->GetGraphVariableBool("IsShouting"sv, is_shouting);

				if (!is_shouting && form->GetFormType() == RE::FormType::Shout) {

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

	bool is_movement_blocking_cast()
	{
		// WASD capture follows the shtb state (ticket 19). bAnimationDriven is
		// owned by the graph wrap (ticket 21), not the DLL. Weapon Arts reuse
		// the same plant: input lock, clip motion still applies.
		if (shtb_state_blocks_movement(MscoCastDriver::is_active() || ArtDriver::is_active())) {
			return true;
		}
		if (current_cast) {
			return current_cast->blocks_movement();
		}
		return false;
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

	bool CastingInstanceSpellRitualConcentration::blocks_movement() const
	{
		return !m_casted;
	}

	CastingInstancePower::CastingInstancePower(RE::TESForm* form) : BaseCastingInstance(form, 0.0f), m_old_form(nullptr), m_reequiped(false)
	{
		m_gcd = 0.5f;
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc) {
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
				auto& dat = pc->GetActorRuntimeData();
				if (dat.selectedPower == m_form) {
					dat.selectedPower = m_old_form;
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
		m_gcd = gcd > 0.0f ? gcd : 1.0f;
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

	bool try_start_art(uint32_t art_id, size_t)
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
		if (!art_class_is_live(art->art_class, GameData::art_stance_of(GameData::getPlayerEquipmentType()))) {
			logger::info("SH2 art: art {} refused, Art Class mismatch", art_id);
			RE::PlaySound(Input::sound_MagFail);
			return false;
		}
		if (GameData::is_art_on_cd(art_id)) {
			logger::info("SH2 art: art {} on cooldown", art_id);
			return false;
		}
		auto* av = pc->AsActorValueOwner();
		if (art->stamina_cost > 0.0f && av && av->GetActorValue(RE::ActorValue::kStamina) < art->stamina_cost) {
			logger::info("SH2 art: unaffordable (need {} stamina)", art->stamina_cost);
			RE::HUDMenu::FlashMeter(RE::ActorValue::kStamina);
			RE::PlaySound(Input::sound_MagFail);
			return false;
		}

		GameData::set_art_selector(art->selector);
		current_cast = std::make_unique<CastingInstanceWeaponArt>(art_id, art->gcd);
		if (!ArtDriver::begin(pc)) {
			logger::info("SH2 art: SH2_ArtStart not consumed (sheathed, mid-swing, or patch missing)");
			current_cast.reset();
			GameData::reset_art_selector();
			return false;
		}
		if (art->stamina_cost > 0.0f && av) {
			av->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina, -art->stamina_cost);
		}
		if (art->cooldown_days > 0.0f) {
			GameData::add_art_cooldown(art_id, art->cooldown_days);
		}
		return true;
	}

}
