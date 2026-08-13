# 14 — Consecutive hotbar casts walk the cast combo

**Type:** feature (driver + Nemesis), parent ticket 11

**What to build:** Two hotbar casts in a row play different clips and read as a combo, the way
MSCO walks `left1 → left2 → left3 → left4`. A second press during a committed cast must actually
start the next cast, not get refused because a cast is already live.

**Blocked by:** None — can start immediately. Ticket 10 already ships the cut this reuses.

**Status:** resolved

## What this is not

Not MCO's attack index (ticket 13). This ticket owns the cast index, including that an
intervening attack does not reset it. The mixed chain `attack1 → attack2 → cast1 → attack3 →
cast2` is parent 11's close-out once 13 and 14 are both green; this ticket only owns the cast-2
half.

## Behaviour

SH2 owns a cast index, independent of MCO's attack index. Consecutive Driver Casts advance it.
The index is not reset by an attack — that is a property of this counter, not a second feature.
The public hotbar path has to honour a second press while a committed cast is still live,
otherwise the clip set is unreachable from the player's hands.

Concentration stays out.

- [x] Two casts in a row play different clips and read as a combo, not a repeat.
- [x] Four casts in a row walk the full clip set and wrap.
- [x] `cast1 → attack → cast2` plays clip 2, not clip 1.
- [x] A second hotbar press during a committed cast starts the next cast through the public
      input path, not only through a test or Papyrus helper.
- [x] Ritual consecutive casts take the same cut.
- [x] An ordinary uninterrupted single cast is unchanged.
- [x] Restore fixtures and close Skyrim after runtime work.

## Comments

**2026-08-13 — public path now chains, observed on Save65.** Profile `Nolvus Awakening`.
Runtime DLL `SpellHotbar2.dll` SHA-256
`CF9FF668BA051D0BD998386157552F8441F3A6A72D1F4B50B728633ACC035984`. Committed DLL also
narrows the `InputModeCast` gate so shout/power/potion still require no live instance
(`53E879BEB7282D0A1ACE7240A835BFB13CB9778F63FFD1B4FBF2EAA705AE4218`). Log:
`Documents\My Games\Skyrim Special Edition\SKSE\SpellHotbar2.log`. Save
`Save65_00000000_0_5861656C6C65_Tamriel_000652_20260812232308_17_1` (Xaelle, Iron Rapier,
Firebolt left). Public path: DevBench `SpellHotbar.castSlot(0)` → `InputModeCast::process_input`
→ `try_start_cast`. `combo_cache_test` green.

The first mid-commit pair was classified as a chain then refused: `try_start_cast` cleared
spellfire before `start_cast` ran the cut, so the live instance looked uncommitted. The
`keep_commitment_until_cut` seam keeps the bit for a chain press; idle starts still drop
leftover shout spellfire. The cut drops the instance with `on_reset_keep_graph` (no
`SH2_CastExit`); `begin()` notifies the next clip from inside the current shtb state. Both
host graphs gained `SH2_CastRight/2/3/4` as local transitions on shared `#shtb$1`.

Uninterrupted single cast (09:31:03): `SH2_CastRight (clip 1) -> true`, left SpellFire,
`SH2_CastExit -> true` at 09:31:04.693. No follow-up log.

Mid-commit walk via one scenario (`castSlot` / wait 600ms ×5) starting at clip 2 (09:31:30):
clip 2 → follow-up chain → clip 3 → chain → clip 4 → (one press refused during clip 4's
pre-spellfire windup, ticket 17) → chain → clip 1 wrap. No `SH2_CastExit` between 2, 3, 4,
and 1; CastExit only after the last clip. That is the full set wrapping through the public
path.

`cast1 → attack → cast2` (09:32:58): after the wrap's clip 1, `Debug.SendAnimationEvent(player,
attackStart)`, then `castSlot(0)` notified `SH2_Cast2 (clip 2) -> true`, not clip 1. The
index has no attack-reset path; `combo_cache_test` `cast_index_is_unchanged_by_an_attack_gap`
covers the counter.

Ritual consecutive casts take the same `start_ritual_cast` cut (`on_reset_keep_graph` +
in-place notify). Fixture slot 0 is Firebolt, so the observed clips were one-handed Driver
Casts.

PrintWindow frames (local, not committed):
`.scratch/mco-integration/evidence/t14/ticket14-chain-{4,6}.png` — distinct clip-3 vs clip-4
poses during the 09:33:29 public-path chain.

Fixture reloaded to the same Save65; `qqq`; DevBench ping offline. MO2 left running.

## Answer

Consecutive Driver Casts walk SH2's own 1→2→3→4→1 index through the public hotbar path. A
second press during a committed cuttable cast cuts without `CastExit` and notifies the next
clip in-place. An intervening attack does not reset the index. Concentration stays out.
Clip-4 windup delivery remains ticket 17.
