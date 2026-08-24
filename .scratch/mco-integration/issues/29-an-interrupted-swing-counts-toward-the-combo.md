# 29 — An interrupted swing counts toward the combo

Opened 2026-08-24, carved out of ticket 28. Owner ruling, verbatim intent:

> should a swing that's interrupted after its hit but before its advance still count? — yes. the
> goal is a combo. the user sees it as an animation, not a "hit frame" — that's how the sausage
> is made.

Plus the standing design agreements this must honor:

- The chain-out window is **dynamic per weapon / animation length**, not a fixed constant.
- **Concentration spells delay/stop/extend the "timer"** — the hold itself never ages the combo
  out (the `credit_held_time` mechanism in `combo_cache.h` implements part of this).

## Current behavior (measured 2026-08-24, 1H sword)

The begin()-time live sample (`fc0c3a1`) reads `MCO_nextattack` at the moment the cast starts,
gated on `IsAttacking`. The clip's advance annotation fires mid-clip (at `MCO_WinOpen` time —
pack data, see `28-entry-seam-analysis-2026-08-24.md`). Result: a cast pressed after the advance
restores the NEXT attack (a1→a2→cast→a3 ✓, owner-verified via OAR log); a cast pressed a beat
earlier truthfully samples the pre-advance value and the interrupted swing REPLAYS
(a1→a2→cast→a2 — owner sees this as the bug it is). Same fixture, coin-flip on press timing
relative to the advance point. Driver log discriminates the cases: `begin live sample next=2`
vs `next=3`, both honored faithfully downstream.

## The design question

Make an interrupted swing hand its SUCCESSOR on, regardless of where in the clip the interrupt
lands. Constraints and tensions to resolve:

- Ticket 11's "preserve, never derive" rejected `sample+1` because moveset LENGTH lives in the
  clip annotations (a derived index can run off the end of a shorter moveset, and wrap points
  are pack data — Mercenary Greatsword chains 1,2,3,4,9,10 with attack4→1).
- The truthful "what comes after the playing swing" value exists in-graph only after the clip's
  own advance fires; before that moment nothing in the graph knows it.
- Candidate directions (weigh, do not assume): delay the cut until the advance has fired (the
  deferral window already spans ~270ms; the advance is often within it — the WinClose sampler
  sometimes catches it already); author a graph-side write on SH2's transition; treat the OAR
  clip set / annotation dump as the successor table (a read of pack data, arguably still
  "preserve"); or re-open the derive rule with the wrap table made explicit.
- Whatever the mechanism, verification is by the OAR Animation Log clip name (or the
  `MCO_currentattack` probe if ticket 30 makes it readable) — never by graph teachings alone
  (trap documented twice in `28-progress-2026-08-24-combo-fix.md`).

## Dependencies

Blocked at the finish line by ticket 30 for 2H weapons (the restore is ignored there entirely);
the 1H path is testable now.
