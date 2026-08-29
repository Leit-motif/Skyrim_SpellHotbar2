# 51 — A staff assigned to a slot leaks a cast instance: button dies, staff dies

**Type:** defect (DLL). Owner-reported 2026-08-26 during the ticket 46/48 manual pass:
"when I tried to assign a staff to spell hotbar 2, all of a sudden the button did not work,
and then the staff itself stopped working, even trying to use it manually."

**Status:** needs-triage — evidence window captured; needs the owner's answers below, then a
source read of the staff slot path.

## Evidence (SpellHotbar2.log, session of 2026-08-26 16:32, still on disk at filing time)

- The dead button is the press gate refusing on a live instance:
  `SH2: slot 0 refused by the press gate (type=3, cast live=true)` at 16:51:34.225 and
  16:51:52.112 — and `cast live` is literally `can_start_new_cast()` =
  `current_cast == nullptr` (`modes.cpp:117`, `casting_controller.cpp:1511`).
- No `casting state active became true` and no `notified` line between the last clean channel
  exit (16:51:06.771 `SH2_CastExit`) and the first refusal — the leaked instance was created
  WITHOUT reaching a graph state, on a silent path.
- The leak SELF-CLEARED between 16:51:52.1 and 16:51:52.7 (a dual-conc channel started
  normally at 16:51:53.000), with no watchdog or retire line — whatever drained it is silent
  too.
- Staff context: the owner was live-testing staves (right-hand staff casts select MSCO
  `Base - Right Staff`, observed on-screen). Vanilla staff releases appear as
  `graph raised a right SpellFire event` with NO driver cast active (16:50:33, 16:50:40,
  16:51:57–16:52:07 stream) — accepted by a STALE armed mask left from earlier dual
  channels. Likely unrelated to the leak but see "Secondary finding" below.

## What the leak is probably not

Not the spike's stuck-`IsCasting` (that refusal is in `allowed_to_cast`, whose new debug line
— `SH2 cast: refused, casting=…` — never printed this session). Not the press-gate debounce
(those refusals follow within ~60ms of a successful fire; these stood 18s+ with nothing
live).

## Questions for the owner

1. Which slot was the staff assigned to, and does assignment itself succeed (icon appears)?
2. Does the button stay dead until something specific (sheathe? wait? recast?), or forever?
3. Does the staff recover on unequip/re-equip, or only on reload?

## Where to look (source)

- What `slot_type` a staff binds as, and which `process_input` branch handles it
  (`input/modes.cpp`); whether `try_start_cast` can construct `current_cast` and then bail
  without `current_cast.reset()` on the staff path (the FF/conc paths reset on
  `graph_refused`; find the path that does not).
- What silently drained the instance ~18s later (GCD retire? `update_cast`?) — that same
  mechanism names how long the "dead button" lasts.
- Why the staff's own manual cast dies while the instance is live: candidate is the
  begin()-time equipped-caster interrupt running on each refused press attempt
  (`msco_cast_driver.cpp` begin isolation).

## Secondary finding (for the arming path's next touch)

`notify_spellfire` accepts vanilla SpellFire events whenever the LAST cast's mask is still
armed and no re-arm has bumped the generation — post-cast vanilla staff/spell releases log
`graph raised a …` and set the seen latch outside any cast. Delivery is safe (the latch is
cleared at cast start), but it pollutes the log's commitment evidence and is exactly the
kind of stale acceptance ticket 46's generation counter exists to stop. Consider clearing
the mask at cast retire alongside `clear_spellfire()`.

## Scope note

Staff slots were never in ticket 46's scope (staff CELLS are ticket 47, and hotbar staff
CASTING may be its own feature gap). This ticket is the leak, not staff presentation.

## Comments

### 2026-08-26 — owner answers, and the reframe they force

Owner's answers to the three questions above:

1. The assignment DID stick, as a **question-mark placeholder icon**. The owner adds: "I tried
   to assign the staff (item) to the bar, I don't even know if you can do this. I don't know how
   SH2 interacts with staves or how to assign them to the hotbar."
2. The dead button was **the staff's own slot**, not a neighbouring spell slot.
3. It stayed dead **until reloading an earlier save that never had the staff assigned** — no
   in-play recovery. The staff's own manual right-hand cast (left mouse button) was broken for
   the same span, and also came back only on the reload.

That question mark is `slot_type::unknown`, and it changes the diagnosis.

**Root cause of the dead button (established, static):** SH2 binds whatever form the menu has
selected with no castability check. `Input::slot_spell` (`input/input.cpp:1009`) passes the
form's ID straight to `Storage::slotSpell`, and `in_binding_menu` explicitly admits the
inventory tab (`current_inv_menu_tab_valid_for_hotbar`), where a staff is selectable. The
classifier then has no arm for `FormType::Weapon`, so `SlottedSkill::update_slot`
(`bar/hotbar.cpp:1074`) drops it in `default:` as `slot_type::unknown`. There is no staff branch
anywhere in the DLL — the only `kStaff` reference in the codebase is animation equipment-type
detection (`game_data/game_data.cpp:814`).

`slot_type::unknown` is produced in three places and **consumed in none**. `InputModeCast::process_input`
tests `weapon_art`, `spell`, `shout`, `lesser_power`, `power`, `potion` and falls off the end
(`input/modes.cpp:105`). The press does nothing — not even the red refusal highlight, which
lives in the `formID == 0` arm. So "the button did not work" is not a leak at all on this path:
it is a bind the DLL accepts, saves, renders, and then silently cannot act on. Reloading a save
without the binding is the only cure because the binding is persisted with the bar.

**The type=3 refusal in the evidence window is therefore a different slot.** A Weapon form
cannot reach `type=3`. The original filing read the `slot 0 ... type=3, cast live=true` line as the
staff's dead button; it is a spell slot refusing on a live instance. Whether that leak is real
and separate, or noise from the same session, is still open — it is no longer this ticket's
headline symptom.

**Still open: why the staff's own LMB cast died.** An `unknown` slot does nothing on press, so
the interruption has to come from somewhere holding state across presses. The candidate the
source supports is the attack-key arm at `input/input.cpp:492`: while
`is_committed_cast_holding_graph() || is_cuttable_follow_through()` admits, every attack press
runs `MscoCastDriver::cancel(pc)`, and cancel's interrupt path reaches `pc->InterruptCast(false)`
(`casts/casting_controller.cpp:1622`). A leaked instance that satisfies `is_cuttable_follow_through()`
would then kill the staff's cast on each LMB press for as long as it lives — matching both
symptoms from one cause. The log's leak self-cleared in ~18s while the owner's episode lasted
until reload, so this is a hypothesis, not a finding. Confirming it needs the owner's own press
(injected input never reaches this hook, verified 2026-08-12) with `is_cuttable_follow_through()`
logged alongside the existing trace line.

**Scope split proposed.** The bind-validation defect is small, self-contained, and reproducible
from the menu with no live-cast state: refuse a non-castable form at `slot_spell`, or classify
and refuse it visibly rather than seating an inert `unknown`. The LMB interrupt is a separate
investigation that needs a live session. Splitting them lets the first ship without waiting on
the second.

**Status:** needs-triage -> the bind defect is diagnosed and ready to spec; the LMB interrupt
needs one instrumented live session with the owner at the keyboard.

### 2026-08-26 — deferred

Owner's call: SH2 does not cover staves at all, so staff behaviour is not worth chasing now.
Deferred.

On whether the bind guard belongs in upstream's MVP: both sites are upstream's own code
(`slot_spell` in pWn3d1337's `a7b47ba` "initial commit for v2", the classifier's `default:` arm
in `3b751f5` "wip stuff"), so this is an upstream defect, not a fork regression. But it is not
MVP-severe there. Upstream has no cast driver, so an `unknown` slot is simply inert and clears
by binding over it — one action, no data loss. The damaging half the owner hit (manual LMB cast
dead until reload) traces to `MscoCastDriver::cancel`, which is fork code; upstream likely does
not reproduce it.

Still worth fixing, and worth offering upstream as a quality fix rather than an MVP gate:
`in_binding_menu` deliberately admits the inventory tab, so every weapon, armor piece,
ingredient and book reaches the same dead slot, and nothing in the codebase ever tells the
player the bind failed. The fix is a type guard at `slot_spell` — accept Spell, Scroll, Shout,
AlchemyItem; refuse the rest with the existing failure sound — before the ID reaches storage.

**Status:** deferred.

The bind guard is now filed separately as ticket 52 and does not wait on this one.


### 2026-08-29 — closed, superseded by ticket 52; residue promoted to ticket 61

The precondition for every symptom in this ticket is a staff sitting in a slot as
`slot_type::unknown`. Ticket 52's bind guard (shipped, owner-accepted 2026-08-28) refuses a
non-castable form at bind time, so that binding can no longer be created — which also means the
LMB-interrupt hypothesis (`MscoCastDriver::cancel` reached from the attack-key arm while a
leaked instance satisfies `is_cuttable_follow_through()`) can no longer be TRIGGERED through
this path, and cannot be confirmed by reproducing the owner's episode. Closing rather than
carrying an unreachable investigation. The hypothesis stays written down here: if a leaked cast
instance is ever observed again, this comment is the first thing to read.

The secondary finding is real and independently reachable, so it moves to its own ticket rather
than dying here: `notify_spellfire` accepts vanilla SpellFire events whenever the last cast's
mask is still armed. Confirmed again 2026-08-29 — a right-hand staff's own releases logged bare
`MRh_SpellFire_Event` traces at 14:57:03-08 with no driver cast active. See ticket 61.
