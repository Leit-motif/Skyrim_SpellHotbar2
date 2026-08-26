# 49 — Ticket 46's build permanently breaks camera and controls in live play

**Type:** defect (DLL), shipping blocker. Regression introduced by the ticket-46 build
(merge `9290613`, deployed 2026-08-26 15:04) and caught by the owner within minutes of live
play, after a headless acceptance matrix passed clean.

**Status:** CLOSED 2026-08-26 by owner finding — **false alarm, external cause: reWASD.** The
owner: "it was not your changes. apparently it was rewasd not swapping profiles correctly...or
something. i dont know. but when i closed rewasd, controls were returned." This explains every
measurement: engine controls enabled, graph clean, speedmult normal, physical ESC dead — the
input was eaten upstream of the game by the remapper, not by the DLL. The quarantine is
lifted; the ticket-46 build redeploys 2026-08-26 for owner manual testing. The measurement
trail below is kept because it is a clean worked example of discriminating engine-side,
DLL-side, and OS-side input loss — and because ticket 50 (the real-input harness) was filed
off this incident and remains open on its own merits: the injected-input blind spot over
`input.cpp` is real regardless of what caused today's symptom.

## Owner report (2026-08-26, verbatim fragments)

- "whenever i touch something, my controls break?"
- "my camera doesn't even work!"
- "it's very obvious that if the camera and my controls are permanently broken after pressing
  a button, unit testing has failed."
- ESC does NOT restore controls (physically pressed, no effect).
- The Ash-of-War movement wedge earlier the same day was the owner's own testing, by their
  ruling — but "this has not been observed previously": the permanent camera+controls break
  is new with this build.

## Measurements while a wedge was live (peer session, same day, art variant)

- `Game.IsMovementControlsEnabled()` / `IsFightingControlsEnabled()` → TRUE
- `bAnimationDriven`, `bMotionDriven`, `IsCastingRight`, `IsAttacking` → all FALSE
- This session, during the owner's camera-dead state: looking, cam-switch, movement, fighting
  controls all TRUE; `speedmult` 107.

So: Papyrus controls enabled, graph clean, speed normal — yet physical input dead. Everything
points at input being eaten before the engine sees it, i.e. DLL-side, on the physical-input
path.

## Why the acceptance matrix could not see it

Injected DevBench input enters DOWNSTREAM of the mod's `PollInputDevices` hook (verified
2026-08-12, noted at `input.cpp` ~:498). The one file in the ticket-46 diff on the
physical-input path (`src/input/input.cpp`) is therefore structurally unreachable by the
harness. Ticket 50 (real-input harness) exists because of this.

## Suspect surface (the whole ticket-46 C++ diff, commit `94d7945`)

1. `src/input/input.cpp` — the `allowed_to_cast` refusal branch rewrite (reads as
   semantically identical; it is the only physical-input-path change, so it is suspect #1
   despite that).
2. `src/events/animationeventhook.cpp/.h` — per-event arming snapshot; isolation now gated
   on the armed mask; `ProcessEvent` signature change.
3. `src/casts/casting_controller.cpp/.h` — `spellfire_state` single-word rework
   (mask+latch+generation), CAS in `notify_spellfire`.
4. `src/casts/combo_cache.h` — armed-gated predicates.
5. `src/casts/msco_cast_driver.cpp/.h` — commitment gate signature.

ESC not closing the break argues AGAINST the RenderManager blocking-frame path
(`input.cpp:444` closes frames on physical ESC-down). The break also occurred in an instance
with zero injected input (the art wedge run), so injection residue does not explain it.

## Repro key (BLOCKED on owner)

Which button, and does it break from a fresh load or only after a cast? Owner's phrasing
"whenever I touch something" suggests it recurs per interaction. Log fragments from the play
window show presses `device=0 key=15` (Tab / Tween Menu) and `device=1 key=0/1` (mouse)
during committed casts, all answered by the press gate.

## Diagnosis rules

- Diagnose OFFLINE first: re-read the diff assuming the symptom is real; then a dedicated
  test profile, never the owner's play profile.
- The rolled-back build serving the owner is the control: if the symptom is gone there, the
  regression is confirmed inside the ticket-46 diff.
- Instrument before theorising (domain rule): a log line at every capture site that names
  which branch set `captureEvent=true` would have named this in one press.

## Acceptance

- [ ] Mechanism named: the exact line(s) that eat physical input after the triggering press.
- [ ] Fix lands with a regression test that exercises the REAL input path (ticket 50 harness,
      or an owner-hands cell explicitly scheduled).
- [ ] Owner plays the fixed build on a test profile and confirms camera + controls survive
      the triggering action.
- [ ] Only then does ticket 46's feature work redeploy.
