# 11 — Make a hotbar cast a first-class member of the attack chain

**Type:** feature (Nemesis patch + driver), the owner's 2026-08-12 MVP definition

**What to build:** A hotbar cast that behaves like an MCO attack in every respect that matters —
it continues the combo it interrupts, it has combos of its own, and it chains with MSCO hand casts
in **both** directions.

**Blocked by:** Nothing. Ticket 10 shipped the cut this builds on.

**Status:** claimed

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

That makes the shape of the fix structural rather than a trick: the cast state has to stop being a
detour out of the chain and become a member of it.

## The mechanism, answered — thuum already solved this and wrote it down

**No spike needed. Do not re-derive any of this.** The sibling project built "resume MCO's combo
from outside" for shouts and closed both of its gating questions with runtime evidence. Sources:
`thuum-fully-animated-shouts-mco/CONTEXT.md` and its `docs/adr/0005-shouts-are-orthogonal-to-the-mco-combo.md`.

- **Combo position is a graph VARIABLE, readable and writable from a DLL.** The set is
  `MCO_nextattack`, `MCO_nextpowerattack`, `MCO_currentattack`, `MCO_currentpowerattack`. Reads
  return real values from a *different graph's* context — "cross-graph linkage by name works" —
  which is what makes this reachable from SH2 at all.
- **The reset is named and located.** `MCO_ReadyEEM` (`#amco$0.txt`, an
  `hkbEvaluateExpressionModifier` over `#amco$1`) sets `MCO_nextattack = 1` and
  `MCO_currentattack = 0`. *"Any path through the ready state resets the combo counter."* That is
  precisely what ticket 10's trace shows happening at `SBF_ReadyStart`, and it confirms the reading
  above from an independent direction.
- **Therefore the write must be late** — after the ready pass, at the `attackStart` edge.
- **And a late write works.** thuum's own "not yet verified" list, now struck through: *"Does a late
  `MCO_nextattack` write survive to continue the combo rather than restart it? **Yes**, when ordered
  after `inRdy`. MCO resumes at whatever index is written."*
- **Preserve, never derive.** Write back the value read at cast start. Not `read + 1`, not
  `MCO_currentattack + 1`. ADR-0005's reasoning is that the last step of a combo has nowhere to
  advance to, so any arithmetic either wraps the player to 1 — punishing a late-combo cast, the
  exact thing this work removes — or silently clamps.

So cell 1 is a known pattern with a precedent, not an architectural risk: snapshot the index at cast
start, and write it back after the cut's ready pass.

**One coordination question is genuinely open and is being sent to thuum** (see
`../questions/question-2026-08-12-sh2-to-shoutmco-combo-index-ownership.md`, sent on the `%TEMP%` channel
the 2026-08-12 handoff arrived on, because ticket 03 records a standing "do not edit that repo"): thuum's ADR-0005 already declares SH2 casts orthogonal to the combo and says *its*
engine preserves the index across them — but that ADR predates ticket 08, and an SH2 cast no longer
enters the shout graph at all, so its engine almost certainly never sees one now. If SH2 writes the
variable itself, we need to be sure the two never both write.

## The four cells

1. **Cast continues the combo it interrupts.** attack1 → cast → attack2, not attack1 → cast →
   attack1. Requires the exit to land somewhere other than `1HM_Ready_State`, chosen by the combo
   position at entry.
2. **Casts have combos of their own.** Consecutive casts play different clips, chained the way MCO
   chains attack1→2→3. Needs a per-position clip set; the slice currently has exactly one clip
   (`MSCO_left1.hkx`) reused for every cast, which is also why a second cast looks identical to the
   first.

   **This cell has two readings, and only one of them is safe.** Both deserve stating because
   thuum's ADR-0005 anticipated the question and rejected one of them by name:

   - *A cast plays a different clip each time* — SH2 keeps its own cast counter, cycles its own
     clips, and never touches `MCO_nextattack`. Orthogonal, composes with everything, no conflict.
     **This is the reading being built.**
   - *A cast occupies a step of MCO's combo*, so `attack1 → attack2 → cast → attack` fires attack 4
     rather than attack 3. ADR-0005 rejected exactly this, for a reason that is not stylistic: at
     the last step of an N-attack moveset there is nowhere to advance to, so MCO either wraps to 1 —
     making a late-combo cast *reset* the combo, the punishment this work exists to remove — or
     clamps, and the rule silently does nothing in the one position a player would notice. The ADR
     notes the rejected shape "is the intuitive one and will be proposed again".

   If the owner wants the second reading anyway, it is a cross-project ADR change and not a quiet
   implementation detail — raise it rather than build it.
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
- [ ] Restore fixtures and close Skyrim after runtime work.

## Out of scope, and this time deliberately rather than by deferral

- Concentration channels. They cannot be cut at all yet (ticket 10), and giving them combo
  behaviour before they can end cleanly would build on sand.
