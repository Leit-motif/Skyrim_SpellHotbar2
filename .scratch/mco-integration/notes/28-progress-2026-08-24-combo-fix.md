# Ticket 28 progress — combo restore: sampling fixed, but the seam is dead at entry, 2026-08-24

Branch `weapon-arts`, commits `6e1eef6` (per-graph probe), `16e0b82` (SGVI-edge sampling +
begin instrumentation), `fc0c3a1` (begin-time live sample), `0f27b83` (consume at attack
initiate). Tree committed, DLL deployed to `Dev - Spell Hotbar 2` (12:29:34).

**CORRECTION over the first version of this note: the sword case was NOT fixed.** The earlier
"PASS" identified the post-cast clip by its WinClose teaching value — the exact variable-vs-clip
trap handoff (a) forbids. Once the OAR text log was writable (see below), the same injected sword
fixture showed the post-cast swing loading `MCO_attack2.hkx` again (13:30:42.407), with the
restore value 3 written and read back standing in the graph. The owner's OAR screenshots show the
same on greatsword (`mco_attack1`, both after a channel and after a fnf cast), including a press
INSIDE the cast window (13:26:58.339, 0.8s after the cast clip), i.e. the ticket-10 direct chain
path also ignores the value.

## What IS established, all measured live 2026-08-24

1. **The sampling half is now correct and stays.** The value restored is the interrupted swing's
   true advance:
   - Live sample at `begin()` gated on graph bool `IsAttacking` (`fc0c3a1`): mid-swing the graph
     holds the advanced value (n=3 during attack2) pre-stomp; the ShoutMCO deferred re-begin
     ~270ms later reads the post-stomp 1 with `IsAttacking=0` and records nothing.
   - `@SGVI|...` payload tags NEVER reach the animation-event sink here (`16e0b82` built the
     parser, the measurement shows zero hits; kept as dead fallback).
   - Consume a pending restore at `MCO_AttackInitiate`/`MCO_PowerAttackInitiate` (`0f27b83`):
     the ready edges never fire on greatsword, and a stuck pending restore let the stomp-undo
     branch pin the combo at the restored value.
2. **The write half works at the variable level and is not the bug.** Writes land in the only
   graph carrying the variable (`DefaultFemale`; `FirstPerson` has none), survive to the attack
   press (probe read-backs n=3 at press time), and are re-asserted through every stomp.
3. **The engine does not consult the variable when an attack ENTERS the attack graph.** With 3
   standing: sword post-cast-end swing → attack2; greatsword post-cast-end swing → attack1;
   greatsword mid-window chain press (owner, physical) → attack1. This is handoff (b)'s outcome
   2 — the data seam is dead from outside, on both the ready path and the ticket-10 direct entry.
4. **Clip identity comes ONLY from the OAR Animation Log.** WinClose teaching values do not name
   the playing clip (measured: taught 4 while attack2 loaded). Do not reuse that shortcut.

## OAR logging: how to make the text log write

`AnimationLogEntry` lines reach `OpenAnimationReplacer.log` only while the in-game Animation Log
window is OPEN (owner opens it; Risa's All In One Menu re-enabled for this — modlist edit with
MO2 closed, priority preserved). Three sessions with identical ini settings and no window wrote
zero entries; with the window open the log floods (SCAR ready-dummy echoes), so filter to
`MCO_attack|MSCO_` when reading. OAR rewrites its ini from stored state at startup: the
`uToggleUIKey` 100→199 edit was reverted (UI still wants F13), `uAnimationActivateLogMode` 1→2
survived.

## Where the investigation goes next (static, no game needed)

The question is what the behavior graphs do on attack entry. hkbVariableBindingSet on
`startStateId` should be read when `AttackNodes_StateMachine` activates — the value stood at 3
then, and attack1/attack2 played anyway. Candidate explanations, decidable by decompiling the
WINNING generated behaviors (`1hm_behavior.hkx` and whichever graph hosts greatsword attacks):

- the Ready→Attack transition targets a specific nested state (`toNestedStateId` /
  FLAG_TO_NESTED_STATE_ID_IS_VALID), bypassing `startStateId` entirely;
- the binding exists on a different property or a different machine than assumed;
- the machine never deactivates between chains, so `startStateId` is only read once per draw.

If entry is hardwired, no variable write can ever work, and the control-seam refactor becomes:
SH2 already holds the correct index (RollingMcoCombo + the begin-time sample are proven) — author
N transitions (or events `SH2_ChainAttackN`) into the specific attack states and have the DLL
pick the event by the cached index, replacing the write-back entirely. Weigh that against
handoff (b)'s bypass-the-reset-state option once the graphs are decompiled; the bypass option
predicts nothing here, because the variable provably survives the reset and is still ignored.

## Remaining owner cells (unchanged)

Channel combo with a real held key, dual self conc `11004`, t0/t+3s frames, dual-cast F&F
ticket, repo-owned probe disable. The probe instrumentation (`combo_probe.*`, `SH2 probe:`
lines) stays until the entry-seam question is settled — it is the measurement kit this ticket
runs on.
