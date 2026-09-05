cmake_minimum_required(VERSION 3.21)

project(SpellHotbar2 VERSION 1.0.0 LANGUAGES CXX)

include(CTest)

# Optional MO2 mod folder for local development. A normal configure never deploys.
set(OUTPUT_FOLDER "" CACHE PATH "Optional mod folder that receives the built SKSE plugin")

# The producer owns this ABI contract. Keep the vendored snapshot byte-equivalent after newline
# normalization; refreshing it requires an explicit producer commit and hash change here.
set(SHOUTMCO_CAST_INTENT_CANONICAL_COMMIT "e952394ba0392cccfcb6dc2ba0cb651e2b048024")
set(SHOUTMCO_CAST_INTENT_CANONICAL_SHA256 "30f6e3a4698f666a4695dd78ea4ab43b17b31709da3dc5deb27981f7795cc9cc")
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/src/extern/ShoutMCO_CastIntent.h" SHOUTMCO_CAST_INTENT_HEADER)
string(REPLACE "\r\n" "\n" SHOUTMCO_CAST_INTENT_HEADER "${SHOUTMCO_CAST_INTENT_HEADER}")
string(REPLACE "\r" "\n" SHOUTMCO_CAST_INTENT_HEADER "${SHOUTMCO_CAST_INTENT_HEADER}")
string(SHA256 SHOUTMCO_CAST_INTENT_SHA256 "${SHOUTMCO_CAST_INTENT_HEADER}")
if(NOT SHOUTMCO_CAST_INTENT_SHA256 STREQUAL SHOUTMCO_CAST_INTENT_CANONICAL_SHA256)
    message(FATAL_ERROR
        "src/extern/ShoutMCO_CastIntent.h differs from the canonical ShoutMCO contract at "
        "${SHOUTMCO_CAST_INTENT_CANONICAL_COMMIT}. Refresh it from the producer and update the pinned hash.")
endif()


find_package(CommonLibSSE CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

set(source_files src/plugin.cpp
                 src/logger/logger.h
                 src/logger/logger.cpp
                 src/rendering/render_manager.h
                 src/rendering/render_manager.cpp
                 src/rendering/texture_registry.h
                 src/smf/imgui_mcp_compat.h
                 src/smf/smf_guest.h
                 src/smf/smf_guest.cpp
                 src/mcp/bind_capture.h
                 src/mcp/bind_capture.cpp
                 src/mcp/mcp_preset_name.h
                 src/mcp/mcp_pages.h
                 src/mcp/mcp_pages.cpp
                 src/papyrus_extensions/papyrus_functions.h
                 src/papyrus_extensions/papyrus_functions.cpp
                 src/storage/storage.h
                 src/storage/storage.cpp
                 src/storage/runtime_state_reset.h
                 src/game_data/game_data.h
                 src/game_data/game_data.cpp
                 src/bar/hotbar.h
                 src/bar/hotbar.cpp
                 src/bar/hotbar_serialization.h
                 src/bar/hotbars.h
                 src/bar/hotbars.cpp
                 src/bar/oblivion_bar.h
                 src/bar/oblivion_bar.cpp
                 "src/input/input.h"
                 "src/input/input.cpp"
                 "src/input/action_inject.h"
                 "src/input/input_event_adapter.h"
                 "src/input/keybinds.h"
                 "src/input/keybinds.cpp"
                 "src/input/modes.h"
                 "src/input/modes.cpp"
                 src/rendering/texture_csv_loader.h
                 src/rendering/texture_csv_loader.cpp
                 src/events/eventlistener.h
                 src/events/eventlistener.cpp
                 src/events/animationeventhook.h
                 src/events/animationeventhook.cpp
                 src/eventS/gameloop_hook.h
                 src/eventS/gameloop_hook.cpp
                 src/lifecycle/first_run_policy.h
                 src/lifecycle/lifecycle.h
                 src/lifecycle/lifecycle.cpp
                 src/game_data/spell_data_csv_loader.h
                 src/game_data/spell_data_csv_loader.cpp
                 src/game_data/spell_casteffect_csv_loader.h
                 src/game_data/spell_casteffect_csv_loader.cpp
                 src/game_data/csv_loader.h
                 src/game_data/custom_transform_csv_loader.h
                 src/game_data/custom_transform_csv_loader.cpp
                 src/game_data/animation_data_csv_loader.h
                 src/game_data/animation_data_csv_loader.cpp
                 src/game_data/art_definition.h
                 src/game_data/art_definition.cpp
                 src/game_data/action_definition.h
                 src/game_data/action_definition.cpp
                 src/game_data/art_icon_resolve.h
                 src/game_data/art_icon_resolve.cpp
                 src/game_data/custom_ability_config.h
                 src/game_data/custom_ability_config.cpp
                 src/game_data/art_data_csv_loader.h
                 src/game_data/art_data_csv_loader.cpp
                 src/casts/casting_controller.h
                 src/casts/casting_controller.cpp
                 src/casts/voice_cast_driver.h
                 src/casts/voice_cast_driver.cpp
                 src/casts/msco_cast_driver.h
                 src/casts/msco_cast_driver.cpp
                 src/casts/art_driver.h
                 src/casts/art_driver.cpp
                 src/casts/clip_translation.h
                 src/casts/clip_translation_driver.h
                 src/casts/clip_translation_driver.cpp
                 src/casts/combo_cache.h
                 src/casts/cast_intent.h
                 src/casts/cast_intent.cpp
                 src/extern/ShoutMCO_CastIntent.h
                 src/game_data/keynames_csv_loader.h
                 src/game_data/keynames_csv_loader.cpp
                 src/rendering/spell_editor.h
                 src/rendering/spell_editor.cpp
                 "src/input/keycode_helper.h"
                 src/rendering/spell_edit_dialog.h
                 src/rendering/spell_edit_dialog.cpp
                 src/game_data/user_custom_spelldata.h
                 src/game_data/user_custom_spelldata.cpp
                 src/storage/user_data_io.h
                 src/storage/user_data_io.cpp
                 src/game_data/cast_anim_ids.h
                 src/game_data/spell_cast_data.h
                 src/game_data/spell_cast_data.cpp
                 src/rendering/potion_editor.h
                 src/rendering/potion_editor.cpp
                 src/game_data/user_custom_entry.h
                 src/game_data/user_custom_entry.cpp
                 src/rendering/icon_edit_dialog.h
                 src/rendering/icon_edit_dialog.cpp
                 src/rendering/art_icon_edit_dialog.h
                 src/rendering/art_icon_edit_dialog.cpp
                 src/rendering/ability_editor.h
                 src/rendering/ability_editor.cpp
                 src/rendering/action_editor.h
                 src/rendering/action_editor.cpp
                 src/game_data/art_pack_gen.h
                 src/game_data/art_pack_gen.cpp
                 src/game_data/custom_ability_runtime.h
                 src/game_data/custom_ability_runtime.cpp
                 src/casts/spell_proc.h
                 src/casts/spell_proc.cpp
                 src/rendering/bar_dragging_config_window.h
                 src/rendering/bar_dragging_config_window.cpp
                 src/rendering/advanced_bind_menu.h
                 src/rendering/advanced_bind_menu.cpp
                 src/rendering/bind_drop.h
                 src/rendering/gui_tab_button.h
                 src/rendering/gui_tab_button.cpp
                 src/game_data/localization.h
                 src/game_data/localization.cpp
)

if(BUILD_TESTING)
add_executable(combo_cache_test src/casts/combo_cache_test.cpp)
target_compile_features(combo_cache_test PRIVATE cxx_std_23)
target_include_directories(combo_cache_test PRIVATE src/casts)

add_executable(clip_translation_test src/casts/clip_translation_test.cpp)
target_compile_features(clip_translation_test PRIVATE cxx_std_23)
target_include_directories(clip_translation_test PRIVATE src/casts)

add_executable(art_pack_gen_test src/game_data/art_pack_gen_test.cpp src/game_data/art_definition.cpp src/game_data/art_icon_resolve.cpp src/game_data/custom_ability_config.cpp)
target_compile_features(art_pack_gen_test PRIVATE cxx_std_23)
target_include_directories(art_pack_gen_test PRIVATE src/game_data)

add_executable(art_data_test src/game_data/art_data_test.cpp src/game_data/art_definition.cpp src/game_data/art_icon_resolve.cpp src/game_data/custom_ability_config.cpp)
target_compile_features(art_data_test PRIVATE cxx_std_23)
target_include_directories(art_data_test PRIVATE src/game_data)

add_executable(action_data_test src/game_data/action_data_test.cpp src/game_data/action_definition.cpp)
target_compile_features(action_data_test PRIVATE cxx_std_23)
target_include_directories(action_data_test PRIVATE src/game_data)

add_executable(bind_capture_test src/mcp/bind_capture_test.cpp)
target_compile_features(bind_capture_test PRIVATE cxx_std_23)
target_include_directories(bind_capture_test PRIVATE src/mcp src/game_data)

add_executable(cast_anim_ids_test src/game_data/cast_anim_ids_test.cpp)
target_compile_features(cast_anim_ids_test PRIVATE cxx_std_23)
target_include_directories(cast_anim_ids_test PRIVATE src/game_data)

add_executable(art_bind_record_test src/storage/art_bind_record_test.cpp)
target_compile_features(art_bind_record_test PRIVATE cxx_std_23)
target_include_directories(art_bind_record_test PRIVATE src/storage)

add_executable(bind_drop_test src/rendering/bind_drop_test.cpp)
target_compile_features(bind_drop_test PRIVATE cxx_std_23)
target_include_directories(bind_drop_test PRIVATE src/rendering)

add_executable(hotbar_serialization_test src/bar/hotbar_serialization_test.cpp)
target_compile_features(hotbar_serialization_test PRIVATE cxx_std_23)
target_include_directories(hotbar_serialization_test PRIVATE src/bar)

add_executable(texture_registry_test src/rendering/texture_registry_test.cpp)
target_compile_features(texture_registry_test PRIVATE cxx_std_23)
target_include_directories(texture_registry_test PRIVATE src/rendering)

add_executable(action_inject_test src/input/action_inject_test.cpp)
target_compile_features(action_inject_test PRIVATE cxx_std_23)
target_include_directories(action_inject_test PRIVATE src/input)

add_test(NAME combo_cache_test COMMAND combo_cache_test)
add_test(NAME clip_translation_test COMMAND clip_translation_test)
add_test(NAME art_pack_gen_test COMMAND art_pack_gen_test)
add_test(NAME art_data_test COMMAND art_data_test)
add_test(NAME cast_anim_ids_test COMMAND cast_anim_ids_test)
add_test(NAME art_bind_record_test COMMAND art_bind_record_test)
add_test(NAME bind_drop_test COMMAND bind_drop_test)
add_test(NAME hotbar_serialization_test COMMAND hotbar_serialization_test)
add_test(NAME texture_registry_test COMMAND texture_registry_test)
add_test(NAME action_inject_test COMMAND action_inject_test)
add_test(NAME action_data_test COMMAND action_data_test)
add_test(NAME bind_capture_test COMMAND bind_capture_test)
endif()

add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${source_files})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

target_include_directories(${PROJECT_NAME} PRIVATE "src")

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Automatically copy to the explicitly requested output folder.
if(OUTPUT_FOLDER)
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")

    message(STATUS "SKSE plugin output folder: ${DLL_FOLDER}")

    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        VERBATIM
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_custom_command(
            TARGET "${PROJECT_NAME}"
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM
        )
    endif()
endif()
