# 31 — A hotbar-fired shout slides the player

**Type:** defect — **owned by ShoutMCO, not this fork**

**Status:** wontfix here. Carried to
[thuum ticket 66](../../../../thuum-fully-animated-shouts-mco/.scratch/shout-mco-engine/issues/66-the-root-stops-the-legs-not-the-translation.md).

## The report and the ruling

Owner, 2026-08-24: **"i thought there's a bug/regression because when i used a shout through sh2,
i slid around instead of movement being blocked. so this is something that's best handled in
shoutMCO."**

The rule is the one set 2026-08-06 and unchanged: a shout blocks movement input for the whole
cast, the way an MCO attack does.

## Why it is not this fork's

A hotbar shout is a real shout. `CastingInstanceShout` swaps `selectedPower`
(`casting_controller.cpp:1461`) and `VoiceCastDriver` presses the native voice button; from the
graph's side nothing distinguishes it from a physical shout key. This fork contributes no root of
its own and should not grow one — `CastingInstanceShout` inherits `blocks_movement() == false`,
and a second root here fighting ShoutMCO's `SHOUT_lock` is the outcome to avoid.

ShoutMCO's root takes the graph out of locomotion and stops the legs; it never removed the
translation, and the control-map suppression that would have is switched off there
(`kRootDuringChain = false`, on finding 14). Thuum ticket 66 has the mechanism, the fix, and the
acceptance. The plant it copies is this fork's own: the `BSIsActiveModifier` bound to
`bAnimationDriven` that closed [ticket 19](19-root-the-player-during-a-driver-cast.md)
(`nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$11.txt`, `$12.txt`).

## The one thing to watch from this side

Ticket 66 predicts a native shout key slides too, and that this was never SH2-specific. If the
drive shows a native press rooting while a hotbar press slides, the difference is in the entry
path and comes back here.
