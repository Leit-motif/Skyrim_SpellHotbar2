# 34 — Ship movement-designed ritual channel animations

**Type:** feature (content — animation assets first, rules second)

**Status:** parked — owner-floated 2026-08-24, deliberately not scheduled. Do not start this
ahead of [ticket 32](32-move-at-half-speed-during-a-concentration-channel.md)'s blend route or
[ticket 33](33-commit-npc-concentration-casts.md)'s shared slow; it depends on both.

## Where this came from

Settling ticket 32's ritual question, the owner kept rituals rooted — and named the future they
would trade that for:

> "i've seen certain perk overhauls which allow for movement while channeling ritual spells. the
> animations are like floating animations and stuff ... we can also ship our own ritual
> animations that are designed for movement (such as floating up in the air and hovering or
> something."

So the root is not the permanent answer for rituals; it is the answer **until animations exist
that make moving look intended.** That ordering matters and is the reason this ticket is parked
rather than open: the rule flip is trivial, the content is the work.

## Why the root stands until then

`cast_ritual_aimed_conc` replaces idles, turn-in-place clips, and shout clips — no walk or run
clip in the set. Upstream authored a caster who plants and pivots, and the DLL roots to match
(`CastingInstanceSpellRitualConcentration::blocks_movement()`, `casting_controller.cpp:1402`).
Opening movement without new assets buys the ticket-32 slide back, on the cast type where it
would look worst.

## Scope, when unparked

- A movement animation set for the ritual channel — the owner's sketch is levitation: floating
  up and hovering while the channel streams. Authoring or sourcing the clips is the bulk of the
  ticket and has no precedent in this fork, which so far reuses shipped clips.
- The blend that plays them, on whichever mechanism ticket 32's spike lands (shout-graph re-entry
  or a locomotion blend in the `shtb` state). Rituals reuse that answer; they do not get a third.
- The rule flip, last: rituals move to the ticket-33 conditioned slow (`SpeedMult` −50 while
  concentration-casting already covers them the moment the root is dropped), for every actor,
  per ADR-0015's amendment. No new mechanism.
- FOMOD option, same distribution logic as ticket 33: a player who skips it keeps rooted rituals.

## Open questions to answer before building

- Hover is more than a clip: does a floating channel need collision/height handling, or is it a
  visual float inside the normal capsule? The perk overhauls the owner cites (Ordinator-family
  "Intense Flames"-style levitation) fake it inside the capsule — confirm on one of them before
  inventing anything.
- Do NPCs get the movement set too? ADR-0015's amendment says whatever rule ships, ships for
  everyone; an NPC hovering mid-ritual is a bigger visual statement than an NPC walking.
