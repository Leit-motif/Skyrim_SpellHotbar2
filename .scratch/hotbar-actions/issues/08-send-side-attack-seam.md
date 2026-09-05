# 08 — Source-agnostic attack seam: cut the cast downstream of the key

**Type:** task

**What to build:** Replace every place Spell Hotbar 2 and Thu'um Reborn decide "that press
was a power attack" by reading One Click Power Attack's config with a seam that sits
downstream of the key, where every attack source looks the same. The cast cut (mco ticket 10,
extended by 45) and Thu'um's shout-lock buffer both move to that seam. The OCPA ini readers,
the `ocpa_power` Action target source, and Thu'um's `sPowerSource` auto-detection go away.

**Blocked by:** 07 (landed 2026-09-05).

**Scope ruling 2026-09-05.** Owner: "There's a lot I need to do in order to transition from MCO
to BFCO. That's going to be its own project because the way Nolvus v6 is set up is insane:
there are so many animations, and every weapon type is mapped to different animation sets per
stance. So I suppose BFCO will be out of scope until the migration." So: build the seam now
against OCPA and vanilla through the `NotifyAnimationGraph` hook, and keep the `PlayIdle` path
as a designed-for extension point. The BFCO trace, the BFCO cells below, and the `PlayIdle`
hook itself wait for the MCO-to-BFCO migration project. The OCPA readers still come out in
this ticket.

**Status:** ready-for-agent

## Why

Owner 2026-09-05: "One-click power attack is what I'm using now, but I plan on moving over to
BFCO, which has its own integration. That means that we need something that is agnostic to
the power attack source because this current listening for one-click power attack is
problematic. It's too brittle. ... I realized that Thu'um Reborn has the same issue."

And on what should be allowed through: "I think we're being too restrictive by trying to
police key events through our framework. If someone wants to cancel a cast with a block or
a power attack or a dodge (or whatever the hell they have on hotkeys), I'm just not even sure
it's worth the hassle of trying to distinguish the ones that should go through versus not go
through."

So the rule is parity with the physical key, not a curated list of which keys may cut.

## Where the coupling lives today

Spell Hotbar 2 (`ng/smf-next` after ticket 07):

- `input.cpp` `is_attack_press`: mapped right attack OR OCPA's two keys read from
  `Data/MCM/Settings/OCPA.ini` / `Data/MCM/Config/OCPA/settings.ini`. Feeds the cast cut, the
  channel chain-out, and the Ability latch capture.
- `ActionTargetSource::ocpa_power` (hidden Power Attack row, id 1) resolves OCPA's key on
  every press. Superseded by the twelve captured rows; kept so old saves load.
- Action admission no longer classifies attacks (ticket 07 finding 8, decided 2026-09-05):
  any costless Action in the committed cuttable span is admitted and cuts.

Thu'um Reborn (`C:\Nolvus\Projects\thuum-reborn`):

- `AttackInputHook` hooks `AttackBlockHandler::ProcessButton` / `UpdateHeldStateActive` and a
  raw `BSInputDeviceManager` sink that watches OCPA's key to buffer a power press during a
  shout. `Settings::PowerSource { kAuto, kOcpa, kHold, kOff }` decides by looking for OCPA's
  config.

## What the sources actually do (read 2026-09-05)

The chain for every source is: key -> the mod's handler decides "power attack" -> the handler
starts the attack on the player's graph -> the graph plays it, or refuses it inside a
committed SH2 state. Step 2 is the only step that differs per mod. The seam belongs at step 3.

Step 3 is NOT one call:

- Vanilla `AttackBlockHandler` and OCPA start the attack by sending a graph event
  (`attackStart`, `attackPowerStartInPlace`, `attackPowerStart_*`, `bashStart`, ...). That
  arrives at `IAnimationGraphManagerHolder::NotifyAnimationGraph`, vfunc 0x1 on
  `PlayerCharacter`, which SH2 already calls itself for `SH2_ArtStart` / `SH2_CastExit`.
- **BFCO does not send attack graph events.** Source `vinymayan/BFCO` branch `og`,
  `src/Events.cpp`: the power press (`Settings::iKeyAttackPowerNUM` + modifier, or a pad
  chord) resolves a `TESIdleForm` from `BFCO.esp` by FormID and plays it through
  `AIProcess::PlayIdle(actor, idle, actor)` (`BFCOIdles::PlayIdleAnimation`, line 217). Only
  the combo key and the key-up send `NotifyAnimationGraph` (`BFCOAttackStart_Comb`,
  `BFCOAttackstart_1`). `MCO_EndAnimation` is sent before the combo idle.
- MCO's own directional power attacks under vanilla input are graph events; ADXP/DAR-era
  idle-driven movesets may also use `PlayIdle`. Assume both paths are live in any load order.

So the hook must catch both `NotifyAnimationGraph` and the idle path, or sit below both. An
idle's animation event reaches the same behavior graph; whether the engine routes
`AIProcess::PlayIdle` through the virtual `NotifyAnimationGraph` or straight into
`BSAnimationGraphManager` is not known from the headers. That is task 1.

## Agent tests this

1. **Trace first.** Add a probe that logs, at `logger::info`, every event name passing
   through a `PlayerCharacter` `NotifyAnimationGraph` vfunc hook (same install pattern as
   `Animation_event_hook`, slot 0x1 on `VTABLE_PlayerCharacter[3]`: `TESObjectREFR.h` lists
   `BSTEventSink<BSAnimationGraphEvent>` at 0x30, the base the existing hook uses as index 2,
   and `IAnimationGraphManagerHolder` at 0x38, so index 3) and every `TESIdleForm` EditorID + its
   `animEventName` passing through a `AIProcess::PlayIdle` hook. One session each with OCPA
   and with BFCO: one light attack, one power attack, one bash, one block, one dodge, one
   sprint attack. Record the names verbatim in this ticket. Remove the probe afterwards.
2. From the trace, write the match rule. Prefer a prefix rule (`attackStart`,
   `attackPowerStart`, `bashStart`, `blockStart`, plus whatever BFCO's idles resolve to) over a
   per-mod list. If the idle path does not pass the virtual, hook `PlayIdle` too and match on
   the idle's `animEventName`.
3. Move the cast cut: when the matched event is sent while
   `is_committed_cast_holding_graph()` or `is_cuttable_follow_through()`, run the existing
   teardown (`cut_committed_cast_for_attack`) BEFORE forwarding to the original, then forward.
   Log one line per cut naming the event and the source (virtual or idle). Same for the
   channel chain-out and the Ability latch rule, each keeping its own predicate.
4. Delete `is_attack_press`, `get_ocpa_keys`, `read_ocpa_keys_from_vfs`,
   `resolve_ocpa_keys_live`, `parse_ocpa_keys`, `OcpaKeys`, and the `ocpa_power` /
   `dodge_hotkey` target sources (a save carrying id 1 or 2 loads as an unbound captured row;
   log it once). The dodge reader (`TK Dodge RE.ini`) goes with them.
5. Thu'um Reborn: replace the OCPA key watch in `AttackInputHook` with the same seam. A power
   press during the shout lock is an outgoing matched event; buffer it by not forwarding and
   replaying after the lock, or by whatever the existing buffer does with the key today. Delete
   `PowerSource::kOcpa` / `kAuto` and `iPowerAttackKeycode`. Keep `kHold` only if the trace
   shows vanilla hold-to-power still needs it (it should not: the hold produces the same
   outgoing event after the delay).
6. CTest for the match rule as a pure function over event names.

## You test this

Direct Cast, weapon drawn, Nolvus Awakening profile, BFCO installed:

- Cast a hotbar spell; during the tail press BFCO's power key. The cast cuts and the power
  attack plays. Same with OCPA installed instead. Same with vanilla hold-to-power and no
  power-attack mod at all.
- Same tail, press block, dodge, bash. Each one ends the cast the way the physical key would
  outside a cast. No red flash (parity with the key, not a hotbar refusal).
- Thu'um: start a shout, press the power key mid-shout under BFCO. The press is buffered and
  the power attack chains after the shout, as it does under OCPA today.
- An Action slot bound to BFCO's power key does the same as the physical key in every cell
  above.

Idle, a refused swing, or a stuck cast state is a fail.

## Notes

Native SKSE only, both mods. No new dependencies. BFCO source is MIT-licensed on GitHub;
nothing is copied from it, only its event names and the `PlayIdle` fact are used.

The hidden Power Attack and Dodge rows (ids 1, 2) are the last consumers of the OCPA and TK
Dodge readers. Removing them is part of this ticket, not a separate cleanup.
