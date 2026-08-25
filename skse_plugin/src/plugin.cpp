#include "logger/logger.h"
#include "papyrus_extensions/papyrus_functions.h"
#include "rendering/render_manager.h"
#include "storage/storage.h"
#include "game_data/game_data.h"
#include "game_data/art_pack_gen.h"
#include "bar/hotbars.h"
#include "input/input.h"
#include "events/eventlistener.h"
#include "events/animationeventhook.h"
#include "events/gameloop_hook.h"
#include "casts/cast_intent.h"
#include "casts/casting_controller.h"
#include "casts/clip_translation_driver.h"
#include "casts/msco_cast_driver.h"


constexpr uint32_t serializazion_id = 0xB8498471; //random generated 4byte

SKSEPluginLoad(const SKSE::LoadInterface * skse)
{
    SKSE::Init(skse);
    SpellHotbar::SetupLogger();
    logger::trace("SpellHotbar2 logger setup!");

    SpellHotbar::Bars::init();

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
        //logger::trace("Received SKSE Message {}", message->type);
        if (message->type == SKSE::MessagingInterface::kPostLoad) {
            //Every SKSE plugin DLL is loaded by now, so ShoutMCO's optional cast-intent export
            //can be resolved. Absent or incompatible is normal and costs nothing (ADR 0005).
            SpellHotbar::casts::CastIntent::negotiate();
            //Write the pointer-art OAR pack HERE, not at kDataLoaded: OAR parses config.json
            //when it builds its replacer mods, which is after this and before our data load, so
            //generating any later means the arts only appear on the next launch (ADR-0016).
            //Pure filesystem work, so it needs nothing the game has not set up yet.
            SpellHotbar::ArtPackGen::generate_and_cache();
        }
        else if (message->type == SKSE::MessagingInterface::kDataLoaded) {
            SpellHotbar::GameData::onDataLoad();
            SpellHotbar::casts::MscoCastDriver::load_charge_curve();
            logger::info("SpellHotbar2 GameData loaded!");
        }
        else if (message->type == SKSE::MessagingInterface::kPreLoadGame ||
                 message->type == SKSE::MessagingInterface::kNewGame) {
            SpellHotbar::casts::CastingController::drop_live_cast();
        }
     });

    //Install animationeventhook -- needed again as of ADR 0004: the cast's commitment point is
    //the clip's own `MLh/MRh_SpellFire_Event`, and this hook is how we hear it.
    SpellHotbar::events::install();
    SpellHotbar::casts::ClipTranslationDriver::install();
    SpellHotbar::events::GameLoopHook::hook();

    auto event_listener = SpellHotbar::events::EventListener::GetSingleton();
    SKSE::GetActionEventSource()->AddEventSink(event_listener);
    auto eventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
    eventSourceHolder->AddEventSink<RE::TESSpellCastEvent>(event_listener);
    eventSourceHolder->AddEventSink<RE::TESHitEvent>(event_listener);
    eventSourceHolder->AddEventSink<RE::TESEquipEvent>(event_listener);
    //eventSourceHolder->AddEventSink<RE::TESPlayerBowShotEvent>(event_listener);

    RE::CriticalHit::GetEventSource()->AddEventSink(event_listener);

    //SKSE::GetActionEventSource()->AddEventSink(event_listener);

    SKSE::GetPapyrusInterface()->Register(SpellHotbar::register_papyrus_functions);
    SpellHotbar::RenderManager::install();

    SKSE::AllocTrampoline(1 << 4);
    SpellHotbar::Input::install_hook();
    logger::info("SpellHotbar2 Papyrus DLL functions registered!");

    auto serialization = SKSE::GetSerializationInterface();
    serialization->SetUniqueID(serializazion_id);
    serialization->SetSaveCallback(SpellHotbar::Storage::SaveCallback);
    serialization->SetLoadCallback(SpellHotbar::Storage::LoadCallback);
    logger::info("SpellHotbar2 serialization registered!");

    return true;
}
