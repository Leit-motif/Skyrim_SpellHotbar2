# Ticket 05 progress — 2026-08-10/11 session (branch claude/spellhotbar2-mco-animation-a6629d)

## What now works (live-verified, save `Codex_T05_Smooth_Riverwood`)

The invalid `NotifyAnimationGraph("ShoutStart")` entry is replaced and the whole voice
pipeline runs for a hotbar spell cast:

1. `try_start_cast` equips a **runtime dummy TESShout** (IFormFactory; word taught +
   unlocked once per session; payload = runtime clone of the do-nothing unbind effect list
   reshaped to `kVoicePower` with 0.9s charge; recovery 0 on all three words).
2. The press goes through **BSInputEventQueue** with full button semantics: down, per-frame
   held repeats (`VoiceCastDriver::tick` from the game loop), timed release at the cast
   time, and a guaranteed release inside `restore()` (a missing release locks the player
   out of attack/sheathe — reproduced and fixed).
3. Telemetry for one Incinerate cast: press → `IsShouting` true +50ms → held 0.55s
   (charge) → timed release → `Voice_SpellFire_Event` +19ms → committed cast, fireball
   delivered, controls clean afterward.
4. OAR selects the MCBO probe clip for the shout states during the cast — user-visible in
   the OAR Animation Log: `1HM_Shout_Inhale/Exhale.HKX` → `SpellHotbar2_MCBO\cast_left_probe`.

Failed approaches, do not retry: ProcessButton direct call (accepted, no transition);
lesser-power proxy (no IsShouting, no shout clip); zero-charge payload (word fires 56ms
after press, no charge phase).

## The remaining defect

With the entry fully valid, the MCBO clip **still T-poses on the inhale and shows nothing
on the exhale** (user-observed 2026-08-11 ~00:00). Ruled out since: skeleton binding
(runtime rig is VANILLA `Skyrim - Animations.bsa`, no loose skeleton_female.hkx anywhere;
97-track positional clips are exactly what vanilla plays), Havok class/version (same
hkaSplineCompressedAnimation as SYHO's working clips).

**Prime suspect: the clip's embedded annotations.** `MSCO_left1.hkx` carries 7 annotations
incl. `MRh_SpellFire_Event` at 0.283s and MSCO window events. Clip annotations fire as
graph events; a right-hand spellfire event mid-shout-charge is an invalid transition at
almost exactly the moment the T-pose appears.

## Next steps (in order)

1. **Strip annotations** from the 16 probe copies (`hkxc-anno-cli` at
   `C:\Tools\SkyrimHKX`, probe at `Spell Hotbar 2 - MCBO Cast Animations\...\cast_left_probe`),
   relaunch, cast Incinerate (slot 0 via `SpellHotbar.castSlot(0)` — needs focused window
   or the user pressing "1"), observe.
2. If still broken: swap probe clips to **Smooth Magic Casting** clips (annotation-free,
   confirmed clean): `Smooth Magic Casting Animation\...\Nolvus OAR Stance Movement
   Framework\Magic Movement\mlh_preCharge.hkx` → the 4 `*_shout_inhale` paths,
   `mlh_Release.HkX` → the 8 `*_shout_exhale*` paths.
3. Polish once a clip plays: map the proxy chargeTime to the spell's cast time, verify
   exhale duration, concentration spells (`replay` path), repeat-cast spam, real shouts
   still SYHO, first person.

## Test harness recipe (this session's working loop)

- Build: VS dev shell + `VCPKG_ROOT=C:\Nolvus\_vcpkg`, `cmake --build build/release` in the
  worktree's `skse_plugin` (post-build copies DLL into `Dev - Spell Hotbar 2`; game must be
  closed). The pex with the `castSlot` declaration is deployed at
  `Dev - Spell Hotbar 2\Scripts\SpellHotbar.pex`.
- Fixture: load save, `SpellHotbar.loadBarsFromFile(<worktree>\.scratch\evidence\
  bars-fixture-probe.json, same)` (slot0 Incinerate on MAIN + 1HSP bars).
- Cast: `SpellHotbar.castSlot(0)` via DevBench papyrus (dispatch requires window focus).
- Evidence: `capture_ingame.ps1` background frames + `SpellHotbar2.log` debug telemetry +
  OAR Animation Log (enabled via `MODS\overwrite\SKSE\Plugins\OpenAnimationReplacer.ini`,
  needs a reference selected in the OAR UI to display).
