# Hotbar Actions

Status: ticket 01 resolved — physical-scancode kind is v1. Frontier is
`issues/02-slot-kind-and-cosave.md`. Research: `research-feasibility.md`. Line:
`ng/smf-next`. Overlay: `Dev - Spell Hotbar 2 SMF Next`.

Created 2026-09-02. Tickets cut 2026-09-04. Ticket 01 admitted the physical-scancode
kind (OCPA and dodge both play from the shout queue).

## Problem Statement

The bar already holds spells, shouts, potions, and Abilities. Combat still has moves that
live on a *different* key: One Click Power Attack on `B`, dodge on whatever that mod owns.
Those keys are not on the bar. The player wants a named icon on a slot that, when the slot
fires, does what pressing that key does.

This is not click-to-cast. The HUD stays `NoInputs`. It is a keybind-activated slot whose
payload is an injected press, not a FormID and not an Ability clip.

## Solution

An **Action** is a fifth thing a hotbar slot can hold. Binding one and pressing that slot
injects a single input on a seam chosen when the Action is authored. One press, one seam.
No ladder, no silent fallback, no double-fire.

Three kinds, not one generic "send a keycode":

| Kind | What the slot stores | Seam | Motivating case |
|---|---|---|---|
| Physical scancode | DX scancode + device | `ButtonEvent` onto `BSInputEventQueue` (existing shout queue), down then up | OCPA on `B` (key 48), dodge |
| Engine control | Control-map `userEvent` name | Same queue, with that `userEvent` and the mapped scancode | First-class Power Attack if the OCPA spike fails |
| VirtualKey binding | Stable `binding_id` | `VirtualKey_GetInterface(2)` → `TriggerBinding` | MCM functions already in the VK catalog |

VirtualKey is an optional guest, detected at runtime. It is not a requirement. It cannot
tap physical `B` (Tap refuses anything outside `100000–9999999`) and its native path is
`BSInputDeviceManager::SendEvent` with an empty `userEvent` — the downstream seam this
repo already proved does not reach SH2's input hook.

Costs are optional and reuse Ability Cost (stamina / magicka / health, cooldown, GCD,
refuse flash). Skip ArtDriver, Ability Selector, and clip/class gating.

## User Stories

1. As a player, I want to bind a named icon called Power Attack to a hotbar slot, so that
   a power attack is reachable the same way a spell is.
2. As a player, I want pressing that slot to fire One Click Power Attack as if I had
   pressed `B`, so I do not have to keep that key in my head during combat.
3. As a player, I want a Dodge Action the same way, so that dodge is on the bar too.
4. As a player, I want the slot to show the icon and name I assigned, so I can tell Actions
   apart from spells and Abilities.
5. As a player, I want an Action I cannot afford to refuse visibly, so a dead press is
   explained rather than silent. (Optional costs; default is free.)
6. As a player, I want a costed Action to show cooldown on the slot, so I can see when it
   is available.
7. As a player, I want a Power Attack Action pressed during a committed hotbar cast to
   chain out the way a physical OCPA press already does (mco-integration ticket 10), not
   to be eaten by the ticket-41 lockout.
8. As a player who uses Beyond VirtualKey, I want an Action that fires an existing VK
   binding, so occasional MCM functions can sit on the bar. If VirtualKey is not
   installed, those rows are hidden, not broken.
9. As a player who does not use VirtualKey, I want Power Attack and Dodge to still work,
   so combat Actions do not depend on that plugin.
10. As the fork maintainer, I want `castSlot` to drive an Action slot, so runtime
    verification uses the existing Papyrus seam.
11. As the fork maintainer, I want Action bindings to survive save/load, so they are
    durable like Ability binds.
12. As the fork maintainer, I want a down-only inject to be illegal, so we do not leave
    the phantom held button VoiceCastDriver already diagnosed.

## Implementation Decisions

### Vocabulary (belongs in `CONTEXT.md`; proposed here)

- **Action** — a bindable hotbar payload that injects input rather than starting a spell,
  shout, potion, or Ability. _Avoid_: Ability, hotkey slot, virtual key (that is VK's
  product), click-to-cast.
- **Action Kind** — physical scancode, engine control, or VirtualKey binding. Chosen in
  the editor. One kind per Action. _Avoid_: fallback chain, try-all seams.
- **Action Editor** — the ImGui that sets name, icon, kind, target (scancode /
  `userEvent` / VK binding), and optional Ability Costs. Opened from the Actions tab.
- **Action Cost** — the same meters as Ability Cost, applied before inject. Default
  zero. _Avoid_: inventing a second cost system.

Do not overload **Ability**.

### Spike first (ticket 01, owner-hands)

Inject OCPA's configured scancode (already read by `get_ocpa_keys()`, logged as device 0
key 48) through the shout-style queue: a down `ButtonEvent` then an up, keyboard device,
that scancode. Compare to a physical `B`.

- If a power attack plays: physical-scancode kind is v1. Engine-control Power Attack is
  not needed for this case.
- If it does not: physical-scancode kind is not a combat product. Power Attack becomes
  engine-control (forward + right attack `userEvent`s, VoiceCastDriver hold shape). Dodge
  waits until its own spike, or is dropped from v1.

Do not ship the bind-menu tab until this cell is closed. A pretty editor over a silent
no-op is the failure mode this spike exists to prevent.

Repeat the cell for dodge with that mod's bound scancode. Same rule.

### Dispatch

- One seam per press, selected by Action Kind. Never queue-inject then SendEvent then
  SendInput on the same activation.
- Physical scancode and engine control reuse `RE::BSInputEventQueue` +
  `ButtonEvent::Create`, the path `InputModeCast` already uses for shouts and
  `VoiceCastDriver` uses for hold/release. Down and up are both required.
- Engine control must set the control-map `userEvent` string. A scancode alone is not
  enough for PlayerControls.
- VirtualKey: `GetModuleHandleW(L"VirtualKey")` + `GetProcAddress("VirtualKey_GetInterface")`
  version 2. Call `TriggerBinding(binding_id)`. Do not tap raw 100xxx from SH2; do not
  reimplement VK's `SendEvent` backend. If the module is absent, hide the kind.
- Recursion: an Action must not inject the same key that just activated its slot.
  Fail closed, log, red flash.

### Press gate

- Ticket 41 (`current_cast` lockout) applies to costed / GCD Actions the way it applies
  to potions: a live SH2 instance refuses a new Action.
- A costless combat Action whose target is an attack press (OCPA scancode, or
  engine-control Power Attack) is an attack press for ticket 10: it may cut a committed
  Driver Cast the same way physical OCPA already does. It is not a new cast instance.
- Equip mode: refuse Actions (same as Abilities today). Direct Cast is the product.

### UI and data

- Bind-menu tab cloned from Abilities (`TabIndex_Arts` in `advanced_bind_menu.cpp`):
  clipper, drag source, right-click opens Action Editor.
- Catalogue is SH2-owned, not TESForms. Name, icon (atlas / form, same as Ability Editor),
  kind, target, optional costs. Custom rows the player fills; a few shipped defaults
  (Power Attack, empty Custom Action N) are enough for v1.
- `slot_type` gains a new arm. `serialize_skill` is currently kind 0 (form) / 1 (art).
  Kind 2 is an Action id. That is a co-save bump (`Storage::save_format` is 7). Old
  saves load; Action slots on those saves are empty.
- `BindPayload` / `apply_bind_drop` gains a third identity. Form, art, and action stay
  mutually exclusive on a slot.
- HUD draws the Action icon with the existing slot widget and cooldown overlay. No
  mouse capture.

### Ownership (ADR-0001)

- **Core Fork**: slot kind, catalogue, editor, inject path, optional VK guest adapter
  (detect-and-call, no hard link).
- **Compatibility Package**: none required. OCPA and dodge are load-order facts, not
  shipped dependencies. VirtualKey is not a requirement.

## Testing Decisions

A good test is an observable move, not a log that a `ButtonEvent` was queued.

- Ticket 01 is owner-hands: physical `B` vs injected 48, then dodge vs its scancode.
  Injected-input evidence alone is forbidden on this path (mco-integration ticket 50).
- After the product exists, `castSlot` on an Action slot is the agent seam. The owner
  still confirms OCPA/dodge identity by eye once per kind.
- Save/load of an Action bind is an agent cell.
- Missing VirtualKey must not crash; VK-kind rows absent from the tab is the pass.

## Out of Scope

- HUD click-to-cast, or removing `ImGuiWindowFlags_NoInputs` from the bar.
- Making Beyond VirtualKey a hard requirement.
- A per-press ladder (queue → SendEvent → SendInput).
- OS `SendInput`, Papyrus `Input.TapKey`, or feeding `BSPCKeyboardDevice` (ticket 50
  harness). If the physical-scancode spike fails, the answer is engine-control Power
  Attack, not a deeper raw injector.
- Rebinding other mods' MCM keys from SH2. VirtualKey already does that; SH2 only
  *triggers* a binding the player already made.
- Owning dodge or power-attack animations. An Action does not start `SH2_Art_State`.
- Ability Class gating, drawn-weapon refuse, or Cast Plant. Those belong to Abilities.
- Landing this on `origin/master` / the first-release archive. Development is
  `ng/smf-next` (`docs/agents/smf-addon-line.md`).

## Tickets

| # | File | Status |
|---|---|---|
| 01 | `issues/01-spike-injected-ocpa-and-dodge.md` | resolved — physical-scancode v1 |
| 02 | `issues/02-slot-kind-and-cosave.md` | ready-for-agent |
| 03 | `issues/03-actions-tab-and-editor.md` | ready-for-agent, blocked by 01+02 |
| 04 | `issues/04-press-path.md` | ready-for-agent, blocked by 01–03 |
| 05 | `issues/05-optional-action-cost.md` | ready-for-agent, blocked by 04 |
| 06 | `issues/06-virtualkey-guest.md` | ready-for-agent, blocked by 03; may wait |

01 is resolved. 02 is the frontier and may run alone. 03 is unblocked once 02 lands.
Do not start 04's engine-control Power Attack arm: 01 admitted the scancode kind.

## Further Notes

- OCPA key lookup already lives in `skse_plugin/src/input/input.cpp` (`get_ocpa_keys`).
  Ticket 01 should use that, not a hardcoded 48.
- VoiceCastDriver (`voice_cast_driver.cpp`) is the hold/release reference, not DevBench
  `input`.
- VirtualKey 1.3.2 API: `VirtualKeyAPI.h` in the source zip in Downloads. Do not vendor
  that tree. Consume V2. Installed runtime may lag (1.3.0 seen on this machine).
