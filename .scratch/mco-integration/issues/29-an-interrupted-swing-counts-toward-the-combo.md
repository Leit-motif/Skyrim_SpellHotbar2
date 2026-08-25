# 29 — An interrupted swing counts toward the combo

**Status:** resolved 2026-08-24 -- mechanism in ADR-0014; probes retired after owner acceptance.

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

## Answer (2026-08-24 evening, commits 638be75 + 86ceadf, verified live)

An interrupted swing now hands its successor on, on all three weapon classes, from either side
of the advance. Mechanism as designed above, plus one correction the first live run forced.

**Implementation** (`combo_cache.h`, `msco_cast_driver.cpp`, `animationeventhook.cpp`):
- The hook forwards `a_event->payload`; the SGVI parser runs over it. Confirmed live: the
  advance arrives every swing as tag `Pie` + payload `@SGVI|MCO_nextattack|N`. (Note the case:
  `Pie`, not `PIE` — the old `is_reset_payload` tag compare never matched, so BOTH pre-existing
  stomp-putback branches were dead code until now.)
- `McoSwingTracker` opens at `MCO_AttackInitiate` with the variable's read-back (= the playing
  index, reconfirmed live), closes at `attackStop`/`MCO_EndAnimation`/our own clip's initiate.
- `McoSuccessorTable` learns `successor[weaponType][playing] = taught value` from the payload
  edge, with the WinClose pairing as fallback. Swings whose initiate consumed a restore record
  but never teach pairs.
- `begin()`: a live sample equal to the open swing's playing index is pre-advance → substitute
  the learned successor; unknown → keep the sample (truthful replay), logged as such.
- **Correction from the first live run (86ceadf): a cut swing's AttackState-exit notify beats
  `attackStop` to the sink** — it arrived with the tracker open and IsAttacking still 1, recorded
  its `1` over the good sample, and taught `successor[1]=1`. A reset always teaches 1 and a clip
  can never teach itself, so `payload_advance_is_recordable` quarantines value 1 from the payload
  edge entirely; wrap pairs (Mercenary's 4→1) ride the reset-safe WinClose sampler instead.

**Evidence (SpellHotbar2.log, clip identity from the packs' own advance payloads — attack N is
the only clip that teaches N+1 in these packs, so the payload names the playing clip from pack
data, not graph teachings):**
- 1H sword, pre-advance (16:58:02): `begin combo resolution swing=open playing=1 | next:
  pre-advance, substituted=2 (was 1)` → armed 2 → follow-up swing's advance reads `playing=2
  taught 3` = attack2 played. The interrupted a1 handed a2 on.
- 1H sword, post-advance (16:57:34): `post-advance, keeping 2` → attack2 played. The coin-flip
  is gone: both press timings land on the successor.
- Greatsword (17:00:12): `pre-advance, substituted=2 (was 1)`; the cut swing's real advance
  even landed in the press-to-cut gap and re-confirmed 2; follow-up = attack2 (`playing=2
  taught 3`, `restore_taught=true`). **The 2H entry honored the restore — see ticket 30's
  answer for why it now does.**
- Warhammer (17:02:04): `post-advance, keeping 2` → armed 2/2 → follow-up = attack2.
- Reset payloads in every position (ready-enter, AttackState-exit, cut-swing death rattle)
  logged and refused: `no open swing -> ignored` / `reset-valued (1) -> left to the WinClose
  edge`.

**Standing agreements honored:** the successor is never derived — every table entry is a value
a clip taught, wrap points included; the chain-out window stays clip-driven (kMaxAgeMs
untouched); `credit_held_time` untouched, so a concentration hold still never ages the combo
out. Known, logged degradation: the table is per-session memory, so the first pre-advance
interrupt of an index the session has never seen completes falls back to today's replay (Elder
Creed's attack5 carries no advance at all, so successor[5] stays honestly unknown forever).

Owner cells left open: feel-check with a real held channel and physical presses (the game is
left running on the fixture save, iron warhammer equipped, sword and greatsword in inventory);
optional OAR Animation Log cross-check of the clip oracle.
