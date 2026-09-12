cmake_minimum_required(VERSION 3.21)

project(SpellHotbar2 VERSION 2.0.10 LANGUAGES CXX)

#output folder to copy dll after building, e.g. "F:/Skyrim Dev/ADT/mods/Spell Hotbar 2".
#Empty by default so a plain configure never deploys anywhere; pass -DOUTPUT_FOLDER=... to enable.
set(OUTPUT_FOLDER "" CACHE PATH "Optional mod folder that receives the built SKSE plugin")


# FLICK's guest header, vendored from github.com/Fuzzlesz/FUCK. It is still moving upstream --
# API 4 landed days before this pin -- so track a commit, not HEAD, and let FUCK::Connect()'s own
# version check fail closed against an older installed host.
set(FLICK_API_CANONICAL_COMMIT "a123c11629da3b86ffd1c8e15081d5d641001ed3")
set(FLICK_API_CANONICAL_SHA256 "9dc5f80490c9067d2cff98c9104b5e2df15b3af6df421546556ed726166ccefa")
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/third_party/flick/FUCK_API.h" FLICK_API_HEADER)
# Normalize line endings before hashing, written as CMake escapes rather than literal newlines
# so no editor or checkout can rewrite them.
string(REPLACE "\r\n" "\n" FLICK_API_HEADER "${FLICK_API_HEADER}")
string(REPLACE "\r" "\n" FLICK_API_HEADER "${FLICK_API_HEADER}")
string(SHA256 FLICK_API_SHA256 "${FLICK_API_HEADER}")
if(NOT FLICK_API_SHA256 STREQUAL FLICK_API_CANONICAL_SHA256)
    message(FATAL_ERROR
        "third_party/flick/FUCK_API.h differs from the pinned FLICK header at "
        "${FLICK_API_CANONICAL_COMMIT}. Refresh it from upstream and update the pinned hash.")
endif()


find_package(imgui CONFIG REQUIRED)
find_package(CommonLibSSE CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)

set(source_files src/plugin.cpp
                 src/logger/logger.h
                 src/logger/logger.cpp
                 src/rendering/render_manager.h
                 src/rendering/render_manager.cpp
                 src/rendering/texture_loader.h
                 src/rendering/texture_loader.cpp
                 src/papyrus_extensions/papyrus_functions.h
                 src/papyrus_extensions/papyrus_functions.cpp
                 src/storage/storage.h
                 src/storage/storage.cpp
                 src/game_data/game_data.h
                 src/game_data/game_data.cpp
                 src/bar/hotbar.h
                 src/bar/hotbar.cpp
                 src/bar/hotbars.h
                 src/bar/hotbars.cpp
                 src/bar/oblivion_bar.h
                 src/bar/oblivion_bar.cpp
                 "src/input/input.h"
                 "src/input/input.cpp"
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
                 src/game_data/spell_data_csv_loader.h
                 src/game_data/spell_data_csv_loader.cpp
                 src/game_data/spell_casteffect_csv_loader.h
                 src/game_data/spell_casteffect_csv_loader.cpp
                 src/game_data/csv_loader.h
                 src/game_data/custom_transform_csv_loader.h
                 src/game_data/custom_transform_csv_loader.cpp
                 src/game_data/animation_data_csv_loader.h
                 src/game_data/animation_data_csv_loader.cpp
                 src/casts/casting_controller.h
                 src/casts/casting_controller.cpp
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
                 src/game_data/spell_cast_data.h
                 src/game_data/spell_cast_data.cpp
                 src/rendering/potion_editor.h
                 src/rendering/potion_editor.cpp
                 src/game_data/user_custom_entry.h
                 src/game_data/user_custom_entry.cpp
                 src/rendering/icon_edit_dialog.h
                 src/rendering/icon_edit_dialog.cpp
                 src/casts/spell_proc.h
                 src/casts/spell_proc.cpp
                 src/rendering/bar_dragging_config_window.h
                 src/rendering/bar_dragging_config_window.cpp
                 src/rendering/advanced_bind_menu.h
                 src/rendering/advanced_bind_menu.cpp
                 src/rendering/gui_tab_button.h
                 src/rendering/gui_tab_button.cpp
                 src/game_data/localization.h
                 src/game_data/localization.cpp
)

add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${source_files})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

target_include_directories(${PROJECT_NAME} PRIVATE "src")

target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXTK)

#automatically copy to output folder if set
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
