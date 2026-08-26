# 52 — Bind guard: refuse a non-castable form instead of seating an inert slot

**Type:** defect (DLL). Split out of ticket 51 on 2026-08-26 after the owner's answers reframed
that ticket. 51 is deferred (staves are out of scope); this half is independent of staves and
worth shipping on its own.

**Status:** ready-for-agent — root cause established statically, fix is local, acceptance is
reproducible from the menu with no cast state involved.

## The defect

`Input::slot_spell` (`input/input.cpp:1009`) binds whatever form the menu currently has
selected, passing its FormID straight to `Storage::slotSpell` with no check that the form is
castable. `in_binding_menu` (`input/input.cpp:1032`) explicitly admits the inventory tab via
`RenderManager::current_inv_menu_tab_valid_for_hotbar()`, so the bind surface includes a menu
that is mostly non-castable items — weapons, armor, ingredients, books, misc.

`SlottedSkill::update_slot` (`bar/hotbar.cpp:1000`) then classifies only Scroll, Spell, Shout
and AlchemyItem. Everything else falls to `default:` as `slot_type::unknown`
(`bar/hotbar.cpp:1074`).

**`slot_type::unknown` is written in three places and read in none.**
`InputModeCast::process_input` tests `weapon_art`, `spell`, `shout`, `lesser_power`, `power`
and `potion`, then falls off the end of the chain (`input/modes.cpp:105`). The press does
nothing at all — not even the red refusal highlight, which lives in the `formID == 0` arm one
level out. The slot renders the generic missing-icon fallback (a question mark), which is not
wired to `unknown` and does not communicate failure.

Net effect: the mod accepts an invalid bind, persists it with the bar, renders it as though it
were a binding, and then silently cannot act on it. Nothing in the codebase ever tells the
player the bind failed.

## Provenance: this is upstream's, and it is not their MVP gate

Both sites are pWn3d1337's own code — `slot_spell` in `a7b47ba` ("initial commit for v2",
2024-05-12), the classifier's `default:` arm in `3b751f5` ("wip stuff", 2024-12-29). Not a fork
regression.

Severity is nonetheless higher in this fork than upstream. Upstream has no cast driver, so an
`unknown` slot is merely inert and clears by binding over it — one action, no data loss. In
this fork the same inert slot sat alongside the staff's manual right-hand cast dying until
reload, which ticket 51 traces to `MscoCastDriver::cancel`. That mechanism is unconfirmed and
stays with 51; it is the reason to fix the guard here first rather than wait on an upstream PR.

Offer the guard upstream as a quality fix after it lands here. Do not frame it to them as
MVP-blocking.

## The fix

A type guard at the bind seam, before the ID reaches storage. `Input::slot_spell` is the right
place: it is the single choke point for menu binding, and refusing there means storage, the
save, and the renderer never see an invalid ID.

- Accept `FormType::Spell`, `FormType::Scroll`, `FormType::Shout`, `FormType::AlchemyItem` —
  the exact set `update_slot` can classify. Derive the set from that switch rather than
  restating it, so the two cannot drift.
- Refuse everything else: return `false`, play the existing failure sound
  (`Input::sound_UIMenuCancel`, as `try_cast_power` uses for a refused shout), and leave the
  slot untouched. A `RE::DebugNotification` is optional and probably noise; the sound plus the
  slot not changing is the feedback.
- Log the refusal at debug with the form type, so a future report of "it won't bind" is one
  grep rather than a session.

**Do not** widen this into handling `unknown` downstream. The point is that an unbindable form
never becomes a slot. `slot_type::unknown` stays as the classifier's defensive default for a
form that changes type or goes missing after binding — that is a different, real case, and this
ticket does not touch it.

**Do not** touch `in_binding_menu`'s admission of the inventory tab. Potions are bound from
there and that is intended.

## Acceptance

Live, owner or agent — no cast state, no combat, no staff required:

1. Open the inventory tab, select a plain weapon (an iron sword is fine), press a hotbar bind
   key. Expected: failure sound, slot unchanged, one debug line naming the refused form type.
   Today: the slot takes a question-mark icon.
2. Same with an ingredient or a book. Same expectation.
3. Regression, the four accepted types still bind: a spell from the magic menu, a shout, a
   scroll, a potion. Each seats and fires as before.
4. Regression, the refused bind leaves no trace: save, reload, confirm the slot is still empty
   rather than holding a persisted `unknown`.

A pre-existing `unknown` slot from an earlier save is out of scope — it stays inert and the
player rebinds over it. Note it in the comments if it turns out to be worth its own cleanup.

## Notes

- Ticket 51 holds the staff investigation and the `MscoCastDriver::cancel` hypothesis. This
  ticket does not depend on it and must not wait for it.
- Ticket 47 (staff cells) is unrelated presentation work.
