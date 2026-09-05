# Hotbar Actions runtime acceptance evidence

Date: 2026-09-05 09:25–09:48 America/Chicago
Source commit: `0ad742eab92ad3108b3f2b2cad669b3a4124e27d` on `ng/smf-next`  
Review: Standards PASS and Spec PASS against fixed point `429b937`; no concrete blockers in source scope.

## Static evidence

- Native Release build completed; source DLL SHA-256: `346908B15B72B887781FCCFD192D779F4B1680898FD23B94AC28E8A8E1332788`.
- CTest: `ctest --test-dir skse_plugin/build/release -C Release --output-on-failure` — 17/17 passed.
- Papyrus `SpellHotbar.psc` compiled with Bethesda `PapyrusCompiler.exe` — 0 errors, 0 warnings; tracked PEX SHA-256: `F993A689159A5ADAF9600D36F1BB5FC0D43A70E3E8B31CD00EBE82D8BA5214F`.
- Release guard: `python python_scripts/build_mod_release.py --check` — passed; 267 files, 2 intended overwrites, 0 redundant files.

## Deployment evidence

- `skyrim-agent` bridge was refreshed and MO2 restarted; doctor reported bridge connected to `Nolvus Awakening`.
- Dry-run and apply used the root `skyrim-agent.json` and staged Data-shaped runtime tree. Apply completed through the bridge at MO2 priority 4621 for `Dev - Spell Hotbar 2 SMF Next`.
- Active profile: `C:\Nolvus\Instances\Nolvus Awakening\MODS\profiles\Nolvus Awakening`.
- Overlay hashes matched the staged source exactly:
  - `SKSE/Plugins/SpellHotbar2.dll`: `346908B15B72B887781FCCFD192D779F4B1680898FD23B94AC28E8A8E1332788`
  - `Scripts/SpellHotbar.pex`: `F993A689159A5ADAF9600D36F1BB5FC0D43A70E3E8B31CD00EBE82D8BA5214F`
  - `SKSE/Plugins/SpellHotbar/localization/translation.txt`: `5C64812817E7DF52B932C04E592672FFFABE92B2D9F3E83E3F6BA99CB0C0AB56`
- The final runtime lease was recovered with the CLI preview/apply path after the ambiguous DevBench drain; `runtime status` is `free`.

## Runtime blocker

- Skyrim was launched through the MO2 `moshortcut://:SKSE` path. DevBench loaded and `ping` returned `pong`.
- The first two launches stalled on a title-screen `inspect` call before a load; both returned the documented 504/`DEVBENCH_DRAIN` because the frame counter did not advance. The second launch used PID 101300 and was later closed gracefully.
- The third launch used PID 120812. `game loadLast` was queued first for `Save17_00000000_0_43532D544553542D4E4557_Tamriel_000251_20260905131608_1_1`; the first post-load `inspect kind=state` then returned `DEVBENCH_DRAIN` after no frame progress.
- The third session was not force-killed. The earlier `active hung` wording is superseded: contemporaneous `CommunityShaders.log` evidence showed disk-cache invalidation followed by `ShaderTiming` progress (`remaining 3412 -> 3211`), so that session is an unresolved load/compile stall rather than a proven Skyrim hang. The lease was recovered and is free.
- No Action matrix cell was claimed: slot dispatch, mode refusal, recursion, cost/GCD/CD, current-cast policy, save/reload, and owner visual/physical cells remained unproven pending a later runtime attempt.

## Follow-up: binding-tab layout fix deployment

- Source fix: commit `6ad3dd5` (`Fit binding menu category tabs`). It caps the fifteen category buttons to the available left-pane width while preserving the existing one-row layout and the 60px scaled maximum. The user-provided visual symptom is recorded at `C:\Users\Rando\AppData\Local\Temp\codex-clipboard-7ad219f2-c6e1-4b66-926a-cdd9aaec292d.png`.
- Static validation was unchanged from the reviewed source: native Release build, CTest 17/17, Papyrus 0 errors/0 warnings, and the release guard passed. The rebuilt DLL SHA-256 is `9B889C7073A0F02ACD6C716D5B478ADCDC4961F8D73196C474975FE74042FF62`.
- `skyrim-agent` dry-run and apply completed through the `Nolvus Awakening` MO2 profile at priority 4621. Deployment record: `.skyrim-agent/deployment.json`, id `20260905T152057819Z`. The active overlay DLL matches the rebuilt SHA; the PEX remains `F993A689159A5ADAF9600D36F1BB5FC0D43A70E3E8B31CD00EBE82D8BA5214F`.
- The named DevBench save request did not create a file or temporary save. The newest existing save, `Save18_00000000_0_43532D544553542D4E4557_Tamriel_000254_20260905145806_1_1`, was preserved and queued for load after deployment.
- The replacement Skyrim process is PID 53740. After the normal multi-minute load window, `CommunityShaders.log` showed cached features valid, shader resources loading, and `InitializeMenuIcons: Loaded 22/22 icons successfully`; no Community Shaders settings or cache were changed. A later DevBench state drain remained ambiguous, so no runtime acceptance cell is claimed. `GetExitCodeProcess` semantics confirmed the process is active (`HasExited=false`); it remains running for owner visual review. The orphaned lease was recovered through the documented CLI preview/apply path and is now free.
- Owner visual check remains open: with the preserved save loaded in the `Nolvus Awakening` profile, open Spell Hotbar 2's Binding Menu and verify that the `Actions` category is visible in the left tab row. Slot dispatch and all earlier action-matrix cells remain unproven.

## Follow-up: held Action mirroring deployment

- Source: `d7b0b3a` on `ng/smf-next` (`eb51046` native held mirroring + `d7b0b3a` deferred load-time release + `faf7293` contract wording). Not pushed.
- Review adjudication: the mirror reviewer's load-time release blocker is neither confirmed nor refuted from the vendored CommonLibSSE-NG headers. `BSInputEventQueue::ClearInputQueue` has no visible call sites, so enqueue-versus-dispatch ordering across a load cannot be proven statically. The change taken is the smallest one safe under both readings: `drop_live_cast` marks held targets pending without emitting, and the per-frame retry emits the up on the first frame `PlayerCharacter::Is3DLoaded()` is true again, bounded at 300 failed queue attempts. `set_input_mode` keeps the immediate release.
- Cross-action recursion: refuted from the seam. SH2's input callback is SMF's per-event hook before `TranslateInputEvent`; the playbook's 2026-08-28 contrast test records that queue-injected events reach `PlayerControls` but never re-enter `process_event`. No key-equality bypass added.
- Static validation: native Release build from the canonical checkout; CTest 17/17. DLL SHA-256 `D5F497A50D5DCFE738300F7423188DAE0008348D143F7F5287BF155BB9167218`. No Papyrus change; PEX unchanged.
- Staging: `.scratch/runtime-dist-20260905` rebuilt as a copy of the live `Dev - Spell Hotbar 2 SMF Next` overlay with the new DLL (now gitignored; the earlier "ignored" wording was wrong).
- Deployment: `skyrim-agent deploy` dry-run then `--apply` under lease `9743bc5a` (agent `fable-d1`); verified active at priority 4621; overlay DLL hash matches the build. Skyrim was not running at deploy time, so no graceful close was needed. Newest save before launch: `Save4_D2FB9A73_0_43532D544553542D4E4557_Tamriel_000314_20260905160723_1_1` (owner's 11:07 session).
- Launch: `moshortcut://:SKSE` against the already-running MO2; SkyrimSE PID 95380 at 11:36. No Community Shaders, INI, or cache changes.

Open owner cells (physical hands required; injected input cannot prove a hold):
- [ ] V directly blocks (control).
- [ ] Slot bound to an Action assigned V: holding the slot key holds Block; releasing stops it.
- [ ] Repeated tap and long hold behave like the physical key; a long hold charges once.
- [ ] Opening a menu or changing mode mid-hold leaves no stuck Block.
- [ ] Save then load while holding leaves no stuck control; log shows `SH2 action: target released (... reason=retry)` after the load.
- [ ] Twelve rows `Action 1`..`Action 12` visible in the Actions tab; existing overlays and names preserved.

### Owner live test of d7b0b3a (2026-09-05 11:49–11:52)

Owner: "i am in-game, have tested, and everything seems to be functioning correctly."

Log (`SpellHotbar2.log`, PID 95380 session, save loaded 11:46:12):
- 15 Action downs, 15 ups, every up logged `target released (... reason=source up)`. No
  `release pending`, `reason=retry`, `refused`, `no room`, or `abandoning` lines.
- Action 100 in slot 7 -> keyboard 47 (V, the Timed Block hotkey — owner: "i meant timed block. not native block"): holds of 1.83 s, 3.68 s, 2.13 s, 1.05 s with
  per-frame `queued held` mirrors carrying the source held duration, then a paired up.
- Action 101 in slot 8 -> 48 (B), later rebound to 79 (Numpad 1, OCPA power): repeated taps of
  0.10–0.94 s, each paired.
- Action 102 in slot 6 -> 81 (Numpad 3, TK Dodge): four taps of 0.12–0.15 s, each paired.
- `user_event=''` on every edge. The winning controlmap (`ZZZ - Personal Bindings`) maps
  `Left Attack/Block` and `Right Attack/Block` to mouse only (keyboard `0xff`), so none of
  these keys have an engine event; they are mod hotkeys, and the empty name is the correct
  resolution. Whether an engine-mapped key resolves its name is untested (fast-follow 07 #4).

Cells this closes (owner-confirmed behavior plus paired log edges):
- [x] Held Action slot mirrors the source hold and releases on source up (V, Timed Block).
- [x] Repeated tap and hold on the same slot behave like the physical key.
- [x] Rebinding a slot's Action target between presses takes effect (48 -> 79 on slot 8).
- [x] Twelve rows usable: Actions 100–102 bound and fired from the shipped list.

Still open (not exercised in this session):
- [ ] Menu open or input-mode change mid-hold leaves no stuck input.
- [ ] Save then load mid-hold: `reason=retry` line after the load, no stuck control.
- [ ] A costed Action charges once for a long hold.
- [ ] A key that has an engine ControlMap event resolves a non-empty `user_event` and the
      engine control responds.
- [ ] Mouse-button and gamepad targets.
- [ ] Modifier held with the slot key.
