# 33 — Commit an NPC's concentration cast

**Type:** spike, then feature (Nemesis patch + FOMOD option)

**Status:** ready-for-agent, **back in the root-everyone shape** — the 2026-08-28 morning
re-flip to the conditioned-record shape lasted one day: the owner played ticket 32's built
half-speed channel that evening and rejected it ("as long as npc's are rooted too, that will
have to suffice. i think that's preferable actually"). Ticket 32 is closed wontfix; the rule is
the 2026-08-24 final one, now stated as a preference: **concentration roots, for every actor.**
Build "The build, restated for the final rule" below, spike first, unchanged.

**Blocked by:** None.

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

## History of the rule, compressed — three owner rulings in one day, 2026-08-24

1. First pass: NPC concentration should be covered somewhere, "maybe a separate patch in the
   fomod." This ticket opened as a root on the vanilla concentration states, flagged as colliding
   with ticket 32's half-speed want.
2. Second pass: "the same rules need to apply to everyone consistently" — concentration became
   half speed for every actor, and this ticket flipped to a shared conditioned `SpeedMult`
   record.
3. Final: on learning half speed requires new animation assets, **movement went out of scope.
   "As long as both players and NPCs are rooted during concentration casts, I think we're good
   to proceed."** The conditioned record dies unbuilt (design preserved in ADR-0015's first
   amendment for the future endeavor), and this ticket returns to its original shape.

The collision that shaped versions 1 and 2 no longer exists: rooting the vanilla concentration
states roots the player's equipped-hand concentration casts too, and that is now the *desired*
outcome, not a conflict. The player's hotbar channel is already rooted today — ticket 28's held
`shtb` state plus the WASD capture — so the player's side of this rule costs zero work.

## The build, restated for the final rule

Root the concentration states MSCO does not cover, with the plant this stack has proven three
times: a `BSIsActiveModifier` binding `bAnimationDriven` (with `bAllowRotation` /
`bHeadTrackSpine` so the caster still pivots and tracks), per SH2's own pair at
`nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$11.txt` / `$12.txt`. Its own Nemesis mod
code, not appended to `shtb` — this patch is optional and `shtb` is not. Shipped as a
dependency-gated optional group in the FOMOD (`python_scripts/create_fomod_installer.py`), gated
on `MSCO.esp`.

The spike above still runs first and unchanged: if MSCO's binding set already covers the
concentration states and something else defeats it, this becomes a different ticket and no patch
is written.

## Acceptance

- [ ] An NPC streaming a concentration spell does not translate for the length of the channel —
      measured displacement on a real enemy mage, not a console-summoned one. (Displacement, not
      footfalls: thuum ticket 66 is the lesson.)
- [ ] The NPC still turns to track its target — `bAllowRotation` stays unbound or true. A mage
      frozen facing the wrong way is worse than one that strafes.
- [ ] An NPC's fire-and-forget cast is rooted, same bar as the player's ticket 19 — confirm
      MSCO's existing coverage rather than assuming (spike question 2).
- [ ] The root lifts on every end path: channel end, interrupt, stagger, death. NPC combat AI
      does not wedge — breaking line of sight or killing the target returns normal movement.
- [ ] The player's equipped-hand concentration cast is rooted by the same patch, matching the
      hotbar channel's existing root — one rule, however the cast started.
- [ ] The FOMOD option installs and uninstalls cleanly, and the patch is absent when the option
      is not chosen.
- [ ] Evidence names the commit, the Nemesis regeneration, the save, and the profile.
