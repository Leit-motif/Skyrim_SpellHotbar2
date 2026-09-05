# 03 — Actions tab and Action Editor

**Type:** task

**What to build:** A bind-menu tab of Action rows. Drag onto a slot. Right-click / Edit
opens the Action Editor: name, icon, kind, target, optional costs (fields only; charging
is ticket 05). Kind pickers that ticket 01 rejected stay hidden.

**Blocked by:** 01, 02

**Status:** superseded — rolled into 02

**Status (superseded — rolled into 02):** ready-for-agent

The bind menu on this line is the SMF guest window (`advanced_bind_menu.cpp`, SMF ticket
04). Add the tab there. Do not restore a second ImGui host.

## You test this

Open the bind menu (existing Open Binding Menu bind, Magic Menu or supported inventory).

1. There is an **Actions** tab. It lists named Actions, at least Power Attack if 01
   admitted that kind.
2. Drag Power Attack onto an empty slot. The slot shows that icon and name, not a spell
   and not an Ability.
3. Right-click the row. Editor sets name and icon. The slot updates.
4. Bind a spell on another slot from Spells. That slot stays a spell. The Action slot is
   untouched.

If Actions appear as fake spells or fake Abilities, it fails.

## Agent tests the rest

5. Drag the Action off. Slot empty.
6. Rebind the same slot spell → Action → Ability. Each bind replaces the previous kind.
7. Save, load. The Action is still on that slot (ticket 02 serialization).
8. If ticket 01 dropped a kind, that picker is absent. VirtualKey rows: hide when
   `VirtualKey.dll` is missing (ticket 06 owns the tap; this ticket only hides the picker).

## Notes

Clone the Abilities tab clipper / drag-source / `InvisibleButton` pattern. Icon picking
reuses Ability Editor atlas / form icon, not a new art pipeline.

Scancode capture: next ButtonEvent while the editor is armed, same idea as key rebind.
Engine-control: pick from a small `userEvent` list (at least the attack chord if 01
failed OCPA). Do not type raw 100xxx; VK bindings wait for ticket 06's list.
