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

## The cost we are accepting

A modifier on a shared state catches every actor in it, which is the mechanism above working as
intended and is also its sharp edge: a root authored on the vanilla concentration states roots
the player's own equipped-hand concentration casts too, not only NPCs. Where that collides with a
deliberate player-facing rule — [ticket 32](../../.scratch/mco-integration/issues/32-move-at-half-speed-during-a-concentration-channel.md)
wants a player channel to move at half speed — the exemption is written as a binding on a
variable one side sets, not as a second root somewhere else.
