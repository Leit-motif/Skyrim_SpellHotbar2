# Which Weapon Art plays is an Art Selector global, not a path per art

Date: 2026-08-12

Status: accepted (amended by ADR-0009)

The Art Selector is SH2's name for which art is live. It is not Ashes of War's worn-item
keyword. The graph is not required to play `AABL_Attack_A.hkx`.

## Context

ADR-0009: the live art is SH2 data, not a hardcoded AABL path. Something still has to name
*which* art is playing so OAR or PIE conditions can branch.

The fork already runs a TESGlobal for cast animations. The Art Selector is that pattern for
Weapon Arts. It is not Ashes of War's worn-item keyword.

## Decision

A slot holds an art id. Pressing it writes an **Art Selector** TESGlobal in the fork's plugin
immediately before the entry event is raised, and clears it when the state exits. Zero means no
SH2 art is live.

Conditions (OAR or PIE) may read that global. They do not replace SH2's bind/press/state/plant.

## Consequences

- Binding means something because SH2 wrote the selector and will play that art's clip, not
  because a worn keyword matched.
- Ashes of War's own hotkey/item path is untouched when SH2 is not in an art.
- Extending the set is catalogue data plus an existing HKX, not a fork rebuild.
