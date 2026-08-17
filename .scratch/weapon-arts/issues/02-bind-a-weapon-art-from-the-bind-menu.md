# 02 — Bind a Weapon Art from the bind menu

Ticket 01 plays an art through `slotArt` / `castSlot`. Players bind spells by opening the bind
menu and dropping a form onto a slot. Arts have to work the same way, without Papyrus.

**Status:** claimed

**Blocked by:** 01

## You test this

Weapon drawn, 1h. Open the bind menu (the existing advanced bind hotkey).

1. There is an **Arts** tab (or equivalent), listing named arts from the catalogue — at least
   Test Art.
2. Bind Test Art onto an empty slot. The slot shows the art's icon and name, not a spell.
3. Close the menu, press that slot. The art plays. Hands unchanged.
4. Bind a spell onto a different slot from the Spells tab. That slot is still a spell. The art
   slot is untouched.

If arts appear as fake spells, or if binding one requires `slotArt`, it fails.

## Agent tests the rest

5. Drag the art off the slot (or bind empty). The slot is empty; pressing it does nothing.
6. Rebind the same slot to a spell, then back to an art. Each bind replaces the previous kind.
7. Save, load. The art is still on that slot (WART / serialisation from ticket 01).

## Notes

The bind menu (`advanced_bind_menu.cpp`) is FormID-keyed: `load_spells()` fills TESForms, tabs
filter by spell school / shout / potion. A Weapon Art has **no FormID** — `ArtDefinition.id` is
the identity. Do not stuff arts into `TESForm*` or into the Spell Editor.

The Spell Editor (`spell_editor.cpp`, `User_custom_spelldata`) overrides gcd/cooldown/anim on an
existing spell form. That is the wrong seam for registering arts. An in-game *custom art
definition* editor, if we want one later, is a new list keyed by art id — after the catalogue
exists (ticket 03). This ticket only binds catalogue rows onto slots.

`Dragged_skill` currently holds `const RE::TESForm*`. It will need a parallel art-id path, or a
small tagged payload, so a drag can be either a form or an art.

## Comments

Claimed 2026-08-17. Bind menu gained an **Arts** tab that lists `ArtDefinition` rows by art id
(not `TESForm*`). Drag payload carries `art_id` beside the form; drop uses `apply_bind_drop` so
an art bind clears FormID and a form bind clears art id. Slot icon/name resolve through
`draw_art_icon` / `resolve_slot_name`.

Unit tests (`bind_drop_test`, `art_data_test`, `art_bind_record_test`, `combo_cache_test`) green.
Plugin Release built to `Dev - Spell Hotbar 2`.

Owner cells 1–4 still open (menu bind, play, mixed spell slot). Agent 5–7 wait on that bind.
