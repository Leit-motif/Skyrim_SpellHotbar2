# MCO Integration for Spell Hotbar 2

Status: in progress — audited 2026-08-23 on the owner's ruling that the pre-08 tickets are out
of date. **04 resolved the same day — the v1 SH2 gate is closed** (thuum 54 + owner ruling on
the fail-open cell + no-polling and provenance evidence). Open: 25 (ready-for-agent, the concentration-loop repair). 06 resolved (crash guard shipped); 24 deferred by owner ruling. Closed by the audit: 02 superseded, 03 and 07 resolved. Everything else
was already resolved, superseded, or closed.

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

**3. A cast pressed mid-swing needs the shared input buffer.** The sibling engine's vanilla-shout
hook cannot see a hotbar key, but duplicating its MCO state tracking here would create two timing
authorities. Owner ruling 2026-08-08: this mod retains its cast payload and asks ShoutMCO's generic
driver API whether to pass through or defer. ShoutMCO releases or abandons once at a confirmed
state; this mod revalidates and executes once on release.

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

The 2026-08-08 owner ruling supersedes the open question as a design gate: Direct Cast does not
attempt `ShoutStart` while an MCO attack is live. Its payload is deferred through ShoutMCO's
generic API until the engine confirms a legal release state. The notify-path experiment remains
useful evidence but no longer chooses the architecture.

Implement the Spell Hotbar adapter after ShoutMCO ticket 50. Preserve ADR-0004's graph-spellfire
commitment for casts that have already started. Resolve the stance/animation path and the T-pose as
separate Spell Hotbar-owned work; both are required for end-to-end publication acceptance.

Boundaries are unchanged from the previous effort and still hold. Generally applicable native
behaviour belongs in the **Core Fork**. Records, presets, configuration and load-order
adaptations belong in the **Compatibility Package**. The SYHO resolution is likely the latter;
everything else here is the former.

~~The sibling engine needs no code change — its ADR-0002 forbids it from reading shout state to
tell a cast from a shout, and that distinction has to be made on this side.~~ **WITHDRAWN
2026-08-05.** ADR-0002 governs **cooldown** state only; the engine already reads `selectedPower`
and `high->currentShout` to identify a driver's cast. And it needs the *only* code change there
is — it never arms on a hotbar cast, so nothing chains until it does. Collision 1 is therefore a
**cross-project design**. The existing spellfire commitment remains ticket 07 here; input deferral
is ShoutMCO ticket 50 plus ticket 04 here, under ADR-0005.
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
- Implementing or editing the sibling MCO shout behaviour engine from this repository. Consuming
  its published driver API is in scope.
- Publishing modified binaries.

## 2026-08-11 direction (owner rulings, post-acceptance of the shtb slice)

The shtb casting slice is owner-accepted (commit `8effcad`: entry from the magic stance,
commit on the clip's left SpellFire at +0.47s). The owner set the integration direction for
the bigger effort the same evening:

- **Goal statement:** a seamless framework of MCO weapon combat + animated Shouts + SH2
  casting, where a hotbar spell never needs to occupy a hand. MCBO is thereby not *needed* —
  but users who run both must stay supported, so nothing may stomp MCBO's states or hooks
  (ours are already separate; keep it that way as a design constraint).
- **Priority axis: weapon swings → SH2 cast → weapon swings.** This requires the SH2 states
  to be distributed into the weapon behaviours (`1hm_behavior` etc., the TK Dodge
  distribution pattern) — the magic-stance entry proved the mechanism, and the greatsword
  refusals proved the weapon graphs are deaf until patched. The magicbehavior side
  (MCBO-style cast chains) is the cheaper follow-up, not the lead.
- **Workspace ruling: split by ownership** (matches the 2026-08-08 driver boundary). The
  generic cast-intent API, MCO state tracking, and release timing live in the thuum repo
  (its ticket 50); the driver adapter, cast states, and Nemesis patches live here. Sessions
  run in whichever repo owns the slice; cross-repo reads stay routine.
- **Concentration combo path, tiered:** (1) a looping conc state in the shtb patch, exit on
  button release — our own state, so nothing interrupts the channel; (2) a chain-out window
  the driver opens at release, honoured by the engine (the Chain Window mechanism thuum
  already ships for shout exhales); (3) true combo-position continuity (attack3, not
  attack1) is engine-owned MCO state via the ticket-50 API. Owner 2026-08-21: concentration
  needs its own **looping animation types**. That slice is ticket 25 (`needs-triage`).
- **Release timing MVP:** clip annotations are the source of truth (inject via
  hkxc-anno-cli where missing); a per-clip/per-combo-step tuning override is a future
  enhancement.
