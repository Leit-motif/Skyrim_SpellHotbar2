# 06 — VirtualKey guest: TriggerBinding

**Type:** task

**What to build:** If `VirtualKey.dll` is loaded, an Action Kind can be a catalog binding
fired through `VirtualKey_GetInterface(2)` → `TriggerBinding(binding_id)`. If the DLL is
absent, that kind is hidden and SH2 still loads. Not a requirement.

**Blocked by:** 03. Can wait; not on the 01 critical path. Tickets 04–05 may ship without
this.

**Status:** ready-for-agent

## You test this

VirtualKey installed (this list has it).

1. Actions tab / editor lists VK bindings by display name / `VKxxx` / alias, not raw
   100xxx.
2. Bind one that already fires a known MCM function from VK's own UI. Press the slot.
   That function runs.
3. Hide `VirtualKey.dll` (or test on a profile without it). SH2 loads. The VK kind is
   absent. Physical / engine Actions still work.

A crash on missing VK fails. Tapping physical `B` through VK fails this ticket's design —
VK Tap refuses non-100xxx keys. Do not implement that.

## Agent tests the rest

4. Detect via `GetModuleHandle` + `GetProcAddress("VirtualKey_GetInterface")`. No static
   link. Version 2 prefix. Missing / old interface: hide kind, log once.
5. `castSlot` on a VK Action calls `TriggerBinding`, not SH2's scancode queue for that
   slot.

## Notes

Source of truth: VirtualKey 1.3.2 `VirtualKeyAPI.h` (zip in Downloads; do not vendor the
tree). Consume V2. Installed runtime may lag the zip (1.3.0 seen on this machine).

Do not reimplement `BSInputDeviceManager::SendEvent` with an empty `userEvent`. That is
VK's native backend and the downstream seam ticket 50 already ruled out for SH2 combat.

Combat Actions stay on tickets 01/04. This ticket is MCM utilities already in the VK
catalog.
