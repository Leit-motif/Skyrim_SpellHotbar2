#include "lifecycle.h"

#include "first_run_policy.h"
#include "../game_data/game_data.h"
#include "../input/input.h"
#include "../logger/logger.h"
#include "../storage/storage.h"
#include "../storage/user_data_io.h"

namespace SpellHotbar::Lifecycle {
    namespace {
        bool initialized_this_game{ false };

        RE::SpellItem* power_for_type(int type)
        {
            switch (type) {
            case 0:
                return GameData::spellhotbar_unbind_slot;
            case 1:
                return GameData::spellhotbar_toggle_dualcast;
            default:
                return nullptr;
            }
        }

        bool has_papyrus_static_function(
            RE::BSScript::IVirtualMachine* vm,
            const RE::BSFixedString& class_name,
            const RE::BSFixedString& function_name)
        {
            RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo> type_info;
            if (vm == nullptr || !vm->GetScriptObjectType(class_name, type_info) || !type_info ||
                type_info->linkedValid != RE::BSScript::ObjectTypeInfo::LinkValidState::kLinkedValid) {
                return false;
            }

            const auto* functions = type_info->GetGlobalFuncIter();
            for (std::uint32_t i = 0; i < type_info->GetNumGlobalFuncs(); ++i) {
                const auto& function = functions[i].func;
                if (function && function->GetIsStatic() && function->GetName() == function_name) {
                    return true;
                }
            }
            return false;
        }

        void remove_legacy_battlemage_power()
        {
            auto* player = RE::PlayerCharacter::GetSingleton();
            auto* legacy_power = GameData::spellhotbar_battlemage_open_perks_power;
            if (player != nullptr && legacy_power != nullptr && player->HasSpell(legacy_power)) {
                player->RemoveSpell(legacy_power);
                logger::info("Removed legacy BattleMage opener power; the tree is available from the Mod Control Panel");
            }
        }

        void run_first_initialization()
        {
            if (initialized_this_game) {
                return;
            }

            auto* player = RE::PlayerCharacter::GetSingleton();
            if (player == nullptr) {
                logger::error("SpellHotbar2 first initialization deferred: player is unavailable");
                return;
            }

            for (int type = 0; type < 2; ++type) {
                auto* power = power_for_type(type);
                if (power != nullptr && !player->HasSpell(power)) {
                    player->AddSpell(power);
                }
            }

            const bool profile_loaded = Storage::IO::load_preset("auto_profile.json", false);
            const std::filesystem::path auto_edits{
                "Data/SKSE/Plugins/SpellHotbar/icon_edits/auto_edits.json"
            };
            const bool edits_loaded = std::filesystem::exists(auto_edits) &&
                                      GameData::load_icon_edits_from_json(auto_edits.string());

            initialized_this_game = true;
            logger::info(
                "SpellHotbar2 first initialization complete (auto profile: {}, auto icon edits: {})",
                profile_loaded,
                edits_loaded);
        }
    }

    void reset()
    {
        initialized_this_game = false;
    }

    void on_new_game()
    {
        remove_legacy_battlemage_power();
        if (should_run_first_initialization(true, Storage::loaded_existing_settings())) {
            run_first_initialization();
        }
    }

    void on_post_load_game()
    {
        remove_legacy_battlemage_power();
        if (should_run_first_initialization(false, Storage::loaded_existing_settings())) {
            run_first_initialization();
        }
    }

    bool player_has_power(int type)
    {
        auto* player = RE::PlayerCharacter::GetSingleton();
        auto* power = power_for_type(type);
        return player != nullptr && power != nullptr && player->HasSpell(power);
    }

    bool toggle_player_power(int type)
    {
        auto* player = RE::PlayerCharacter::GetSingleton();
        auto* power = power_for_type(type);
        if (player == nullptr || power == nullptr) {
            return false;
        }

        if (player->HasSpell(power)) {
            player->RemoveSpell(power);
        } else {
            player->AddSpell(power);
        }
        return player->HasSpell(power);
    }

    bool open_battlemage_tree()
    {
        if (GameData::spellhotbar_battlemage_open_perks_power == nullptr) {
            logger::warn("Cannot open BattleMage tree: SpellHotbar_BattleMage.esp is not loaded");
            return false;
        }

        auto* skyrim_vm = RE::SkyrimVM::GetSingleton();
        if (skyrim_vm == nullptr || skyrim_vm->impl == nullptr) {
            logger::error("Cannot open BattleMage tree: Papyrus VM is unavailable");
            return false;
        }

        static const RE::BSFixedString class_name{ "CustomSkills" };
        static const RE::BSFixedString function_name{ "OpenCustomSkillMenu" };
        if (!has_papyrus_static_function(skyrim_vm->impl.get(), class_name, function_name)) {
            logger::error("Cannot open BattleMage tree: CustomSkills.OpenCustomSkillMenu is unavailable");
            return false;
        }

        auto* args = RE::MakeFunctionArguments(std::string{ "SpellHotbar_Battlemage" });
        RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> result;
        const bool dispatched = skyrim_vm->impl->DispatchStaticCall(class_name, function_name, args, result);
        if (!dispatched) {
            logger::error("Cannot open BattleMage tree: CustomSkills.OpenCustomSkillMenu is unavailable");
        }
        return dispatched;
    }

    bool process_spell_cast(RE::FormID spell)
    {
        auto* toggle_spell = GameData::spellhotbar_toggle_dualcast;
        auto* toggle_global = GameData::global_spellhotbar_use_dual_casting;
        if (toggle_spell == nullptr || toggle_global == nullptr || spell != toggle_spell->GetFormID()) {
            return false;
        }

        const bool enabled = toggle_global->value == 0.0F;
        toggle_global->value = enabled ? 1.0F : 0.0F;
        RE::PlaySound(enabled ? "MAGCloakIn" : "MAGCloakOut");
        return true;
    }
}
