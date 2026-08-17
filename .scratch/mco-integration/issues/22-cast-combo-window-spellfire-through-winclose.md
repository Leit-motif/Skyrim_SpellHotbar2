# 22 — Cast combo window from SpellFire through WinClose

**Type:** defect (driver timing)

**What to build:** A follow-up hotbar press during a Driver Cast chains in a comfortable
envelope, the way an MCO attack chains in recovery — one press inside the window, not a mash
from clip start and not a wait until a late marker. Open when the spell is out (SpellFire);
close at WinClose. Ticket 18 kept a GCD and paced the clip; this ticket is the **window
shape**.

**Blocked by:** None. Ticket 18 is resolved; this supersedes its open-at-WinOpen /
close-at-CastExit gate.

**Status:** resolved

## How this showed up

Owner 2026-08-15, after ticket 18: they do not want to mash. They want to continue the combo
in a comfortable envelope, like `MCO_Recovery` where the combo window would be open on an
MCO attack. Clip 3 in a chain felt slow because each step waits for WinOpen.

Ticket 20 is **not** this. That file is inbound during an MCO *attack* recovery; owner
superseded it. This is SH2's own cast-combo window.

## What is already true (do not re-derive)

`classify_hotbar_cast_press` in `skse_plugin/src/casts/combo_cache.h` chains only when
`committed_cuttable_holding_graph && combo_window_open`. Commitment is SpellFire
(`is_committed_cast_holding_graph`). The window bit is a separate latch.

Today `MscoCastDriver::observe_graph_event` sets `combo_window` on `MSCO_WinOpen` /
`MCO_WinOpen` / `MSCO_winopen` / `MCO_winopen`, and clears it on `begin` / `SH2_CastExit` /
`cancel` / `finish`. It never reads WinClose. Tests in `combo_cache_test.cpp` encode that:
`committed_press_before_winopen_is_the_gcd`, `msco_winopen_tag_opens_the_combo_window`
(SpellFire and WinClose are *not* the gate).

All four `MSCO_left1`–`left4` (loose MCBO HKX, dumped 2026-08-15):

| Marker | Authored |
|---|---|
| SpellFire | ~0.28s base / ~0.48s OAR left |
| `MCO_winopen` | 0.80s |
| `MCO_winclose` / `MCO_recovery` | 1.20s |
| clip / `SH2_CastExit` | 1.667s |

Cast clips have **no** `MCO_AllowRecovery`. `MCO_recovery` at 1.2s is end-of-clip recovery
for chaining *out* to an attack, not an inbound opener. Do not wait for it to *open* the
window. Attack chain-out (ticket 10) stays on `is_committed_cast_holding_graph()` with **no**
window bit.

## The work

TDD at `combo_cache.h` / `combo_cache_test.cpp`, then the driver latch.

1. **Open** the combo window when a committed Driver Cast observes left SpellFire
   (`MLh_SpellFire_Event` — these clips fire left). Commitment already happens there;
   the window bit can rise on the same event. Do not open on WinOpen.
2. **Close** on `MSCO_WinClose` / `MCO_WinClose` / `MSCO_winclose` / `MCO_winclose`. Also
   still clear on CastExit / cancel / begin of the next clip (a chain starts a new window).
3. Pre-SpellFire stays `refuse` (GCD / no mash-through). After WinClose, a press is
   `refuse` until CastExit kills the instance, then `start` — not a late chain.
4. No input buffer in this ticket. One press inside the open window chains immediately
   (ticket 14's in-place next clip, no CastExit). Buffering an early press to fire at
   WinOpen is a later polish if the visual cut still feels early.

Match WinClose the same way WinOpen is matched today (four casings). Flip the tests that
say SpellFire is not the gate and WinClose is not the gate.

## What this is not

- Not mash-through (ticket 14). Not inbound during an MCO swing (tickets 04 / 20).
- Not clip playback speed / `MSCO_attackspeed` (ticket 18, shipped).
- Not animmotion (ticket 21).
- Not concentration. Cuttable FNF only, same as 14/18.

## Acceptance

- [x] After SpellFire, one comfortable press chains to the next clip (Save65, Firebolt,
      public path). Owner 2026-08-16: feels the same as MSCO casts — approved.
- [x] A press before SpellFire is still refused (no 1→2→3→4 mash-through). Tests:
      `committed_press_before_spellfire_is_the_gcd`.
- [x] A press after WinClose, before CastExit, does not chain. Tests:
      `committed_press_after_winclose_is_refused`.
- [x] Combo index still walks 1→2→3→4→1. Clip-4 windup delivery unchanged (ticket 17).
- [x] Ticket 10 attack cut still works without the window bit.
- [x] Restore fixtures and close Skyrim after runtime work. Owner playtested and quit;
      DevBench ping offline 2026-08-16 17:54. No extra in-game session.

## Answer

Open the combo window on left SpellFire (`MLh_SpellFire_Event`); close it on the four
WinClose casings. WinClose does not end shtb state. Pre-SpellFire and post-WinClose
presses `refuse`. Landed as `809e031` on `origin/master` (plus this ticket close).

## Comments

**2026-08-16 — owner: approved.** Feels the same as MSCO casts.

**2026-08-16 — agent: SpellHotbar2.log from the playtest.** 16:53:47–16:53:51 Save65
Firebolt chain: `SH2_CastRight` → `MLh_SpellFire_Event` (window open) → follow-up
`SH2_Cast2` → SpellFire → `SH2_Cast3` → SpellFire → `SH2_Cast4`, then `SH2_CastExit`.
No CastExit between those clips. Later lines also show `MCO_WinClose` / `MSCO_WinClose`
closing the window without tearing the state. DLL playtested:
`DFDA32536F4903CA976FF2B2E7449D60FAED0F44092B7CF060BDFA74C6072F53`.
