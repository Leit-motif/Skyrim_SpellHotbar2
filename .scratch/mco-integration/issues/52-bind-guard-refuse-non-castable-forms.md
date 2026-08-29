# 52 — Bind guard: refuse a non-castable form instead of seating an inert slot

**Type:** defect (DLL). Split out of ticket 51 on 2026-08-26 after the owner's answers reframed
that ticket. 51 is deferred (staves are out of scope); this half is independent of staves and
worth shipping on its own.

**Status:** built, awaiting live acceptance — guard implemented and the DLL compiles and links
clean; the acceptance pass below is not run. See the comment at the bottom.

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

## Comments

### 2026-08-28 — built

Implemented as specced, three files, 26 lines added and none removed:

- `SlottedSkill::is_bindable_form(RE::FormID)` (`bar/hotbar.h:70`, `bar/hotbar.cpp:1098`) —
  rather than restating the accepted form types, it runs the classifier on a throwaway
  `SlottedSkill` and refuses anything that lands in `slot_type::unknown` (or `empty`). The two
  cannot drift because there is only one list, and it is the switch itself.
- `Input::slot_spell` (`input/input.cpp:1011`) refuses before the FormID reaches
  `Storage::slotSpell`: `logger::debug` naming the FormID and the numeric form type,
  `RE::PlaySound(sound_UIMenuCancel)`, `return false`. The slot is never touched.

Two things worth recording about the derived-set approach. The clear/unbind spell still binds:
`update_skill_assignment` short-circuits it to `slot_type::blocked` before the switch, so the
guard admits it. And a Spell form whose `As<RE::SpellItem>()` comes back null is now refused
rather than seated as `unknown` — the classifier's own defensive arm, reached at bind time
instead of after.

Not done: nothing downstream of `unknown` was touched, and `in_binding_menu` is unchanged.

**Build:** compiles and links clean (43/43, warnings pre-existing). The post-build deploy copy
into the MO2 mod folder FAILED — `SkyrimSE.exe` (pid 71576) and MO2 were live at build time and
hold the deployed DLL. The fresh binary sits at `skse_plugin/build/release/SpellHotbar2.dll`
(2026-08-28 20:34); the deployed copy is still 2026-08-26. Re-run `build-release.bat` with the
game closed to deploy, then run acceptance 1-4.

### 2026-08-28 — live session: the press cells are owner-hands, proven not assumed

Launched the guarded DLL (deployed 21:13, game up 21:19, owner's latest save, `playerLoaded`
frame 11648) and tried to drive acceptance 1 headlessly. It cannot be driven. The proof is a
contrast inside one session, not the playbook's general warning:

- Injection is alive: `input {key:15, userEvent:"Tween Menu"}` opened TweenMenu, confirmed via
  `menu action=list`.
- The bar is alive and slot 0 is bound: `SpellHotbar.castSlot(0)` ran a full cast — graph events,
  `SH2_CastExit`, `SH2_ArtSelector=0`, the usual trace.
- The same slot-0 bind key (`GetKeyBind(0)` = scan 75, Numpad 4) injected as a tap produced
  **zero SpellHotbar2 log lines** — both with InventoryMenu open (the bind path) and in-game
  (the press path). Not a refusal, not a press-gate line. Nothing.

So `Input::processAndFilter`, which is the only caller of `Input::slot_spell`, never sees an
injected event: DevBench's events reach MenuControls/PlayerControls but bypass SH2's hooked
dispatch site (`RELOCATION_ID(67315, 68617)`). This is the playbook's "physical input path /
input-hook changes -> owner hands" row, now with a positive control on both sides. Ticket 50's
harness is what would close it.

**Oracles ready for the owner's pass** (no pixels needed):
- `SpellHotbar.saveBarsToFile(path)` dumps the live bars to JSON — that answers "slot unchanged"
  and "the accepted types still seat". Pre-press snapshot taken this session.
- The guard's own `logger::debug` line in SpellHotbar2.log names the refused FormID and form
  type. A successful bind logs nothing (`Storage::slotSpell_internal` is silent), so the JSON is
  the oracle for cell 3, and the log is the oracle for cells 1 and 2.

Game left running for the owner's presses.
