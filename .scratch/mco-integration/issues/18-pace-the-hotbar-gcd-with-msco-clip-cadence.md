# 18 — Pace the hotbar GCD with MSCO's clip cadence

**Type:** defect (driver timing)

**What to build:** Keep a GCD. Make it follow MSCO clip pace instead of SH2's hardcoded 1.0s/1.5s
timer, and instead of mash-through. Consecutive hotbar casts chain at `MSCO_WinOpen`, the same
window MSCO uses, and the clip plays at MSCO's charge-time → animation-speed curve.

**Blocked by:** None.

**Status:** claimed

## Ruling (2026-08-13, restated 2026-08-14)

Last night, during ticket 17 close-out: *"i still wanted the GCD, just wanted it shorter than
whatever is baseline in SH2 natively."* A mash of **1** walked 1→2→3→4 with no stop — that was
ticket 14 chaining at SpellFire, not a missing GCD.

Today: keep a GCD; follow MSCO clip pace; **not** 1.0s/1.5s; **not** mash-through. The 2026-08-12
checkbox *"owner does not feel a separate GCD"* is stale wording and is not the gate.

Ticket 20 is superseded: the recovery-window ask was this GCD tune, not inbound during an MCO
swing.

## What this is not

Not a second ShoutMCO release-timing cache (ADR-0005). Observing `MSCO_WinOpen` on a Driver Cast
this mod started is the same class as observing SpellFire (ADR-0004). Writing `MSCO_attackspeed`
is clip playback, not a release policy. Attack chain-out (ticket 10) still uses
`is_committed_cast_holding_graph()` without the WinOpen bit.

Out of scope: concentration, potions, ritual GCD as its own feature, CSV/spell-editor GCD column,
tickets 19/20.

## Behaviour

- Ticket 14's combo walk stays: cut without CastExit, in-place next clip. Gate it on WinOpen,
  not SpellFire.
- After `SH2_CastExit`, the instance dies. No leftover 1.0s/1.5s tail. Clip-end as lockout is
  **not** "shorter" (~1.67s ≈ today's 1.5s); WinOpen is the reading that matches last night.
- Write `MSCO_attackspeed` from `GetChargeTime()` using MSCO.ini's exponential curve. Mechanic
  off → 1.0. INI is read once at `kDataLoaded`; do not live-read MCM or call MSCO.dll.
  Owner 2026-08-14: this is good enough; equipped-hand MSCO playing faster than SH2 is an
  accepted hand-cast advantage (Firebolt tap ≈ 1.25x vs SH2's authored 0.5s → ~0.815x).
- Bind shtb clip `playbackSpeed` to `MSCO_attackspeed` in **both** `magicbehavior` and
  `1hm_behavior`. 1hm does not already have the variable (MSCO only added it to magicbehavior),
  so the 3-list (variableNames + variableInfos + wordVariableValues) is added there. Initial
  value is 1.0f so a missed write does not freeze the clip.

- [x] Mash of 1 on Firebolt no longer walks 1→2→3→4 at SpellFire; the next clip starts at WinOpen.
- [x] Uninterrupted clip 1 has no dead wait after CastExit.
- [ ] Slower-charge FNF plays a slower clip and later WinOpen than Firebolt (mechanic on).
- [x] Equipped-hand MSCO vs SH2 cadence: owner accepts SH2 slower (hand-cast advantage). Do not
  match live MSCO charge input or MCM sliders mid-session.
- [x] Combo index still walks 1→2→3→4→1. Clips 1–3 / clip-4 delivery unchanged (ticket 17).
- [ ] Restore fixtures and close Skyrim after runtime work.

## Comments

**2026-08-14 — agent: GCD is WinOpen, clip plays at MSCO charge speed.** Profile `Nolvus Awakening`,
Save65 (Xaelle, Iron Rapier, Firebolt left). DLL SHA-256
`F90BF05E330E5CF01D28DF7DDA695E6CD82827EE89B774C8D97A2A0D0920818A`. Log
`SpellHotbar2.log` from 21:43. `combo_cache_test` green. Nemesis rebuild 21:38:44 (1045
anims) after a first-pass ERROR(1003): variableInfos MOD_CODE had landed in
`characterPropertyInfos`; moved to the end of `variableInfos` (AMCO's `#0087` layout).
Zero `#shtb$` / `$variableID` leftovers; clip bind `variableIndex` 142 = `MSCO_attackspeed`.

Curve loaded from MSCO.ini (`mechanic=true exp=true base=0.15 p=0.17`). Every begin wrote
`MSCO_attackspeed=0.81491184` (`wrote=true`) for Firebolt 0.5s.

Uninterrupted clip 1 (`t18-uninterrupted`):
```
SH2_CastRight (clip 1) -> true
MSCO_attackspeed=0.815 wrote=true
SpellFire +0.535s
MSCO_WinOpen +0.793s
SH2_CastExit +2.000s
```
Clip is slower than last night's ~1.50s at speed 1.0 (1.667/0.815 ≈ 2.05s). Instance died at
CastExit; no 1.0s/1.5s tail. Clip 4 CastExit → next start was 77ms later.

600ms mash (`t18-mash-600`), the last-night walk:
```
clip 4 start
+0.600s  refuse (clip-4 windup, before SpellFire)
+1.154s  SpellFire
+1.200s  refuse (46ms after SpellFire, 175ms before WinOpen)
+1.375s  MSCO_WinOpen
+1.892s  chain → clip 1
```
Last night this interval chained at SpellFire. It does not now. Combo still walks when the
press lands after WinOpen (clip 1→2→3 on the 1s chain; wrap 4→1). Clip 4 still delivers at
SpellFire +1.13s, not the 0.5s timer floor.

Slower-charge FNF and MSCO-vs-SH2 feel need an owner press. Save65 reloaded (magicka 1000,
health 500, Firebolt left, Iron Rapier). **Game left running** for that — mash 1, then an
equipped-hand MSCO Firebolt, then a slower FNF if one is on the bar.

**2026-08-14 — owner: keep the simple curve; hand-cast advantage is fine.** SH2 stays on
authored `GetChargeTime()` + MSCO.ini once at DataLoaded. No MCM live-sync, no MSCO.dll
path. *"honestly, this is good for now, no need to make it too complex. and i suppose this
gives casting by hand an advantage."*
