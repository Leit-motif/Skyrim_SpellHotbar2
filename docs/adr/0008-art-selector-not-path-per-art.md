# Which Weapon Art plays is an Art Selector global, not a path per art

Date: 2026-08-12

Status: accepted

## Context

ADR-0007 names `Animations\AABL_Attack_A.hkx` as the compatibility contract. Something still has
to choose *which* replacement of that path plays when the player presses a bound slot.

The fork already runs this mechanism for cast animations: a TESGlobal written natively, read by
OAR `CompareValues` conditions, with zero as the resting value. Pointing that pattern at attack
clips is the smallest contract that lets an animation author add an art without touching the fork.

The rejected alternative — one distinct animation path per art — is recorded in ADR-0007.

## Decision

A slot holds an art id. Pressing it writes an **Art Selector** TESGlobal in the fork's plugin
immediately before the entry event is raised, and clears it when the state exits. Zero is the
resting value and means *no fork art*, so an installed Art Pack's own conditions win unchanged.

Art Packs are OAR submods whose conditions compare the Art Selector, at a priority above the
packs they coexist with.

## Consequences

- Binding means something regardless of what the player is wearing: the selector, not a worn
  keyword, chooses the clip.
- Worn-item behaviour is exactly preserved when the selector is zero.
- Extending the set does not require a Nemesis re-run or a fork rebuild.
