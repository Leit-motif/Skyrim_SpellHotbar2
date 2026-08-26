# Ticket 44 — spike results, 2026-08-26

Live run on the auto-loaded CS-Test save (read-only; in-memory fixture mutations only, never
saved). Branch `ticket-44-per-hand-matrix`. Artifacts: `44-spike-artifacts.md`. Verdict:
**shape A**, recorded as ADR-0018.

## Q1 — OAR sees both globals inside the shtb state, in both stances: YES

Named winners from the OAR Animation Log (`bAnimationLogWriteToTextLog` echoes to
`OpenAnimationReplacer.log`, which the run read headlessly):

- Right-assigned cast (`0x835 == 1`): `SH2 Spike - Right Probe (CastingSource)` replaced
  `MSCO_left1.hkx` on `SH2_CastRight_Clip` — drawn-1H stance 10:29:28, magic stance 10:31:30.
- Left-assigned cast (`0x835 == 0`, `0x815 == 1`): `SH2 Spike - AnimType Probe
  (SpellAnimationType)` — drawn-1H 10:29:47, magic stance 10:31:35.
- Frame with the winning submod on-screen mid-cast:
  `evidence/44-spike/right-cast-magic-2.png` (PrintWindow captures the OAR overlay; the old
  "overlay uncapturable" note applied to the capture-provider path, not PrintWindow).
- The probe clips are additionally self-identifying: only they carry `MRh_SpellFire_Event`,
  so the SH2 log alone corroborates every selection.

Fully sheathed has no listener at all (`notified SH2_CastRight -> false`) — the two hosting
stances are drawn-1H (`1hm_behavior`) and magic (`magicbehavior`), as ticket 08 left them.

## Q2 — right SpellFire commits exactly once, vanilla isolated: YES

Right-assigned cast with Firebolt EQUIPPED in the right hand (magic stance, 10:31:31):

```
SH2 cast: isolated right-hand caster before vanilla SpellFire
SH2 cast: graph raised a right SpellFire event
SH2 cast: armed payload delivered at its own SpellFire (0.51s on the cast clock)
```

One delivery line, no fallback warning, the event swallowed before vanilla. `MRh` resolved in
BOTH hosting graphs — the `1hm_behavior` registration works (names `#0085` + eventInfos
`#0087`, see the two-array lesson in `44-spike-artifacts.md`).

Left-assigned casts played the animtype probe's MRh clip; the left-armed mask refused the
event and delivery fell back to clip end (`no SpellFire event; delivering the payload
anyway`) — the designed degradation, observed exactly as predicted.

## Dual control

`setSlotHand(0,3)` + cast (10:36:27): fast dual writes animation-type 10016, so neither probe
matched and the stock MLh clip played; the dual mask (both bits) accepted MLh, commitment
point fired, one delivery, left caster isolated. `SBF_CastStopDual` seen — the engine dual
path engaged.

## The four-step walk — a FOURTH left-only seam, found and fixed

First walk attempts stayed on clip 1: `is_msco_combo_window_open_event` matched only
`MLh_SpellFire_Event`, so an MRh clip never opened the combo window, never set
`clip_committed`, never advanced the index. Fixed (commit `cbccd1b`) to accept either hand;
after redeploy the chained walk ran `SH2_CastRight → SH2_Cast2 → SH2_Cast3 → SH2_Cast4 →`
wrap to clip 1, with clip 1 committing at `MRh` and clips 2–4 (stock, unprobed) committing
their windows at `MLh` (10:45:18–10:45:54).

## Observations for the implementation tickets

1. **Stuck `IsCasting` refusal.** After a left-assigned cast with Firebolt equipped in both
   hands, every subsequent press died silently in `allowed_to_cast` (`pc->IsCasting`), with
   no log line — the refusal branch for the outer press gate logs, the `allowed_to_cast`
   branch does not. A sheathe/draw cycle cleared it. Needs characterization; a log line in
   the silent branch would have saved twenty minutes.
2. **Test seam added:** `SpellHotbar.setSlotHand(slot, hand)` (0=auto 1=left 2=right 3=dual
   4=voice), commit `1c5740f`. The bars JSON carries no hand field and the binding menu is
   mouse-only; the whole per-hand acceptance matrix drives through this.
3. Isolated (non-chained) casts always start at clip 1; the 1→4 walk is chain behavior. The
   index survives a short idle gap between chains (first press of a later run entered clip 4).
4. The MSCO staff-swap hazard (matrix hazard 2) stays UNPROVEN — no staff on the test save;
   one staff-equipped cast with the Animation Log open settles it during ticket 46.
