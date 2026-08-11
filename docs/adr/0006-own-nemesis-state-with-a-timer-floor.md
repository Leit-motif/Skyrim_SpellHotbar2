# Drive casts through the fork's own Nemesis state, with the authored cast time as delivery floor

A hotbar cast's animation now enters a state this fork ships itself: the `shtb` Nemesis patch
appends `SH2_CastRight_State` to magicbehavior's root state machine, entered by
`SH2_CastRight`, exited by `SH2_CastExit` (or its own end-of-clip trigger), announcing itself
through `SH2_CastEnter` / `SH2_CastDone`. The state plays an MCBO casting clip. This supersedes
the shout-graph premise: CONTEXT.md's "the mod owns no casting graph of its own" and ADR-0004's
`Voice_SpellFire_Event` wording describe the retired voice path. It was retired on evidence:
this load order's shout-path clips T-pose, MSCO's own entry events are combo-chain-only and
unreachable from idle, and MCBO animates only real hand casts, which a hotbar cast is not.

ADR-0004's reasoning survives the move; only the event names change. The commitment point is
the clip's own SpellFire annotation (`MRh_SpellFire_Event` for the slice's clip, 0.283s in),
read from the graph rather than invented by a timer, for exactly 0004's reasons: the payload
lands at the frame the animation shows it leaving the hand, and no cross-mod handshake is
built where the graph already raises the fact.

What 0004 left implicit is now explicit: **the annotation leads, the authored cast time is the
floor.** Commitment protects a cast from interruption after spellfire; it is not the sole
trigger of delivery. If the annotation never arrives — an OAR override replaced the clip, a
Nemesis rebuild dropped the state's wiring (this happened: the `sh2c` build compiled the state
with a null generator) — the cast delivers when its own timer expires, logged as a warning,
instead of silently delivering nothing. A broken animation layer degrades to vanilla-timed
casting, never to a dead button.

Deliberate deviation, recorded against ADR-0001: the driver and the Nemesis patch currently
live in the core fork, and a load order without the patch fails every animated hotbar cast at
entry (`SH2_CastRight` returns false; the log says so). Accepted while the slice is proven;
the split — and whether a failed entry should itself fall back to a non-animated cast — is
resettled when the slice graduates to a Compatibility Package.
