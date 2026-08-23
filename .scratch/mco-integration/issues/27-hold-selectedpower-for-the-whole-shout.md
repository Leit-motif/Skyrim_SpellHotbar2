# 27 — Hold a hotbar shout's `selectedPower` swap until `IsShouting` falls

**Type:** task

**Status:** resolved

**Origin:** thuum ticket 62 (`thuum-fully-animated-shouts-mco/.scratch/shout-mco-engine/issues/
62-pack-routing-sh2-first-class.md`). Branch `ticket-62-shout-identity`: `91f8a3f` (spike),
`972bc8b` (fix), `c43749d` (trace strip).

## The contract this establishes

**During a hotbar shout cast, `selectedPower` holds the cast shout for the full shout-graph
lifetime** — from before the injected `"Shout"` ButtonEvent until the player's `IsShouting` graph
bool falls (8 s timeout guard). Everything downstream that reads the equipped-shout slot — the
engine's `BeginCastVoice` window key, SKSE `kVoiceFire` `sourceForm` per word, OAR
`IsEquippedShout` per clip activation and echo — sees the cast shout, not the previously equipped
one. This is the identity primitive the Thu'um pack keys on (its ADR-0005 lineage: SH2 owns the
cast payload); no new API, global, or graph variable was needed.

## What was wrong

`CastingInstancePower` swapped the cast shout into `selectedPower` at construction and restored
it at `reequip_old_power()` — the first update tick after `kVoiceFire`, measured 2026-08-22 at
~1.8 s **before** `shoutStop` (restore 55.114, shoutStop 56.959). Words 2–3 of a 3-word shout
fired after the restore, so their `kVoiceFire` reported the **equipped** shout (a cooldown
misattribution inside SH2's own bookkeeping) and OAR's echo re-selection flipped the exhale clip
to the equipped shout's family mid-shout.

## The fix

The hold outlives the cast instance (which dies at GCD expiry), so it lives on the controller:
`reequip_old_power()` asks a virtual `defer_power_write_back(pc)`; powers restore immediately as
before, `CastingInstanceShout` defers while `IsShouting` is true. `update_cast` completes the
write-back on the falling edge. Leak guards: flush before a chained swap and before save
serialization; discard on game load; write-back applies only if the slot still holds what was
swapped in (a mid-shout player equip wins); immediate restore when the graph never entered.
Diagnostics at `logger::debug` (`SH2 power: holding/restored/dropping ...` with the reason).

## Evidence

Owner-driven acceptance 2026-08-22, profile Nolvus Awakening, save `Save13_…_20260822165038`:
FB from bar with UF equipped → `Submod: Fire Breath` both clips; UF from bar with FB equipped →
`Submod: Default` only; vanilla presses show no swap. Logs archived in the thuum repo,
`.scratch/shout-mco-engine/evidence/t62/`.
