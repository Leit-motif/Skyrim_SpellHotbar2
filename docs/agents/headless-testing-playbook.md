# Headless testing playbook

What an agent can and cannot validate in a live Skyrim session through DevBench, and which
instrument answers which question. Written 2026-08-26 after the ticket 44/46/48 test campaigns.
Standing owner rulings bind everything here: **no agent screenshot capture** (frames were
technically capturable but useless as evidence); **telemetry reads, papyrus calls, and injected
input stay allowed**; visual cells go to owner eyes.

## The core misconception

"Read the name of the animation that played" is not a thing the engine publishes. Skyrim's
behavior graph does not expose "clip file currently playing" through any console, papyrus, or
DevBench seam. What it publishes instead:

- **Animation events** (`MRh_SpellFire_Event`, `MCO_WinOpen`, …) — readable when a sink or our
  DLL logs them.
- **Graph variables** (`SH2_ArtSelector`, `bAnimationDriven`, …) — readable via
  `Actor.GetAnimationVariable*` member calls through the papyrus tool.
- **OAR's Animation Log** — OAR *does* know the resolved clip path, but its text-log echo
  depends on in-game UI state (`bAnimationLogOnlyActiveGraph` + a reference selected in the log
  window) that cannot be recreated headlessly. Verified dead in a fresh session 2026-08-26; do
  not plan on it.

So clip identity is established by **inference from things the engine does publish**, not by
reading a name. That inference is usually sufficient — see the oracle ladder.

## The oracle ladder for "which animation played"

Ordered cheapest-first. Climb only as far as the acceptance cell requires.

1. **Selector inputs.** Read the ESP globals the OAR submods condition on — `0x835`
   (CastingSource: 0=left, 1=right; dual writes 0) and `0x815` (animation-type; dual is its own
   id 10016/10017) — plus `IsEquippedType` per physical hand. Proves what the selection *should*
   resolve to. The dual downgrade and `set_animtype_global` now log the id written, so
   SpellHotbar2.log corroborates.
2. **Event-hand identity.** Which SpellFire arrived — `MRh_` vs `MLh_` — proves the graph took
   the right-hand vs left-hand path. This is how the right/right-self cells went green in both
   stances for ticket 46.
3. **Annotation-timestamp oracle.** Each clip's SpellFire annotation time is unique enough that
   (event hand, wall-delta from `notified` × the logged `MSCO_attackspeed`) names which clip
   *bytes* played. Build the expected table from `hkxc-anno-cli dump` first. This is the strongest
   headless clip-identity proof we have and closed 51/57 art-pack clips on one launch.
4. **Pose trajectory** (candidate, untested). DevBench's `record` tool captures pose trajectories
   at a chosen `intervalMs` to a file — record, drive the action, stop, diff the trajectory
   against a reference capture afterward. This is the "record everything, inspect later" shape
   without pixels: the data lands on disk, so token cost is only what we read back. Worth a
   calibration spike (do two known-different clips produce cleanly distinguishable
   trajectories?) before any cell depends on it.
5. **Owner eyes.** What survives all of the above: OAR submod *naming* on screen, visual/FX
   identity, colour, "does it look right." Phrase these cells as owner cells from the start; do
   not burn a session approximating them.

**The instrument gap worth building — API surface verified 2026-08-26:** OAR's
`src/API/OpenAnimationReplacerAPI-Animations.h` (ersh1/OpenAnimationReplacer, main branch;
installed DLL is 3.2.0) exposes exactly the read we want:

```cpp
struct ReplacementAnimationInfo {
    RE::BSString animationPath{};   // the resolved clip file path
    RE::BSString projectName{};
    RE::BSString variantFilename{};
    RE::BSString subModName{};      // the OAR submod that won
    RE::BSString modName{};
};
class IAnimationsInterface1 {
    [[nodiscard]] virtual ReplacementAnimationInfo
        GetCurrentReplacementAnimationInfo(RE::hkbClipGenerator* a_clipGenerator) noexcept = 0;
    // + ClearConditionStateData(clipGenerator | TESObjectREFR*)
};
// GetAPI(InterfaceVersion::Latest) -> IAnimationsInterface*  (V1 is current)
```

`subModName` and `modName` are in the struct, so **submod naming — until now an owner-eyes
cell — becomes headless** once the probe exists. The one engineering question left is supplying
the `hkbClipGenerator*`: either **pull** (walk the player graph's active nodes on demand — a
DevBench extension tool returning every active clip's info) or **push** (hook
`hkbClipGenerator::Activate` and log the info per activation, giving a time-ordered record of
every clip that played — the "record everything, read back later" shape with no pixels).
Push is the better fit for cast-matrix cells; a clip can activate and retire between polls.
Request mechanics follow Ersh's usual pattern (exported `RequestPluginAPI_Animations`;
`GetAPI` returns nullptr on version mismatch, which doubles as the runtime version check).

## Why frame capture stays retired

Three independent reasons, any one of which is sufficient:

- **Owner ruling.** Frames cost owner patience and proved nothing a log line didn't.
- **The interesting pixels aren't in the frame.** SH2's hotbar is an ImGui/present-hook overlay
  that no capture path on this instance composites — `capture kind=providers` returns `[]`,
  native capture returned the world with no hotbar, Community Shaders' path returns black.
- **Timing.** A single post-hoc frame races the action, as the owner observed. The fix for
  timing would be frame *recording*, which no tool here provides — and for animation identity
  the oracles above are cheaper and more precise than pixels anyway. The `record` tool is the
  legitimate "grab everything, pick afterward" instrument, and it records poses, not frames.

For any visual cell: prove the *call* (a log line adjacent to the draw/refusal site — the
ticket 41 pattern), hand the pixels to the owner.

## Instruments and their sharp edges

**`input` tool** (devbench-input SKSE plugin, injects into `BSInputDeviceManager`):

- Always pass `userEvent` (control-map name from `controlmap.txt`), not just the scan code —
  `ok:true` with no effect means a wrong or missing `userEvent`.
- `holdSeconds` is ignored unless `action:"hold"` is also set. A swing needs a hold ≈0.2s as
  `device:"mouse"`, `key:0`, `userEvent:"Right Attack/Block"`. Taps are often too short for
  consumers to register.
- It cannot move the mouse cursor. Anything mouse-positional (the Binding Menu tab strip, bar
  dropdown) is owner-only by construction.
- Injection enters **downstream of `PollInputDevices`**, so a green injected matrix proves
  nothing about the physical-input path or the DLL's input hook. Input-hook changes carry an
  owner-hands cell until ticket 50's upstream harness exists.
- Mod handlers that gate on "is the bound key still held" may not see injected state — establish
  which half applies before asking the owner for a press.

**`papyrus` tool:**

- Globals and natives callable directly; **member functions** via
  `self={"form":"0x14"}` (e.g. `Actor#EquipItemEx` for left-hand equipping, `Actor#GetAnimationVariableBool` for graph reads).
- SH2 test seams: `castSlot(int)` (fires a slot exactly like its keybind),
  `slotArt(slot, artId)` (writes the live bar — clear with `slotArt(slot, 0)` after),
  `setSlotHand(slot, hand)` (0=auto 1=left 2=right 3=dual 4=voice), `GetKeyBind(int)`,
  `loadBarsFromFile(path, path)`.
- A native registered with `RegisterFunction` is only VM-callable once the `.pex` declares it.
- Re-read the live tool descriptions before declaring a seam limit; the server's own text is the
  authority and has overturned two "impossible" claims already.

**`inspect` / logs:**

- `inspect kind=state|scene|player|effects|…` for engine state; `GET /api/tools` or
  `inspect kind=registrants` for the live tool registry (a stale MCP tool list is a snapshot —
  call unlisted tools via `scenario` steps or `POST /api/tool/<name>`).
- SpellHotbar2.log is the primary oracle surface; when a diagnosis needs an ordering fact,
  **instrument first** (log the value at every candidate edge, drive one clean repetition, read
  the sequence), then remove the probe.
- `no animmotion keys` alone is not a fault (several Ashes clips are genuinely stationary); a
  real fault is the runtime's `latch N (winopen=… hitframe=…)` disagreeing with that file's own
  annotation dump.

**`record` tool:** `action=start|stop|status|replay`, `intervalMs`, `path`. Pose trajectories,
capture-then-inspect. Also offers `replay` — untested here.

## Hard headless-impossible cells

| Cell | Why | Route |
| --- | --- | --- |
| Visual/FX identity, colour, submod naming on screen | No capture composites the overlay; frames banned | Owner eyes |
| Binding Menu contents per bar | Mouse-only UI, no cursor injection | Owner eyes |
| Physical input path / input-hook changes | Injection enters downstream | Owner hands (until ticket 50 harness) |
| "Bound key still held" gates | Injected state may not reach the mod's handler | Establish per-mod, then owner |
| Dual-cast on CS-Test | Level-3 fixture has no dual-cast perks; scripted dual silently downgrades to left (now logged) | Owner's character, or in-memory perk grant |

## Iteration economics (the ten-minute problem)

The load is ~3–5 min launch-to-menu plus save load; treat a running session as the scarce
resource and plan test batches around what forces a relaunch.

Needs a full quit-and-relaunch:

- **DLL changes** — the build's link step fails while Skyrim holds the DLL, so C++ iteration is
  gated on quit (`qqq`), build, relaunch.
- **Nemesis behavior changes** — regenerate (Update Engine first if the file set changed), then
  relaunch.
- **Anything newly deployed through MO2** — the VFS is snapshotted at launch.

Live in the running session, no relaunch:

- All papyrus seams and fixtures (`castSlot`, `slotArt`, `setSlotHand`, `loadBarsFromFile`,
  console commands, in-memory perk/spell grants).
- All telemetry reads and injected input.

Consequence: **front-load the batch.** Before quitting a session, sweep every open cell that the
live seams can reach — the whole cast matrix, both stances, all selector states — rather than
one cell per launch. Land the C++/behavior changes for several tickets, relaunch once, sweep
again. Warn the owner before driving an instance they may be playing in; in-memory mutations
persist if they save. One game instance, one session: check `ListAgents` before restarting, and
copy logs out before anything rolls them.

## Worked recipe: the per-hand cast matrix

The current work's exact test, fully headless:

1. Confirm DevBench online (`devbench_status`), game loaded (`playerLoaded=true` after the
   readiness gate), latest save read-only.
2. Fixture: `loadBarsFromFile` with the bars probe JSON; `setSlotHand(slot, hand)` for the hand
   under test; draw the weapon (`input {key:19, userEvent:"Ready Weapon"}` — `castSlot` refuses
   while sheathed).
3. Drive: `castSlot(slot)` per cell — right aimed, right self, dual (dual only on a fixture
   with the perks; watch for the logged downgrade).
4. Read: `0x835`/`0x815` globals and the `set_animtype_global` log line (rung 1); which
   SpellFire hand arrived (rung 2); annotation-timestamp match against the expected table
   (rung 3) where clip bytes matter.
5. Record verdicts per cell; name the save, profile, and log lines. Leave visual identity as an
   explicitly-open owner cell — never report it green.

## Open instrument work

- **OAR-API clip-name probe — SMOKE-TESTED LIVE 2026-08-28, PASSED.** The `cliplog` tool ships
  in devbench-input 0.2.0 (repo main `b0938cf`, deployed to both MO2 copies of
  `DevBenchInput.dll`). It hooks `hkbClipGenerator::Activate` (vfunc 0x4, installed at
  kDataLoaded so OAR's replacement runs first), records every activation into a 512-entry ring —
  clip name plus OAR's resolved path/project/variant/submod/mod — and exposes
  `cliplog action=start|stop|status|read|clear` with `since`/`filter`/`limit` on read. Recording
  is OFF by default.

  The vendored-API worry is closed: OAR's `main`-branch API resolves at runtime and the
  replacement fields populate. Verbatim from the 2026-08-28 session (frame ~110120, owner's live
  save), one entry carrying full provenance including the `_variants_` pick:

  ```
  clip:    Animations\male\MT_WalkForward.hkx
  mod:     EVG CLAMBER - Slope Animations
  submod:  Upwards Walk
  project: DefaultFemale
  path:    data\meshes\actors\character\animations\OpenAnimationReplacer\CLAMBER\Upwards Walk\female\_variants_mt_walkforward
  variant: fWUP1.hkx
  ```

  That promotes the probe to rung 1 of the ladder and makes submod naming headless, as designed.
  Four operating notes the smoke test bought, each of which cost a wrong reading first:

  1. **`oarApi` is acquired lazily at `start`, so `status` on a stopped probe reports
     `oarApi:false`.** The very first reading of the session looked like "OAR is missing"; the
     same session returned `true` the instant recording began. Only trust that field while
     `recording:true`.
  2. **Empty replacement fields mean the clip was NOT replaced, not that the probe failed.**
     Vanilla clips (`RunForward.hkx`, the creature `*_Wolf.hkx` set) come back blank in the same
     read as the populated entries above. Do not diagnose from a blank-field entry alone.
  3. **The missing per-actor filter costs more than it sounds.** One nearby companion creature
     produced roughly 25 of 30 entries in a 2-second window, and a 512-entry ring drains fast at
     that rate. Always pass `filter`; budget for `dropped` on anything longer than a few seconds.
  4. **Nothing records while a blocking menu is up, and Papyrus casts refuse there.**
     `SpellHotbar.castSlot` logs `castSlot(0): not in ingame state` and returns without casting.
     An empty ring is ambiguous between "no animation played" and "the owner is in a menu" —
     check `waitUntil:"noBlockingMenu"` or the SH2 log before concluding the hook is broken.

  **SH2's own cast clips are captured too — proven the same session.** With the weapon DRAWN,
  `castSlot(0)` put the driver clip in the ring with its submod named:

  ```
  clip:    Animations\MSCO_left3.hkx
  mod:     Spell Hotbar 2 Casts
  submod:  SH2 Cast - Dual (aimed)
  project: DefaultFemale
  path:    data\meshes\actors\character\animations\OpenAnimationReplacer\SpellHotbar2Casts\cast_dual\MSCO_left3.hkx
  ```

  5. **`castSlot` needs the weapon DRAWN. A sheathed player gets
     `notified SH2_Cast3 (clip 3) -> false` and no clip at all.** This cost two runs before
     `Actor.IsWeaponDrawn` on `0x14` returned `false` and named it. The shtb cast states live in
     the drawn combat graphs, so sheathed there is no state to enter and the notify is refused
     rather than errored — `castSlot` still logs `processed`, which reads like success. Drive
     `Actor.DrawWeapon` on `0x14`, wait ~2s, confirm with `IsWeaponDrawn`, then cast. Standing
     still is NOT the requirement; drawn is.
  6. **SCAR floods the ring on a drawn 1H stance.** `SCAR_1hmReadyDummy.hkx` activated 24 times
     in ~600ms of a drawn idle, all with blank replacement fields. It is the single biggest
     consumer of ring space in a combat-ready state — filter it out explicitly on any read that
     has to span more than a second or two.

  One thing the capture raised and then settled, recorded so nobody re-investigates it: the
  winning submod was `SH2 Cast - Dual (aimed)` on a cast the DLL logged as `isolated left-hand
  caster (spell in left hand)`. That is correct behaviour, not a mis-selection. The pack has
  `cast_right`, `cast_right_self`, `cast_dual`, `cast_dual_self` and **no `cast_left`**; both
  folders ship identical `MSCO_left1-4.hkx` clips. The right submods gate on animtype `0x815`
  == 1/2 AND casting source `0x835` == 1, while the dual submods gate on `0x815` == 10016/10017
  with NO source condition — deliberately, because a dual cast leaves `0x835` at the left
  source, so one submod covers both the left-hand and dual cases. The cast wrote 10016, so
  `cast_dual` won as designed. Read "Dual (aimed)" as "left-or-dual (aimed)"; the folder name is
  the only misleading part. Owner confirmed the same session that dual casting behaves as
  expected.

- **Ticket 50 real-input harness** — upstream injection so input-hook changes stop being
  owner-only.
- **Pose-trajectory calibration** — one spike to learn whether `record` distinguishes known
  clips; if yes, it becomes rung 4 for clips whose annotations are too similar for rung 3.
