# 29 — An interrupted swing counts toward the combo

Status: claimed

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

## Design decision (2026-08-24, this session)

Two findings reframe the candidate list:

1. **The `@SGVI` advance almost certainly DOES reach the sink — in the event's `payload` field.**
   The engine splits an annotation `PIE.@SGVI|MCO_nextattack|3` at the first `.`: tag `PIE`,
   payload `@SGVI|MCO_nextattack|3`. The driver already receives tag-`PIE` events (it keys the
   reset re-write on them), but `Animation_event_hook` never forwards `a_event->payload` — the
   16e0b82 parser only ever saw tags. So "SGVI never reaches the sink" was measured against the
   wrong field. Pending live confirmation this run.
2. **The pre-advance sample is disambiguable.** At `MCO_AttackInitiate` the variable reads back
   as the playing swing's index N (measured 2026-08-24). A begin()-time live sample equal to N
   is therefore pre-advance (the swing would replay); a sample different from N is the successor.

Mechanism (preserve, never derive — every number is one a clip taught):

- Forward the payload; parse `@SGVI` out of it. The advance is then sampled at the moment the
  clip writes it, which also catches an advance landing inside the ShoutMCO deferral gap.
- Track the open swing: `(kind, playing index, taught-by-restore)` opened at
  `MCO_AttackInitiate`/`MCO_PowerAttackInitiate` (real swings only), closed at
  `attackStop`/`MCO_EndAnimation`/our own clip's initiate.
- Learn a successor table `successor[weaponType][playing] = advance value` from the clips' own
  teachings (payload edge primary, WinClose pair fallback). Entries are pack data observed at
  runtime, never arithmetic; swings entered off a restore don't teach pairs (their playing index
  is unverified while ticket 30 is open). Weapon-type keying stops a pack switch from handing a
  successor into a moveset that never taught it.
- At begin(), a pre-advance sample (v == playing) substitutes the learned successor; unknown
  successor keeps today's behavior (truthful replay) and logs it.

The ready-enter/AttackState-exit reset payloads (`@SGVI|MCO_nextattack|1` from #0009/#0786) are
excluded from learning by the swing tracker + IsAttacking gate — recording them would re-open the
exact stomp the rolling cache exists to outlive.

Rejected: delaying the cut to the advance point (up to ~0.8s input latency), a static
annotation-dump table (requires replicating OAR's runtime condition resolution), and `sample+1`
(ticket 11's standing rejection — wrap points are pack data).

## Dependencies

Blocked at the finish line by ticket 30 for 2H weapons (the restore is ignored there entirely);
the 1H path is testable now.
