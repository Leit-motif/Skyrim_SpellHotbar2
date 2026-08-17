# 21 — Apply MSCO animmotion on a Driver Cast

**Type:** defect (graph / presentation)

**What to build:** A Driver Cast plays the same small clip step a native MSCO left-hand cast
does. Ticket 19 plants the player; this ticket is the missing translation from the clip
itself.

**Blocked by:** None. Ticket 19 is resolved; this is leftover presentation on the same clips.

**Status:** deferred — cosmetic only; graph-wrap path exhausted (see 2026-08-16). Revisit via HKX animmotion spike or magic-graph bridge if product cares.

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

**Source of the OAR row:** submod `MSCO - Default Animations (Inquisitor)` inside
**Magic Casting Behavior Overhaul** at
`meshes\actors\character\animations\OpenAnimationReplacer\MSCO Animations\Base - default\`.
Requires **Open Animation Replacer** at runtime. **Not** `Smooth Magic Casting Animation`
(Nexus 45799 — empty on Nolvus Awakening; do not cite).

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

- [ ] A Driver Cast on the current fixture (Save72, Prisoner, QASmoke, Noble Rapier,
      Ice Spike left; Save65 was overwritten) shows the same small MSCO step a native
      left-hand Ice Spike does. Owner's eyes; this claim is visual.
- [ ] Ticket 19's plant still holds: WASD does not translate during the clip.
- [ ] A ticket-10 cut still unroots (state exit drops the wrap; WASD capture follows
      `is_active()`).
- [ ] Consecutive clips (ticket 14, no CastExit between them) keep the wrap for the whole
      walk.
- [ ] Restore fixtures and close Skyrim after runtime work.

## Comments

**2026-08-16 — agent: post-`bHeadTrackSpine` Nemesis build, Save72 loaded.**
Profile `Nolvus Awakening`, QASmoke, Ice Spike left, Noble Rapier right. Nemesis
rebuild from prior session (Update Engine + Build, `bHeadTrackSpine` invert on wrap).

| Probe | Result |
|---|---|
| First `castSlot(0)` | **failed** — weapon sheathed; `SH2_CastRight` → **false** |
| `DrawWeapon()` then `castSlot(0)` | `SH2_CastRight` → **true**; SpellFire; `SH2_CastExit` |
| Pose recording (third person, ~6s) | **n=1** pose sample in QASmoke — telemetry inconclusive |
| `max_xy` from anchor | ~0 (same QASmoke blind spot as before) |

Step acceptance still needs **owner eyes** on that second cast (game left running).
If still no step: `bHeadTrackSpine` invert alone is not the translator; next is
1hm vs nested-magic / OAR path, not SH2 Papyrus seams.

**2026-08-16 — owner + agent: defer ticket. Graph-wrap pursuit stopped.**
Owner: post-`bHeadTrackSpine` Nemesis build still **no step** on SH2 cast 1 vs native LH
Ice Spike. Agent agrees ROI on current path is poor.

**Findings (do not re-derive):**
- Functional cast path is fine (plant ticket 19, SpellFire, combo chain). Gap is **only** the
  small forward translation on Driver Cast.
- Native LH MSCO: nested `magicbehavior`, entered via MSCO.dll `BeginCastLeft`; `MSCO_IAM_LR`
  wrap (not `bAnimationDrivenIsActiveModifier`). Rapier drawn → SH2 plays `1hm_behavior`;
  raw `MSCO_start_left` from 1hm idle returns false.
- SH2 wrap tried: `bAnimationDriven` + `bAllowRotation`; + `HKSMoveON`; + `bHeadTrackSpine`
  invert (no `bIsMSCO` / `bMSCO_*` / `IsCastingLeft`). Plants WASD; **never produced step**
  (owner eyes + Save65/72 pose; QASmoke pose telemetry blind).
- OAR Base-default carries animmotion keys on `MSCO_left1`; loose MCBO HKX has `0 0 0` at t=0.
  Same clip name on both paths; motion difference is graph layer / entry path, not missing HKX.
- Injected `ButtonEvent` (mouse Left Attack) from DLL: queued, no native clip (wrong entry).
  OS mouse does not reach unfocused Skyrim; DevBench has no input tool.

**If reopened later (bounded options only):**
1. HKX — embed animmotion on SH2-owned copies; one-clip in-game test before batching four.
2. Architecture — bridge Driver Cast into nested magic MSCO path without stealing ownership vars.
3. Accept gap — document known cosmetic difference vs native LH when weapon drawn.


Save72, rapier drawn, Ice Spike left. Native left-hand cast: small forward move. SH2
cast 1: no movement. That is the acceptance miss; HKS/`HKSMoveON` wrap is closed as
the translator.

Patch in tree (and copied to `Dev - Spell Hotbar 2`): SH2 clip wrap now also binds
`bHeadTrackSpine` inverted, matching `MSCO_IAM_LR` slot 2. Still does **not** bind
`bIsMSCO` / `bMSCO_*` / `IsCastingLeft`. Plant bindings unchanged. Needs Nemesis
Update Engine then Build (game down), then one SH2 vs native look.



**2026-08-15 — agent: native path is not reachable from 1hm idle; HKS wrap is the wrong analogue.**
Save65, profile `Nolvus Awakening`, third person (pose samples only appear when the clip
actually bobs Z). Diagnostic DLL reverted; `castSlot` is SH2_CastRight again.

| Probe | Result |
|---|---|
| SH2 `castSlot(0)` with rapier drawn | `bAnimationDriven`/`HKSMoveON` true; Z bob ~4.2; **max_xy = 0** |
| C++ `NotifyAnimationGraph("MSCO_start_left")` with rapier drawn | **false** (1hm has no MSCO SM; nested magicbehavior is not receptive) |
| Papyrus `Spell.Cast` / `player.sae` / same-frame mouse Left Attack tap | no clip (Z unchanged) |

Merged graphs: native `MSCO_IAM_LR` on **magicbehavior only** binds `bIsMSCO`, `NotCasting`
(inverted), `bHeadTrackSpine` (inverted), `bAllowRotation`, `bMSCO_LRCasting`. It does **not**
set `bAnimationDriven`. `1hm_behavior` has no `MSCO_IAM_*` / `MSCO_start_left` at all — only
SH2's `MSCO_left*` clips. Same HKX path / OAR Base-default (`conditions: []`). Same clip
`flags=0` and `playbackSpeed` binding.

So the HKS wrap can plant (ticket 19) and cannot be the translator. Native motion lives in
MSCO's magicbehavior nest, entered through MSCO.dll `BeginCastLeft`, not a raw notify from
1hm. Next: either a real left-hand Firebolt press for the native `max_xy` signature, or
transplant `MSCO_IAM_LR`'s non-ownership bindings (`bHeadTrackSpine` invert + `bAllowRotation`)
onto the 1hm SH2 wrap without setting `bIsMSCO`.

**2026-08-15 — agent: HKSMoveON retest after correct Update Engine + Build (Save65 closed).**
Nemesis Update Engine (110s) + Build (1045 anims, 108s). Merged `1hm_behavior`: `SH2_CastRight_MG`
has 3 bindings (65/27/126 = `bAnimationDriven` / `bAllowRotation` / `HKSMoveON`).

Save65 reloaded (Xaelle, Iron Rapier drawn, left Firebolt equipped). Four `castSlot(0)` via
Papyrus with pose recording (`recording_1786843773.json`):

| Probe | Result |
|---|---|
| `bAnimationDriven` @ +400 ms after each cast | **true** |
| `HKSMoveON` @ +400 ms after each cast | **true** |
| Pose `max_xy` from anchor across casts | **0.00** (Z bob ~3.7 only) |
| Log | `SH2_CastRight` → SpellFire → `SH2_CastExit`; plant holds |

**Owner was right:** adding `HKSMoveON` and running Update Engine first did not produce visible
or measurable forward movement. Acceptance cell still red. Next: compare why HKS/OAR animmotion
applies on native MSCO path but not through SH2's bare state entry (clip flags? OAR winner?
MSCO_IAM vs this wrap?).

**2026-08-15 — agent: Nemesis rebuild + Save65 telemetry (game left running).**
Nemesis Update Engine + Build clean (1045 anims, 92s). Post-build XML: zero `#shtb$` /
`$variableID` leftovers; `SH2_CastRight_State.generator` → `SH2_CastRight_MG` →
`bAnimationDrivenIsActiveModifier` on both graphs.

Save65 (Xaelle, Iron Rapier, Firebolt left), profile `Nolvus Awakening`. Three
`castSlot(0)` driver casts via Papyrus; log shows `SH2_CastRight` → `SH2_Cast2` →
`SH2_Cast3`, all `-> true`, SpellFire left, combo windows open.

| Probe | Result |
|---|---|
| `bAnimationDriven` @ +200/+400/+800 ms after cast | **true** (graph wrap; DLL no longer writes) |
| `bAnimationDriven` at rest after clip | **false** |
| Pose recording across casts (`max_xy` from anchor) | **0.00** (ticket 19 plant holds) |
| DevBench pose Z delta during cast | ~3.8 (vertical root bob only in sampled poses) |

The MSCO **step vs native left-hand Firebolt** cell is still visual — compare in-game now
while Skyrim is up. If the step is missing on `magicbehavior` only, add the third binding
(`HKSMoveON`) per ticket escalation note.

**2026-08-15 — agent: graph wrap + DLL cleanup shipped; Nemesis not driven (MO2 down).**
Each cast state's generator is now an `hkbModifierGenerator` with
`bAnimationDrivenIsActiveModifier` (bindings: `bAnimationDriven`, `bAllowRotation` only) on
both `magicbehavior` and `1hm_behavior`. Removed `MscoCastDriver::set_rooted` /
`SetGraphVariableBool("bAnimationDriven")`; WASD capture unchanged. Patch copied to
`Dev - Spell Hotbar 2`; `combo_cache_test` and `SpellHotbar2` build green. Needs Nemesis
Update Engine + Build, then Save65 step comparison vs native MSCO.
