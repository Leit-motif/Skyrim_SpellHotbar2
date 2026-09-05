# 02 — Hotbar Actions rollup: slot kind, catalogue, editor, press, and co-save

**Type:** task

**What to build:** This is the claimed implementation rollup for tickets 02–05. Build the
Hotbar Actions product slice: `slot_type` grows an Action arm; slots store an action id, not
a FormID and not an `art_id`; kind 2 serializes; the co-save bumps; the Actions catalogue,
SMF bind-menu/editor, physical Action press path, and optional Action costs land together.
VirtualKey remains ticket 06. The product slice is split into isolated implementation work,
but this ticket is the single acceptance/closure record.

**Blocked by:** Nothing. Ticket 01 decides *which kinds exist*; this ticket does not wait
to land the identity.

**Status:** claimed — Hotbar Actions 02–05 rollup

Tickets 03–05 are superseded as standalone work and their acceptance criteria are carried
by this rollup.

## Agent tests this

1. A slot can hold an Action id. Binding a spell or Ability onto that slot clears the
   Action; binding an Action clears form and art (`BindPayload` stays mutually exclusive).
2. Save, load. The Action id is still on the slot. Format 7 saves still load; Action slots
   on those saves are empty.
3. `serialize_skill` kind 0 = form, 1 = art, 2 = action. Old kind-1 Ability slots still
   load.
4. Unit / bind-drop tests cover the third identity the way `bind_drop_test.cpp` covers art.
5. Twelve generic rows named `Action 1` through `Action 12` (ids 100–111) are present in
   the catalogue. Ids 1 and 2 load but are hidden.
6. A save written before this change keeps its assigned Action names and slot overlays.
   Recorded 2026-09-05 (ticket 07 finding 9): Action overlays -- name, icon, target, costs --
   are NOT co-save state. They live in `action_overlays.json` under the icon-edits user dir and
   are shared across every character on the install, matching `art_icons.json`. That is the right
   scope: the mod hotkeys they mirror (OCPA, TK Dodge, Timed Block) are per-install settings too.
   Only the slot assignment is per-character co-save state.
7. Only the initial down edge is costed: a held slot charges cost, cooldown, and GCD once.
8. A mode change or a game load during a held mirror releases the target.

## You test this

Nothing visual until ticket 03. If a loaded format-7 save loses Ability binds, it fails.

## Notes

`Storage::save_format` is 7. This bump is 8 unless the inner HOTB versioning can grow a
kind without invalidating 7 — prefer an explicit format bump over clever reuse.

Catalogue rows: `{id, name, icon, kind, target, optional costs}`. Ship twelve configurable
rows named `Action 1` through `Action 12`, ids 100–111. Superseded 2026-09-05: the earlier
"stub Power Attack + empty Custom Action N" default set is withdrawn — Power Attack (id 1)
and Dodge (id 2) were ticket 01 test fixtures and remain hidden catalogue entries only so
previously saved bindings still load. Do not invent Ability Class, clip paths, or ArtDriver
here.

Native SKSE only: no new Papyrus scripts, no new dependencies, minimal changes.

All runtime cells here are unproven.

Do not overload `slot_type::weapon_art`.
