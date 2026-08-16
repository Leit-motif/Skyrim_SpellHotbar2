# 21 — Apply MSCO animmotion on a Driver Cast

**Type:** defect (graph / presentation)

**What to build:** A Driver Cast plays the same small clip step a native MSCO left-hand cast
does. Ticket 19 plants the player; this ticket is the missing translation from the clip
itself.

**Blocked by:** None. Ticket 19 is resolved; this is leftover presentation on the same clips.

**Status:** implemented — Nemesis re-run and Save65 visual acceptance pending

## How this showed up

Owner playtest, 2026-08-15, immediately after ticket 19's plant landed. Native MSCO casts
have a little animmotion. The SH2 Driver Cast of the same clip does not.

Not a second HKX. The shtb clip generators already name `Animations\MSCO_left1.hkx` through
`left4`. The motion is in the playing file and in how the graph wraps the clip.

## What is already true (do not re-derive)

Dumped 2026-08-15. Animation Motion Revolution is enabled.

| File | `animmotion` keys |
|---|---|
| Loose `MSCO_left1.hkx` in MCBO `animations\` | only `animmotion 0 0 0` at t=0 |
| OAR **Base - default** (`priority` 6700, `conditions: []`) | `0 -12 0` at 0.42s, then `0 45 0` at 0.57s — the small step |
| Probe copies in `Spell Hotbar 2 - MCBO Cast Animations` | **zero annotations** (stripped in ticket 05) |

OAR Base-default is always eligible for that filename, so a Driver Cast and a native MSCO
cast can play the same OAR file. Native MSCO still moves; SH2 does not. The miss is the
**graph wrap**, not a missing copy on disk.

Native MSCO wraps the left/right state machine in `MSCO_MG_LR` (`BSIsActiveModifier`
`MSCO_IAM_LR` + `MSCO_IAM_IsCastingLR`). That writes `bIsMSCO`, `bMSCO_LRCasting`,
`bAllowRotation`, `IsCastingLeft` / `IsCastingRight` while the state is live. It does
**not** wrap with `bAnimationDrivenIsActiveModifier`.

HKS custom magic in the same merged `magicbehavior` / `1hm_behavior` **does**: a
`hkbModifierGenerator` whose modifier is a `BSIsActiveModifier` named
`bAnimationDrivenIsActiveModifier`, bindings `bIsActive0 → bAnimationDriven`,
`bIsActive1 → bAllowRotation` (and HKS's `HKSMoveON` on magicbehavior). That is the wrap
that makes AMR's translation stick while WASD is planted.

SH2's `SH2_CastRight_State` (and 2/3/4) is a **bare** `hkbClipGenerator` (`flags=0`) as the
state's `generator`. Ticket 19 writes `bAnimationDriven` from the DLL. That plants against
WASD. It does not register the clip as the motion source.

The ticket 05 probe is still enabled and still has stripped copies, but it only replaces
shout paths (`1hm_shout_exhale` and friends). Driver Cast does not use those. Do not inspect
or "fix" those files as this ticket.

## The work

Wrap each shtb clip generator, on **both** `magicbehavior` and `1hm_behavior`, in the HKS /
MCO `bAnimationDrivenIsActiveModifier` pattern already in the merged graphs. All four clips
(`MSCO_left1`–`left4`). Bind `bAnimationDriven` and `bAllowRotation`. Do not bind
`HKSMoveON` unless a live miss shows it is required — that variable is HKS's, not ours.

Do **not** reuse `MSCO_IAM_LR` / `MSCO_IAM_IsCastingLR`. Those advertise MSCO ownership
(`bIsMSCO`, `bMSCO_LRCasting`, `IsCastingLeft` / `Right`) to MSCO.dll. Spec: MCBO
coexistence — ours stay separate.

Once the wrap is live, the DLL `SetGraphVariableBool("bAnimationDriven")` in
`MscoCastDriver::set_rooted` is redundant with the modifier. Remove it in the same change
so the two cannot fight on exit. Keep the WASD capture in `is_movement_blocking_cast()` —
that layer is ticket 19's plant and is not this wrap.

A graph change needs a Nemesis re-run (`Update Engine`, then Build) before it can be
tested. After the build: zero literal `#shtb$` / `$variableID` leftovers in the merged
files; each state's `generator` is the modifier generator, not the bare clip.

## What this is not

- Not another copied HKX, and not re-annotating pack files. OAR already carries the keys.
- Not un-rooting. Ticket 19's plant stays.
- Not the ticket 05 probe, and not shout-path OAR.
- Not setting `bIsMSCO` / `bMSCO_*` / `IsCastingLeft` to pick a different OAR submod.

## Acceptance

- [ ] A Driver Cast on Save65 (Xaelle, Iron Rapier, Firebolt left) shows the same small
      MSCO step a native left-hand Firebolt does. Owner's eyes; this claim is visual.
- [ ] Ticket 19's plant still holds: WASD does not translate during the clip.
- [ ] A ticket-10 cut still unroots (state exit drops the wrap; WASD capture follows
      `is_active()`).
- [ ] Consecutive clips (ticket 14, no CastExit between them) keep the wrap for the whole
      walk.
- [ ] Restore fixtures and close Skyrim after runtime work.

## Comments

**2026-08-15 — agent: graph wrap + DLL cleanup shipped; Nemesis not driven (MO2 down).**
Each cast state's generator is now an `hkbModifierGenerator` with
`bAnimationDrivenIsActiveModifier` (bindings: `bAnimationDriven`, `bAllowRotation` only) on
both `magicbehavior` and `1hm_behavior`. Removed `MscoCastDriver::set_rooted` /
`SetGraphVariableBool("bAnimationDriven")`; WASD capture unchanged. Patch copied to
`Dev - Spell Hotbar 2`; `combo_cache_test` and `SpellHotbar2` build green. Needs Nemesis
Update Engine + Build, then Save65 step comparison vs native MSCO.
