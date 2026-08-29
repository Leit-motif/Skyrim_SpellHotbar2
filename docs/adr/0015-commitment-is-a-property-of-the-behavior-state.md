# Commitment is a property of the behavior state, not of a DLL

Date: 2026-08-24

Status: accepted

## Context

The owner's goal for this stack, stated 2026-08-24: **a seamless modernized MCO-feel combat
experience using Spell Hotbar 2 as the interface.** MCO's feel is commitment — you choose an
action, you are in it, and you do not steer out of it. That has to hold across every action the
hotbar can start, or the interface is the seam the player notices.

Four mods currently root, or fail to root, four different ways, and the four outcomes fall along
one line:

| Mod | Mechanism | Rooted? |
|---|---|---|
| MSCO | `BSIsActiveModifier` + binding set on its `magicbehavior` states (`#msco$30.txt`), plus OAR submods that name NPCs explicitly | yes, **player and NPC alike** (owner-reported; see below) |
| SH2 Driver Cast | `BSIsActiveModifier` on the `shtb` state binding `bAnimationDriven` (`#shtb$11.txt`, `$12.txt`) | yes, ticket 19 |
| SH2 input capture | WASD swallowed in the DLL's input dispatch while a `shtb` state is held | yes, player only, and only as a second layer |
| ShoutMCO | `SHOUT_lock` + `moveStop` from the DLL, telling the graph to leave locomotion | **no — the legs stop, the body slides** (thuum ticket 66) |

One hedge on that first row, because the decision below leans on it. That MSCO roots casts is
the owner's recollection, and its DLL sources under `magic-casting-behavioral-overhaul/ref/src/`
contain no root at all — no `bAnimationDriven` write, no `moveStop`, no `ToggleControls` — while
its Nemesis patch uses the same modifier-and-binding-set shape SH2's plant uses. So the mechanism
is graph-side beyond reasonable doubt; **which flags that binding set actually carries is not
resolved** (two of the five bind raw variable indices), and the ticket that acts on this reads
them first.

The one that fails is the one that tried to do it from outside the state. Skyrim's player
locomotion is controller-driven, so an event that changes which animation plays does not change
whether the actor translates. And ShoutMCO's other attempt, `ToggleControls`, was measured in
that repo's finding 14 to root nothing visible while costing the moveset its direction.

## Decision

**A root is authored on the behavior state that owns the action, as an animation-driven flag,
and nowhere else.** A DLL may decide *when* a state is entered and left; it does not implement
the commitment.

Three consequences we are choosing deliberately:

- **Whoever owns the state owns the root.** A shout's root is ShoutMCO's because ShoutMCO owns
  the shout patch. A Driver Cast's root is this fork's. An NPC's cast root is MSCO's. There is no
  case for a second mod reaching in from the side, and a DLL-side root that fights a graph-side
  one is a defect rather than defence in depth.
- **NPC coverage comes free where the state is shared, and is unreachable where it is not.**
  MSCO roots NPCs without a single line about NPCs, because every actor entering an
  animation-driven state is rooted by it. SH2 will never root an NPC, because no NPC is ever sent
  into a `shtb` state — that is a property of the design, not a gap to close in this fork.
- **A gap in NPC commitment is a gap in a shared state, so it is fixed there.** NPC concentration
  casting is unrooted today because MSCO's coverage stops short of the vanilla concentration
  states, not because anything decided NPCs should stay mobile. The patch belongs on those
  states.

**Distribution does not follow ownership.** ADR-0001 keeps load-order adaptations out of the Core
Fork, and an optional Nemesis patch over vanilla states is exactly that — but the player installs
one FOMOD, and `python_scripts/create_fomod_installer.py` already builds dependency-gated
optional groups. A patch may therefore ship as an option in this installer while being authored
and owned elsewhere. Shipping a separate small mod for one behavior file is the outcome to avoid.

## Amended 2026-08-24, same day: one rule per action, for every actor

The owner, closing the loop after the NPC question:

> "i want all casts rooted/slowed for concentration for both the player and npc's. seamless. the
> same rules need to apply to everyone consistently."

That kills the per-actor exemption this ADR's cost section entertained. **Rules are per-action,
never per-actor**: a fire-and-forget cast roots whoever casts it; a concentration channel moves
at half speed whoever holds it. The player is not a special case, and neither is the NPC.

It also forces a second primitive, because the two rules are different kinds of thing:

- **Commitment is binary and lives in the behavior state** — the decision above, unchanged. A
  `BSIsActiveModifier` can stop translation; it cannot express "half."
- **A graded slow is an actor value and lives in a conditioned magic effect** — a `SpeedMult`
  modifier active only while the actor is concentration-casting, carried by one ability record
  every actor gets. One record, one condition, and the player and the enemy mage read the same
  number.

The split is not a compromise; each primitive is the only one that can do its job. What stays
forbidden is the third thing: a DLL toggling controls or writing actor values on its own clock,
which is the shape that slid.

## Amended again 2026-08-24, hours later: concentration roots too; the slow is tabled

On learning the half-speed path required new animation assets (the moving-cast blend, and
authored clips for rituals), the owner cut movement from scope outright:

> "movement is out of scope. As long as both players and NPCs are rooted during concentration
> casts, I think we're good to proceed, and we can table the movement and blend work for a
> future endeavor."

So the per-action rule set is now uniform in the simplest possible way: **every cast type in
scope — fire-and-forget, concentration, ritual — roots every actor**, all through the one
primitive this ADR names, the behavior-state modifier. The graded-slow primitive (the
conditioned `SpeedMult` record) was the right design for a rule that no longer exists; it is
recorded above so the future endeavor does not rediscover it, and nothing builds it now. The
per-actor exemption stays dead either way — that part of the first amendment survives.

## Amendment below WITHDRAWN the same evening — the built half-speed channel was rejected live

The owner played the build and closed the question for good (2026-08-28): "as long as npc's are
rooted too, that will have to suffice. i think that's preferable actually. moving while
attacking at range should be for archery." **Concentration roots, every actor — now as a
positive design choice, not a tabled cost.** Move-while-attacking-at-range belongs to archery.
The second amendment's rule set stands; ticket 33 builds the root patch; ticket 32 is closed
wontfix with the full record. The withdrawn amendment is kept below for the history.

## ~~Amended 2026-08-28: movement is back in scope for concentration~~ (withdrawn, see above)

The owner un-parked [ticket 32](../../.scratch/mco-integration/issues/32-move-at-half-speed-during-a-concentration-channel.md).
The "future endeavor" of the second amendment is now. What changes, and what does not:

- **The concentration channel trades the binary root for the graded slow.** The channel state's
  `bAnimationDriven` plant comes off, a locomotion blend goes into the held state (legs from the
  ordinary locomotion clips, layered under the inhale pose — route B, inside the fork's own
  state), and the conditioned `SpeedMult` record the first amendment designed finally gets
  built: one ability, −50 points while concentration-casting, every actor.
- **Every other cast type keeps its root unchanged** — fire-and-forget, art clips, and ritual
  concentration (the owner's 2026-08-24 ruling that rituals plant stands).
- **The per-action, every-actor rule is unchanged and is why ticket 33 flips back** to
  distributing the shared record instead of authoring a root patch on the vanilla concentration
  states.
- The forbidden third thing stays forbidden: no DLL toggling controls or writing actor values on
  its own clock. The DLL applies and dispels the ability on the channel's own start/teardown
  edges; the number lives in the record.

## Amended 2026-08-29 (later the same day): the root can be the GENERATOR, and the DLL block is gone

Ticket 58 shipped commitment as the Nemesis patch `shcr` and it is owner-confirmed live, player
and NPC. The mechanism is not the one either the decision or the amendment below assumed, so both
need adjusting.

**What `shcr` does.** Where casting meets locomotion, it replaces the `generator` param outright:
`MagicCastingLocomotionState` (`#0926`), `MagicCast_Standing` (`#0930`), and the two turn states
(`#0965`, `#0998`) each get a thin `hkbModifierGenerator` over vanilla `LeftHandMagicCast_MSG`
(`#0088`), wrapped in a `BSIsActiveModifier` binding one new variable, `bAllowRotation`, so the
actor can still track its target. Five new nodes, three declaration nodes, 14 files, zero
contested nodes. The shape is Enemy Magelock's, minus its player/NPC split and its staff rework.

**No flag is planted, and none is needed.** The cast stops routing through a locomotion-blending
state, so there is nothing left to translate the actor. Ticket 54 measured `bAnimationDriven`
never rising on the cast the owner certifies as the correct root — the flag was never the thing
doing the work on this path. So the decision's rule holds in substance and widens in form: **the
root is authored in the behavior graph, on the state that owns the action.** On a full-body state
that is an animation-driven flag; where an action's state routes through locomotion, it is the
generator that gets replaced. Neither is the DLL's.

**The layered-state carve-out below is withdrawn, and its code is deleted.** With the generator
replaced there is no live locomotion layer left to fight, so the player's input block has nothing
to fix. It was also inert as shipped: its gate required `bAnimationDriven`, which the same
measurement says never rises here, so it never fired once. Removed from `input/input.cpp`, along
with the predicates in `casts/combo_cache.h` and their tests. The forbidden third thing is
forbidden again with no exception: **no DLL-side root, and no DLL-side movement block either.**

The failed alternative is worth keeping named. `shcc` — ticket 33's flag plant on the six vanilla
concentration states, three attempts — is retired and deleted. Layered states root nobody via
flags; that is the whole lesson.

## ~~Amended 2026-08-29: on a LAYERED state, the player's movement INPUT block is the DLL's~~ (withdrawn, see above)

Owner-approved after the moving-entry defect on ticket 33's `shcc` patch: already walking, press
Flames from the left hand, and "the character stutters and keeps moving then eventually stops
after a while." From standstill the same cast roots cleanly.

The decision above is unchanged — **the root is still authored on the state, as
`bAnimationDriven`, and nowhere else.** What this amendment adds is a distinction the original
decision did not have to make, because until now every state this fork rooted was full-body:

- **A FULL-BODY state needs nothing else.** `SH2_Channel_State` plays its own clip; entering it
  exits locomotion, so the legs are already stopped when the plant roots the controller and held
  movement keys have nothing left to feed. That is why the retired DLL capture (ticket 35,
  `c73b4f1`) was duplication and stays retired.
- **A LAYERED state does.** The six vanilla concentration states `shcc` patches are layered over
  live locomotion — that is exactly why vanilla walks while channeling. The plant roots the
  controller while held keys keep driving the still-running locomotion layer, and the two fight:
  entry momentum carries, steering is dead, the stop arrives late. **No behavior-graph primitive
  gates the player controller's input while the locomotion layer runs**, so the graph cannot
  express this and the block is the DLL's, narrowly.

The narrow block, gated on the live engine state rather than on SH2's own bookkeeping: swallow
the player's four movement controls while an equipped-hand `MagicCaster` is mid-cast with a
`kConcentration` spell **and** the player's graph reports `bAnimationDriven`. Both halves are
load-bearing — the cast alone would leave a patchless user's vanilla walk-casting broken, and
`bAnimationDriven` alone would catch every MCO attack.

Three things this does not do, and each is the earlier decision holding:

- It does not root. It releases the keys; the plant on the state does the rooting, and a DLL-side
  root that fought a graph-side one would still be the defect this ADR named.
- It does not touch NPCs. The behavior-only root is proven green on a real enemy mage: NPC AI
  stops issuing movement intent when it casts, so there is no input to swallow and no player
  controller in the loop.
- It is not the forbidden third thing. Nothing toggles controls wholesale or writes an actor
  value on the DLL's own clock. Attack, cast and menu controls are untouched, only presses are
  swallowed (an up event always travels), and every unreadable read — null caster, null spell,
  unreadable graph — fails open to vanilla movement.

## The cost we are accepting

A modifier on a shared state catches every actor in it, which is the mechanism above working as
intended and is also its sharp edge: a root authored on the vanilla concentration states roots
the player's own equipped-hand concentration casts too, not only NPCs. Where that collides with a
deliberate player-facing rule — [ticket 32](../../.scratch/mco-integration/issues/32-move-at-half-speed-during-a-concentration-channel.md)
wants a player channel to move at half speed — the exemption is written as a binding on a
variable one side sets, not as a second root somewhere else.

## Amended 2026-08-29 (evening): commitment ships unconditionally

Owner ruling, closing ticket 58: **no FOMOD option for the casting-commitment patch — not
deferred, refused.** "i have a vision for what i want and it is seamlessness and commitment
also applies to magic (mco type rules)." `shcr` is part of the mod the way the `shtb` cast
states are: always installed, always on, no install-time gate. Any earlier framing of
commitment as an optional payload (the retired `shcc` FOMOD group was one) is superseded.
Rotation tracking during the committed cast is a requirement of this design, not polish — a
committed caster that cannot face a moving target is useless against dodge mods — and it is
carried by the `bAllowRotation` modifier on the same nodes.
