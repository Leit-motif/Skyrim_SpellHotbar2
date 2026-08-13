# 11 — Make a hotbar cast a first-class member of the attack chain

**Type:** feature (Nemesis patch + driver), the owner's 2026-08-12 MVP definition

**What to build:** A hotbar cast that behaves like an MCO attack in every respect that matters —
it continues the combo it interrupts, it has combos of its own, and it chains with MSCO hand casts
in **both** directions.

**Blocked by:** Nothing. Ticket 10 shipped the cut this builds on.

**Status:** split — do not implement this file; work 12–15. Acceptance stays open on the children.

## State this ticket starts from

- **Baseline:** `spell-hotbar-2` branch `main`. Ticket 10 shipped the cut this builds on and is
  owner-verified (32 of 32 cuts chained into a swing).
- **One change is built but NOT deployed:** power-attack chaining (SH2 reads OCPA's key from its own
  config). It compiles and links clean; the copy into `Dev - Spell Hotbar 2` failed with the game
  holding the DLL, so **the deployed DLL is one build behind the source**. Re-run
  `skse_plugin/build-release.bat` with Skyrim closed before testing anything.
- **Nothing in this ticket is started.** No graph edit, no driver change, no clip work.
- **One question is out and unanswered:** `../questions/question-2026-08-12-sh2-to-shoutmco-combo-index-ownership.md`,
  also at `%TEMP%`. It asks whether ShoutMCO ever writes `MCO_nextattack` for an SH2 driver cast. A
  "no" closes the collision risk; a "yes" changes the design. Do not start the write path without
  reading its `## Answer` section.

## The owner's words, because the scope is theirs and not mine to trim

> "it works as an mvp, but does not preserve combo state… this must function in the same way as an
> mco attack. it must continue the combo, it must also have combos (we will have to figure out how
> to chain animations for consecutive casts). also needs to work — meaning chain — with MSCO casts.
> Into and out of. that is the mvp. not just into, but also out of."

## This is SH2's work. Saying otherwise was wrong.

Ticket 08 filed combo-position continuity as "ShoutMCO ticket-50 API, thuum repo", and the
2026-08-12 handoff repeated it. **That is wrong for this direction and should not be repeated.**
The `shtb` patch is this repository's: `SH2_CastRight_State`, its entry transition, its exit
transition, and its clip are all authored in `nemesis/Nemesis_Engine/mod/shtb/`. Where the cast
state sits in the combo chain is therefore a property of a file this repo owns. ShoutMCO's
cast-intent API answers a different question — whether a press may *start* a cast during someone
else's animation — and cannot answer this one.

## What resets the combo, from ticket 10's own trace

The cut works, and the trace shows exactly why the swing that follows is `attack1`:

```
41.620  notified SH2_CastExit -> true
41.626  attackStop, SBF_ReadyStart, MSCO_MagicReady      <-- the graph passes through READY
41.627  SBF_NormalAttackStart, MCO_AttackInitiate        <-- a fresh combo, from the top
```

`#shtb$1.txt` — the state-local exit transition — is authored `toStateId 0`, which is
`1HM_Ready_State`. **Every cast therefore returns to the drawn idle before anything else can
happen, and an attack launched from idle is by definition the first of a combo.** The reset is not
MCO defending itself; it is this patch routing through the one state that means "no combo in
progress".

The obvious reading of that is "rehome the cast state inside MCO's chain". The mechanism section
below shows there is a much cheaper fix that leaves the transition alone.

## The mechanism, answered — thuum already solved this and wrote it down

**No spike needed. Do not re-derive any of this.** The sibling project built "resume MCO's combo
from outside" for shouts and closed both of its gating questions with runtime evidence. Sources:
`thuum-fully-animated-shouts-mco/CONTEXT.md` and its `docs/adr/0005-shouts-are-orthogonal-to-the-mco-combo.md`.

- **Combo position is a graph VARIABLE, readable and writable from a DLL.** The set is
  `MCO_nextattack`, `MCO_nextpowerattack`, `MCO_currentattack`, `MCO_currentpowerattack`. Reads
  return real values from a *different graph's* context — "cross-graph linkage by name works" —
  which is what makes this reachable from SH2 at all.
- **The variable IS the position, proven at the graph.** `MCO_Attack.hkx`'s
  `AttackNodes_StateMachine` binds its own `startStateId` to `MCO_nextattack` through an
  `hkbVariableBindingSet` (`memberPath startStateId`, `variableIndex 230`). Its ten states
  `AttackNodesState1..10` have `transitions: null` and `wildcardTransitions: null` — there is no
  attack1→attack2 edge anywhere. Selecting the clip *is* reading the variable.
- **The combo is advanced by the animations themselves, not by MCO.** Each moveset clip carries a
  Payload Interpreter annotation at t=0.06: `mco_attack1.hkx` → `@SGVI|MCO_nextattack|2`,
  `attack2` → `|3`, `attack3` → `|1|` (the wrap). A 5-hit moveset simply annotates further.
  **This is the hard evidence behind "never derive an index":** moveset length is data living in
  someone else's animation files, so it is not a thing this mod can compute.
- **The reset is two notify-event payloads, and `MCO_ReadyEEM` is NOT what this load order runs.**
  thuum's CONTEXT.md names `MCO_ReadyEEM` (`#amco$0.txt`); `ADXP MCO 1.6.0.6 Bug Fixes` reuses the
  `amco` patch code, wins the merge, and replaces it — `MCO_ReadyEEM` is **absent from the merged
  graph**. What actually resets, verified in the merged `1hm_behavior`:
  - `1HM_Ready_State` (stateId 0) `enterNotifyEvents` → `#0006`, which fires PIE payloads
    `@SGVI|MCO_nextattack|1`, `@SGVI|MCO_nextpowerattack|1`, `@SGVI|MCO_currentattack|0`,
    `@SGVI|MCO_currentpowerattack|0`.
  - `AttackState` (stateId 10) `exitNotifyEvents` → `#0784`, the identical payload set.

  The first is exactly what ticket 10's trace shows at `SBF_ReadyStart`. **The second is the one
  that bites this ticket** — see the snapshot problem below.
- **Therefore the write must be late** — after the ready pass, at the `attackStart` edge.
- **And a late write works.** thuum's own "not yet verified" list, now struck through: *"Does a late
  `MCO_nextattack` write survive to continue the combo rather than restart it? **Yes**, when ordered
  after `inRdy`. MCO resumes at whatever index is written."* Ordering after the ready pass is the
  requirement; with `MCO_ReadyEEM` gone the thing to order after is `#0006`'s payload burst.
- **Preserve, never derive.** Write back the value read at cast start. Not `read + 1`, not
  `MCO_currentattack + 1`. ADR-0005's reasoning is that the last step of a combo has nowhere to
  advance to, so any arithmetic either wraps the player to 1 — punishing a late-combo cast, the
  exact thing this work removes — or silently clamps.

So cell 1 is a known pattern with a precedent, not an architectural risk. But the graph dig surfaced
a constraint thuum's summary does not spell out:

**The snapshot cannot be taken at cast start.** The combo resets on *leaving* `AttackState`, not
only on entering ready — and a cast cannot begin mid-swing at all (ticket 08: the entry transition
lives only in `1HM_Ready_State`). By the time a cast is able to start, the attack it followed has
already ended and stomped `MCO_nextattack` to 1. A read at cast start therefore returns the reset
value every time, and restoring it would preserve nothing.

This is why thuum carries a `RollingCombo` — a rolling sample of `MCO_nextattack` taken while
attacking and kept with a max age (theirs is 5 s), rather than a single read at the interruption.
SH2 needs the same shape. Their own comment block, "WHERE THE COMBO POSITION IS CAPTURED, AND WHY
IT IS NOT AT `BeginCastVoice`", is the write-up of this exact trap; read it before building.

`MCO_currentattack` is not an alternative source: it is a write-only mirror each state sets on
entry, and it is zeroed by the same payloads.

**One coordination question is genuinely open and is being sent to thuum** (see
`../questions/question-2026-08-12-sh2-to-shoutmco-combo-index-ownership.md`, sent on the `%TEMP%` channel
the 2026-08-12 handoff arrived on, because ticket 03 records a standing "do not edit that repo"): thuum's ADR-0005 already declares SH2 casts orthogonal to the combo and says *its*
engine preserves the index across them — but that ADR predates ticket 08, and an SH2 cast no longer
enters the shout graph at all, so its engine almost certainly never sees one now. If SH2 writes the
variable itself, we need to be sure the two never both write.

## The four cells

1. **Cast continues the combo it interrupts.** attack1 → cast → attack2, not attack1 → cast →
   attack1. Snapshot `MCO_nextattack` / `MCO_nextpowerattack` at cast start, write them back after
   the cut's ready pass. Note this needs **no change to the exit transition**: routing through
   `1HM_Ready_State` is fine once the index is restored afterwards, which is a far smaller change
   than rehoming the state inside MCO's chain.
2. **Casts have combos of their own.** Consecutive casts play different clips, chained the way MCO
   chains attack1→2→3. Needs a per-position clip set; the slice currently has exactly one clip
   (`MSCO_left1.hkx`) reused for every cast, which is also why a second cast looks identical to the
   first.

   **Settled by the owner 2026-08-12, and it is the safe reading.** Asked whether a cast should
   occupy a step of MCO's combo, the answer was the orthogonal one:

   > "i expect attack1 -> attack2 -> cast -> attack3. eventually i would want
   > attack1 -> attack2 -> cast1 -> attack3 -> cast2. as well as cast1 -> cast2 -> cast3 -> cast4
   > (as you see in MSCO)"

   So there are **two independent counters, and each is preserved across the other's activity**:

   | | advances on | preserved across | owned by |
   |---|---|---|---|
   | MCO attack index (`MCO_nextattack`) | an attack | a cast | MCO, read and restored by SH2 |
   | SH2 cast index | a cast | an attack | SH2, entirely its own |

   `attack1 → attack2 → cast1 → attack3 → cast2` falls out of that directly, and so does a pure
   `cast1 → cast2 → cast3 → cast4` chain. Nothing here writes `read + 1` into MCO's variable, so
   thuum's ADR-0005 is satisfied rather than amended — the shape it rejected (a cast *advancing*
   MCO's index, firing attack 4) is not what was asked for.
3. **Chain INTO a cast from an MSCO hand cast.** The owner's 2026-08-11 matrix already records this
   direction working (`LH cast → SH2 chains`), so this cell is mostly confirmation on the current
   build.
4. **Chain OUT of a cast into an MSCO hand cast.** The same matrix records this NOT working
   (`SH2 → LH cast no chain`), and it is the mirror of ticket 10's cut: a hand-cast press during a
   committed hotbar cast should end the state the same way an attack press now does. Likely the
   cheapest of the four, and deliberately listed separately so it cannot be quietly dropped.

## Acceptance

- [ ] attack1 → cast → attack continues at the next combo position, owner-verified by eye.
- [ ] Two casts in a row play different clips and read as a combo, not a repeat.
- [ ] MSCO hand cast → hotbar cast chains, with no regression from today.
- [ ] Hotbar cast → MSCO hand cast chains.
- [ ] Power attack chains in every case above, not only the light attack.
- [ ] A real MCO swing, a real shout, and an ordinary uninterrupted cast are all unchanged.
- [ ] Close-out, after 13 and 14: `attack1 → attack2 → cast1 → attack3 → cast2` — both counters
      independent. Not its own ticket; it is the joint demo of those two.
- [ ] Restore fixtures and close Skyrim after runtime work.

## Out of scope, and this time deliberately rather than by deferral

- Concentration channels. They cannot be cut at all yet (ticket 10), and giving them combo
  behaviour before they can end cleanly would build on sand.

## Comments

**2026-08-12 — built on `ticket-11-combo-chain` (repo choice A: SH2, not ShoutMCO).**

The combo-index ownership question is answered: ShoutMCO never reads or writes `MCO_nextattack`
for a driver cast. SH2 writes the variables itself. Collision risk closed.

What shipped:

- `skse_plugin/src/casts/combo_cache.h` — `RollingMcoCombo` (5 s cap, preserve never derive) and
  `CastComboIndex` (1→2→3→4→1, not reset by attacks). Standalone test `combo_cache_test`.
- Driver: rolling sample at `MCO_AttackInitiate` / `MCO_PowerAttackInitiate` / `HitFrame`; restore
  `MCO_nextattack` + `MCO_nextpowerattack` on the ready-pass tags after a cast exit. `begin()`
  sends `SH2_CastRight` / `SH2_Cast2` / `SH2_Cast3` / `SH2_Cast4` from the cast index.
- Consecutive-cast cut in `start_cast` / `start_ritual_cast`. Left-hand cast press cut in the
  input hook (Spell/Scroll in the left hand only).
- `shtb` patch: `SH2_Cast2/3/4` states and clips (`MSCO_left2/3/4.hkx`) on both `1hm_behavior`
  and `magicbehavior`. Shared exit transition. No new graph variables.

Acceptance stays open until the four cells are seen in game. Agents cannot drive ticket 10/11
input-hook cells (injected input never reaches `DispatchInputEvent`). Papyrus `castSlot` can
drive consecutive casts. Owner presses for the attack-cut and left-hand-cast cells.

**2026-08-12 evening — owner playtest, adversarial review session. Do not fix yet; record only.**

1. **Spell release vs clip 4 windup.** SH2 casts look like the spell releases at the start of
   the animation. That reads fine on clips 1–3. Clip 4 has a windup, and the spell fires
   *during* the windup, which looks off. (Session log already showed clip 4 hitting the 0.5s
   authored-time floor before `MLh_SpellFire_Event` at ~0.92s — same symptom, owner-confirmed
   by eye.)
2. **Left-hand spell + hotbar 1 fires both.** With a spell in the left hand, pressing 1 casts
   the hotbar spell *and* the left-hand spell. MSCO ↔ SH2 chaining is not working on multiple
   levels — not only the dedicated cut cells. Cells 3 and 4 stay open; this is a new, broader
   failure than “SH2 → LH cast no chain.”
3. **GCD vs MSCO pace.** SH2's global cooldown may need to follow the pace of the MSCO clips.
   It may need to read MSCO's variable cooldown logic rather than keeping its own timer.
4. **Rooting.** An SH2 cast is supposed to root the player. Currently the player can move
   during the cast.
5. **Power-attack cell, owner-verified:** `attack1 → pattack2 → sh2_cast1 → pattack3` works.
6. **Left-hand Flames cannot chain.** It is a concentration spell. Confirmed in game: it
   resets the combo. Matches the ticket's concentration out-of-scope note; not a cell-4
   counterexample.

**2026-08-12 evening — split into 12–15, playtest items filed as 17–19.**

Ticket 11 is the parent, not an implement unit. Children:

- 12 ADR-0005: combo position is not release timing (blocks 13)
- 13 restore MCO combo position across a hotbar cast
- 14 consecutive hotbar casts walk the cast combo (includes: an intervening attack does not
  reset the cast index)
- 15 MSCO and hotbar chain in both directions

16 was dropped: mixed-chain orthogonality is not a third seam. Persist-across-attack lives on
14; the joint demo `attack1 → attack2 → cast1 → attack3 → cast2` is this parent's close-out
once 13 and 14 are both green.

Playtest items, siblings not children: 17 clip-4 windup delivery, 18 GCD vs MSCO cadence, 19 rooting.

**2026-08-12 — ticket 12 resolved.** ADR-0005 now names combo-position restore as an exception,
not a second timing cache. Ticket 13 is unblocked. 14 and 15 remain frontier.
