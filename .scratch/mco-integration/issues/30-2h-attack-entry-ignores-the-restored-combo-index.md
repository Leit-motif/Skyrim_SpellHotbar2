# 30 — 2H attack entry ignores the restored combo index (and 1H's success is unexplained)

Opened 2026-08-24, carved out of ticket 28. The owner's bar, set explicitly: **"it worries me
that you can't explain why 1H works — i wouldn't call that resolving the ticket."** Acceptance
here is a MECHANISM that fits every fact below — including the 1H success — plus the fix it
implies, verified per weapon class by the OAR Animation Log.

## The facts (all measured 2026-08-24; sources: `28-progress-2026-08-24-combo-fix.md`,
## `28-entry-seam-analysis-2026-08-24.md` rounds 1–2, SpellHotbar2.log probes)

1. With `MCO_nextattack=3` written, read back, and standing in the `DefaultFemale` graph at the
   attack press: **1H sword plays attack3** (post-advance sample; owner OAR screenshot and
   injected runs), **greatsword and warhammer play attack1**, every time, both fnf and channel,
   both the post-cast-end path and the mid-window chain press.
2. The graphs are weapon-agnostic node for node (round 2): one `MCO_Attack.hkx` (winner: ADXP
   MCO 1.6.0.6 Bug Fixes), one `hkbBehaviorReferenceGenerator #0863`, four weapon state infos
   over the same machinery, `startStateId`→variable-230 binding `#0030` on both AttackNodes
   machines, no `toNestedStateId` bypass, no 2H behavior file anywhere in the load order, no
   second host of `MCO_ClipGenerator_MCO_attack1` in any winning behavior, no MCO DLL.
3. `MCO_Attack.hkb` is `VARIABLE_MODE_DISCARD_WHEN_INACTIVE`; before commit `63fcf81` the
   variable was absent from `0_master` and BOTH weapons failed. Declaring it in `0_master` (via
   the shtb Nemesis patch) flipped 1H to working across two independent regenerations. It did
   nothing for 2H.
4. `MCO_currentattack` (which `AttackNodesState<N>` sets to N in-graph) declared in `0_master`
   the same way reads **0 at all times from the root graph** — even mid-swing (`c=0` in every
   probe line, commit `cc9e7e5`). So a parent-declared name does NOT live-link with the nested
   graph's storage, which falsifies the naive sync model and leaves the 1H flip UNEXPLAINED.
5. `@SGVI|...` payload tags never reach the SKSE animation-event sink. Per-graph writes are
   moot: only `DefaultFemale` carries the variable. Every SKSE-visible write moment was tested.
6. The sampling half (begin()-time `IsAttacking`-gated live sample; consume at
   `MCO_AttackInitiate`; WinClose fallback) is correct and stays — see ticket 29 for its one
   remaining semantics gap.

## Round-3 program (next session, fresh approach — the owner's read: "the problem is our
## approach")

- Produce a mechanism hypothesis that explains BOTH the 1H flip and the 2H immunity, then a
  discriminating experiment per hypothesis. Candidates yet unexamined: Havok's actual
  variable-linking semantics at `hkbBehaviorReferenceGenerator` boundaries (read serde-hkx /
  CommonLib / Havok SDK sources rather than inferring); the `MCO_Attack_StartStateId` ping-pong
  selector's role; runtime ordering of the PIE ready-stomp vs activation, which differs per
  weapon only through clip timing.
- Instrumentation available: the `SH2 probe:` per-graph read at every WinClose/press/begin
  (`c=` field for `MCO_currentattack`), the OAR text log (writes only while the Animation Log
  window is open — Risa's menu → OAR UI, toggle F13; OAR reverts ini key rebinds).
- The probe suite (`combo_probe.*`, `SH2 probe:` lines, the `0_master` `MCO_currentattack`
  declaration) stays in place until this closes; it is the measurement kit.
- Handoff (b)'s ADR remains DEFERRED until the mechanism is named — an ADR enshrining an
  unexplained behavior would be wrong twice.

## State of the branch

`weapon-arts` through `cc9e7e5` (+ notes/tickets after): sampling fixes `fc0c3a1`/`0f27b83`,
SGVI parser `16e0b82` (dead fallback), shtb `0_master` declarations (four variables), probe
instrumentation. DLL deployed 12:29→15:0x builds; Nemesis output regenerated twice with Update
Engine; 1H channel combo owner-verified working when the cast lands post-advance.
