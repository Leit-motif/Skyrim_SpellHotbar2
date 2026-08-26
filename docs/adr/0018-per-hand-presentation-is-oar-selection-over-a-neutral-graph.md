# Per-hand cast presentation is OAR selection over one neutral graph matrix

Date: 2026-08-26

Status: accepted

## Context

The MSCO Driver Cast integration collapsed every fire-and-forget cast onto four left-named
clips: `MscoCastDriver::begin` discarded the resolved hand, `arm_spellfire` armed only the
left SpellFire event, and both vanilla-isolation seams interrupted only the left caster. The
owner wants upstream's left/right/dual distinction back, plus staff variations — across every
cast family (ticket 44). Two shapes were on the table: **A**, keep the four Nemesis states
neutral and let OAR pick the hand/equipment variant of each clip path; **B**, register
first-class left/right/dual state sets in the graphs and select by entry event.

The handoff's decision rule: spike whether OAR reliably sees the two selection globals inside
a live shtb state in both hosting graphs, and whether a right-hand SpellFire clip commits
exactly once without vanilla firing an equipped right-hand spell. Shape B wins if OAR cannot
see the state or the symmetric commitment machinery proves substantial.

The spike (2026-08-26, live on the CS-Test save, both stances) answered both:

- OAR selected probe submods conditioned on `SpellHotbar_CastingSource` (`0x835`) and on
  `SpellHotbar_SpellAnimationType` (`0x815`) inside `SH2_CastRight_State`, by name in the OAR
  Animation Log, in both the drawn-1H stance and the magic stance
  (`.scratch/mco-integration/evidence/44-spike/right-cast-magic-2.png` shows the winning
  submod on-screen mid-cast). The globals are written before the entry notify in all four
  start paths, and OAR evaluates them per activation.
- A clip carrying `MRh_SpellFire_Event` committed at its authored frame ("graph raised a
  right SpellFire event", "armed payload delivered at its own SpellFire"), exactly once, with
  Firebolt equipped in the right hand and the right caster interrupted before vanilla saw the
  event. A dual-armed cast accepted the stock clip's MLh and delivered once.
- The symmetric machinery turned out to be four small seams, not a rework: a per-hand arming
  mask (the two-bit mask already existed), a hand-mapped isolation predicate in the animation
  event hook, `MRh_SpellFire_Event` registered in `1hm_behavior`'s event tables, and the
  graph-side commitment predicate accepting either hand's SpellFire.

## Decision

**Shape A.** The four Driver Cast graph states stay neutral and left-named
(`SH2_CastRight`/`SH2_Cast2-4` on `Animations\MSCO_left1-4.hkx`); hand, dual, and staff
presentation is selected by OAR submods conditioned on the two ESP globals plus
`IsEquippedType 8` per physical hand, exactly the vocabulary upstream's own tree already
uses. The graph grows no new cast states for hands.

The SpellFire contract becomes per-clip and per-hand:

- Each variant clip carries the SpellFire event of the hand it was authored for; both
  `MLh_SpellFire_Event` and `MRh_SpellFire_Event` are registered in both hosting graphs.
- `arm_spellfire(hand)` arms the resolved hand's event (dual arms both); ticket 43's latch
  still delivers once.
- The animation event hook isolates the caster the arriving event names, and swallows the
  event before vanilla, symmetrically.
- The graph-side commitment point (`is_msco_combo_window_open_event` — combo window,
  clip-committed, cast-index advance) is ANY hand's SpellFire.

Assets follow the owner's ruling: MSCO's shipped `MSCO_right*` and `MSCO_dual*` clips fill
the right and dual cells, and MSCO's staff submods are already the Dragon Age staff
animations, so no donor adaptation pipeline is needed.

## Consequences

- The "left" in the state and path names is now an explicit lie the module owns: the path is
  a neutral key OAR replaces, documented here and at `MscoCastDriver::begin`. Renaming would
  break `kCastEvents`, the Nemesis patch, and every log in the evidence tree for zero
  behavior.
- Every new pack must author the condition matrix correctly — a wrong token silently no-ops
  at load. The Animation Log (and its `bAnimationLogWriteToTextLog` echo to
  `OpenAnimationReplacer.log`) is the verification instrument; the spike proved it names
  winners headlessly.
- MSCO's own submods compete on the same clip paths (`Base - default` at 6700, its staff
  pairs at 6800/6801, all far below fork priorities). SH2's cast pack must either
  outprioritize them per cell or deliberately compose with them; the staff cells in
  particular must decide compose-vs-own before shipping.
- Event registration in a hosting graph is TWO index-aligned arrays: names (`#0085`) and
  `eventInfos` (`#0087`) in `1hm_behavior`. Adding a name without its info entry fails the
  whole graph's HKX output at Nemesis time (observed: ERROR 1003).
- Dual selection stays encoded as its own animation-type id with `CastingSource` left at 0 —
  upstream's convention, kept. The dual-over-1.51s leak into the single-hand family is a bug
  against this contract and is filed separately.
- MSCO's right/dual clips must be audited for SpellFire annotations before they fill cells;
  clips without one need the event stamped (the Art Pack's in-DLL stamping is the
  precedent). A missing annotation degrades to ticket 43's clip-end fallback rather than
  breaking cadence — the contract's designed failure mode.
