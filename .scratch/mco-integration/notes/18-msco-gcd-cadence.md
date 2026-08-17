# 18 — MSCO GCD cadence (research + implement)

Do not re-derive. Numbers are from the 2026-08-14 research pass and the implement that followed.

## What SH2 was doing

`CastingInstanceSpell` hardcoded `m_gcd` to 1.0s one-handed / 1.5s dual. Timer started at authored
charge time (`GetChargeTime()`, floor 0.25s; Firebolt 0.5s). Idle restart was blocked for
**cast time + GCD** (~1.5s Firebolt). The spell-editor/CSV GCD column is unused — do not revive it.

Ticket 14's mash-through: `classify_hotbar_cast_press` returned `chain` whenever a committed
cuttable Driver Cast held the graph. `cut_committed_cast_for_combo` drops the instance, so `m_gcd`
never ran during combo. After an uninterrupted clip, the tail still existed (clip 1 CastExit
~1.07–1.50s then a short wait; clip 4 ~1.49s already ate the 1.5s budget).

## MSCO has no GCD timer

Cadence is the clip. MSCO chains at **WinOpen**, not SpellFire. Ticket 14 chaining at SpellFire
was ~0.3s early.

| Marker (clip 1, speed 1.0) | Authored | Live SH2 (Firebolt) |
|---|---|---|
| SpellFire | ~0.48s OAR left variants (base HKX is MRh @ 0.28s) | +0.45–0.48s |
| `MSCO_WinOpen` / `MCO_winopen` | 0.80s | +0.67–0.69s |
| Clip duration / CastExit | 1.667s | ~+1.50s |

## v2 charge conversion

Desmos [px706ivga2](https://www.desmos.com/calculator/px706ivga2). Installed `MSCO.ini`
(mod default and New Overwrites agree on the curve):

- Exponential (shipped): `speed = clamp((i / clamp(charge, a, b))^p, j, k)`
- `i=0.15` (BaseTime), `a=0` (Shortest), `b=2` (Longest), `p=0.17` (ExpFactor),
  `j=0.6` (MinSpeed), `k=1.25` (MaxSpeed)
- Firebolt 0.5s → ~**0.815**; 2.0s charge → ~**0.644**; charge 0 → clamp to max 1.25
- Linear: piecewise around base time (author suggests i=0.5)
- Mechanic off → speed 1.0

SH2 Driver Casts never went through MSCO's `BeginCast*` path, so MSCO.dll never wrote
`MSCO_attackspeed` for them. Every shtb clip generator had `variableBindingSet` null and
`playbackSpeed` 1.0.

## ADR-0005

Observing `MSCO_WinOpen` on a Driver Cast this mod started is the same class as SpellFire.
Writing `MSCO_attackspeed` is clip playback, not ShoutMCO release. Attack chain-out still uses
`is_committed_cast_holding_graph()` without the WinOpen bit — only `classify_hotbar_cast_press`
/ `can_accept_hotbar_cast` get it.

## Implement shape

1. `classify_hotbar_cast_press(live, committed, combo_window_open)` — chain only if committed
   **and** window open.
2. Driver sets `combo_window` on `MSCO_WinOpen` / `MCO_WinOpen` / `MSCO_winopen` / `MCO_winopen`;
   clears on begin / CastExit / cancel / finish.
3. After SpellFire, a cuttable FNF instance lives until `!MscoCastDriver::is_active()`, not
   `is_gcd_expired()`. Do not OR them — that would kill the instance at 1.5s while clip 1 still
   plays to 1.67s. `CastingInstanceSpell::m_gcd = 0`. Potions/powers/shouts/conc keep their GCD.
4. `begin(pc, hand, charge_time)` writes `MSCO_attackspeed` before the notify. Curve loaded from
   `Data/SKSE/Plugins/MSCO.ini` at `kDataLoaded` and again at each `begin()` so a saved INI
   applies without restart. Info-log only on first load or a value change (mash must not
   spam). Do not call MSCO.dll or Menu Framework. Charge input is SH2's authored
   `GetChargeTime()`. Owner 2026-08-14 accepted SH2 vs equipped-hand feel as a hand-cast
   advantage; MSCO.log later showed the same Firebolt 0.5s → 0.815, so that gap is not a
   different curve input.
5. magicbehavior: MSCO already registered the variable. Bind clips `$0/$6/$8/$10` to `#shtb$12`.
6. 1hm_behavior: add the 3-list (`#0085` names, `#0087` infos, `#0086` values). Bind clips
   `$0/$4/$6/$8` to `#shtb$10`. Initial value `1065353216` (IEEE 1.0f).

   `#0087` has `characterPropertyInfos` (9) between `variableInfos` and `eventInfos`. A
   variableInfos MOD_CODE must sit at the end of **variableInfos** (after `iCrossbowState`'s
   INT32), not at the last POINTER before `eventInfos` — that POINTER is a character
   property. Putting it there made Nemesis ERROR(1003) on `1hm_behavior.xml` (infos nested
   into characterPropertyInfos, declared 154 vs 163 types). AMCO's `#0087` is the template.

## Menu Framework is not a settings API

SKSE Menu Framework v3 only hosts ImGui: `AddSectionItem` + open/close/render events.
MSCO's sliders (`igDragFloat` / `igCheckbox` in `settings.cpp`) mutate MSCO.dll RAM.
INI persist is CSimpleIni `settings::save()`, behind an **Unsaved changes / Save** button —
not `WritePrivateProfile`, and not on every drag. No `RequestInterface`. Re-reading
`MSCO.ini` follows last Save, not the live sliders.

MSCO.log on Save65 (Enable Log on): equipped-hand Firebolt is
`LeftMSCOStart | chargeTime=0.500 | speed=0.815` — same x and y SH2 writes. The feel
gap is not a different charge input. Do not treat tap-as-shortest as what MSCO.dll does.
