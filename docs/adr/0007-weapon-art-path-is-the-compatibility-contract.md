# The Weapon Art clip path is the compatibility contract

Date: 2026-08-12

Status: superseded by ADR-0009

The `AABL_Attack_A.hkx` path was a ticket-01 placeholder so an existing Ashes of War OAR pack
could win on a familiar filename. The product does not require that name or that pack's
machinery. See ADR-0009.

## Context

A Weapon Art is a bindable attack animation played from a hotbar slot without equipping anything.
Existing Art Packs — `Ashes of War` is the concrete case — are OAR replacements of
`animations\AABL_Attack_A.hkx`. No `Ashes of War` submod references `Additional Attack By Loop.esp`;
only the items plugin that supplies keywords. Whatever plays that path gets the art.

One alternative is a distinct animation path per art. That removes the condition file, but fixes
the number of arts at Nemesis-patch time, requires shipping an inert placeholder clip per slot,
and still needs condition files the moment an art wants to be weapon-specific.

## Decision

The fork's Weapon Art state plays `Animations\AABL_Attack_A.hkx`. That path is the compatibility
contract with every existing Art Pack. Renaming it is a breaking change, not a refactor.

Which art plays is not encoded in the path. An Art Selector global, written immediately before
entry and cleared on exit, is read by OAR conditions (ADR-0008).

## Consequences

- Existing Art Packs keep working with no patch, no repack, and no dependency on the mod whose
  name the file happens to carry.
- The fork may ship an inert placeholder at that path so a load order without `Additional Attack
  by Loop` still has a clip to play. The placeholder carries no behaviour.
- Adding an art is a folder and a condition file; it requires no change to the fork.
