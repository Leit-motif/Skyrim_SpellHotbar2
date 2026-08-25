# 30 — 2H attack entry ignores the restored combo index (and 1H's success is unexplained)

**Status:** resolved 2026-08-24 -- the mechanism is named (the nested MCO selector reads ROOT
storage; 2H was a write race), recorded in ADR-0014, and the probe suite is retired. The owner's
bar -- explain why 1H worked -- is met in the analysis below.

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

Status: resolved

## Answer (2026-08-24 evening): a write-ordering race over the ROOT graph's storage, not a
## graph difference — and the 1H success is explained by the same mechanism

The round-3 program asked for one mechanism fitting every fact, including the 1H flip. Here it
is, assembled from Havok-source research plus a live run in which **the 2H entry honored the
restored index on both greatsword and warhammer** (17:00 and 17:02, clip identity from the
packs' own advance payloads — see ticket 29's answer for that oracle).

### The mechanism

1. **Every behavior graph — root and nested — owns a private `hkbVariableValueSet`.** Confirmed
   in genuine Havok 2013 SDK source (Project Anarchy drop; research note
   `notes/30-havok-variable-linking-research.md`). Nested
   graphs link their variable names to the ROOT'S ids through a dedicated linking pass
   (`hkbSymbolLinker` / `m_internalToRootVariableIdMap`) — to the root, not to the intermediate
   graph. `VARIABLE_MODE_DISCARD_WHEN_INACTIVE` is documented verbatim: values are reset every
   `activate()`.
2. **`SetGraphVariableInt` reaches the root's storage** (`BShkbAnimationGraph` holds exactly one
   `hkbBehaviorGraph*`); nested-graph writes never surface there — which is why the
   `MCO_currentattack` probe reads 0 from the root even mid-swing (fact 4). One direction only.
3. So the value the nested `MCO_Attack.hkb` selector consumes at attack entry is **whatever the
   ROOT's `MCO_nextattack` holds when the nested graph comes back to life**. Three consequences:
   - **No root declaration → no link target → universal attack1** (pre-`63fcf81`, fact 3's
     "before" half). The declaration didn't create a sync feature; it created the root-side
     storage the always-present link needs.
   - **1H worked after the declaration** because the 1H exit path emits `SBF_ReadyStart` /
     `MSCO_MagicReady` at ready — both are consume edges (`is_restore_edge`), so SH2 re-wrote
     the restore into the root AFTER MCO's ready-enter stomp-to-1 and BEFORE the attack press.
     Root held the restore at activation. (Fact 3's "after" half, previously unexplained.)
   - **2H kept failing** because those ready edges never fire on the 2H path (measured, in the
     `is_restore_edge` comment). The stomp payloads land at ready-enter; the only remaining
     consume edge, `MCO_AttackInitiate`, fires after the nested graph has already activated and
     read its `startStateId`. The stomp always won the race. Same graph, different EVENT
     vocabulary on the way out — which is why round 2 found the topology identical (fact 2).
4. **Why it works now, on every weapon:** the stomps arrive as `Pie`-tagged payload events, and
   both of SH2's stomp-putback branches were DEAD before this session — `is_reset_payload`
   compared the tag against `"PIE"` (wrong case) and the SGVI parser only ever saw tags. Ticket
   29's payload forwarding revived the pending-restore putback: every stomp is now answered
   with a re-write within the same event dispatch (`restore pending -> put ours back`,
   17:00:13.015), so the root holds the restore when the nested graph activates. Measured
   result: greatsword restore 2 → attack2 (`playing=2 taught 3`, `restore_taught=true`),
   warhammer restore 2 → attack2. The "2H immunity" was the stomp winning a race whose
   equalizer had never actually been wired in.

### What each fact maps to

| Fact | Explanation |
|---|---|
| 1. 1H honors 3, 2H plays attack1 (15:0x builds) | ready-edge rewrite exists on 1H only; 2H stomp wins |
| 2. Graphs weapon-agnostic, one nested instance | the race is in event timing, not topology |
| 3. `0_master` declaration flipped 1H only | declaration = root storage for the link; 1H alone had a post-stomp rewrite moment |
| 4. `MCO_currentattack` reads 0 at root | nested→root writes don't propagate; the link serves root→nested |
| 5. `@SGVI` tags never reach the sink | they arrive as PAYLOADS on tag `Pie`; the hook forwarded tags only |
| 6. Sampling half correct | unchanged; ticket 29 finished its semantics |

### Named residue (honest gaps, no behavioral consequence today)

- The exact sync granularity inside the closed binary (copy at `activate()` vs per-frame sync of
  linked variables) is not observable: `hkbBehaviorGraph::activate()`/`update()` bodies were
  never published even in the open Havok drop. Both variants predict everything measured.
- The pre-declaration sword playing attack2 (13:30 observation) is consistent with the 1H nested
  instance serving its own stale storage across the cast rather than resetting — warm vs cold
  across the two weapon paths — but with the root link now authoritative this distinction no
  longer changes behavior, and settling it would need the binary.

The probe suite (`combo_probe.*`, `SH2 probe:` lines, the `0_master` declarations) stays until
the owner has felt the combo on their save; the `MCO_currentattack` declaration is now known to
be unreadable-by-design from the root and can be dropped whenever the probes retire. Handoff
(b)'s ADR is unblocked: the mechanism is named.

### Probe retirement (2026-08-24, after owner acceptance)

The measurement kit is gone: `combo_probe.*` deleted, every `SH2 probe:` line removed, the
throwaway `combo_restore_pending/peek` accessors and the ungated attack-press probe in `input.cpp`
removed. What replaced it is two `logger::debug` lines that fire once per cast and once per
advance rather than per graph per event.

Two deliberate keeps:

- **The `0_master` declarations stay.** `MCO_nextattack` is load-bearing — it IS the root storage
  the nested selector reads (ADR-0014). `MCO_currentattack` is inert by that same mechanism, but
  removing it means editing a base `#NNNN.txt`, which invalidates the Nemesis engine cache and
  costs an Update Engine plus full regeneration — perturbing the patch that makes this work, for
  no functional gain. Retire it whenever a regeneration is needed for another reason.
- **The dead `is_reset_payload` branch is deleted, not fixed.** It compared the tag against
  `"PIE"` while the engine raises `Pie`, so neither of its two call sites had ever executed. The
  payload branch does that job with strictly better gating; reviving a second putback would only
  duplicate a write.

ADR-0014 records the mechanism and the design rule for both tickets.
