#include "smf_guest.h"

#include "../input/input.h"
#include "../logger/logger.h"
#include "../rendering/render_manager.h"

#include <array>

namespace SpellHotbar::SmfGuest {
    namespace {
        using WindowInterface = SKSEMenuFramework::Model::WindowInterface;

        SKSEMenuFramework::Model::HudElement* hud_registration{nullptr};
        SKSEMenuFramework::Model::InputEvent* input_registration{nullptr};
        std::array<WindowInterface*, static_cast<std::size_t>(Window::count)> windows{};
        bool ready{false};
        bool attempted{false};

        constexpr std::size_t index(Window window)
        {
            return static_cast<std::size_t>(window);
        }

        void __stdcall render_hud()
        {
            RenderManager::render_hud();
        }

        void __stdcall render_spell_editor()
        {
            RenderManager::render_spell_editor_window();
        }

        void __stdcall render_potion_editor()
        {
            RenderManager::render_potion_editor_window();
        }

        void __stdcall render_bar_dragging()
        {
            RenderManager::render_bar_drag_window();
        }

        void __stdcall render_bind_menu()
        {
            RenderManager::render_bind_menu_window();
        }

        bool has_required_exports(HMODULE module)
        {
            if (!GetProcAddress(module, "RegisterHudElement")) {
                logger::error("SKSE Menu Framework is missing required export 'RegisterHudElement'");
                return false;
            }
            if (!GetProcAddress(module, "AddWindow")) {
                logger::error("SKSE Menu Framework is missing required export 'AddWindow'");
                return false;
            }
            if (!GetProcAddress(module, "RegisterInpoutEvent")) {
                logger::error("SKSE Menu Framework is missing required export 'RegisterInpoutEvent'");
                return false;
            }

            constexpr float required_version = 3.14F;
            const auto get_version = reinterpret_cast<float (*)()>(
                GetProcAddress(module, "GetMenuFrameworkVersion"));
            if (!get_version) {
                logger::error("SKSE Menu Framework is missing required export 'GetMenuFrameworkVersion'");
                return false;
            }
            const auto installed_version = get_version();
            if (installed_version < required_version) {
                logger::error(
                    "SKSE Menu Framework {} is incompatible; Spell Hotbar 2 requires {} or newer",
                    installed_version,
                    required_version);
                return false;
            }

            constexpr std::array required_exports{
                "LoadTexture",
                "DisposeTexture",
                "igGetIO",
                "igGetStyle",
                "igGetMainViewport",
                "igGetWindowDrawList",
                "igGetForegroundDrawList_Nil",
                "igBegin",
                "igEnd",
                "igImage",
                "ImDrawList_AddImage",
                "ImDrawList_AddText_Vec2",
                "ImDrawList_AddText_FontPtr",
                "ImGuiListClipper_ImGuiListClipper",
                "ImGuiListClipper_destroy",
                "ImGuiListClipper_Begin",
                "ImGuiListClipper_Step"};

            for (const auto* export_name : required_exports) {
                if (!GetProcAddress(module, export_name)) {
                    logger::error("SKSE Menu Framework is missing required export '{}'", export_name);
                    return false;
                }
            }
            return true;
        }
    }

    bool install()
    {
        if (attempted) {
            return ready;
        }
        attempted = true;

        const auto module = GetMenuFrameworkModule();
        if (!module) {
            logger::error("SKSE Menu Framework is required; Spell Hotbar 2 UI will remain disabled");
            return false;
        }
        if (!has_required_exports(module)) {
            logger::error("SKSE Menu Framework API is incompatible; Spell Hotbar 2 UI will remain disabled");
            return false;
        }

        hud_registration = SKSEMenuFramework::AddHudElement(render_hud);
        input_registration = SKSEMenuFramework::AddInputEvent(Input::process_event);
        windows[index(Window::spell_editor)] = SKSEMenuFramework::AddWindow(render_spell_editor);
        windows[index(Window::potion_editor)] = SKSEMenuFramework::AddWindow(render_potion_editor);
        windows[index(Window::bar_dragging)] = SKSEMenuFramework::AddWindow(render_bar_dragging);
        windows[index(Window::bind_menu)] = SKSEMenuFramework::AddWindow(render_bind_menu);

        ready = hud_registration != nullptr && input_registration != nullptr;
        for (auto* window : windows) {
            ready = ready && window != nullptr;
            if (window) {
                window->IsOpen.store(false);
                window->BlockUserInput.store(true);
            }
        }

        if (!ready) {
            close_all_windows();
            logger::error("SKSE Menu Framework rejected one or more Spell Hotbar 2 registrations; UI will remain disabled");
            return false;
        }

        logger::info("Spell Hotbar 2 registered one HUD element, one input callback, and four blocking windows with SKSE Menu Framework");
        return true;
    }

    bool is_ready()
    {
        return ready;
    }

    bool open_window(Window window)
    {
        if (!ready || window == Window::count) {
            return false;
        }
        close_all_windows();
        windows[index(window)]->IsOpen.store(true);
        return true;
    }

    void close_window(Window window)
    {
        if (window != Window::count) {
            if (auto* interface = windows[index(window)]) {
                interface->IsOpen.store(false);
            }
        }
    }

    void close_all_windows()
    {
        for (auto* window : windows) {
            if (window) {
                window->IsOpen.store(false);
            }
        }
    }

    bool is_window_open(Window window)
    {
        return window != Window::count && windows[index(window)] && windows[index(window)]->IsOpen.load();
    }

    bool is_any_window_open()
    {
        return std::ranges::any_of(windows, [](const auto* window) {
            return window && window->IsOpen.load();
        });
    }
}
