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

## You test this

Nothing visual until ticket 03. If a loaded format-7 save loses Ability binds, it fails.

## Notes

`Storage::save_format` is 7. This bump is 8 unless the inner HOTB versioning can grow a
kind without invalidating 7 — prefer an explicit format bump over clever reuse.

Catalogue rows: `{id, name, icon, kind, target, optional costs}`. Shipped defaults can be
stub Power Attack + empty Custom Action N. Do not invent Ability Class, clip paths, or
ArtDriver here.

Do not overload `slot_type::weapon_art`.
