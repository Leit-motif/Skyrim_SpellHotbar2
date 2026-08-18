# 05 — Consume clip translation on shtb states

A bound Weapon Art (and a Driver Cast) plays the clip but stays planted in XY. For Disengage
the motion **is** the art. Cast Plant already blocks WASD; this ticket is the missing body
translation from the clip's own `animmotion` keys.

**Blocked by:** None

**Status:** claimed

## You test this

Same fixture as tickets 02–04: `Nolvus Awakening`, `SH2ArtBind04`, Prisoner, QASmoke, Noble
Rapier drawn.

1. Bind Disengage, press the slot, stand still (no WASD). The character leaps back. A jump in
   place fails.
2. During that leap, hold WASD. You still cannot walk or strafe out of the clip (Cast Plant
   holds).
3. Press a Driver Cast slot (Ice Spike). The small forward step a native MSCO left-hand cast
   has should appear. A cast that bobs in Z but never moves XY is the same miss as today.

If Disengage plays and XY is unchanged, it fails — the clip is not the defect.

## Agent tests the rest

4. Live position (scene inspect or equivalent) changes in XY across Disengage. AMR may already
   log that it parsed the keys; that log is not acceptance. The actor must move.
5. After the art, left-click is combo hit 1 (ticket 01). Selector returns to 0.
6. No notify into another mod's state, and no ownership variables from another mod (see
   constraints). Restore fixtures and close Skyrim after runtime work.

## What is already true (do not re-derive)

Disengage's playing file carries ~3 m of `animmotion` on Y (peak ~`-318`), duration 2.333 s.
Dumps: `.scratch/weapon-arts/disengage-aow.txt`, `disengage-delia.txt` (same keys). AMR logged
`Animation=Animations\AABL_Attack_A.hkx ... custom translation ends at 2.3333` when
`SH2_Art_Clip` activated. Live scene inspect after `slotArt` + `castSlot`: position
**unchanged** while `SH2_ArtStart` / `SH2_ArtExit` fired.

AMR does not shove the actor. It fills translation when the engine asks
`ProcessTranslationData` for that clip generator. The shtb sibling of `weapDrawReady` never
asks. Same wrap, same miss, on Driver Cast: `bAnimationDriven` true, `max_xy` = 0.

mco-integration ticket 21 already wrapped both graphs in `bAnimationDrivenIsActiveModifier`
(`bAnimationDriven`, `bAllowRotation`, later `HKSMoveON`, `bHeadTrackSpine` invert). Planted
WASD; **never produced XY**. Native MSCO moves because it lives in nested `magicbehavior`
entered through MSCO.dll. That path is closed.

## Constraints

Spell Hotbar 2 owns the trigger, the shtb state, and the consume. It does not borrow another
mod's moving state.

Do: keep `SH2_ArtStart` / `SH2_CastRight` (and siblings) and the fork's clip generators.
Change the graph around those clips so the character controller treats them as
animation-driven locomotion — vanilla variables, or a SH2-owned DLL apply of the same
annotation deltas AMR already parsed.

Do not: `NotifyAnimationGraph("MSCO_start_left")`, MSCO.dll `BeginCastLeft`, Additional
Attack's AABL event, or set `bIsMSCO` / `IsCastingLeft` / `bMSCO_*`. Do not copy `.hkx`.
Do not re-run ticket 21's HKS wrap as the plan.

Copying `.hkx` into SH2 to "embed motion" is also out: the keys are already in the playing
file.

## Ordered attempts

1. **Graph, still SH2's state.** Make `SH2_Art_State` / Driver Cast states a motion-driven
   clip the way a vanilla power attack is — parent context or the consume stack, not another
   sibling wrap of the same two bools. Pattern from the merged `1hm_behavior`; events stay
   ours.
2. **DLL fallback.** While `ArtDriver` / `MscoCastDriver` is active, apply the annotation
   deltas ourselves. Fully owned; last resort because it can fight Havok.

Ticket 04 is independent. Motion is proven when Disengage's **current** clip moves.

Spec story 14 and CONTEXT **Cast Plant**: WASD lock is mco-integration 19; clip translation
is this ticket. Archaeology for the failed HKS wrap: mco-integration 21 (do not re-open it).

## What this is not

Not a second animation. Not un-planting WASD. Not calling Ashes of War, Additional Attack,
or MSCO at runtime. Not treating AMR's parse warning as proof the body moved.

## Comments

2026-08-17 agent: Attempt 2 (SH2-owned DLL consume). Graph nest into `AttackState` was not
taken — `#4802` / `#0088` are replaced by other mods (ticket 08), and a sibling SM with
`IsAttacking` is still a sibling of `weapDrawReady`. Interpolation matches AMR (header-only
`clip_translation.h`; `clip_translation_test` green). Runtime: `hkbClipGenerator` Activate
on `SH2_Art_Clip` / `SH2_Cast*_Clip`, apply XY deltas from `animmotion` while `ArtDriver` /
`MscoCastDriver` is live.

Live on `Nolvus Awakening` / `SH2ArtBind04` / Prisoner / QASmoke / Noble Rapier drawn:

- Cell 4: `slotArt(0, 12)` Disengage, `castSlot(0)`. Scene XY `-1620.77, 1577.14` → mid-clip
  `-1587.42, 1893.93` (Δ ≈ **318** units, ~3.2 m; matches peak Y `-318.549`). After clip
  `-1590.63, 1863.44` (held, slight settle). Log: `SH2 motion: bound SH2_Art_Clip (140
  animmotion keys)`, `SH2_ArtStart` → true, `SH2_ArtExit`.
- Cell 5: after art, `getArtSelector` = 0, `MCO_nextattack` = 1.
- Cell 6: notifies were `SH2_ArtStart` / `SH2_ArtExit` / `SH2_CastRight` / `SH2_CastExit`.
  No `MSCO_start_left`, no AABL event, no `bIsMSCO` / `IsCastingLeft` / `bisAABL`. Clip
  annotations still fire `MCO_WinOpen` (the playing file's own keys).
- Driver Cast (owner cell 3 telemetry): `castSlot(2)` Ice Spike, skill type=3,
  `SH2_CastRight_Clip` (4 animmotion keys). XY `-1590.63, 1863.44` → `-1590.17, 1867.80`
  (Δ ≈ **4.4** units). Ticket 21's wrap was `max_xy = 0`.

Owner cells 1–3 still need eyes. Fixture left running: Disengage on slot 1 (index 0), Ice
Spike on slot 3, rapier drawn, selector 0, original QASmoke position. Do not quit.