# 46 — Per-hand fire-and-forget cast pack: right and dual submods over the neutral graph

The first implementation slice of ADR-0018 (shape A). The graph stays as it is; hand and dual
presentation is OAR submods conditioned on the two ESP globals, following the selection
matrix (`notes/44-selection-matrix.md`) and the spike evidence
(`notes/44-spike-results-2026-08-26.md`).

**Status:** ready-for-agent

**Blocked by:** nothing. The C++ SpellFire contract (per-hand arming, per-hand isolation,
any-hand commitment point, `MRh` registration in both graphs) already landed with ticket 44.

## Scope

Ship a fork-owned OAR replacer (working name `SpellHotbar2Casts`) under
`data/meshes/actors/character/animations/OpenAnimationReplacer/`, covering fire-and-forget
aimed and self for right and dual:

| Submod | conditions | clips |
| --- | --- | --- |
| `cast_right` | `0x815 == 1` AND `0x835 == 1` | `MSCO_right1..4` bytes on the `MSCO_left1..4.hkx` path names |
| `cast_right_self` | `0x815 == 2` AND `0x835 == 1` | same right set (or a self split if the owner wants one) |
| `cast_dual` | `0x815 == 10016` | `MSCO_dual1..4` bytes on the path names |
| `cast_dual_self` | `0x815 == 10017` | same dual set |

Left keeps the bound clips (no submod needed). Owner ruling 2026-08-26: MSCO's shipped clips
fill these cells (its staff sets are already the Dragon Age animations); no donor pipeline.

## Contract details (all from ADR-0018 / the spike)

- **Replacement files must keep the base path's file name** (`MSCO_left1.hkx` etc.) — OAR
  binds by path match; the submod folder is the identity. The Art Pack precedent applies.
- **Annotation audit first:** dump `MSCO_right1..4` and `MSCO_dual1..4` with
  `hkxc-anno-cli`. A right clip must carry `MRh_SpellFire_Event` at its throw frame; a dual
  clip carries whichever hand's event its art throws with (either commits — the mask arms
  both). Stamp missing events (per-file stamping, the Art Pack's in-DLL generation is the
  precedent; per-submod byte-unique stamps REQUIRED per that ticket's lesson). Also verify
  each clip's end trigger — the shtb states rely on the shared `SH2_CastExit` clip trigger
  arrays, which live on the STATE, not the clip, so replacements inherit it; confirm live.
- **Priorities:** above MSCO's `Base - default` (6700), its staff pairs (6800/6801), and any
  `Base - default Variation`; the fork's own Art Pack sits at 2,000,001,001 — stay below the
  probe band (2,000,002,xxx is retired) but distinct from the Art Pack.
- **MSCO interplay:** decide compose-vs-own per cell. MSCO's `CurrentDeliveryType` submods
  almost certainly never match an SH2 cast (no charging caster — spike inference, unproven);
  its staff pairs DO match on pure equipment. Hazard 2's staff-swap check (one staff-equipped
  cast with the Animation Log open) belongs to this ticket's live run.
- **The SYHO duplicate** (`Spell Hotbar 2 - OAR Priority Over SYHO`) duplicates UPSTREAM's
  tree only; a fork-owned pack is not duplicated there. Touch upstream's submods nowhere.

## Acceptance (the handoff's matrix, FF slice)

- left / right / dual assigned casts each select their set (Animation Log names the submod)
  and deliver once; right and dual commit at their authored SpellFire with no fallback
  warning; Auto resolves to the same result as the explicit assignment.
- Four chained casts walk 1→2→3→4 per hand set, each step committing at its own event.
- Equipped left/right spells do not double-fire (both isolation paths, both stances).
- Both hosting stances (drawn-1H, magic) pass every row. Drive hands with
  `SpellHotbar.setSlotHand`, casts with `castSlot`; read `SpellHotbar2.log` +
  `OpenAnimationReplacer.log`; frames via `capture_ingame.ps1` (it captures the OAR overlay).
- Visual identity: right clips visibly cast with the right hand — frame or owner eyes, per
  the domain rule; a graph event is not visual proof.
- Regression: concentration channel, ritual, Ability arts, MCO attack chain-out, combo
  restoration, Cast Plant all stay closed.
- Characterize the stuck-`IsCasting` refusal (spike observation 1) and add a log line to
  `allowed_to_cast`'s silent branch.

## Out of scope

Staff cells and cross-hand staff (ticket 47), the dual >1.51s family leak (ticket 48),
concentration/ritual per-hand audit (they ride upstream's existing shout-path submods; audit
when a family is touched).
