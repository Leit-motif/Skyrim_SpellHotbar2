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

## The cost we are accepting

A modifier on a shared state catches every actor in it, which is the mechanism above working as
intended and is also its sharp edge: a root authored on the vanilla concentration states roots
the player's own equipped-hand concentration casts too, not only NPCs. Where that collides with a
deliberate player-facing rule — [ticket 32](../../.scratch/mco-integration/issues/32-move-at-half-speed-during-a-concentration-channel.md)
wants a player channel to move at half speed — the exemption is written as a binding on a
variable one side sets, not as a second root somewhere else.
