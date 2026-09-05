# Hotbar Actions — feasibility

**Status:** research complete — plan is `spec.md`. No SH2 code changes. Tickets uncut.
**Date:** 2026-09-02.
**Prompt:** owner idea — an "Action" bind-menu tab, in the vein of Abilities, whose slot fires a keycode rather than a spell or clip. Motivating cases: One Click Power Attack bound to `B`, and dodge, both reached through a hotbar slot as if the player pressed the key. Beyond VirtualKey (Nexus 187350) is a named future integration, not v1.
**Owner clarification 2026-09-02:** "assign an icon" was never click-to-cast. It means a hotbar slot whose activation sends a virtual keypress.
**This session had no DevBench, MO2, or Skyrim.** Runtime claims below are from prior tickets in this repo, SH2 source, the VirtualKey Nexus page, and VirtualKey 1.3.2 source (`VirtualKey Source 187350 1.3.2`, extracted under `virtualkey-src/`). Anything that still needs a live press is labelled **Unverified**.

**Confidence labels:** **Confirmed** = quoted from this repo or a first-party page. **Inferred** = reasonable conclusion from those sources, not stated verbatim. **Unverified** = needs a live press.

---

## Verdict

**Yes, as a product.** The bind-menu tab, named icon, optional costs, and slot persistence are the Ability pattern with a different payload. That half is cheap.

**The hard half is who hears the injected key.** SH2 already injects `ButtonEvent`s for shouts and that reaches the engine. Whether it reaches OCPA, other SKSE hotkey mods, or Beyond VirtualKey depends on which seam the listener sits on — and this repo already has a named split there.

Do not overload **Ability**. An Ability is a bindable animation (`CONTEXT.md`). An Action that sends a key is a new glossary term.

---

## What the owner asked for

1. A bind-menu tab ("Action") whose rows are assignable icons with a name (example: Greatsword icon, name "Power Attack").
2. Binding one to a hotbar slot, same as an Ability.
3. Activating the slot sends a keycode into Skyrim / SKSE so whatever is bound to that key (OCPA on `B`, and later other hotkey mods) fires.
4. Optional stamina / magicka / health / cooldown costs, only if nearly free.
5. Later: account for [Beyond VirtualKey](https://www.nexusmods.com/skyrimspecialedition/mods/187350).

---

## Closest existing analog

Abilities, not spells.

| Surface | Ability today | Action (proposed) |
|---|---|---|
| Bind-menu tab | `TabIndex_Arts` in `advanced_bind_menu.cpp` | A fifteenth tab, same clipper + drag-source pattern |
| Catalogue row | `ArtDefinition` (name, icon, class, costs, cooldown, GCD) | A smaller row: name, icon, key, optional costs |
| Editor | `AbilityEditor` | Copy the name/icon/cost fields; replace clip/class with a key capture |
| Slot identity | `slot_type::weapon_art` + `art_id` | A new `slot_type` + action id. Serialization today is a 0/1 kind flag (`hotbar.cpp` `serialize_skill`) — a third kind is a co-save bump |
| Press path | `InputModeCast::process_input` → `try_start_art` | Same gate, then inject instead of `ArtDriver::begin` |
| HUD | Icon + CD overlay; slot keybind fires it | Same |

Owner clarification: this is a keybind-activated slot that injects another key, not HUD click-to-cast.

---

## How a press would fire

SH2 already has the injection primitive used by shouts:

- `InputModeCast::process_input` builds `RE::ButtonEvent::Create(device, "Shout", shoutKey, 1.0f, 0.0f)` and returns it as `addEvent`.
- `cast_slot` and `VoiceCastDriver` push that onto `RE::BSInputEventQueue`.
- **Confirmed (VoiceCastDriver comment, 2026-08-08/10):** calling `ShoutHandler::ProcessButton` directly was accepted and never transitioned the graph. The event has to travel the full dispatch. A down-only event also reads as an instant tap and **leaves a phantom held button** that blocks attack/sheathe; the driver reproduces down / per-frame held / up.

An Action slot would reuse that queue, with a different `userEvent` / scancode.

The motivating OCPA key is already known to this plugin. Ticket 10 logged the owner's power-attack key as `device=0, key=48`. DirectInput `DIK_B` is `0x30` = 48. `input.cpp` `get_ocpa_keys()` reads `Data/MCM/Settings/OCPA.ini` / `Data/MCM/Config/OCPA/settings.ini` `iKeycode` and treats that scancode as an attack press for chain-out.

So v1 does not need to invent "what is B". The open question is only whether an injected 48 is indistinguishable from a physical 48 **to OCPA**. Dodge is the same question for whichever dodge mod owns the bound key: engine dispatch vs that mod's own hook.

---

## Three consumer classes (this is the real design)

### 1. Engine control-map actions — **Confirmed possible**

Attack, shout, sneak, etc. Inject `ButtonEvent` with the control-map `userEvent` name and the mapped scancode. Host `AGENTS.md` and DevBench-input both require the name; a scancode alone can return `ok` and do nothing. SH2 shout injection is this class, live.

A first-class "Power Attack" that synthesizes the engine chord (forward + right attack) would live here and would **not** need OCPA to hear anything. That is a different product from "send whatever is on B".

### 2. SKSE mods listening on the engine `InputEvent` dispatch — **Inferred likely**

SkyUI MCM KeyMaps and SKSE `RegisterForKey` typically sit on the same dispatch `VoiceCastDriver` already feeds. Pushing a keyboard `ButtonEvent` with the DX scancode onto `BSInputEventQueue` should look like a real key to those listeners **if** they read the event list rather than the device buffer.

**Unverified** per target mod. No live injection of OCPA's key 48 was attempted in this session.

### 3. Mods that poll DirectInput / a private raw hook — **Unverified, and OCPA is in this bucket until proven otherwise**

Ticket 10, open risk: *"OCPA reads raw input, on its own hook, so the order between its power attack and this cut is not something this mod controls."* A physical 48 **does** reach SH2's `PollInputDevices` hook (logged). That does not prove OCPA's own hook is the engine event list.

This repo's standing input fact (ticket 50, `papyrus_functions.cpp`, `input.cpp` ~498): **DevBench sink injection never reaches SH2's `PollInputDevices` hook.** `PushOntoInputQueue` is a different seam — it appends to the list the next poll dispatches, which is how shout injection works. Whether OCPA sees that list, or only the keyboard device buffer, is the v1 spike.

If OCPA does not see queue-injected events, the generic "send keycode" Action will silently no-op for the motivating example. Fallbacks, in order:

1. Hold/release a fully-formed keyboard `ButtonEvent` (the VoiceCastDriver shape) and log whether OCPA fires. One owner press vs one injected 48.
2. If that fails: a Power Attack Action that injects the engine attack `userEvent` (and, if MCO needs it, the power-attack chord) instead of proxying OCPA.
3. Do not take OS `SendInput` / Papyrus `Input.TapKey` as the in-process path. Host notes already record those as focus-dependent and unusable headlessly; they are also the wrong layer inside a SKSE plugin that already owns `BSInputEventQueue`.

---

## Beyond VirtualKey — optional guest, not a requirement

Nexus 187350, *Beyond VirtualKey - MCM Hotkey Manager*, v1.3.2 source read 2026-09-02. Identity stays `VirtualKey.dll` / `VirtualKey_GetInterface` / `Data/SKSE/Plugins/VirtualKey/`. Installed on this machine as 1.3.0 (`VirtualKey - Virtual Hotkeys for MCM - beta`).

It is solving a different problem: move occasional SkyUI MCM KeyMaps onto `VK001`… and tap those from VK's UI / Risa / Papyrus / a guest API. It is not a universal "send B as if the player pressed it" service.

**Confirmed from 1.3.2 source:**

- `Tap` / `Down` / `Up` refuse anything outside `100000–9999999` (`IsVirtualKey` in `VKeyConstants.h`; `ApiTap` in `main.cpp`). You cannot ask it to tap OCPA's 48.
- Native delivery is `BSInputDeviceManager::SendEvent` of a keyboard `ButtonEvent` whose `userEvent` is empty and whose `idCode` is the 100xxx value (`NativeInputBackend.cpp`). That is the same *downstream* seam ticket 50 proved does not reach SH2's `PollInputDevices` hook, and an empty `userEvent` does not drive PlayerControls.
- The same tap also fires VirtualKey's own Papyrus `RegistrationMap` (`OnKeyDown` / `OnKeyUp`). `SKSEInputPatch` widens SKSE's `RegisterForKey` range (physical cap is `kMaxRealMacros = 282`) so unmodified Papyrus scripts can see 100xxx.
- Guest surface: `VirtualKey_GetInterface(2)` → `TriggerBinding(bindingId)` / `CopyBindings`. V2 is the ABI to consume; raw 100xxx is an implementation detail (`VirtualKeyAPI.h`).
- Their own architecture note (`DEVELOPMENT.md`): a physical `ButtonEvent` fans out to VK/SKSE/hotkey mods *and* ControlMap. VK does not consume or replace that press. Combat/native SKSE hooks are explicitly out of scope on the Nexus page.

**Do not make it a hard requirement.** SMF and Thu'um Reborn are requirements because the feature cannot exist without them. Actions can exist without VK: SH2 already injects shout `ButtonEvent`s via `BSInputEventQueue`, which is *upstream* of the next poll and carries a real `userEvent`. VK's tap cannot cover power attack or dodge unless those mods were rebound to a VK *and* their native hook accepts 100xxx — the page lists "custom low-level input handling" and "restrict keys to the normal physical range" as failure cases, which is OCPA.

**Optional integration, same pattern as Risa inside VK itself:** `GetModuleHandle` + `GetProcAddress("VirtualKey_GetInterface")`. If present, an Action can be a catalog binding (`TriggerBinding`). If absent, that Action kind is hidden. No requirement, no version lock to a plugin that shipped API v1–v4 in weeks.

---

## Ladder vs one seam

Do not try queue-inject then fall back to raw input on the same press.

- Queue `PushOntoInputQueue` always "succeeds." There is no oracle that OCPA or dodge actually played. The fallback never runs, or you fire both and double-swing.
- VK already fans out two backends on one tap (native `SendEvent` + Papyrus). Adding SH2's queue on top of that for the same key is a third copy for anyone on both seams.
- The only honest "raw" SKSE path is feeding `BSPCKeyboardDevice` during poll (ticket 50, unbuilt). OS `SendInput` is the wrong layer inside a plugin that already owns the input queue.

**Dispatch by Action kind at edit time, one seam per press:**

| Kind | Seam | Covers |
|---|---|---|
| Engine control | SH2 `ButtonEvent` + `userEvent` onto `BSInputEventQueue` (existing shout path) | Attack, shout, sneak. First-class Power Attack if you do not want to proxy OCPA |
| Physical scancode | Same queue, DX code, keyboard device | OCPA `B`, dodge, other native SKSE hotkeys — **Unverified** per target |
| VirtualKey binding | `TriggerBinding(id)` if VK is loaded | MCM functions already in the VK catalog |

The OCPA/dodge spike is still one injected 48 vs one physical `B`. That cell decides whether the physical-scancode kind is real or whether Power Attack has to be the engine-control kind.

---

## Costs

**Inferred nearly free** if scoped to the existing Ability Cost check: stamina / magicka / health refuse + flash + `sound_MagFail`, then deduct, then optional cooldown / GCD overlay. `try_start_art` already does that in one function. An Action that is "just a key" can call the same helpers and skip `ArtDriver`, the Ability Selector, and clip/class gating.

Not free: a new catalogue, a new editor, a third slot-serialization kind, and a decision on whether a costed Action participates in the ticket-41 press gate (`current_cast` lockout). A costless tap that is supposed to chain *out* of a cast (power attack) wants the opposite of that lockout — ticket 10 already treats OCPA's key as an attack press that ends the cast state.

---

## What v1 is, if built

A new slot kind whose catalogue row is `{id, name, icon, dx_scancode, device, optional costs}`. Bind-menu tab + editor cloned from Abilities. Press path: Ability Cost check (if any) → `ButtonEvent` onto `BSInputEventQueue` with a real down/up pair.

**Spike before any of that ships:** inject OCPA's configured scancode from `cast_slot` (or a throwaway native) and see whether a power attack plays. That one cell decides whether the generic keycode product covers the motivating example, or whether Power Attack has to be a first-class engine action instead of a key proxy.

VirtualKey `TriggerBinding` is an optional Action kind, not v1 and not a requirement. Engine-control Power Attack is the fallback if the OCPA scancode spike fails.

---

## Sources

- `skse_plugin/src/bar/hotbar.h` — `slot_type` enum
- `skse_plugin/src/bar/hotbar.cpp` — `serialize_skill` kind 0/1
- `skse_plugin/src/input/modes.cpp` — `InputModeCast::process_input`, shout `addEvent`
- `skse_plugin/src/input/input.cpp` — `get_ocpa_keys`, `is_attack_press`, PollInputDevices / PushOntoInputQueue comments
- `skse_plugin/src/casts/voice_cast_driver.cpp` — queue injection + hold/release lesson
- `skse_plugin/src/papyrus_extensions/papyrus_functions.cpp` — `cast_slot`, "injected InputEvents never reach `m_keybind.isDown()`"
- `skse_plugin/src/casts/casting_controller.cpp` — `try_start_art` cost/CD gate
- `skse_plugin/src/rendering/render_manager.cpp` — HUD `NoInputs`
- `skse_plugin/src/rendering/advanced_bind_menu.cpp` — Abilities tab
- `.scratch/mco-integration/issues/10-chain-a-committed-cast-into-an-mco-attack.md` — OCPA key 48, raw-hook risk
- `.scratch/mco-integration/issues/50-real-input-test-harness.md` — DevBench injection is downstream of the SH2 hook
- `CONTEXT.md` — Ability definition
- houseCARL `housecarl_nexus_mod` + GraphQL description for Nexus 187350 (2026-09-02)
- VirtualKey 1.3.2 source: `NativeInputBackend.cpp`, `PapyrusDispatcher.cpp`, `VirtualKeyAPI.h`, `VKeyConstants.h`, `main.cpp` `ApiTap` / `ApiTriggerBindingV2`
