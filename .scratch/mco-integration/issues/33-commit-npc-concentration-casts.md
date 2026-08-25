# 33 — Commit an NPC's concentration cast

**Type:** spike, then feature (Nemesis patch + FOMOD option)

**Status:** needs-triage — the shape is settled by
[ADR-0015](../../../docs/adr/0015-commitment-is-a-property-of-the-behavior-state.md); the size
is not, and the spike is one read of MSCO's patch.

**Blocked by:** None. Shares a decision with
[ticket 32](32-move-at-half-speed-during-a-concentration-channel.md) — see "The collision".

## What the owner asked for

2026-08-24, on being told NPC concentration casts are unrooted:

> "this would be something i would want to cover with either sh2 or shout-mco, or even a small
> optional mod, but that just sounds annoying to the user. maybe it would be a separate patch in
> the fomod."

and the goal it serves:

> "i'm after a seamless modernized mco-feel combat experience utilizing sh2 as the interface."

An enemy mage who strafes while streaming a beam is the seam. Every other actor in the fight
commits; that one does not.

## Where it goes, and why it is not a new mod

ADR-0015 settles the ownership question: a root is authored on the state that owns the action, so
this is a patch on the vanilla concentration states, and NPC coverage falls out of the state being
shared rather than out of anything addressing NPCs. **It is not SH2's** — no NPC ever enters a
`shtb` state — and it is not ShoutMCO's, which owns shouts. It is MSCO coverage, and
`magic-casting-behavioral-overhaul/` is the workspace that holds MSCO's sources and patch
lifecycle.

Distribution is a separate question with a separate answer: ship it as an optional group in this
fork's FOMOD, which `python_scripts/create_fomod_installer.py` already builds, gated on
`MSCO.esp` being active the way the existing optional groups gate on their own plugins. The owner
is right that a standalone mod for one behavior file is friction the player should not have to
absorb.

## The spike — one read, before any authoring

MSCO's DLL does **no** rooting: `ref/src/` has no `bAnimationDriven` write, no `moveStop`, no
`ToggleControls`. Its Nemesis patch carries the plant, in the same shape SH2 uses — a
`BSIsActiveModifier` with an `hkbVariableBindingSet` over its `magicbehavior` states.
`Nemesis_engine/mod/msco/magicbehavior/#msco$30.txt` binds five members: `bIsMSCO`,
`bAllowRotation`, `bMSCO_LRCasting`, and **two raw variable indices, 65 and 66, that have not
been resolved to names.**

Answer three things from the patch and the graph, and the size of this ticket is known:

1. Which flags does MSCO's binding set actually carry — is `bAnimationDriven` among indices 65
   and 66, or does MSCO commit a cast some other way?
2. Which states does it cover, and which state does an NPC's concentration cast actually run
   through? "Concentration is not covered" is the owner's reading and the thing to confirm before
   patching anything.
3. Does MSCO's own OAR set already distinguish the case? It ships `Base - default NPCs 1` and
   `NPCs 2` submods, so NPCs are not an afterthought there and the gap may be narrower than it
   looks.

If MSCO's plant already covers the state and something else is defeating it, this becomes a
different ticket and the patch is not written.

## The build, if the spike says the state is unpatched

Copy the plant: a `BSIsActiveModifier` on the concentration state binding `bAnimationDriven`
(with `bAllowRotation` / `HKSMoveON` / `bHeadTrackSpine` as SH2's pair does —
`nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$11.txt`, `$12.txt`). Nemesis mod code of its
own, not appended to `shtb`: this patch is optional and `shtb` is not.

## The collision, and it is the real design question here

**The vanilla concentration states are the player's too.** A root authored there commits the
player's own equipped-hand concentration cast, while ticket 32 wants a hotbar channel to move at
half speed. Left alone that ships two rules for one action, decided by which button started it —
the opposite of seamless.

Three ways out, and this ticket should not be built before one is chosen:

- **Exempt the player by binding**, per ADR-0015: the modifier's active flag reads a variable
  that is only set for the player, and SH2 clears it for a channel it started. One rule, one
  place, an exemption rather than a second root.
- **Give the player the same treatment as the NPC** — root both, and drop ticket 32. Cheapest, and
  it contradicts the owner's stated preference about 6–10 second channels.
- **Give the NPC the same treatment as the player** — half speed for everyone, which needs the
  moving-cast blend ticket 32's routes are about and is therefore gated on that ticket, not this
  one.

## Acceptance

- [ ] An NPC streaming a concentration spell does not translate for the length of the channel —
      observed on a real enemy mage, not a console-summoned one, with the OAR Animation Log naming
      the clip.
- [ ] The NPC still turns to track its target, if the chosen flags allow rotation. A mage frozen
      facing the wrong way is worse than one that strafes.
- [ ] NPC combat AI does not wedge: interrupting the channel, killing the target, or breaking line
      of sight all return the actor to normal movement.
- [ ] The player's rule, whichever of the three was chosen, is the same whether the cast came from
      a hotbar slot or an equipped hand.
- [ ] The FOMOD option installs and uninstalls cleanly, and the patch is absent when the option is
      not chosen.
- [ ] Evidence names the commit, the Nemesis regeneration, the save, and the profile.
