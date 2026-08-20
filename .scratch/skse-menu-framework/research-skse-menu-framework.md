# SKSE Menu Framework as SH2 UI host — research note

**Status:** research only (no tickets, no SH2 code changes).  
**Date:** 2026-08-20.  
**Decision already made:** use SKSE Menu Framework (SMF), not Prisma UI. This note does not re-litigate that.  
**Confidence labels:** **Confirmed** = quoted from a primary source. **Inferred** = reasonable conclusion from those sources, not stated verbatim.

---

## 1. Identity

| Field | Value | Confidence | Source |
|---|---|---|---|
| Product name | SKSE Menu Framework (in-game: “Mod Control Panel”) | Confirmed | [Nexus 120352](https://www.nexusmods.com/skyrimspecialedition/mods/120352) description |
| Current source repo | https://github.com/QTR-Modding/SKSE-Menu-Framework-3 | Confirmed | Nexus “Source code: v3”; GitHub `CMakeLists.txt` `PRODUCT_NAME` / `BEAUTIFUL_NAME` |
| Historical v2 repo | https://github.com/Thiago099/SKSE-Menu-Framework-2 | Confirmed | Nexus description (v2 era) |
| Official example | https://github.com/QTR-Modding/SKSE-Menu-Framework-3-Example | Confirmed | Nexus + GitHub |
| Alternate wrapper | [Quantumyilmaz/SKSE-MCP](https://github.com/Quantumyilmaz/SKSE-MCP) | Confirmed | Nexus description (“if you do not want to use the header file”) |
| License | GNU LGPL 2.1 | Confirmed | https://raw.githubusercontent.com/QTR-Modding/SKSE-Menu-Framework-3/master/LICENSE (first line: “GNU LESSER GENERAL PUBLIC LICENSE Version 2.1”) |
| Git tags / GitHub Releases | **None** on `QTR-Modding/SKSE-Menu-Framework-3` | Confirmed | GitHub API `/releases` = `[]`, `/tags` = `[]` |
| CMake project version | `3.13.0.0` | Confirmed | `CMakeLists.txt` `project(${PRODUCT_NAME} VERSION 3.13.0.0 …)` |
| Exported `GetMenuFrameworkVersion()` | `#define MENU_FRAMEWORK_VERSION 3.7f` | Confirmed | `src/SKSEMenuFramework.cpp` |
| Nexus latest MAIN file | **SKSE Menu Framework v3.13-Hotfix2** (2026-07-09) | Confirmed | houseCARL `housecarl_nexus_mod` id `120352` |
| Nexus page “version” field | `3.12-Hotfix` (lags newest MAIN file) | Confirmed | same Nexus lookup |
| Consumer header on Nexus | OPTIONAL “SKSEMenuFramework Header v3.11” (2026-07-05); also misc “3 Header (Fixed)” | Confirmed | Nexus files list |
| Canonical header in git | `resources/SKSEMenuFramework.h` (README: “update the header file”) | Confirmed | https://github.com/QTR-Modding/SKSE-Menu-Framework-3/blob/master/resources/SKSEMenuFramework.h |
| Author | Thiago / SkyrimThiago (`AUTHOR_NAME` in CMake) | Confirmed | CMake + Nexus |
| Installed on this machine | `SKSEMenuFramework.dll` **3.13.0**, Address-Library / version-independent; configs `SKSEMenuFramework.ini`, theme JSONs | Confirmed | houseCARL `housecarl_skse_inventory` filter `SKSEMenuFramework`, Nolvus Awakening |
| SKSE / game runtime | CommonLibSSE-NG plugin; `ENABLE_SKYRIM_SE/AE/VR` all ON; Address Library required | Confirmed | SMF `CMakeLists.txt`; Nexus requirements (Address Library, SSE Engine Fixes) |
| Pinned SKSE loader version | **Not stated** in CMake/README. | Inferred | No `SKSE_VERSION` constraint found; DLL is Address-Library independent per houseCARL peek |

### How mods consume SMF

**Confirmed (Nexus description + v2 changelog + header):**

1. Player installs `SKSEMenuFramework.dll` as a **hard runtime requirement** (Nexus: “this mod to be a hard requirement”; v2.0 changelog: “no longer the need for a dll loader or to import imgui as a dll on either the client mods or this mod”).
2. Modder copies `SKSEMenuFramework.h` into the consumer project (header-only client). The header resolves APIs with `GetModuleHandleW(L"SKSEMenuFramework")` + `GetProcAddress` (`resources/SKSEMenuFramework.h` lines 9–24, 35–38).
3. Consumer **does not link** SMF as a CMake/vcpkg package. SMF’s own `vcpkg.json` is for **building the host**, not for guests (`name: sksemenuframework`, deps: spdlog, simpleini, nanosvg, nlohmann-json, DirectXTK, …).
4. Guest UI is written against the **`ImGuiMCP`** namespace generated in the same header (cimgui-style wrappers), not against a privately compiled Dear ImGui. Official example: `#include` header then `ImGuiMCP::Button(...)` (`SKSE-Menu-Framework-3-Example/src/UI.cpp`).
5. Detection: `SKSEMenuFramework::IsInstalled()` checks `std::filesystem::exists("Data/SKSE/Plugins/SKSEMenuFramework.dll")` — a **cwd-relative path**, not `GetModuleHandle` (`resources/SKSEMenuFramework.h` 36–38).
6. Registration is optional at compile time: if the DLL is missing, `GetProcAddress` helpers return null and the inline wrappers no-op (example: `if (!SKSEMenuFramework::IsInstalled()) return;`).

**Inferred:** there is no SKSE `PluginDeclaration` dependency field that the loader enforces. “Hard requirement” is a Nexus/mod-manager contract. Guests should still treat SMF as optional until MCM is retired (see §10).

**LGPL note (inferred from license + DLL consumption):** dynamic `GetProcAddress` against the installed DLL is the intended distribution model. SH2 would ship neither SMF source nor a static link of ImGui from SMF.

---

## 2. Public API inventory

Primary inventory: **consumer header** `resources/SKSEMenuFramework.h` + **host exports** `include/SKSEMenuFramework.h` (`FUNCTION_PREFIX extern "C" __declspec(dllexport)`). Docs: `Usage.md`, `README.md` (same repo).

### 2.1 Sections / MCP pages

| API | Signature / notes | Confidence |
|---|---|---|
| `SKSEMenuFramework::SetSection(std::string key)` | Stores `Model::Internal::key`; later `AddSectionItem` prefixes `key + "/" + menu` | Confirmed — header 237, 144–148 |
| `SKSEMenuFramework::AddSectionItem(std::string menu, RenderFunction)` | Host `AddSectionItem(const char* path, RenderFunction)` splits path on `'/'` into a tree (`src/SKSEMenuFramework.cpp` `SplitString` + `AddToTree`) | Confirmed |
| Nested pages | Example: `"Folder Example/Example 2"` (`SKSE-Menu-Framework-3-Example/src/UI.cpp`) | Confirmed |

`RenderFunction` = `void(__stdcall*)()` (`header` Model typedef).

### 2.2 Independent windows

| API | Notes | Confidence |
|---|---|---|
| `AddWindow(RenderFunction, bool doesWindowPauseGame = true)` | Host export is **one-arg** `AddWindow(RenderFunction)`. Guest header sets `result->BlockUserInput = doesWindowPauseGame` after the call (header 151–158) | Confirmed |
| `WindowInterface` | `std::atomic IsOpen{false}`; `std::atomic BlockUserInput{true}` (header 53–57; host `WindowManager.cpp`) | Confirmed |
| `GetMainWindow()` | Returns MCP main window interface; example toggles `GetMainWindow()->IsOpen` (Example5 OnInput) | Confirmed |
| `AddWindowWithView(RenderFunction, std::string viewName, bool pause)` | Present in **guest header** (`GetProcAddress("AddWindowWithView")`). **Not** in host `include/SKSEMenuFramework.h` export list. | Confirmed header-only; **Inferred** dead/unimplemented on 3.13 host |

Host also creates its own MCP + settings windows at plugin load (`src/plugin.cpp`: `AddWindow(UI::RenderMenuWindow)` / `AddWindow(UI::RenderConfigWindow)`).

### 2.3 Input callbacks

| API | Notes | Confidence |
|---|---|---|
| Host export name | **`RegisterInpoutEvent`** (typo is the real export) | Confirmed — `include/SKSEMenuFramework.h`, `src/SKSEMenuFramework.cpp` |
| Guest | `AddInputEvent(InputEventCallback)` → `new Model::InputEvent(callback)` which `GetProcAddress("RegisterInpoutEvent")` | Confirmed |
| Callback | `bool(__stdcall*)(RE::InputEvent*)`; `true` = strip that event from the game queue | Confirmed — `InputEventHandler.cpp` `Process` unlinks events when callback returns true |
| Docs | README “Input Capturing and blocking” | Confirmed |

Host only runs guest input callbacks when `!ImGui::IsAnyItemActive()` (`Hooks.cpp` `ProcessInputQueueHook::thunk`).

### 2.4 HUD callbacks

| API | Notes | Confidence |
|---|---|---|
| `AddHudElement(HudElementCallback)` | `void(__stdcall*)()`; registered via `RegisterHudElement` | Confirmed |
| Draw timing | `HudManager::Render()` is called **inside SMF’s `ImGui::NewFrame()`**, every frame, even when no MCP window is open (`Hooks.cpp` `Render()`) | Confirmed |
| Typical use | `ImGuiMCP::GetForegroundDrawList()` overlays; skip when `IsAnyBlockingWindowOpened()` (example + README) | Confirmed |

### 2.5 Texture / font

| API | Notes | Confidence |
|---|---|---|
| `LoadTexture(path, ImVec2 size={0,0})` | SVG, DDS, “most conventional image files” (README). Cached in host `TextureLoader`. | Confirmed |
| `DisposeTexture(path)` | README 3.4 feature | Confirmed |
| `PushFont(name)` / `FontAwesome::PushSolid/Regular/Brands/Pop` | Fonts: all `.ttf`/`.otf` in `Data/SKSE/Plugins/Fonts` plus JSON size overrides (`Usage.md`) | Confirmed |
| Host fonts | `FontManager::LoadFonts` at D3D init (`Hooks.cpp`) | Confirmed |

### 2.6 Events

| API | Notes | Confidence |
|---|---|---|
| `Model::EventType` | `kNone=0, kOpenMenu=1, kCloseMenu=2, kBeforeRender=3, kAfterRender=4` | Confirmed — guest header |
| `AddEvent(callback, priority)` | `RegisterEventPriority` | Confirmed |
| Dispatch | `kOpenMenu`/`kCloseMenu` from `WindowManager::Open/Close`; `kBeforeRender`/`kAfterRender` around the ImGui frame (`Hooks.cpp` `Render()`) | Confirmed |

### 2.7 Gamepad / keyboard nav

**Confirmed (Nexus description + `Hooks.cpp`):**

- MCP toggle: INI `ToggleKey` (default F1), `ToggleKeyGamePad` (default LB), modes SinglePress / Hold / DoublePress.
- When a blocking window is open: `EnableImGuiInput()` sets `ImGuiConfigFlags_NavEnableKeyboard` and `NavEnableGamepad`; when not paused, `DisableImGuiInput()` sets `NoMouse`, `NavNoCaptureKeyboard`, and clears nav flags.
- Nexus: D-pad selects section, A interacts, B back; Escape closes; “When any number of windows are open, all the user input will be redirected to them.”
- Gamepad support claimed since v2.1 changelog (“Gamepad now is now fully supported”).
- Host `ProcessInputQueueHook` swallows the event list (dummy `{nullptr}`) when `UI::Renderer::ProcessOpenClose` captures the toggle, and when `WindowManager::ShouldTheGameBePaused()` it `TranslateInputEvent` then strips all but PrintScreen.

### 2.8 VR

**Confirmed:**

- CMake `ENABLE_SKYRIM_VR ON`; FetchContent `alandtse/imgui-vr-helper` tag `v1.8.0`.
- `Hooks.cpp` includes `ImGuiVRHelperClientSDK.h`; `ConnectVRHelper()` on SKSE `kPostPostLoad`.
- If helper missing: “menu stays on the flat mirror.”
- Nexus changelog v3.10: “vr support added by alandtse.”

**Inferred for SH2:** Nolvus Awakening is SE/AE, not VR. VR is a host capability SH2 can ignore unless the fork ships VR.

### 2.9 Other guest APIs

| API | Host export? | Notes |
|---|---|---|
| `IsAnyBlockingWindowOpened()` | Yes | Guest header comment says “If returns true, the player is currently controlling the game” — **that comment is inverted vs the function name**. Implementation: `return WindowManager::ShouldTheGameBePaused()` (`src/SKSEMenuFramework.cpp`). README example treats true as “menu capturing.” **Confirmed implementation; Confirmed buggy comment.** |
| `SetHotkeyEnabled` / `IsHotkeyEnabled` | Yes | Example5 key N |
| `GetMenuFrameworkVersion` | Yes | Returns **3.7f** even on a 3.13 binary (stale `#define`) unless the installed DLL was rebuilt with a newer constant. **Confirmed source.** Installed 3.13.0 DLL not disassembled for this constant. |
| `SetWindowsPauseGame` | Typedef in guest header; **not** in host export list | **Inferred** unused |

### 2.10 What SMF is not

**Confirmed (host sources + README):** no settings schema, no INI writer for guests, no Papyrus API, no SkyUI MCM bridge. Guests own persistence. This matches the existing fork note: “SKSE Menu Framework v3 only hosts ImGui” (`.scratch/mco-integration/notes/18-msco-gcd-cadence.md`).

---

## 3. Hosting model

**Confirmed: SMF is the ImGui host.** Guests are guests.

Exact sequence (`src/plugin.cpp` + `src/Hooks.cpp`):

1. `SKSEPluginLoad` → `Hooks::Install()` which installs:
   - `D3DInitHook` — trampoline at `REL::RelocationID(75595, 77226, 75595) + Relocate(0x9, 0x275, 0x9)` (same family as SH2).
   - `RenderUIHook` — **two** MenuManager call sites `35556/36555` and `38085/39039` (not DXGI Present).
   - `ProcessInputQueueHook` — `REL::RelocationID(67315, 68617, 67315) + Relocate(0x7B, 0x7B, 0x81)`.
2. D3D init thunk: `ImGui::CreateContext()` then `ImGui_ImplWin32_Init` + `ImGui_ImplDX11_Init` on the game’s device/context/swapchain window; `SetWindowLongPtrA(..., GWLP_WNDPROC, WndProcHook::thunk)`.
3. Each UI frame: `ImGui_ImplDX11_NewFrame` / Win32 NewFrame / `ImGui::NewFrame` → HUD callbacks → if any window `IsOpen`, `UI::Renderer::RenderWindows()` → `ImGui::Render` → `g_vrHelper.RenderFrame()` (DX11 draw or VR helper).

**Guest vs host (Confirmed):**

- Guest **must not** create a second host. Intended pattern: `IsInstalled` → `SetSection` → `AddSectionItem` / `AddWindow` / `AddHudElement` / `AddInputEvent`.
- Guest render functions run **on SMF’s context** during SMF’s frame (section items inside MCP; windows when `IsOpen`; HUD every frame).
- Guest **may** remain a guest-only plugin: no D3D hook, no input hook, no `imgui.lib`. Nexus: “There is no need to use hooks or input management.”

**Inferred:** a consumer that still needs a per-frame HUD with no input can either (a) `AddHudElement` (guest) or (b) keep its own present hook (second host — see §4).

---

## 4. Conflict with SH2’s existing ImGui host

SH2 **is already an ImGui host**, copied from Wheeler (comment in `render_manager.h`).

### 4.1 SH2 hooks (Confirmed)

| Hook | Location | What it does |
|---|---|---|
| D3D init | `RenderManager::D3DInitHook` `REL::RelocationID(75595, 77226)` + `VariantOffset(0x9, 0x275, 0x00)` | `ImGui::CreateContext()`; `ImGui_ImplWin32_Init`; `ImGui_ImplDX11_Init`; `SetWindowLongPtrA` WndProc (`render_manager.cpp` 912–972) |
| Present | `DXGIPresentHook` `REL::RelocationID(75461, 77246)` + offset `0x9` | NewFrame → `RenderManager::draw()` (HUD + editors) → `ImGui_ImplDX11_RenderDrawData` (988–1004) |
| Input | `OnInputEventDispatch` same call site as SMF: `RELOCATION_ID(67315, 68617)` + `0x7B` (`input.cpp` 23–41) | `processAndFilter` then original dispatch; also feeds `ImGui::GetIO()` key/mouse events (313+) |
| WndProc | `RenderManager::WndProcHook` | `WM_KILLFOCUS` → `io.ClearInputKeys()` (902–909) |

CMake: `find_package(imgui CONFIG REQUIRED)` (`skse_plugin/CMakeLists.txt`). Style: `ImGui::StyleColorsDark()` (`render_manager.cpp` ~49).

### 4.2 What happens if both stay hosts

**Confirmed overlap:**

1. **Same D3D init call site.** Both trampoline `75595/77226`. Each calls `ImGui::CreateContext()`. Dear ImGui’s `CreateContext()` sets the **current** context to the new one. The later plugin’s context wins; the earlier plugin’s `ImGui_ImplDX11_*` state is bound to a context that is no longer current unless it `SetCurrentContext` every frame (SH2 does not).
2. **Same input dispatch call site** (`67315/68617+0x7B` on SE/AE). Both `write_call<5>`. Whichever loads **last** wraps whoever is currently at that site. SMF, when it believes a blocking window is open, can replace the event list with `{nullptr}` (`Hooks.cpp` `ProcessInputQueueHook::thunk`). SH2’s `processAndFilter` then either never sees events or runs on an empty list — **order-dependent, both outcomes are bad for hotbar keys and bind-menu interception**.
3. **WndProc chaining.** Both `SetWindowLongPtrA`. If each calls saved `func`, KILLFOCUS can chain. Last installer is outermost.
4. **Two present paths.** SH2 draws on DXGI Present; SMF draws on MenuManager. Both can `ImGui_ImplDX11_RenderDrawData` per frame on **different contexts** → flickering, missing HUD, device-object mismatch, or both HUDs fighting the swapchain.

Quote — SMF creates its own context:

```text
ImGui::CreateContext();
...
if (!ImGui_ImplWin32_Init(desc.OutputWindow)) { ... }
if (!ImGui_ImplDX11_Init(device, context)) { ... }
```

(`QTR-Modding/SKSE-Menu-Framework-3/src/Hooks.cpp` `D3DInitHook::thunk`)

Quote — SH2 does the same:

```text
ImGui::CreateContext();
if (!ImGui_ImplWin32_Init(sd.outputWindow)) { ... }
if (!ImGui_ImplDX11_Init((ID3D11Device*)device, (ID3D11DeviceContext*)context)) { ... }
```

(`skse_plugin/src/rendering/render_manager.cpp` 950–964)

Quote — SMF input capture when MCP is paused:

```text
if (isInputCapturedByOpenClose) {
    constexpr RE::InputEvent* const dummy[] = {nullptr};
    originalFunction(a_dispatcher, dummy);
} else {
    if (WindowManager::ShouldTheGameBePaused()) {
        UI::TranslateInputEvent(a_event);
        originalFunction(a_dispatcher, RemoveNonPrintScreenInputs(...));
```

(`Hooks.cpp` `ProcessInputQueueHook::thunk`)

**Inferred (strong):** keeping `ImGui_ImplDX11` + SH2’s input hook **while SMF is installed** is not a supported dual-host configuration. Migration must pick **one** ImGui context. ADR-0003 already predicted that a “shared host” does not remove SH2’s present hook if the HUD stays; the new decision is to **move** hosting, not to run two hosts.

---

## 5. Example consumer mods

### 5.1 Official example (source) — **Confirmed**

Repo: https://github.com/QTR-Modding/SKSE-Menu-Framework-3-Example  
`src/plugin.cpp` calls `UI::Register()` from `SKSEPluginLoad` (not `kPostLoad`).  
`src/UI.cpp` `UI::Register()`:

```cpp
if (!SKSEMenuFramework::IsInstalled()) { return; }
SKSEMenuFramework::SetSection(MOD_NAME);
SKSEMenuFramework::AddSectionItem("Font Awesome", Example4::Render);
SKSEMenuFramework::AddSectionItem("Add Item", Example1::Render);
SKSEMenuFramework::AddSectionItem("Folder Example/Example 2", Example2::Render);
UI::Example2::ExampleWindow = SKSEMenuFramework::AddWindow(Example2::RenderWindow, true);
SKSEMenuFramework::AddHudElement(Example5::RenderOverlay);
SKSEMenuFramework::AddInputEvent(Example5::OnInput);
UI::Example5::NonPausingWindow = SKSEMenuFramework::AddWindow(Example5::RenderWindow, false);
SKSEMenuFramework::AddEvent(UI::Example6::OnEvent, 0);
```

Widgets use **`ImGuiMCP::`**, not `ImGui::`.

**Inferred risk:** registering in `SKSEPluginLoad` before SMF has loaded makes `GetProcAddress` fail until a later call; the example does not retry. Safer pattern for SH2: register on `kPostLoad` / `kPostPostLoad` (SMF itself connects VR helper on `kPostPostLoad`).

### 5.2 MSCO on this machine — **Confirmed presence, inferred SMF guest**

`MSCO.dll` (Magic Casting Behavior Overhaul) embeds the string `Data/SKSE/Plugins/SKSEMenuFramework.dll` (houseCARL peek). It also imports `d3d11.dll` / `dxgi.dll` (could be its own graphics or SMF-only). SH2’s MCO notes treat MSCO’s charge curve as saved from “MSCO's SKSE Menu Framework page” (`.scratch/mco-integration/issues/18-pace-the-hotbar-gcd-with-msco-clip-cadence.md`). **Source for MSCO’s Register() was not in this workspace.**

### 5.3 Nexus-listed alternative wrapper

[SKSE-MCP](https://github.com/Quantumyilmaz/SKSE-MCP) — same host DLL, different C++ façade. Not required if SH2 uses the official header.

---

## 6. SH2 current UI inventory (this repo)

### 6.1 SkyUI MCM (Papyrus)

| File | Owns |
|---|---|
| `papyrus/Scripts/Source/SpellHotbarMCM.psc` | SkyUI `SKI_ConfigBase`. `Pages[7]`: `$Keybinds`, `$Settings`, `$Bars`, `$Perks`, `$Presets`, `$Spells`, `$Util`. `GetVersion() = 6`. |
| `papyrus/Scripts/Source/SpellHotbar.psc` | `hidden` native declarations for the SKSE plugin (the MCM’s backend). |
| `papyrus/Scripts/Source/SpellHotbarInitQuestScript.psc` | Quest; `SpellHotbar_MCM` property; powers/debug — **not** settings UI. |
| `papyrus/Scripts/Source/SpellHotbarBattleMageInitQuestScript.psc` | BattleMage init. |
| `papyrus/Scripts/Source/SpellHotbarOpenBattleMagePerkTree.psc` | Perk-tree opener. |
| `papyrus/Scripts/Source/SpellHotbarToggleDualCastingEffect.psc` | Dual-cast effect. |
| `python_scripts/output_mcm_code.py` | Generator leftover for MCM fragments. |

**Keybinds page:** 12 skill `AddKeyMapOption`s via `SpellHotbar.getKeyBind(0..11)`; prev/next bar (indices 13/12 swapped in UI); open bind menu (22); modifiers 14–18; Oblivion 19–21 (`SpellHotbarMCM.psc` 97–135). Maps to `Input::keybind_id` (`keybinds.h`).

**Settings page:** input mode, HUD show modes, layout/anchor/scale/offsets, potion GCD, shout CD flag, Oblivion bar geometry (`137–193`). All native getters/setters except nothing on this page uses TESGlobal.

**Bars page:** per-bar enabled + inherit mode for sneak/melee/1H-shield/1H-spell/dual/2H/ranged/magic (IDs like `1296124239` = `'MAIN'+1`). Native `getBarEnabled` / `getInheritMode`.

**Perks page:** **Papyrus TESGlobal properties** on the MCM script (`SpellHotbar_BattleMage_*`), **not** natives (`255–272`). Native side still resolves the same globals from `SpellHotbar.esp` (`game_data.cpp` 379–386).

**Presets page:** save/load config JSON, bars JSON, icon-edit JSON via natives (`275–287`, IO in `user_data_io.cpp`).

**Spells page:** toggles that call `openSpellEditor` / `openPotionEditor` (`289–291`).

**Util page:** reload resources/data, clear bars, drag bars (`showDragBar`), add/remove powers (`playerKnowsPower` / `togglePlayerPowerKnowledge`) (`294–318`).

### 6.2 Native ImGui surfaces

All drawn from `RenderManager::draw()` (`render_manager.cpp` 1938+), mutually exclusive editors then HUD:

| Surface | Files | Opened from | Input |
|---|---|---|---|
| **HUD bar** | `draw()` ~2112–; `bar/hotbar.cpp` `draw_in_hud`; `oblivion_bar.cpp` | Always (fade/`shouldShowHUDBar`) | `ImGuiWindowFlags_NoInputs` |
| **Magic/inventory bind overlay** | `draw()` ~2013–2110 `draw_in_menu` | Magic menu / valid inventory tab / fav menu (VL/WW) | NoInputs; binding is SH2 input hook |
| **Spell editor** | `spell_editor.cpp/.h`, `spell_edit_dialog.cpp/.h` | MCM `openSpellEditor` → `RenderManager::open_spell_editor` | Full ImGui; blocks game keys (`should_block_game_key_inputs`) |
| **Potion editor** | `potion_editor.cpp/.h` | MCM `openPotionEditor` | Same |
| **Icon edit dialog** | `icon_edit_dialog.cpp/.h` (namespace `PotionEditor` in that header) | Nested in spell/potion editors | Same |
| **Bar drag window** | `bar_dragging_config_window.cpp/.h`; `show_drag_frame` in `draw()` | MCM `showDragBar(0/1)` | Mouse drag |
| **Advanced bind menu** | `advanced_bind_menu.cpp/.h` | Keybind `open_advanced_bind_menu` (id 22) / `open_advanced_binding_menu` | Own capture; ADR-0003: gated with Magic Menu + `processAndFilter` |
| **Tab buttons / textures** | `gui_tab_button.*`, `texture_loader.*`, `texture_csv_loader.*`, `texture_buttons_loader.*` | Supporting | — |

`RenderManager::should_block_game_cursor_inputs` / `should_block_game_key_inputs` / `close_key_blocking_frames` exist specifically because SH2 owns input while editors are up.

### 6.3 Persistence

| Store | Files | Contents |
|---|---|---|
| SKSE cosave | `storage/storage.cpp` `SaveCallback`/`LoadCallback`; id `0xB8498471` in `plugin.cpp`; format `6` | Hotbar layout settings (`'HOTB'`), per-bar slots, art binds, user spell/icon data (`game_data.cpp` serialize helpers) |
| JSON presets | `storage/user_data_io.cpp` | `My Games/Skyrim Special Edition/SpellHotbar/{presets,bars,icon_edits}` and `Data/SKSE/Plugins/SpellHotbar/{presets,bars,icon_edits}/` |
| TESGlobal | `SpellHotbar.esp` via MCM Perks page + `game_data.cpp` | BattleMage proc chances, timed block, override perks |
| CSV/data packs | `game_data/*_csv_loader.cpp`, `localization.cpp` | Icons, spell metadata, key names — not MCM |

**No SH2 INI for player settings** was found as the live store; settings live in the **cosave** plus optional JSON presets. (SMF’s own `SKSEMenuFramework.ini` is unrelated.)

### 6.4 Papyrus ↔ native bridge

`plugin.cpp` 63: `SKSE::GetPapyrusInterface()->Register(SpellHotbar::register_papyrus_functions)`.  
Full native list: `papyrus_functions.cpp` 650–746. MCM is a **thin SkyUI front-end** over those natives, except Perks (TESGlobal) and power toggles (natives `playerKnowsPower`).

---

## 7. ADR-0003 vs SMF migration

**Decision (quote, `docs/adr/0003-keep-the-mods-own-imgui-interface.md`):**

> The fork will not migrate the interface to SKSE Menu Framework or any other shared menu host. The interface stays as upstream built it.

**Rationale (quote):**

> Spell Hotbar 2 draws three surfaces with different requirements. The hotbar itself renders every frame with no input capture, so the plugin's `DXGIPresent` hook has to stay whatever else changes. The advanced bind menu depends on the mod's own per-event input interception, which decides whether a key reaches the game at all and is gated on the vanilla Magic Menu being open; a shared host is not built to arbitrate that. Only the spell, potion, icon and bar-drag editors are shaped like the config windows SKSE Menu Framework exists to host, and they are the surface the player touches least. A migration would therefore remove no hooks, resolve no conflicts with other ImGui-based mods in the load order, and change only the door to the least-used third of the interface.

**How to treat ADR-0003 in this research (owner, 2026-08-20):** the recorded “will not migrate” line is a **constraint list to re-test**, not a freeze. Ranking criterion is the most modern, performant architecture. A later ADR amendment is documentation after a decision, not a prerequisite for exploring guests.

**Re-test of the rationale (inferred from SMF 3.13 source vs ADR text):**

- HUD still needs a per-frame no-input overlay. SMF `AddHudElement` is the guest-shaped equivalent (`HudManager::Render` every MenuManager frame). Open spike: MenuManager vs DXGI Present under ENB / Display Tweaks.
- Advanced bind still needs SH2 to decide whether a key reaches the game while Magic Menu is open. SMF `AddInputEvent` can strip events; it does not encode that gate. Keep the policy in `Input::in_binding_menu` / `processAndFilter` (or a successor), not in SMF.
- SMF does not persist SH2 settings. Co-save format 6 and JSON presets stay SH2-owned.
- Dual-host (keep `ImGui_ImplDX11` **and** SMF context) is **not** performant and is unsafe (§4). Guest widgets must use `ImGuiMCP::`. HUD either moves to `AddHudElement` or SH2 must **not** `CreateContext` when SMF is present.

**If the work proceeds:** amend ADR-0003 to “SMF is the ImGui host / view; SH2 owns settings, binds, saves, and Magic-Menu bind arbitration.” That amendment can still retire SkyUI MCM.

---

## 8. MCM replacement matrix

Proposed MCP section: `SetSection("Spell Hotbar 2")` (or `$Spell Hotbar` to match MCM `ModName`). Nested `AddSectionItem` paths below. Papyrus “can go away” means **that control no longer needs SkyUI**; TESGlobal/ESP/quests may remain.

| MCM page / control | Native / store today | Proposed SMF page | Independent window? | Papyrus can go away? |
|---|---|---|---|---|
| Keybinds / all `getKeyBind` | `Input::rebind_key` / `keybinds.h` | `Keybinds` | Optional capture helper via `AddInputEvent` while rebinding | **Yes** (natives stay as C++ API; Papyrus decls only needed if MCM remains) |
| Settings / input mode, HUD show, layout, scales, Oblivion bar | `Bars::*`, `Input::modes` | `Settings` (+ maybe `Settings/Oblivion`) | No | **Yes** |
| Settings / `showDragBar` (also Util) | `RenderManager::start_bar_dragging` | Button on Settings **or** `AddWindow(BarDragging::draw_window, true)` | **Yes** — current UX is a drag overlay | Yes for MCM toggle |
| Bars / enabled + inherit | `Bars::hotbars` | `Bars` | No | **Yes** |
| Perks / BattleMage sliders | TESGlobal via MCM properties; also `GameData::global_spellhotbar_perks_*` | `Perks` | No | **MCM Papyrus yes**; ESP globals **stay**. SMF page should write the same `TESGlobal*` the native code already resolved. |
| Presets / save-load JSON | `Storage::IO` | `Presets` | No | **Yes** |
| Spells / open editors | `open_spell_editor` / `open_potion_editor` | `Editors` section items **or** `AddWindow` for each editor | **Yes** — tables are full-screen tools (`spell_editor.cpp` `ImGuiWindowFlags_NoCollapse \| NoResize`) | Yes for MCM toggles |
| Util / reload, clear bars | natives | `Util` | No | **Yes** |
| Util / add-remove powers | natives + spells on player | `Util` or `Powers` | No | **Yes** |
| Init quest / BattleMage perk tree opener | separate `.psc` | Out of MCP (in-world power) | — | **No** (gameplay script, not MCM) |

**Inferred:** `SpellHotbar.psc` native stubs can remain for console/debug (`castSlot`) even after MCM retirement.

---

## 9. Recommended architecture

There is **no** `SettingsService` / `BindingService` / `ProfileService` / `EditorState` type in this repo today. Those names are the proposed **facade** over existing types. SMF is an adapter, not a store.

```
                    ┌─────────────────────────────────────┐
                    │  SMF guest adapter (new)            │
                    │  SetSection / AddSectionItem         │
                    │  AddWindow / AddHudElement (later)   │
                    │  ImGuiMCP widgets only               │
                    └──────────────┬──────────────────────┘
                                   │ calls
         ┌─────────────┬───────────┼────────────┬──────────────┐
         ▼             ▼           ▼            ▼              ▼
   Settings      Binding      Profile      EditorState     (HUD)
   façade        façade       façade       façade          façade
         │             │           │            │              │
         ▼             ▼           ▼            ▼              ▼
   Bars::*        Input::*    Storage::    SpellEditor     hotbar
   modes.cpp      keybinds    IO +         PotionEditor    draw_in_hud
   GameData       BindMenu    SaveCallback BarDragging     (today:
   TESGlobal*     slotSpell   art binds    icon dialog     DXGIPresent)
```

| Proposed façade | Grounded types / files |
|---|---|
| **SettingsService** | `SpellHotbar::Bars::*` (`hotbars.h` 41–73), `Input::is_oblivion_mode` / `modes.cpp`, `GameData` HUD show helpers, TESGlobal perk pointers in `game_data.cpp` |
| **BindingService** | `Input::keybind_id`, `rebind_key`, `processAndFilter`, `Storage::slotSpell`, `BindMenu::*`, `RenderManager::current_selected_item_bindable` |
| **ProfileService** | `Storage::IO::{load,save}_preset`, bar/icon JSON, `Storage::SaveCallback` format 6 (do **not** change record layout in the SMF UI work) |
| **EditorState** | `SpellEditor::{show,hide,is_opened,renderEditor}`, `PotionEditor::*`, `BarDraggingConfigWindow`, `icon_edit_dialog`, `User_custom_spelldata` |
| **SMF adapter** | New files only (not written in this research). Register on `kPostLoad`. `GetMenuFrameworkVersion` is **unreliable** (stuck at 3.7f in source) — prefer `IsInstalled()` + optional `GetModuleHandle`. |

**Confirmed constraint:** editor widgets must be ported from `ImGui::` (vcpkg imgui) to `ImGuiMCP::` (header wrappers) **or** they will mutate the wrong context. That is the bulk of the editor migration, not SMF registration.

**HUD:** ADR-0003 was right that the bar is a no-input overlay. SMF’s `AddHudElement` is the guest-shaped equivalent (`HudManager::Render` every frame). Textures: SH2 uses `ID3D11ShaderResourceView*` via `TextureImage` (`render_manager.h`). **Inferred:** those SRVs *might* be passable as `ImGuiMCP::ImTextureID` on the same D3D11 device; if ImGui version/backend differs, they will not. Fallback: `LoadTexture` for MCP chrome only; keep atlas loading in SH2 and share SRVs after a spike.

---

## 10. Phased migration plan

### Phase 0 — Detection / optional dependency

- Ship SMF as **optional**. `IsInstalled()`; if false, today’s MCM + SH2 ImGui host unchanged.
- **Blocker:** optional-dep + SMF installed **still** creates the dual-host crash/glitch if SH2 always `CreateContext`. Detection is not enough: if SMF is present, SH2 must **not** initialize its own ImGui (or must not load). **Inferred:** Phase 0 for “MCM still works without SMF” implies **two binaries or a runtime branch that skips `RenderManager` ImGui init when SMF exists** — but then HUD/editors disappear unless already registered as guests. Treat “optional” as **MCM fallback when SMF absent**, and **guest-only ImGui when SMF present**.

### Phase 1 — Settings pages (MCP)

- Register `Keybinds`, `Settings`, `Bars`, `Perks`, `Presets`, `Util` as `AddSectionItem` renderers calling the façades.
- Leave SH2 DXGIPresent HUD running **only if** SMF is absent. If SMF present, HUD still on SH2 present hook is the dual-host bug — so Phase 1 in a SMF-present build **requires** at least a temporary `AddHudElement` that calls existing `draw_in_hud` **or** delaying Phase 1 until Phase 3.
- **Blocker (hook order):** cannot keep both input trampolines. If SMF present, SH2 must filter via `AddInputEvent` **and** `processAndFilter` logic, or wrap after SMF (not viable if SMF drops the list). **Recommended:** when SMF present, install SH2’s game-input logic as an `AddInputEvent` callback (plus remaining `processAndFilter` for bind menu) and **do not** trampoline `67315` ourselves.
- **Blocker (controller capture):** while MCP `BlockUserInput` is true, SMF eats gamepad. Dual-input baseline (`CONTEXT.md`) will see hotbar casts dead inside MCP — acceptable if documented; not acceptable if a non-pausing window is left open (`AddWindow(..., false)`).
- Save/config format: **no change**. Pages call existing setters so cosave/JSON stay format 6.

### Phase 2 — Editors as `AddWindow`

- Spell / potion / icon / bar-drag: `AddWindow(render, true)`; set `IsOpen` instead of `SpellEditor::show()`.
- Port `ImGui::` → `ImGuiMCP::` (tables, clipper, popups).
- Fonts: SH2 custom fonts vs SMF `PushFont` / Fonts folder. **Open:** keep SH2 font files loaded through SMF Fonts dir or accept MCP theme fonts.
- **Blocker:** `should_block_game_key_inputs` duplicates SMF `BlockUserInput`. Prefer SMF’s flag; delete SH2’s parallel cursor capture once windows are SMF windows.

### Phase 3 — Optional HUD registration

- `AddHudElement` with existing `NoInputs` HUD draw (main + Oblivion fade).
- Remove `DXGIPresentHook` and SH2 `CreateContext`.
- Magic-menu overlay (`draw_in_menu`) is **not** a HUD; it is a NoInputs overlay while a vanilla menu is open. Options: (a) same HudElement with `if (magMenu)` branch; (b) non-blocking `AddWindow`. (a) matches current code structure.

### Phase 4 — Advanced bind menu

- Hardest. ADR-0003 still applies: SH2 decides whether a key reaches the game while Magic Menu is open.
- SMF `AddInputEvent` **can** strip events (same mechanism). **Inferred:** bind menu can stay a SH2 state machine (`BindMenu::is_opened`) whose render is an SMF `AddWindow(..., false)` or HudElement, with `AddInputEvent` calling today’s `processAndFilter` bind-menu branch.
- **Do not** put bind-menu key routing inside MCP pages.

### Phase 5 — MCM retirement

- Remove `SpellHotbarMCM.psc` from the plugin ESP / SkyUI quest once Phase 1 feature-complete **and** SMF is a declared requirement.
- **Blocker — SkyUI users without SMF:** either keep MCM indefinitely as fallback, or make SMF a hard Nexus requirement (matches SMF author’s model). Owner decision.
- Remaining Papyrus: init quest, powers, BattleMage perk tree — **not** retired.

### Named blockers (summary)

| Blocker | Why |
|---|---|
| Dual ImGui context | Both call `CreateContext` + DX11 impl |
| Shared input trampoline `67315+0x7B` | SMF can drop the event list |
| Save format | Must not churn cosave/JSON for a UI host swap |
| SkyUI-only users | MCM retirement vs optional SMF |
| Controller capture | SMF redirects all input when blocking windows open |
| `GetMenuFrameworkVersion` == 3.7f | Cannot feature-detect 3.9+ APIs by that float |
| `RegisterInpoutEvent` typo | Must use exact export |
| Editor port to `ImGuiMCP` | Large mechanical rewrite |
| Texture/font atlas | SH2 DDS atlases vs SMF `LoadTexture` |
| Bind menu vs Magic Menu | SMF not built to arbitrate vanilla-menu key routing |

---

## 11. Open questions (owner)

1. **Hard vs optional SMF?** If optional, accept a dual code path (SkyUI MCM + SH2 ImGui host when SMF missing; guest-only when present). If hard, Nexus requirement + amend ADR-0003 in one step.
2. **HUD in Phase 1 or Phase 3?** Guest settings pages are unsafe while SH2 still `CreateContext`. Prefer “HUD HudElement in the same change that disables SH2 ImGui init.”
3. **Amend ADR-0003 text** to “SMF is the ImGui host; SH2 remains owner of settings/binds/saves; Magic-menu bind arbitration stays SH2 input policy.” Yes/no on HUD moving in the same amendment.
4. **Advanced bind menu in SMF windows or keep vanilla-menu overlay?** ADR rationale prefers the latter.
5. **Perks page:** write TESGlobals from C++ (already have pointers) or keep a tiny Papyrus MCM forever?
6. **Gamepad rebind UX:** SkyUI `AddKeyMapOption` vs ImGui key capture under SMF nav — acceptable clunk?
7. **Theme:** adopt `SKSEMenuFrameworkThemes` (Skyrim JSON) vs keep `StyleColorsDark` inside guest renderers (`ImGuiMCP` style pushes).
8. **Header pin:** Nexus Header 3.11 vs git `resources/SKSEMenuFramework.h` vs installed DLL 3.13 — pin git header to CMake 3.13.
9. **Registration timing:** `kPostLoad` vs example’s `SKSEPluginLoad`.
10. **Personal vs core fork:** SMF as Core Fork dependency or Compatibility Package only (`CONTEXT.md` language)?
11. **`AddWindowWithView`:** ignore until it exists in host exports.
12. **License/distribution:** LGPL 2.1 host; SH2 remains whatever license the fork already uses — confirm no static imgui copy from SMF tree.

---

## Source list

| Source | Role |
|---|---|
| https://github.com/QTR-Modding/SKSE-Menu-Framework-3 | Host source (`Hooks.cpp`, `plugin.cpp`, `SKSEMenuFramework.cpp`, `WindowManager.cpp`, `InputEventHandler.cpp`, `CMakeLists.txt`, `LICENSE`, `Usage.md`, `README.md`, `include/SKSEMenuFramework.h`, `resources/SKSEMenuFramework.h`) |
| https://github.com/QTR-Modding/SKSE-Menu-Framework-3-Example | Guest registration pattern |
| https://www.nexusmods.com/skyrimspecialedition/mods/120352 | Distribution, INI, changelog, requirements (via houseCARL) |
| houseCARL `housecarl_nexus_mod` / `housecarl_skse_inventory` | Nexus files 3.13-Hotfix2; local DLL 3.13.0; MSCO embeds SMF path |
| This repo `skse_plugin/**`, `papyrus/Scripts/Source/SpellHotbarMCM.psc`, `docs/adr/0003-*.md` | SH2 inventory |
| Upstream comparison not required for API; HUD/MCM layout matches this fork’s files above | — |

**Not used as authority:** Prisma UI, secondary blogs, uncited Discord.
