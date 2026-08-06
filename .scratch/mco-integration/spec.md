# MCO Integration for Spell Hotbar 2

Status: ready-for-agent

Supersedes `../baseline-adoption/spec.md`, which is closed. Created 2026-08-03.

## Problem Statement

Direct Cast is the reason this fork exists: a hotbar slot casts its bound spell without
equipping it, and therefore without occupying a hand. The player's combat is driven by MCO.
Those two systems currently collide, and the collisions are not cosmetic — one of them
silently destroys the spell.

Every fact below is already recorded in `CONTEXT.md` findings 1–11 and was established by
static inspection of this repository, cross-referenced against the sibling MCO shout
behaviour engine project. This specification does not re-derive them; it turns them into
work.

The mod owns no casting graph. A cast is three notifications into the vanilla shout graph —
`ShoutStart`, `MT_BreathExhaleShort`, `ShoutStop` — with the clip chosen by OAR. That design
is what makes Direct Cast possible, and it is also the source of every problem here.

## The four collisions

**1. A chain-out destroys the cast.** The engine's chain works by cutting the shout with
`shoutStop` and firing `attackStart` from the ready state. `shoutStop` clears `IsShouting`,
and this mod tears down the cast instance the moment `IsShouting` goes false — it advances
its own cast timer and fires the spell itself, rather than using the graph's
`Voice_SpellFire_Event`. So a chain taken during a cast destroys it **before the timer fires
and the spell never goes off**. For a real shout the same cut is harmless, because vanilla
puts `Voice_SpellFire_Event` 0.1 s into the exhale and the magic is already out. This is data
loss, not a missing feature.

**Corrected 2026-08-05 — the mechanism is right, the tense is wrong.** The engine side traced the
first live hotbar cast (its ticket 37) and found it **never arms**: a hotbar cast does not raise
`BeginCastVoice`, so no window is scheduled, the attack press is forwarded straight to the game,
and no cut is ever sent. The two mods are safe together today. The paragraph above describes what
happens the moment the engine *is* made to engage — which is precisely what the integration
requires — so it is the price of collision 1's fix, not a hazard the player currently faces. See
`CONTEXT.md` finding 12.

**2. Casts land on the branch MCO chaining does not serve.** The vanilla graph's six exhale
events encode word count *and stance*. `MT_BreathExhaleShort` selects the sheathed branch;
the `CombatReady_*` events select weapon-drawn. The mod fires the sheathed event
unconditionally, so every cast lands outside the branch MCO chaining is built around —
whatever the player has drawn. Fixing this is not merely an animation-quality fix; it is the
precondition for any chaining integration at all.

**3. A cast pressed mid-swing tears the attack down.** The sibling engine shipped protection
for this (its ticket 15): the press is held until `HitFrame` and handed to the game after the
swing lands. It hooks `ShoutHandler::ProcessButton` and matches the `Shout` user event, which
a hotbar key never reaches. The *pattern* transfers; the hook point does not. Anything built
here needs its own hold on this mod's own input path.

**4. Another mod owns the animation outright.** `SYHO - Shout Your Heart Out` declares OAR
submod priorities `99999990`–`99999996`. This mod's 56 submods top out at `99901002`. SYHO's
lowest outranks this mod's highest, so wherever both sets of conditions pass SYHO wins every
contested cast. Observed live on 2026-08-03: every hotbar cast played SYHO's shout animation.
SYHO's clip does not loop, so a concentration channel has no animation that can represent it.

## The open question

Not answerable by static inspection, and it gates the design of collisions 1 and 3.

A real shout pressed mid-MCO-attack *is* honoured — the sibling project drove it three times
on a live fixture and the graph entered the shout every time, tearing the outgoing attack
down through `MCO_AttackExitNotify` → `attackStop` → `inRdy` about 3 ms later. But those
traces entered through the shout **control**; this mod enters by notifying `ShoutStart`
directly with no shout equipped. Same destination, different entry. So:

- Is the **notify** path honoured from an attack state, as the control path is?
- Does the liveness check survive the teardown pass that entry provokes? That pass runs
  through `inRdy` about 3 ms in. If `IsShouting` reads false at any point across it, the cast
  dies on its first update.

The second is the more dangerous, because it would make casting-from-combat fail
intermittently and be misdiagnosed as a transition problem.

## Solution

Answer the open question first, because it can invalidate the design of the rest. Then fix
the stance-blind release event, because chaining cannot be addressed until casts reach the
`CombatReady_*` branch. Then handle the two input-timing collisions. The SYHO conflict is
independent and can proceed in parallel.

Boundaries are unchanged from the previous effort and still hold. Generally applicable native
behaviour belongs in the **Core Fork**. Records, presets, configuration and load-order
adaptations belong in the **Compatibility Package**. The SYHO resolution is likely the latter;
everything else here is the former.

~~The sibling engine needs no code change — its ADR-0002 forbids it from reading shout state to
tell a cast from a shout, and that distinction has to be made on this side.~~ **WITHDRAWN
2026-08-05.** ADR-0002 governs **cooldown** state only; the engine already reads `selectedPower`
and `high->currentShout` to identify a driver's cast. And it needs the *only* code change there
is — it never arms on a hotbar cast, so nothing chains until it does. Collision 1 is therefore a
**cross-project design**, tracked as ticket 38 in the engine repo and ticket 03 here.
`CONTEXT.md` findings 6, 8 and 12 carry the detail. The out-of-scope line below still holds as a
working rule — this repo does not *edit* the engine — but it no longer means the engine is
uninvolved.

## Secondary scope

Cleaning up inefficient code in this fork. Explicitly secondary: it is not a prerequisite for
any integration work above, and it must not be bundled into an integration change. It gets
its own ticket and its own review.

## Testing Decisions

Proportion the evidence to the claim. This effort is not a compatibility audit and there is
no acceptance matrix.

- A behavioural claim about the graph, the cast lifecycle, or input timing needs a live
  observation, because none of it is provable statically.
- A claim about which animation plays needs a **captured frame**. Record identity or an
  effect enumeration proves a thing is attached, never what it looks like.
- Static inspection and a successful build support diagnosis and never establish that an
  integration works.
- Reuse the existing fixture: the tested binary with a proven producing commit, the
  fingerprinted `Nolvus Awakening` profile, and the controlled disposable save recorded in
  `../baseline-adoption/fixture.md`. Restore fixtures and close Skyrim after runtime work.
- Prefer one discriminating control over many absolute measurements. The previous effort
  settled the entire cost question with a single paired cast after burning several on
  per-spell figures.

## Out of Scope

- Re-validating Direct Cast. It works; assume the repository's advertised behaviour.
- Resuming Baseline Adoption tickets 03–07 or the acceptance matrix.
- Changing the sibling MCO shout behaviour engine.
- Publishing modified binaries.
