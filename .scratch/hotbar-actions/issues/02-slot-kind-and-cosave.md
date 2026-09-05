# 02 — Action slot kind, catalogue, and co-save

**Type:** task

**What to build:** `slot_type` grows an Action arm. Slots store an action id, not a FormID
and not an `art_id`. Kind 2 serializes. Co-save bumps. An empty catalogue exists so later
tickets have somewhere to put rows. Press may be a stub (red flash / log).

**Blocked by:** Nothing. Ticket 01 decides *which kinds exist*; this ticket does not wait
to land the identity.

**Status:** ready-for-agent

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
