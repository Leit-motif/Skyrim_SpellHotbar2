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

**Status:** closed 2026-09-05 — Hotbar Actions 02–05 rollup accepted by the owner

**Status (superseded — see the top):** claimed — Hotbar Actions 02–05 rollup

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

## Closure 2026-09-05

Owner: "close out ticket 02, waive the mouse and gamepad cells."

Cells, with evidence (all on `ng/smf-next`, builds `d7b0b3a` and `76737e6`, overlay
`Dev - Spell Hotbar 2 SMF Next`; log evidence in `runtime-acceptance-20260905.md` and ticket 07):

1. Slot holds an Action id, three-way exclusivity — `bind_drop_test` (extended in ticket 07
   finding 17); owner bound Actions 100–102 onto slots 6–8 live.
2. Save/load keeps the Action id — owner sessions across Save4–Save7 today; format 7 loading
   covered by `hotbar_serialization_test` (version 7 kind 2 is not an Action record).
3. Kind 0/1/2 serialization — `hotbar_serialization_test`; WART art slots restored on every load
   today (`WART: restored art 1010/1011` at 15:43:39).
4. Unit tests — CTest 18/18.
5. Twelve rows `Action 1`..`Action 12`, ids 1 and 2 hidden — `action_data_test`; owner used the
   shipped list live.
6. Overlays persist — per-install JSON sidecar, recorded above; names survived every reload today.
7. Down edge only is costed — owner: magicka cost charged once per press, stacking with TK
   Dodge's own stamina cost (13:0x); held mirror events carry no cost path (ticket 04 cell 8).
8. Mode change / game load releases the target — menu mid-hold and door transition (13:05)
   paired `reason=source up`; save-then-load mid-hold released with `reason=retry` at 14:45 and
   again at 15:43:36.

Waived by owner 2026-09-05: mouse-button and gamepad Action targets ("i can't afford" is not the
reason here — the owner does not use them: "Mouse-button and gamepad targets (owner does not use
them; skipped)"). They stay unproven and are not a release blocker.

Do not overload `slot_type::weapon_art`.
