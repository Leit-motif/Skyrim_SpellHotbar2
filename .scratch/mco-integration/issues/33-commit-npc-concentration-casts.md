# 33 — Commit an NPC's concentration cast

**Type:** spike, then feature (conditioned-ability record + Nemesis coverage check + FOMOD option)

**Status:** ready-for-agent — the shape is settled by
[ADR-0015](../../../docs/adr/0015-commitment-is-a-property-of-the-behavior-state.md) and its
2026-08-24 amendment (one rule per action, every actor); the open design question below is
resolved by owner ruling.

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

## The collision — RESOLVED 2026-08-24 by owner ruling, same day it was posed

> "i want all casts rooted/slowed for concentration for both the player and npc's. seamless. the
> same rules need to apply to everyone consistently."

Of the three ways out, the owner took the third, and hardened it: **rules are per-action, for
every actor** (ADR-0015 amendment). So this ticket's build is not a root on the concentration
states after all:

- **Fire-and-forget casts root for everyone.** The player's is built (ticket 19). The NPC side is
  the spike's question 2: confirm MSCO's plant covers NPC fire-and-forget states, and patch any
  state it misses with the same modifier shape.
- **Concentration slows to half for everyone, and is NOT rooted.** The uniform mechanism is a
  conditioned ability: one record carrying `SpeedMult` −50, its effect conditioned on the actor
  concentration-casting, applied to player and NPCs alike (SPID or a vanilla-race ability — the
  spike picks). A behavior state cannot express "half"; an actor value can, and one record gives
  every actor the same number. No DLL writes the value on its own clock.

The dependency direction flips too: the NPC needs no animation work — vanilla NPC casting already
blends locomotion, so a slowed NPC walks correctly today. **The player's hotbar channel is the
only actor whose pose breaks when movement opens** (ticket 32's static clip). So ticket 32's blend
route is the long pole, this ticket's slow works for NPCs immediately, and the player's
equipped-hand concentration casts get the slow for free from the same record.

## Acceptance

- [ ] An NPC streaming a concentration spell moves at half its normal rate for the length of the
      channel — measured translation on a real enemy mage, not a console-summoned one.
- [ ] An NPC's fire-and-forget cast is rooted, same bar as the player's ticket 19.
- [ ] The slow lifts on every end path: channel end, interrupt, death, cell change. No `SpeedMult`
      residue on any actor.
- [ ] NPC combat AI does not wedge: interrupting the channel, killing the target, or breaking line
      of sight all return the actor to normal movement.
- [ ] The player's numbers match the NPC's — same root for fire-and-forget, same −50 for
      concentration, whether the cast came from a hotbar slot or an equipped hand.
- [ ] The FOMOD option installs and uninstalls cleanly, and the patch is absent when the option is
      not chosen.
- [ ] Evidence names the commit, the Nemesis regeneration, the save, and the profile.
