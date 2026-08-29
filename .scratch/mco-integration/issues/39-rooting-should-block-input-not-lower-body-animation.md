# 39 — Rooting a cast should block input, not freeze the legs

Ticket 38 removed the forward glide from a driver cast, and the glide is gone. But it went too far:
the character now stands with a still lower body. The owner wants MSCO's own animation preserved —
the character visibly **steps forward, legs moving** — while the *player's* movement input is what
gets blocked.

**Blocked by:** nothing. Ticket 38's rooting change is the thing being corrected, so read it and
`docs/adr/0015-commitment-is-a-property-of-the-behavior-state.md` first.

**Status:** ready-for-agent — triaged 2026-08-29 against the source and against ticket 54's
measurement. The suspect is named; start there rather than re-opening the diagnosis wide.

## Triage 2026-08-29: the discriminator this ticket asked for already exists

The ticket wanted to know which of two layers freezes the legs, and said a frame comparison was
the way to find out. Ticket 54 answered half of it by measurement on a different question, and
the answer points hard at candidate (2), the `bAnimationDriven` state modifier:

- **The root the owner certifies as correct does not use the flag.** Ticket 54 measured
  `bAnimationDriven` never rising on the MSCO fire-and-forget cast the owner points to as the
  right feel — recorded in ADR-0015's 2026-08-29 amendment. The reference behavior gets its
  commitment from generator replacement, not from an animation-driven state.
- **Our shtb states still bind it, everywhere.** 24 files under
  `nemesis/Nemesis_Engine/mod/shtb/` bind `bAnimationDriven` (13 in `1hm_behavior`, 10 in
  `magicbehavior`, 1 in `0_master`), untouched by ticket 58.
- **Ticket 58's `shcr` did not change this path.** `shcr` replaces the generators behind the
  vanilla locomotion-adjacent casting nodes and binds `bAllowRotation` only — two binds, no
  `bAnimationDriven` anywhere in the patch. A hotbar driver cast still runs an animation-driven
  shtb state, so nothing about the commitment rebuild has already fixed this by accident.

So the shape of the fix is now plausible to state before the frame is captured: apply `shcr`'s
mechanism to the shtb states — commitment from what the state routes through, not from a flag
that freezes the actor — and the legs keep MSCO's authored motion while input stays blocked.
That is a hypothesis, not a finding. **The frame comparison below is still the acceptance
evidence**, and per the domain rule a visual claim needs a committed screenshot; a log line
proving a state is active never shows what the legs did.

One caution carried from ticket 58: `shcr` and `shtb` both patch behavior, and Nemesis resolves
single-value conflicts last-checked-wins. Produce the contention read before authoring, the way
ticket 58 did.

## The distinction this ticket exists to hold

Three separate things have been collapsed into one:

1. **Animation-driven root motion** — the clip's own animmotion track translating the actor.
2. **Lower-body animation** — the legs playing their authored stepping motion, in place or not.
3. **Player movement input** — WASD / stick actually steering the character during the cast.

The desired behavior is (2) **kept**, (3) **blocked**, and (1) is the knob ticket 38 turned. The
current result reads as (2) also being lost, which is what the owner is reporting.

## Owner report 2026-08-25

> Forward Glide is gone, but this was not necessarily the desired behavior. It was a fallback. What
> I wanted was a preservation of the animations as they are in MSCO — the character actually steps
> forward, there's leg movement. Whereas with our implementation of these casting animations
> through Spell Hotbar 2, the interpretation of rooting movement was taken as stopping all lower
> body movement, which was not correct. We just wanted to block input, and so maybe there's still a
> holdover that is blocking the legs from moving.

The owner's own hypothesis — a holdover from an earlier rooting attempt still suppressing lower-body
animation — is the first thing to check, not the last.

## Where to look

The owner suspects a leftover, and there are two plausible layers:

- **The behavior state.** ADR-0015 made commitment a property of the state, and the state-modifier
  `bAnimationDriven` path is what roots the actor. Whether it also suppresses the lower body is the
  question; a state that is animation-driven with no motion track can present as planted legs.
- **Ticket 38's own change.** `SH2_Cast*_Clip` no longer consuming animmotion is precisely (1). If
  the legs are still moving in the clip data, the freeze is coming from somewhere else and (1) is
  not the culprit — that is a strong discriminator worth running first.

Ticket 35 retired the DLL movement capture, so any surviving movement suppression in the plugin is
by definition a leftover. Check it against the tickets 32 and 34 parked work before assuming.

## Narrowed 2026-08-25 by ticket 38's own telemetry

The ticket-38 session ran both its cells on the deployed DLL right after the owner's pass, and the
second one is a discriminator for this ticket:

- **Disengage (art 12, stamped clip, 140 animmotion keys) leapt ~315 units XY**, matching ticket
  05's ~318. So `SH2_Art_Clip` still consumes animmotion and still translates the actor.
- A fire-and-forget driver cast froze XY to the last digit through the clip, with no
  `SH2_CastRight_Clip` motion bind.

Motion consumption is therefore alive and working where it is supposed to be. That weakens
candidate (1) — ticket 38's animmotion change — as the cause of the leg freeze, and points at the
**`bAnimationDriven` state modifier** side instead. Start there rather than re-running the
animmotion comparison.

This is telemetry from a peer session, not a frame. It says motion translates; it still does not
show what the legs do. The frame comparison below is unchanged as the acceptance evidence.

## Diagnose before building

Do not fix this by reverting ticket 38. The glide removal is wanted; only the leg freeze is not.
Establish which layer kills the lower body first — capture a frame mid-cast and compare against the
same clip played outside a driver cast (a plain MSCO swing, or the pre-38 build). A frame is the
evidence here; a log line proving a state is active never shows what the legs did.

## You test this

Profile `Nolvus Awakening`. Weapon drawn, cast a fire-and-forget spell from a hotbar slot.

1. The character's legs play their stepping animation as MSCO authored it.
2. The character does not glide or translate forward across the ground.
3. Holding a movement key during the cast does not steer the character.

## What this is not

Not a request to restore the forward glide (ticket 38 removed it deliberately and correctly). Not
the concentration channel's rooting, which the owner accepted in the same pass ("I was rooted") —
though if one mechanism serves both, a fix here must not regress that.

## Comments

Owner 2026-08-25: reported during the tickets 25/06/14 acceptance pass, as the answer to ticket
38's owner-eyes cell. Ticket 38's cell passes on its own terms; this is the follow-on.
