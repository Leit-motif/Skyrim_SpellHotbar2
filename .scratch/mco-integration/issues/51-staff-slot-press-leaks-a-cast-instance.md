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
