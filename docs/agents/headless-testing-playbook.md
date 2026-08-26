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

- **OAR-API clip-name probe** — the single highest-value build; turns clip identity (path,
  variant, submod, mod) into one read. API surface verified against the header — see the
  oracle-ladder section; remaining work is the clip-generator supply (push hook preferred) and
  a live smoke test.
- **Ticket 50 real-input harness** — upstream injection so input-hook changes stop being
  owner-only.
- **Pose-trajectory calibration** — one spike to learn whether `record` distinguishes known
  clips; if yes, it becomes rung 4 for clips whose annotations are too similar for rung 3.
