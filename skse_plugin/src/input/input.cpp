#include "input.h"
#include <chrono>
#include <fstream>
#include <iterator>
#include <optional>
#include <unordered_map>
#include "input_event_adapter.h"
#include "keybinds.h"
#include "../logger/logger.h"
#include "../mcp/bind_capture.h"
#include "../rendering/render_manager.h"
#include "../casts/casting_controller.h"
#include "../casts/combo_cache.h"
#include "../casts/msco_cast_driver.h"
#include "../casts/art_driver.h"
#include "../storage/storage.h"
#include "modes.h"
#include "../rendering/advanced_bind_menu.h"
#include "../game_data/action_definition.h"

namespace {
    thread_local SpellHotbar::Input::InputEventAdapter<RE::InputEvent> input_event_adapter;
}
namespace SpellHotbar::Input {

    inline constexpr std::tuple<uint32_t, RE::INPUT_DEVICE> get_device_and_input(const RE::ButtonEvent* bEvent) {
        uint32_t key = 0;
        RE::INPUT_DEVICE dev = RE::INPUT_DEVICE::kNone;
        if (bEvent->GetDevice() == RE::INPUT_DEVICE::kKeyboard) {
            key = bEvent->GetIDCode();
            dev = RE::INPUT_DEVICE::kKeyboard;
        }
        else if (bEvent->GetDevice() == RE::INPUT_DEVICE::kMouse) {
            key = bEvent->GetIDCode();
            dev = RE::INPUT_DEVICE::kMouse;
        }
        else if (bEvent->GetDevice() == RE::INPUT_DEVICE::kGamepad) {
            dev = RE::INPUT_DEVICE::kGamepad;
            //Map Gamepad keys to 0-x
            switch (bEvent->GetIDCode()) {
            case RE::BSWin32GamepadDevice::Keys::Key::kUp:
                key = 0;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kDown:
                key = 1;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kLeft:
                key = 2;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kRight:
                key = 3;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kStart:
                key = 4;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kBack:
                key = 5;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kLeftThumb:
                key = 6;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kRightThumb:
                key = 7;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kLeftShoulder:
                key = 8;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kRightShoulder:
                key = 9;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kA:
                key = 10;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kB:
                key = 11;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kX:
                key = 12;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kY:
                key = 13;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kLeftTrigger:
                key = 14;
                break;
            case RE::BSWin32GamepadDevice::Keys::Key::kRightTrigger:
                key = 15;
                break;
            default:
                //Invalid key
                key = 0;
                dev = RE::INPUT_DEVICE::kNone;
            }
        }

        return std::make_tuple(key, dev);
    }

    std::optional<ActionInputDevice> action_input_device(RE::INPUT_DEVICE device)
    {
        switch (device) {
        case RE::INPUT_DEVICE::kKeyboard:
            return ActionInputDevice::keyboard;
        case RE::INPUT_DEVICE::kMouse:
            return ActionInputDevice::mouse;
        case RE::INPUT_DEVICE::kGamepad:
            return ActionInputDevice::gamepad;
        default:
            return std::nullopt;
        }
    }

    namespace {

        // Which key the right attack is bound to on this device, or kInvalid.
        //
        // Keyboard and mouse only. The gamepad ids this file works in are its own 0-15 ordinals
        // (get_device_and_input), while GetMappedKey answers in the engine's ids, so a gamepad
        // comparison here is between two different alphabets and can only be wrong. kNone is
        // excluded for a harder reason: it is -1, and GetMappedKey indexes deviceMappings[device]
        // behind an assert that a release build drops.
        //
        // Only the right attack. The left control is block, or a left-hand cast, and neither is
        // the "spell into a swing" the chain is for.
        uint32_t get_attack_key(RE::INPUT_DEVICE key_device)
        {
            if (key_device != RE::INPUT_DEVICE::kKeyboard && key_device != RE::INPUT_DEVICE::kMouse) {
                return RE::ControlMap::kInvalid;
            }
            auto control_map = RE::ControlMap::GetSingleton();
            auto user_events = RE::UserEvents::GetSingleton();
            if (!control_map || !user_events) {
                return RE::ControlMap::kInvalid;
            }
            return control_map->GetMappedKey(user_events->rightAttack, key_device);
        }

        // Is this press one that starts an attack -- the mapped right attack, or one of OCPA's
        // power-attack hotkeys? Reported together because the chain-out treats them alike: it ends
        // the cast state and lets the press reach whoever handles it.
        bool is_attack_press(uint32_t key_code, RE::INPUT_DEVICE key_device, uint32_t attack_key)
        {
            if (attack_key != RE::ControlMap::kInvalid && key_code == attack_key) {
                return true;
            }
            if (key_device != RE::INPUT_DEVICE::kKeyboard) {
                return false;
            }
            const auto& ocpa = get_ocpa_keys();
            return (ocpa.power != 0 && key_code == ocpa.power) ||
                   (ocpa.dual != 0 && key_code == ocpa.dual);
        }

        uint32_t get_left_attack_key(RE::INPUT_DEVICE key_device)
        {
            if (key_device != RE::INPUT_DEVICE::kKeyboard && key_device != RE::INPUT_DEVICE::kMouse) {
                return RE::ControlMap::kInvalid;
            }
            auto control_map = RE::ControlMap::GetSingleton();
            auto user_events = RE::UserEvents::GetSingleton();
            if (!control_map || !user_events) {
                return RE::ControlMap::kInvalid;
            }
            return control_map->GetMappedKey(user_events->leftAttack, key_device);
        }

        bool left_hand_holds_spell(RE::PlayerCharacter* pc)
        {
            if (!pc) {
                return false;
            }
            auto* obj = pc->GetEquippedObject(true);
            return obj && (obj->Is(RE::FormType::Spell) || obj->Is(RE::FormType::Scroll));
        }

        // A left-hand cast press during a committed hotbar cast: the left control is block
        // when the left hand holds a weapon or shield, so this only matches when it would
        // actually start an MSCO hand cast.
        bool is_left_hand_cast_press(RE::PlayerCharacter* pc, uint32_t key_code, RE::INPUT_DEVICE key_device)
        {
            const uint32_t left_key = get_left_attack_key(key_device);
            return casts::cut_committed_cast_for_left_hand_press(
                casts::CastingController::is_committed_cast_holding_graph(),
                left_hand_holds_spell(pc),
                left_key != RE::ControlMap::kInvalid && key_code == left_key);
        }
    }

    static InputEventDecision<RE::InputEvent> process_event_impl(RE::InputEvent* event)
    {
        if (!event) {
            return {};
        }

        //don't react to inputs outside of the game:
        auto pc = RE::PlayerCharacter::GetSingleton();
        if (!pc || !pc->Is3DLoaded()) {
            return {};
        }

        RE::InputEvent* addEvent = nullptr;
        auto [shoutKeyDev, shoutKey] = get_shout_key_and_device();

        bool captureEvent = false; // Capture this event? (do not forward to Skyrim)

            // SMF runs this callback before TranslateInputEvent. Returning true
            // unlinks the event from the queue, so ImGui never sees the click.
            // Blocking SH2 windows already pause the game through
            // WindowInterface::BlockUserInput; do not capture cursor/key events
            // for that case. Bind-capture still consumes the next down edge.
            if (event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
                RE::ButtonEvent* bEvent = event->AsButtonEvent();
                if (bEvent) {
                    auto [key_code, key_device] = get_device_and_input(bEvent);
                    bool is_pressed = bEvent->IsPressed();

                    auto& capture = Mcp::bind_capture();
                    if (capture.action_armed()) {
                        if (bEvent->IsDown()) {
                            const bool is_escape =
                                key_device == RE::INPUT_DEVICE::kKeyboard && key_code == 1;
                            if (is_escape) {
                                capture.apply_action_down_edge(true, ActionInputDevice::keyboard, -1);
                            } else if (const auto device = action_input_device(key_device)) {
                                const int dx = input_to_dx_scancode(
                                    key_device, static_cast<uint8_t>(key_code));
                                capture.apply_action_down_edge(false, *device, dx);
                            }
                        }
                        captureEvent = true;
                    } else if (capture.armed()) {
                        if (bEvent->IsDown()) {
                            const bool is_escape =
                                key_device == RE::INPUT_DEVICE::kKeyboard && key_code == 1;
                            const bool rebindable =
                                key_device == RE::INPUT_DEVICE::kKeyboard ||
                                key_device == RE::INPUT_DEVICE::kMouse ||
                                key_device == RE::INPUT_DEVICE::kGamepad;
                            if (is_escape) {
                                capture.apply_down_edge(true);
                            } else if (rebindable) {
                                const int pending_id = capture.pending_id();
                                if (capture.apply_down_edge(false) == Mcp::CaptureApply::rebound) {
                                    const int dx = input_to_dx_scancode(
                                        key_device, static_cast<uint8_t>(key_code));
                                    if (dx >= 0) {
                                        rebind_key(pending_id, dx);
                                    }
                                }
                            }
                        }
                        captureEvent = true;
                    } else {
                    if (key_device == RE::INPUT_DEVICE::kKeyboard) {
                        if (key_code == 56 || key_code == 184) {
                            mod_alt.update(key_code, key_device, is_pressed);
                        }
                    }
                    mod_1.update(key_code, key_device, is_pressed);
                    mod_2.update(key_code, key_device, is_pressed);
                    mod_3.update(key_code, key_device, is_pressed);
                    mod_dual_cast.update(key_code, key_device, is_pressed);
                    mod_show_bar.update(key_code, key_device, is_pressed);

                    //update all keybind states
                    for (size_t i = 0; i < key_spells.size(); ++i) {
                        key_spells[i].update(key_code, key_device, is_pressed);
                    }
                    key_oblivion_cast.update(key_code, key_device, is_pressed);
                    key_oblivion_potion.update(key_code, key_device, is_pressed);

                    const bool smf_blocking = RenderManager::should_block_game_key_inputs();

                    if (RenderManager::is_dragging_bar() && key_device == RE::INPUT_DEVICE::kKeyboard && key_code == 1 && bEvent->IsDown()) {
                        RenderManager::stop_bar_dragging();
                    }

                    if (smf_blocking && bEvent->IsDown()) {
                        if (key_device == RE::INPUT_DEVICE::kKeyboard && key_code == 1) {
                            RenderManager::close_key_blocking_frames();
                        }
                        else if (RenderManager::is_bind_menu_opened() && Input::key_open_advanced_bind_menu.isValidBound()
                            && Input::key_open_advanced_bind_menu.matches(key_code, key_device)) {
                            RenderManager::close_key_blocking_frames();
                        }
                    }

                    // Chain a committed cast into an MCO attack. The shtb cast state has no
                    // transition to the attack states -- SH2's patch authored an entry and a
                    // state-local exit and nothing else -- so a press during the cast is
                    // silently refused for the whole clip (live-verified 2026-08-12: an attack
                    // press at 0.9s leaves stamina untouched, the same press at 1.8s spends it).
                    // Ending the state on the press is what turns the clip's ~1.1s of tail after
                    // spellfire into the start of a swing.
                    //
                    // The press itself is not touched: it travels the rest of this dispatch and
                    // reaches the game exactly as it does today. An earlier revision captured it
                    // and re-queued a copy, meaning to buy the graph a frame; it does not,
                    // because this hook runs inside PollInputDevices and PushOntoInputQueue
                    // appends to the very chain being dispatched. Both events reach the graph's
                    // queue in the order they were sent, which is the ordering the cut needs and
                    // the only one available. Leaving the press alone is also what makes this
                    // fail-safe: a graph that refuses the cut gives the player today's behaviour
                    // rather than a swallowed attack.
                    //
                    // The gate is BOTH halves of the cuttable span (ticket 45). Ticket 43 retires
                    // the instance at GCD expiry, so `is_committed_cast_holding_graph` -- which
                    // needs a live `current_cast` -- goes false while the clip plays on, and the
                    // follow-through is exactly the tail this cut exists to use. The
                    // follow-through predicate covers retirement to clip end; together they run
                    // from the commitment point to the end of the clip. A cast still CHARGING has
                    // a live instance and no commitment, so neither half admits it and a press
                    // then keeps today's behaviour.
                    if (!captureEvent && pc && bEvent->IsDown() && in_ingame_state() &&
                        (casts::CastingController::is_committed_cast_holding_graph() ||
                         casts::CastingController::is_cuttable_follow_through()))
                    {
                        const uint32_t attack_key = get_attack_key(key_device);
                        // Traced for every press during a cast, matching or not, because this is
                        // the one branch an agent cannot drive -- injected input never reaches
                        // this hook (verified 2026-08-12), so the owner's own press is the only
                        // test and it has to say why it failed without a second session. Two key
                        // numbers that disagree are the whole diagnosis.
                        logger::trace("SH2 cast: press during a committed cast (device={}, key={}, attack key={})",
                            static_cast<int>(key_device), key_code, attack_key);

                        // A payload still owed when the cut lands is paid out first, the same as
                        // the five other cut seams (ticket 43). The armed poll's clip-end fallback
                        // would catch it a frame later anyway; delivering here keeps one delivery
                        // story and one log line per payload.
                        if (is_attack_press(key_code, key_device, attack_key)) {
                            casts::CastingController::cut_committed_cast_for_attack(pc);
                        } else if (is_left_hand_cast_press(pc, key_code, key_device)) {
                            casts::CastingController::cut_committed_cast_for_attack(pc);
                        }
                    }

                    // Chain out of a concentration channel. A channel has no clip left to cut
                    // -- its start clip ended and the hold is sustained by the OAR idle loop --
                    // so the thing an attack has to end is the channel itself. The press is not
                    // captured, the same fail-safe the cast cut above uses: a channel that
                    // refuses to end still gives the player an ordinary swing.
                    if (!captureEvent && pc && bEvent->IsDown() && in_ingame_state() &&
                        casts::CastingController::is_channel_chainable())
                    {
                        const uint32_t attack_key = get_attack_key(key_device);
                        if (casts::should_cut_channel_for_attack(
                                true, is_attack_press(key_code, key_device, attack_key))) {
                            casts::CastingController::cut_channel_for_attack(pc);
                        }
                    }

                    if (!captureEvent && pc && bEvent->IsDown() && in_ingame_state() &&
                        casts::ArtDriver::is_active())
                    {
                        const uint32_t attack_key = get_attack_key(key_device);
                        const bool attack = is_attack_press(key_code, key_device, attack_key);
                        if (casts::should_capture_attack_during_ability(
                                true, casts::ArtDriver::latch_open(), attack)) {
                            captureEvent = true;
                            logger::debug("SH2 art: captured attack before Ability latch");
                        } else if (casts::should_cut_ability_for_attack(
                                       true, casts::ArtDriver::latch_open(), attack)) {
                            logger::debug("SH2 art: attack pressed after Ability latch; ending the state");
                            casts::ArtDriver::cancel(pc);
                        }
                    }

                    if (!captureEvent && !smf_blocking) {
                        bool handled{ false };
                        if (!Bars::disable_non_modifier_bar || Input::mod_1.isDown() || Input::mod_2.isDown() || Input::mod_3.isDown()) {

                            for (size_t i = 0; i < key_spells.size() && !handled; ++i) {
                                const auto& bind = key_spells[i];

                                if (bind.matches(key_code, key_device))
                                {
                                    if (in_binding_menu())
                                    {
                                        if (!Bars::disable_menu_binding && bEvent->IsDown()) {
                                            handled = true;
                                            RE::TESForm* form = get_current_selected_spell_in_menu();
                                            if (form) {
                                                slot_spell(form, i);
                                            }
                                            if (mod_1.isDown() || mod_2.isDown() || mod_3.isDown()) {
                                                //Do not forward keypress to game if modifier was used, this allows easy double binding with modifiers
                                                captureEvent = true;
                                            }
                                        }
                                    }
                                    else if (in_ingame_state())
                                    {
                                        if (bEvent->IsDown()) {
                                            handled = true;
                                            auto skill = GameData::get_current_spell_info_in_slot(i);
                                            if (GameData::isVampireLord() &&
                                                GameData::global_vampire_lord_equip_mode && GameData::global_vampire_lord_equip_mode->value > 0.0f &&
                                                !Input::is_equip_mode())
                                            {
                                                //If Vampire Lord and using Not Equipmode -> use special VL mode (equip spells & cast powers) instead
                                                //The global can turn of this behaviour
                                                InputModeVampireLord::getSingleton()->process_input(skill, addEvent, i, bind, shoutKeyDev, shoutKey);
                                            }
                                            else if (InputModeBase::current_mode) {
                                                InputModeBase::current_mode->process_input(skill, addEvent, i, bind, shoutKeyDev, shoutKey);
                                            }
                                        }
                                        else if (bEvent->IsUp()) {
                                            handled = true;
                                            //check for release of power/shout key release event
                                            if (casts::CastingController::is_currently_using_power()) {

                                                float ct = casts::CastingController::get_current_casttime();
                                                if (!addEvent) {
                                                    addEvent = RE::ButtonEvent::Create(shoutKeyDev, "Shout", shoutKey, 0.0f, ct); //default shout key
                                                }
                                            }
                                        }
                                        else if (bEvent->IsRepeating()) {
                                            //Check in Oblivion Mode for holding slot key down to show bar.
                                            InputModeBase::current_mode->process_key_update(bind, i, bEvent->HeldDuration());
                                        }
                                        if (handled && (mod_1.isDown() || mod_2.isDown() || mod_3.isDown())) {
                                            //Do not forward keypress to game if modifier was used, this allows easy double binding with modifiers
                                            captureEvent = true;
                                        } else if (handled && in_ingame_state()) {
                                            const auto skill = GameData::get_current_spell_info_in_slot(i);
                                            if (casts::capture_hotbar_press_to_prevent_dual_fire(
                                                    skill.type == slot_type::spell, left_hand_holds_spell(pc))) {
                                                captureEvent = true;
                                                logger::debug("SH2 cast: captured hotbar press to prevent dual fire");
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (!handled && Input::is_oblivion_mode() && in_ingame_state())
                        {
                            bool cast = key_oblivion_cast.matches(key_code, key_device);
                            bool potion = key_oblivion_potion.matches(key_code, key_device);
                            if (cast || potion)
                            {
                                if (bEvent->IsDown()) {
                                    size_t index = keybind_id::oblivion_potion;
                                    if (cast) {
                                        index = keybind_id::oblivion_cast;
                                    }
                                    handled = true;
                                    auto skill = GameData::get_current_spell_info_in_slot(index);

                                    if (InputModeBase::current_mode) {
                                        if (cast) {
                                            InputModeBase::current_mode->process_input(skill, addEvent, index, key_oblivion_cast, shoutKeyDev, shoutKey);
                                        }
                                        else if (potion) {
                                            InputModeBase::current_mode->process_input(skill, addEvent, index, key_oblivion_potion, shoutKeyDev, shoutKey);
                                        }
                                    }
                                }
                            }
                        }

                        if (!handled && in_binding_menu())
                        {
                            if (key_open_advanced_bind_menu.matches(key_code, key_device) && bEvent->IsDown()) {
                                handled = true;
                                RenderManager::open_advanced_binding_menu();
                                RE::PlaySound(sound_UISkillsForward);
                            }
                            else if (key_next.matches(key_code, key_device) && bEvent->IsDown()) {
                                handled = true;
                                Bars::menu_bar_id = Bars::getNextMenuBar(Bars::menu_bar_id);
                                RE::PlaySound(sound_UISkillsForward);
                            }
                            else if (key_prev.matches(key_code, key_device) && bEvent->IsDown()) {
                                handled = true;
                                Bars::menu_bar_id = Bars::getPreviousMenuBar(Bars::menu_bar_id);
                                RE::PlaySound(sound_UISkillsBackward);
                            }
                        }

                    }
                    }

                }
            }

        return InputEventDecision<RE::InputEvent>{ .capture = captureEvent, .injected = addEvent };
    }

    bool __stdcall process_event(RE::InputEvent* event)
    {
        return input_event_adapter.process(event, process_event_impl);
    }

    KeyModifier::KeyModifier(RE::INPUT_DEVICE device, uint8_t code1, uint8_t code2)
        : input_device(device), keycode(code1), keycode2(code2), isDown1(false), isDown2(false)
    {}

    void KeyModifier::update(uint32_t key_code, RE::INPUT_DEVICE key_device, bool is_pressed)
    {
        if (key_device == input_device) {
            if (keycode > 0 && key_code == keycode)
            {
                isDown1 = is_pressed;
            }
            else if (keycode2 > 0 && key_code == keycode2)
            {
                isDown2 = is_pressed;
            }
        }
    }

    void KeyModifier::rebind(int dx_scancode)
    {
        auto [device, code] = dx_scan_code_to_input(dx_scancode);
        input_device = device;
        keycode = code;
        keycode2 = code;

        if (input_device == RE::INPUT_DEVICE::kKeyboard) {
            //ctrl
            if (keycode == 29 || keycode == 157) {
                keycode = 29;
                keycode2 = 157;
            }
            //shift
            else if (keycode == 42 || keycode == 54) {
                keycode = 42;
                keycode2 = 54;
            }
            //alt
            else if (keycode == 56 || keycode == 184) {
                keycode = 56;
                keycode2 = 184;
            }
        }
        isDown1 = false;
        isDown2 = false;
    }

    int KeyModifier::get_dx_scancode()
    {
        return input_to_dx_scancode(input_device, keycode);
    }

    int KeyModifier::get_dx_scancode2()
    {
        return input_to_dx_scancode(input_device, keycode2);
    }

    bool KeyModifier::isValidBound()
    {
        return input_device == RE::INPUT_DEVICE::kKeyboard || input_device == RE::INPUT_DEVICE::kMouse || input_device == RE::INPUT_DEVICE::kGamepad;
    }


    KeyBind::KeyBind(RE::INPUT_DEVICE device, uint8_t code) : input_device(device), keycode(code), m_isDown(false)
    {
    }

    bool KeyBind::matches(uint32_t key_code, RE::INPUT_DEVICE key_device) const
    {
        return key_device == input_device && key_code == keycode;
    }

    int KeyBind::get_dx_scancode() const
    {
        return input_to_dx_scancode(input_device, keycode);
    }

    void KeyBind::assign_from_dx_scancode(int code)
    {
        auto [dev, key] = dx_scan_code_to_input(code);
        input_device = dev;
        keycode = key;
        m_isDown = false;
    }

    void KeyBind::update(uint32_t key_code, RE::INPUT_DEVICE key_device, bool is_pressed)
    {
        if (matches(key_code, key_device)) {
            m_isDown = is_pressed;
        }
    }

    void KeyBind::unbind()
    {
        input_device = RE::INPUT_DEVICE::kNone;
        keycode = 0Ui8;
        m_isDown = false;
    }

    bool in_ingame_state() {
        const auto ui = RE::UI::GetSingleton();
        
        if (!ui || ui->GameIsPaused() || !ui->IsCursorHiddenWhenTopmost() || !ui->IsShowingMenus() || !ui->GetMenu<RE::HUDMenu>() || ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME) || ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME))
        {
            return false;
        }
        else {
            return true;
        }
    }

    std::tuple<RE::INPUT_DEVICE, uint8_t> dx_scan_code_to_input(int dx_scancode)
    {
        RE::INPUT_DEVICE input_device{ RE::INPUT_DEVICE::kNone };
        uint8_t keycode{ 0Ui8 };
        if (dx_scancode < 0)
        {
            input_device = RE::INPUT_DEVICE::kNone;
            keycode = 0Ui8;
        }
        else if (dx_scancode < 256) {
            input_device = RE::INPUT_DEVICE::kKeyboard;
            keycode = static_cast<uint8_t>(dx_scancode);
        }
        else if (dx_scancode < 266) {
            input_device = RE::INPUT_DEVICE::kMouse;
            keycode = static_cast<uint8_t>(dx_scancode - 256);
        }
        else {
            input_device = RE::INPUT_DEVICE::kGamepad;
            keycode = static_cast<uint8_t>(dx_scancode - 266);
        }

        return std::make_tuple(input_device, keycode);
    }

    int input_to_dx_scancode(RE::INPUT_DEVICE device, uint8_t code)
    {
        if (device == RE::INPUT_DEVICE::kNone) {
            return -1;
        }

        int offset{ 0 };
        if (device == RE::INPUT_DEVICE::kMouse) {
            offset = 256;
        }
        else if (device == RE::INPUT_DEVICE::kGamepad) {
            offset = 266;
        }
        return static_cast<int>(code) + offset;
    }

    namespace {
        std::string read_data_file(std::string_view path)
        {
            std::ifstream in{ std::string{ path } };
            if (!in) {
                return {};
            }
            return { std::istreambuf_iterator<char>{ in }, std::istreambuf_iterator<char>{} };
        }
    }

    // One Click Power Attack's keys, read from its own config so they are configured in one
    // place rather than copied here and left to drift. The cast-cut detector keeps a session
    // snapshot, while Action dispatch has a live resolver below so a changed VFS config is
    // reflected on the next press.
    //
    // A power attack does not travel the control map at all -- OCPA is a mod hotkey, which is
    // why the right-attack lookup above answers kInvalid for it and why three power-attack
    // presses during a cast were seen and declined on 2026-08-12. The press itself does reach
    // this hook, so knowing the key is the whole of what was missing.
    //
    // Fails open: no config, no key, and only the mapped right attack chains.
    OcpaKeys read_ocpa_keys_from_vfs()
    {
        constexpr std::string_view paths[]{
            "Data/MCM/Settings/OCPA.ini"sv,
            "Data/MCM/Config/OCPA/settings.ini"sv,
        };

        for (const auto& path : paths) {
            const auto text = read_data_file(path);
            if (text.empty()) {
                continue;
            }
            auto found = parse_ocpa_keys(text);
            logger::info("SH2 cast: OCPA keys read from {} (power={}, dual={})", path, found.power, found.dual);
            return found;
        }
        logger::info("SH2 cast: no OCPA config found; only the mapped right attack chains out of a cast");
        return OcpaKeys{};
    }

    const OcpaKeys& get_ocpa_keys()
    {
        static const OcpaKeys keys = read_ocpa_keys_from_vfs();
        return keys;
    }

    OcpaKeys resolve_ocpa_keys_live()
    {
        return read_ocpa_keys_from_vfs();
    }

    uint32_t read_dodge_hotkey_from_vfs()
    {
        constexpr std::string_view path{ "Data/SKSE/Plugins/TK Dodge RE.ini"sv };
        const auto text = read_data_file(path);
        const auto found = parse_dodge_hotkey(text);
        if (found == 0) {
            logger::info("SH2 action spike: no DodgeHotkey in {}", path);
            return 0U;
        }
        logger::info("SH2 action spike: dodge hotkey read from {} (key={})", path, found);
        return found;
    }

    uint32_t get_dodge_hotkey()
    {
        static const uint32_t key = read_dodge_hotkey_from_vfs();
        return key;
    }

    uint32_t resolve_dodge_hotkey_live()
    {
        return read_dodge_hotkey_from_vfs();
    }

    namespace {

        std::optional<uint32_t> gamepad_event_code(uint8_t key)
        {
            using GamepadKey = RE::BSWin32GamepadDevice::Keys::Key;
            switch (key) {
            case 0:
                return static_cast<uint32_t>(GamepadKey::kUp);
            case 1:
                return static_cast<uint32_t>(GamepadKey::kDown);
            case 2:
                return static_cast<uint32_t>(GamepadKey::kLeft);
            case 3:
                return static_cast<uint32_t>(GamepadKey::kRight);
            case 4:
                return static_cast<uint32_t>(GamepadKey::kStart);
            case 5:
                return static_cast<uint32_t>(GamepadKey::kBack);
            case 6:
                return static_cast<uint32_t>(GamepadKey::kLeftThumb);
            case 7:
                return static_cast<uint32_t>(GamepadKey::kRightThumb);
            case 8:
                return static_cast<uint32_t>(GamepadKey::kLeftShoulder);
            case 9:
                return static_cast<uint32_t>(GamepadKey::kRightShoulder);
            case 10:
                return static_cast<uint32_t>(GamepadKey::kA);
            case 11:
                return static_cast<uint32_t>(GamepadKey::kB);
            case 12:
                return static_cast<uint32_t>(GamepadKey::kX);
            case 13:
                return static_cast<uint32_t>(GamepadKey::kY);
            case 14:
                return static_cast<uint32_t>(GamepadKey::kLeftTrigger);
            case 15:
                return static_cast<uint32_t>(GamepadKey::kRightTrigger);
            default:
                return std::nullopt;
            }
        }

        std::optional<RE::INPUT_DEVICE> native_action_device(ActionInputDevice device)
        {
            switch (device) {
            case ActionInputDevice::keyboard:
                return RE::INPUT_DEVICE::kKeyboard;
            case ActionInputDevice::mouse:
                return RE::INPUT_DEVICE::kMouse;
            case ActionInputDevice::gamepad:
                return RE::INPUT_DEVICE::kGamepad;
            }
            return std::nullopt;
        }

    }  // namespace

    bool queue_action_tap(const ActionInput& input)
    {
        auto queue = RE::BSInputEventQueue::GetSingleton();
        if (!queue) {
            logger::error("SH2 action: BSInputEventQueue missing (device={}, scancode={})",
                static_cast<int>(input.device), input.dx_scancode);
            return false;
        }
        if (!input.is_bound() || input.dx_scancode > 281U) {
            logger::warn("SH2 action: invalid physical target (device={}, scancode={})",
                static_cast<int>(input.device), input.dx_scancode);
            return false;
        }

        const auto device = native_action_device(input.device);
        if (!device) {
            logger::warn("SH2 action: unsupported input device {}", static_cast<int>(input.device));
            return false;
        }

        const auto [decoded_device, decoded_code] =
            dx_scan_code_to_input(static_cast<int>(input.dx_scancode));
        if (decoded_device != *device) {
            logger::warn("SH2 action: device/scancode mismatch (device={}, scancode={})",
                static_cast<int>(input.device), input.dx_scancode);
            return false;
        }

        uint32_t event_code = decoded_code;
        if (*device == RE::INPUT_DEVICE::kGamepad) {
            const auto gamepad_code = gamepad_event_code(decoded_code);
            if (!gamepad_code) {
                logger::warn("SH2 action: unsupported gamepad scancode {}", input.dx_scancode);
                return false;
            }
            event_code = *gamepad_code;
        }

        const auto phases = keyboard_tap_phases();
        const auto queued_button_events = static_cast<size_t>(queue->buttonEventCount);
        const auto max_button_events = static_cast<size_t>(RE::BSInputEventQueue::MAX_BUTTON_EVENTS);
        if (queued_button_events > max_button_events ||
            phases.size() > max_button_events - queued_button_events) {
            logger::warn(
                "SH2 action: input queue has no room for tap (device={}, scancode={}, queued={}, required={}, capacity={})",
                static_cast<int>(*device), input.dx_scancode, queued_button_events, phases.size(),
                max_button_events);
            return false;
        }

        // AddButtonEvent uses the queue's embedded ButtonEvent storage. The old Create/Push pair
        // allocated two raw events, while ClearInputQueue only reset the queue links and counts;
        // using embedded slots removes that raw-event ownership ambiguity. Preflight both phases
        // above, then let CommonLib own the embedded slots for the lifetime of this input queue.
        for (size_t i = 0; i < phases.size(); ++i) {
            queue->AddButtonEvent(*device, static_cast<std::int32_t>(event_code),
                phases[i].value, phases[i].held_duration);
            logger::info(
                "SH2 action: queued {} (device={}, scancode={}, event_code={}, value={}, held={}, accepted=true)",
                phases[i].value > 0.0f ? "down" : "up", static_cast<int>(*device), input.dx_scancode,
                event_code, phases[i].value, phases[i].held_duration);
        }
        return true;
    }

    bool queue_keyboard_tap(uint32_t scancode)
    {
        return queue_action_tap(ActionInput{ ActionInputDevice::keyboard, scancode });
    }

    std::tuple<RE::INPUT_DEVICE, uint8_t> get_shout_key_and_device()
    {
        RE::INPUT_DEVICE dev{ RE::INPUT_DEVICE::kNone };

        auto controlmap = RE::ControlMap::GetSingleton();
        uint32_t shoutkey = 0U;
        if (controlmap) {
            shoutkey = controlmap->GetMappedKey("Shout", RE::INPUT_DEVICE::kKeyboard);
            if (shoutkey >= 255) {
                shoutkey = controlmap->GetMappedKey("Shout", RE::INPUT_DEVICE::kMouse);

                if (shoutkey >= 255) {
                    shoutkey = controlmap->GetMappedKey("Shout", RE::INPUT_DEVICE::kGamepad);

                    if (shoutkey >= 255) {
                        shoutkey = 0;
                    }
                    else {
                        dev = RE::INPUT_DEVICE::kGamepad;
                    }
                }
                else {
                    dev = RE::INPUT_DEVICE::kMouse;
                }
            }
            else {
                dev = RE::INPUT_DEVICE::kKeyboard;
            }
        }
        return std::make_tuple(dev, static_cast<uint8_t>(shoutkey));
    }

    int get_shout_key_dxcode()
    {
       auto [dev, code] = get_shout_key_and_device();
       return input_to_dx_scancode(dev, code);
    }

    bool allowed_to_instantcast(RE::FormID skill)
    {
        auto pc = RE::PlayerCharacter::GetSingleton();
        if (!pc || !pc->Is3DLoaded()) {
            return false;
        }

        if (GameData::is_skill_on_cd(skill)) {
            return false;
        }

        const auto* control_map = RE::ControlMap::GetSingleton();
        if (!control_map || !control_map->IsMovementControlsEnabled()) // || !control_map->IsFightingControlsEnabled()) this is not working in 1170 and probably not needed anyway
        {
            return false;
        }

        if (pc->GetOccupiedFurniture()) {
            return false;
        }

        return !pc->IsOnMount();
    }

    bool allowed_to_cast(RE::FormID skill, bool allow_sprint)
    {
        auto pc = RE::PlayerCharacter::GetSingleton();
        if (allowed_to_instantcast(skill) && pc) {
            auto as = pc->AsActorState();

            bool inJumpState{ false };
            //bool bowDrawn{ false };
            pc->GetGraphVariableBool("bInJumpState"sv, inJumpState);
            //pc->GetGraphVariableBool("bInJumpState"sv, bowDrawn); //TODO look for bow anim

            //Check if player currently is casting, also check staffs
            bool isCasting = pc->IsCasting(nullptr);

            const bool sprinting = !allow_sprint && as->IsSprinting();
            const bool swimming = as->IsSwimming();
            if (isCasting || sprinting || swimming || inJumpState) { //|| bowDrawn);
                // Ticket 46, spike observation 1: this branch used to refuse every press in
                // silence, and a MagicCaster left charging by an interrupted cast reads as
                // IsCasting for as long as it stays stuck -- twenty minutes of live time spent
                // on a refusal that names itself in one line.
                logger::debug("SH2 cast: refused, casting={} sprinting={} swimming={} jumping={}",
                    isCasting, sprinting, swimming, inJumpState);
                return false;
            }
            return true;
        }
        else return false;
    }

    RE::TESForm* get_current_selected_spell_in_menu()
    {
        RE::UI* ui = RE::UI::GetSingleton();
        if (!ui) return nullptr;
        if (!SpellHotbar::GameData::hasFavMenuSlotBinding()) {
            // code taken from Wheeler
            auto* magMenu = static_cast<RE::MagicMenu*>(ui->GetMenu(RE::MagicMenu::MENU_NAME).get());
            auto* invMenu = static_cast<RE::InventoryMenu*>(ui->GetMenu(RE::InventoryMenu::MENU_NAME).get());
            //bool valid_tab = false;
            
            /*if (invMenu) {
                valid_tab = RenderManager::current_inv_menu_tab_valid_for_hotbar();
            };*/
            if (!magMenu && !invMenu) return nullptr; //&& !valid_tab

            if (magMenu) {
                RE::GFxValue selection;
                magMenu->uiMovie->GetVariable(&selection, "_root.Menu_mc.inventoryLists.itemList.selectedEntry.formId");
                if (selection.GetType() == RE::GFxValue::ValueType::kNumber) {
                    RE::FormID formID = static_cast<std::uint32_t>(selection.GetNumber());
                    return RE::TESForm::LookupByID(formID);
                }
            }
            else if (invMenu) {
                //invMenu->uiMovie->GetVariable(&selection, "_root.Menu_mc.inventoryLists.itemList.selectedEntry.formId");
                RE::ItemList* item_list = invMenu->GetRuntimeData().itemList;
                if (item_list != nullptr) {
                    RE::ItemList::Item* item = item_list->GetSelectedItem();
                    if (item != nullptr && item->data.objDesc != nullptr) {
#undef GetObject // undefine stupid windows definition so GetObject() can be called
                        RE::TESBoundObject* obj = item->data.objDesc->GetObject();
#ifdef UNICODE //redefine it
#define GetObject  GetObjectW
#else
#define GetObject  GetObjectA
#endif // !UNICODE
                        if (obj != nullptr) {
                            RE::FormID formID = obj->GetFormID();
                            return RE::TESForm::LookupByID(formID);
                        }
                    }
                }
            }
        }
        else {
            auto* favMenu = static_cast<RE::FavoritesMenu*>(ui->GetMenu(RE::FavoritesMenu::MENU_NAME).get());
            if (!favMenu) return nullptr;

            auto& root = favMenu->GetRuntimeData().root;

            if (root.GetType() == RE::GFxValue::ValueType::kDisplayObject && root.HasMember("itemList")) {
                RE::GFxValue itemList;
                root.GetMember("itemList", &itemList);

                if (itemList.GetType() == RE::GFxValue::ValueType::kDisplayObject && itemList.HasMember("selectedEntry")) {
                    RE::GFxValue selectedEntry;
                    itemList.GetMember("selectedEntry", &selectedEntry);

                    if (selectedEntry.GetType() == RE::GFxValue::ValueType::kObject && selectedEntry.HasMember("formId")) {
                        RE::GFxValue formId;
                        selectedEntry.GetMember("formId", &formId);

                        if (formId.GetType() == RE::GFxValue::ValueType::kNumber) {
                            RE::FormID formID = static_cast<std::uint32_t>(formId.GetNumber());
                            return RE::TESForm::LookupByID(formID);
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    bool slot_spell(RE::TESForm* form, size_t index)
    {
        if (form != nullptr)
        {
            if (!SpellHotbar::SlottedSkill::is_bindable_form(form->GetFormID())) {
                // Non-castable form (weapon, armor, ingredient, book, ...): refuse at the bind
                // seam so storage, the save and the renderer never see an unusable slot.
                logger::debug("Refused to bind form {:08X}: form type {} is not castable",
                              form->GetFormID(), static_cast<uint32_t>(form->GetFormType()));
                RE::PlaySound(sound_UIMenuCancel);
                return false;
            }

            SpellHotbar::Storage::menu_slot_type slot_type{ Storage::menu_slot_type::magic_menu };
            if (GameData::isVampireLord()) {
                slot_type = Storage::menu_slot_type::vampire_lord;
            }
            else if (GameData::isWerewolf()) {
                slot_type = Storage::menu_slot_type::werewolf;
            }
            else if (SpellHotbar::GameData::isCustomTransform()) {
                auto casttype = SpellHotbar::GameData::getCustomTransformCasttype();
                if (casttype == SpellHotbar::GameData::custom_transform_spell_type::fav_menu ||
                    casttype == SpellHotbar::GameData::custom_transform_spell_type::fav_menu_switch) {
                    slot_type = SpellHotbar::Storage::menu_slot_type::custom_favmenu;
                }
            }
            return SpellHotbar::Storage::slotSpell(form->GetFormID(), index, slot_type);
        }
        return false;
    }

    bool in_binding_menu()
    {
        auto ui = RE::UI::GetSingleton();
        if (!ui) {
            return false;
        }
        const auto* control_map = RE::ControlMap::GetSingleton();
        if (control_map && (control_map->textEntryCount > 0))
        {
            return false;
        }

        if (GameData::hasFavMenuSlotBinding())
        {
            return ui->GetMenu(RE::FavoritesMenu::MENU_NAME).get() != nullptr;
        }
        else 
        {
            auto* magMenu = ui->GetMenu(RE::MagicMenu::MENU_NAME).get();
            if (magMenu) {
                return true;
            }
            else {
                return RenderManager::current_inv_menu_tab_valid_for_hotbar();
            }
        }
    }
}
