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
the clip's own SpellFire annotation, read from the graph rather than invented by a timer, for
exactly 0004's reasons: the payload lands at the frame the animation shows it leaving the hand,
and no cross-mod handshake is built where the graph already raises the fact.

**Amended 2026-08-12, on two runtime corrections this ADR predates.** Neither changes the
decision; both change what the decision names, and a reader who trusts the original text
arrives at the wrong event and the wrong graph.

- *The annotation is `MLh_SpellFire_Event` at 0.483s, not `MRh_` at 0.283s.* `MSCO_left1.hkx`
  raises the LEFT-hand event whatever hand the cast chose, so the driver armed the left bit only
  (`casting_controller.cpp`, `arm_spellfire`). Measured at +0.46–0.50s on every observed cast,
  across two sessions. **Superseded in part by ADR-0018 (2026-08-26):** per-hand clip variants
  carry the event of the hand they were authored for, arming follows the resolved hand (dual
  arms both), and `MRh_SpellFire_Event` is registered in both hosting graphs. The commitment
  principle — the clip's own annotation, timer as floor — is unchanged.
- *The patch is no longer magicbehavior-scoped.* Ticket 08 distributed the state into
  `1hm_behavior` as well, which is what makes a cast from a drawn weapon possible at all; a clip
  annotation resolves against its HOSTING graph's event table, so every graph the state is
  distributed into must register `MLh_SpellFire_Event` too. This also settles the footprint note
  ticket 08 left against this ADR and ADR-0001's deviation record.
- *`SH2_CastEnter` does arrive, and before spellfire* (+0.197s, traced 2026-08-12) — but the
  driver deliberately does not use it. The notify's own return is the entry signal, and
  `SH2_CastDone` never arrives at all.
- *Timer expiry while the clip is still playing is not the missing-annotation fallback.*
  Ticket 17, 2026-08-13: clip 4's SpellFire is at ~0.92s, past the 0.5s authored time. The
  annotation still leads: clips 1–3 deliver at SpellFire even when it lands before 0.5s.
  The fallback (timer expiry *and* clip ended, no annotation) is what "floor" names for a
  clip that never raises SpellFire.

What 0004 left implicit is now explicit: **the annotation leads, the authored cast time is the
floor.** Commitment protects a cast from interruption after spellfire; it is not the sole
trigger of delivery. If the annotation never arrives — an OAR override replaced the clip, a
Nemesis rebuild dropped the state's wiring (this happened: the `sh2c` build compiled the state
with a null generator) — the cast delivers when its own timer expires *and the clip has ended*,
logged as a warning, instead of silently delivering nothing. A broken animation layer degrades
to vanilla-timed casting, never to a dead button. Timer expiry while the clip is still playing
is not that fallback: see the 2026-08-13 amendment above.

Deliberate deviation, recorded against ADR-0001: the driver and the Nemesis patch currently
live in the core fork, and a load order without the patch fails every animated hotbar cast at
entry (`SH2_CastRight` returns false; the log says so). Accepted while the slice is proven;
the split — and whether a failed entry should itself fall back to a non-animated cast — is
resettled when the slice graduates to a Compatibility Package.
