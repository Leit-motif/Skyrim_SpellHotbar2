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

**Confirmation still owed:** whether MCO's combo position is a graph variable, pure state chaining,
or DLL-side state decides *how* a cast rejoins the chain — resume at the right index, or transition
directly into the next attack state. That is under investigation; nothing here should be built on a
guess about it.

## The four cells

1. **Cast continues the combo it interrupts.** attack1 → cast → attack2, not attack1 → cast →
   attack1. Requires the exit to land somewhere other than `1HM_Ready_State`, chosen by the combo
   position at entry.
2. **Casts have combos of their own.** Consecutive casts play different clips, chained the way MCO
   chains attack1→2→3. Needs a per-position clip set; the slice currently has exactly one clip
   (`MSCO_left1.hkx`) reused for every cast, which is also why a second cast looks identical to the
   first.
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
