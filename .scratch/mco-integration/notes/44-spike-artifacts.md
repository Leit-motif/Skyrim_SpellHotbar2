# 44 — Spike artifacts: per-hand SpellFire, right-hand probe clip, OAR probes

Built 2026-08-26 in an isolated worktree of `main`. **Nothing here was deployed** — no MO2 mod,
game, or Nemesis run was touched. The coordinator deploys and runs.

The spike exists to answer two questions with one live session:

- **Q1** — can OAR select a per-hand clip variant for the fork's Nemesis-registered Driver Cast
  states by conditioning on `SpellHotbar_CastingSource` (`SpellHotbar.esp` `0x835`) and
  `SpellHotbar_SpellAnimationType` (`0x815`), in **both** hosting graphs (`magicbehavior`,
  `1hm_behavior`)?
- **Q2** — can a right-hand SpellFire clip (`MRh_SpellFire_Event`) commit the cast exactly once
  without vanilla also firing an equipped right-hand spell?

---

## 1. Artifact inventory

| # | Artifact | Path (worktree-relative) |
|---|---|---|
| 1 | Per-hand SpellFire arming | `skse_plugin/src/casts/casting_controller.cpp` |
| 2 | Per-hand caster isolation (predicate) | `skse_plugin/src/casts/combo_cache.h` |
| 2 | Per-hand caster isolation (hook) | `skse_plugin/src/events/animationeventhook.cpp` |
| 2 | Unit tests for the predicate | `skse_plugin/src/casts/combo_cache_test.cpp` |
| 3 | Right-hand probe clip | `data/…/OpenAnimationReplacer/SpellHotbar2Spike/right_probe/MSCO_left1.hkx` |
| 3 | Second copy for the animtype probe | `data/…/OpenAnimationReplacer/SpellHotbar2Spike/animtype_probe/MSCO_left1.hkx` |
| 4a | `MRh_SpellFire_Event` graph registration | `nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#0085.txt` |
| 4b | OAR probe replacer mod (3 configs) | `data/…/OpenAnimationReplacer/SpellHotbar2Spike/**/config.json` |
| 5 | This file | `.scratch/mco-integration/notes/44-spike-artifacts.md` |

`data/…` is `data/meshes/actors/character/animations/`.

---

## 2. Piece 1 — per-hand SpellFire arming

`CastingController::arm_spellfire(hand_mode)` no longer discards the resolved hand. It now arms:

| resolved hand | mask |
|---|---|
| `left_hand` | `fire_left` |
| `right_hand` | `fire_right` |
| `dual_hand` | `fire_left \| fire_right` |
| anything else (`auto_hand`, `voice`) | `fire_left` + a `logger::debug` line naming the value |

`used_hand` is always resolved by `GameData::set_weapon_dependent_casting_source()` before the
call, so the default arm is a diagnostic, not an expected path.

**Delivery-once is untouched.** `notify_spellfire()` only raises the `spellfire_seen` latch, and
ticket 43's delivery path reads that latch once per cast. A dual cast that raises both authored
events sets the same flag twice and delivers once. Neither the latch, `clear_spellfire()`, nor
`is_cast_committed()` changed.

The old "Minimal slice … per-hand masking returns with the per-hand clip matrix" comment is
replaced with the comment that describes what now happens and why arming both for dual is safe.

## 3. Piece 2 — per-hand vanilla-caster isolation

`combo_cache.h`: `isolate_left_hand_caster_before_vanilla_spellfire(bool, bool)` became

```cpp
enum class SpellFireHand { none, left, right };

[[nodiscard]] constexpr SpellFireHand isolate_caster_before_vanilla_spellfire(
    bool driver_cast_active, SpellFireHand event_hand) noexcept;
```

Still a pure constexpr predicate, still unit-tested. `none` doubles as "this event is not a
SpellFire" and "nothing to isolate", so one value answers the question and names the caster.

`animationeventhook.cpp::ProcessEvent_PC` maps the tag to `SpellFireHand`
(`MLh_SpellFire_Event` → left, `MRh_SpellFire_Event` → right, anything else → none), asks the
predicate, and on a hit interrupts `kLeftHand` / `kRightHand` accordingly before taking the same
skip-vanilla path as before. The debug line now names the hand:
`SH2 cast: isolated {left|right}-hand caster before vanilla SpellFire`.

**Dual:** both events arrive, each isolates its own hand. That is correct — both equipped hands
must be silenced — and the SpellFire latch still delivers once.

Tests: the existing `driver_cast_isolates_before_vanilla_sees_left_spellfire()` was ported to the
new signature, and `driver_cast_isolates_the_hand_the_spellfire_event_names()` was added for the
right-hand and the not-active-driver cases.

## 4. Piece 3 — the right-hand probe clip

### Source

`MSCO_left1.hkx` is **not shipped by this repo** — it comes from the MSCO mod. The clip that
actually plays for a Driver Cast today is the OAR winner:

```
…\MODS\mods\MSCO Magic Casting Behavior Overhaul - custom animations
  \meshes\actors\character\animations\OpenAnimationReplacer
  \MSCO Animations\Base - default\MSCO_left1.hkx
```

(submod `MSCO - Default Animations (Inquisitor)`, `priority 6700`, `conditions: []`; it shadows the
same path in the base MSCO mod through the VFS). SHA-256 `0DADD8EA546D42E9…`, 13 152 bytes. The
probe is a copy of **that** file, so the only difference between control and probe is the
annotation's hand.

### Annotation dump — BEFORE

```
# Animation: MSCO_left1.hkx
#
# duration: 1.666667
# numberOfTransformTracks: 97
# annotations: 10
#
0.000000 animmotion 0 0 0
0.000000 PIE.@SGVI|MSCO_nextleft|2
0.000000 PIE.@SGVI|MSCO_nextright|1
0.483333 MLh_SpellFire_Event
1.636667 MCO_Recovery
0.000000 animmotion 0 0 0
0.416667 animmotion 0 -12 0
0.566667 animmotion 0 45 0
0.680000 MSCO_WinOpen
1.633333 MSCO_WinClose
```

### Annotation dump — AFTER (re-dumped from the written binary)

```
# Animation: MSCO_left1.hkx
#
# duration: 1.666667
# numberOfTransformTracks: 97
# annotations: 10
#
0.000000 animmotion 0 0 0
0.000000 PIE.@SGVI|MSCO_nextleft|2
0.000000 PIE.@SGVI|MSCO_nextright|1
0.483333 MRh_SpellFire_Event
1.636667 MCO_Recovery
0.000000 animmotion 0 0 0
0.416667 animmotion 0 -12 0
0.566667 animmotion 0 45 0
0.680000 MSCO_WinOpen
1.633333 MSCO_WinClose
```

One string changed. Timestamp `0.483333` kept, all ten annotations kept, both `PIE.@SGVI` combo
payloads kept, the `MSCO_WinOpen`/`MSCO_WinClose` window and `MCO_Recovery` kept, `animmotion`
keys kept, duration and 97 transform tracks kept.

`hkxc verify`:

```
Verifying...
Complete hkx reproduction: …\SpellHotbar2Spike\right_probe\MSCO_left1.hkx
Verifying...
Complete hkx reproduction: …\SpellHotbar2Spike\animtype_probe\MSCO_left1.hkx
```

### Two deviations from the brief, both deliberate

1. **The file is named `MSCO_left1.hkx`, not `SH2_spike_right1.hkx`.** OAR binds a submod to a base
   animation by **path match**: the graph requests `Animations\MSCO_left1.hkx`, so the replacement
   inside the submod folder must carry that exact name. A file called `SH2_spike_right1.hkx` would
   match no base animation and silently do nothing — the failure mode the OAR skill warns about.
   The submod *folder* name (`right_probe`) is what appears in the Animation Log, and that is what
   identifies the probe at runtime.
2. **The written file is 15 456 bytes, not 13 152.** `hkxc-anno-cli update` round-trips HKX → XML →
   HKX, and serde-hkx's re-serialization is not byte-identical to the original packer. `hkxc verify`
   reports a complete reproduction and the re-dump matches, so this is a serialization-layout
   difference, not content loss. It is worth knowing before anyone diffs sizes.

3. The Dragon Age donor pack (`C:\Nolvus\Projects\_animations\dragon-age-staff-animations`) was
   **not touched**. Donor assignment stays owner-gated.

## 5. Piece 4a — graph event registration

Ticket 08's lesson: a clip annotation resolves against its **hosting graph's** event table.

**`magicbehavior` — no edit needed, and here is the evidence.** `MRh_SpellFire_Event` is already in
the *base* event table of the patch base
`nemesis/Nemesis_Engine/mod/shtb/magicbehavior/#0077.txt` at line 29 — above the
`<!-- MOD_CODE ~shtb~ OPEN -->` block, inside the stock `numelements="97"` list. `MLh_SpellFire_Event`
is there too (line 45), which is exactly why the `shtb` block in that file adds neither. Same for
`0_master/#0106.txt` (both events in base, lines 12 and 20).

**`1hm_behavior` — edit made.** The base list in
`nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#0085.txt` is `numelements="461"` and contains
**no** `MRh_*` event at all; that is why the `shtb` block already had to add
`MLh_SpellFire_Event` by hand. `MRh_SpellFire_Event` was added immediately after it, inside the
same `MOD_CODE ~shtb~` block, following the existing pattern exactly:

```diff
 				<hkcstring>MLh_SpellFire_Event</hkcstring>
+				<hkcstring>MRh_SpellFire_Event</hkcstring>
 				<hkcstring>SH2_Cast2</hkcstring>
```

One added line, CRLF preserved, `numelements` untouched (Nemesis recomputes it for the injected
block, as it already does for the eight `SH2_*` names).

> ### WARNING — this needs **Update Engine** before **Launch**
>
> The change edits a base `#NNNN.txt` under `Nemesis_Engine\mod\shtb`, which invalidates the
> Nemesis engine cache. If you press Launch first, Nemesis writes one line to `PatchLog.txt` —
> `Engine update required due to this file: <path>` — generates nothing, and leaves the window
> open, so a wrapper polling for `Behavior generation complete` will wait out its entire timeout
> against a refusal that landed in the first second.
>
> Press **Update Engine**, wait for `Engine update complete` in `UpdateLog.txt`, *then* Launch.
> `run-nemesis.ps1 -Tick shtb -Apply -UpdateEngine` does all three in order. Run exactly one
> Nemesis instance, and do not kill its window while MO2 still holds the dispatch.

## 6. Piece 4b — the OAR probe submods

New replacer mod `SpellHotbar2Spike` under
`data/meshes/actors/character/animations/OpenAnimationReplacer/` — the same shipped-data location
and the same `animations\OpenAnimationReplacer\` root the fork's `SpellHotbar2Arts` pack already
uses, so the submod folder mirrors paths relative to `…\character\animations\` and a file named
`MSCO_left1.hkx` sitting directly in the submod folder matches the graph's
`Animations\MSCO_left1.hkx`. (MSCO's own submods are laid out identically; upstream Spell Hotbar 2
uses the other legal root, `…\character\OpenAnimationReplacer\`, with an `animations\` subfolder —
both are valid, and this fork's convention was followed.)

```
SpellHotbar2Spike/
  config.json                    (mod-level: name + the IsPlayer conditionPreset)
  right_probe/
    config.json                  priority 2000002002
    MSCO_left1.hkx               the MRh probe clip
  animtype_probe/
    config.json                  priority 2000002001
    MSCO_left1.hkx               same clip, duplicated
```

### Why two probes, each on ONE variable

Conditioning one submod on both globals makes a miss ambiguous. Instead each probe tests exactly
one variable, and the priorities are ordered so a single pair of casts separates them:

| Cast | `0x835` (source) | `0x815` (animtype) | Expected winner | Reads as |
|---|---|---|---|---|
| right-assigned fire-and-forget | 1 | 1 | `right_probe` | OAR sees the casting-source global |
| left-assigned fire-and-forget | 0 | 1 | `animtype_probe` | OAR sees the animation-type global |
| either, nothing wins | — | — | MSCO `Base - default` | OAR sees **no** SpellHotbar global while the shtb state is active → shape B |

`right_probe` is the higher priority on purpose: on a right cast both conditions are true, and the
more specific variable must be the one the log names.

### Grammar provenance

The `CompareValues` / global-form shape is copied verbatim from upstream Spell Hotbar 2's own
installed `cast_1h_right/config.json`, not invented:

```json
"Value A": { "form": { "pluginName": "SpellHotbar.esp", "formID": "835" } },
"Comparison": "==",
"Value B": { "value": 1.0 }
```

Values confirmed from the fork's code, not assumed:

- **`0x835` right = `1`.** `set_weapon_dependent_casting_source()`
  (`game_data.cpp:1879-1919`) writes `static_cast<int>(RE::MagicSystem::CastingSource)`;
  `kLeftHand = 0`, `kRightHand = 1`. Dual writes the **left** source (`0`) as its representative.
  Upstream's `cast_1h_left` / `cast_1h_right` compare `0x835` against `0.0` / `1.0`, agreeing.
- **`0x815` fire-and-forget aimed = `1`.** `cast_anim_ids.h:30` — `kAimed{ 1U, 10016U }`;
  `start_cast()` calls `set_animtype_global(anim)` with the family's primary id. Upstream's
  single-hand cast submods compare `0x815` against `1.0`, agreeing. (The dual variant is `10016`,
  so a dual cast will match **neither** probe — expected, and not a failure signal.)

The `IsPlayer` `conditionPresets` entry is copied verbatim from upstream's mod-level config
(`OR { IsActorBase Skyrim.esm:14, IsActorBase Skyrim.esm:7 }`) so probe and probed agree on who
counts as the player. Note lowercase `conditions` at submod top level and capital `Conditions`
inside the `OR`.

### Priorities

Everything competing for `Animations\MSCO_left1.hkx` today is far below these:

| Submod | priority |
|---|---|
| MSCO `Base - default` (Inquisitor) | 6 700 |
| upstream `Spell Hotbar 2` cast submods | 99 000 0xx |
| `Spell Hotbar 2 - OAR Priority Over SYHO` | 101 000 0xx |
| fork `SpellHotbar2Arts` Custom Ability | 2 000 001 001 |
| **`animtype_probe`** | **2 000 002 001** |
| **`right_probe`** | **2 000 002 002** |

### Known spike-only side effect

While `animtype_probe` is installed, a **left**-assigned fire-and-forget cast also gets the MRh
clip. With piece 1 in place that cast arms `fire_left` only, so the MRh event does not commit it
and delivery falls back to ticket 43's cut/clip-end latch — visible in the log as
`SH2 cast: no SpellFire event; delivering the payload anyway`. That is expected during the spike,
**not** a regression. Delete the whole `SpellHotbar2Spike` folder once the A-vs-B decision is
recorded.

---

## 7. Deployment

Every artifact in this ticket lands in the **same MO2 mod**:

```
C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Dev - Spell Hotbar 2
```

That mod's tree mirrors the repo's shipped-data roots one-for-one:

| Repo | MO2 mod |
|---|---|
| `data/meshes/…` | `Dev - Spell Hotbar 2\meshes\…` |
| `nemesis/Nemesis_Engine/mod/shtb/…` | `Dev - Spell Hotbar 2\Nemesis_Engine\mod\shtb\…` |
| built `SpellHotbar2.dll` | `Dev - Spell Hotbar 2\SKSE\Plugins\SpellHotbar2.dll` |

The DLL copy is normally done by the build itself: `skse_plugin/configure-release.bat` sets
`-DOUTPUT_FOLDER=C:/Nolvus/Instances/Nolvus Awakening/MODS/mods/Dev - Spell Hotbar 2` and
`CMakeLists.txt:163-181` adds a post-build `copy_if_different` into `<OUTPUT_FOLDER>/SKSE/Plugins`.
**That copy was NOT performed by this session** — see §8.

There is no repo script that deploys `data/` or `nemesis/`; `deploy/weapon-art-icons/` is a
different mod (`Dev - Spell Hotbar 2 Weapon Art Icons`) and does not cover them. The coordinator
copies those two trees by hand or by whatever flow the owner normally uses.

---

## 8. Proof commands and outputs

### Build

`skse_plugin/build-release.bat` and `configure-release.bat` both hard-wire
`PLUGIN=C:\Nolvus\Projects\spell-hotbar-2\skse_plugin` (the **canonical** checkout, not this
worktree) and `configure-release.bat` hard-wires `OUTDIR` to the live MO2 mod. Running either from
here would have built the wrong tree *and* deployed. The build was therefore reproduced with the
same underlying commands — same `vcvars64.bat`, same cmake, same preset, same pinned `cl.exe` —
but pointed at the worktree and at a scratch output folder:

```bat
set "VCPKG_ROOT=C:\Nolvus\_vcpkg"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set "PATH=…\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"
cd /d "<worktree>\skse_plugin"
cmake --preset release "-DOUTPUT_FOLDER=<scratchpad>/spike-out" "-DCMAKE_CXX_COMPILER=…\14.41.34120\…\cl.exe"
cmake --build build/release --config Release
```

(The plain cmake call without `vcvars64.bat` fails with `fatal error C1083: cstdint`.)

Result — clean, 72/72:

```
[71/72] Building CXX object CMakeFiles\SpellHotbar2.dir\src\rendering\advanced_bind_menu.cpp.obj
[72/72] Linking CXX shared library SpellHotbar2.dll
```

Only pre-existing `C4100`/`C4551` warnings; none in the four files this ticket changed.

The DLL landed at `<scratchpad>/spike-out/SKSE/Plugins/SpellHotbar2.dll`. The live mod's DLL is
untouched — `Dev - Spell Hotbar 2\SKSE\Plugins\SpellHotbar2.dll` still reads
`LastWriteTime 8/25/2026 11:34:14 PM, 1984512 bytes`, i.e. yesterday's build.

**The coordinator must build-and-deploy for the live run** (`configure-release.bat` +
`build-release.bat` from the canonical checkout, after merging this branch).

### Unit tests

All seven test executables the CMake file defines, run from the worktree build:

```
--- art_bind_record_test.exe   exit=0   ok
--- art_data_test.exe          exit=0   ok
--- art_pack_gen_test.exe      exit=0   ok
--- bind_drop_test.exe         exit=0   ok
--- cast_anim_ids_test.exe     exit=0   ok
--- clip_translation_test.exe  exit=0   ok
--- combo_cache_test.exe       exit=0   ok
TOTAL FAILING SUITES: 0
```

`combo_cache_test` is the one carrying the new per-hand isolation cases.

### HKX

```
> hkxc verify …\SpellHotbar2Spike\right_probe\MSCO_left1.hkx
Verifying...
Complete hkx reproduction: …\right_probe\MSCO_left1.hkx

> hkxc verify …\SpellHotbar2Spike\animtype_probe\MSCO_left1.hkx
Verifying...
Complete hkx reproduction: …\animtype_probe\MSCO_left1.hkx
```

### JSON

All three `config.json` files parse (`ConvertFrom-Json`, no errors).

---

## 9. Live-run checklist for the coordinator

Setup, in order:

1. Merge/deploy the four trees into `Dev - Spell Hotbar 2` (§7).
2. **Nemesis: Update Engine, then Launch** with `shtb` ticked (§5's warning box).
3. In the OAR MCM / in-game editor, open **Animation Log** and set it to log replacements. Confirm
   **Detected Problems** shows nothing for `SpellHotbar2Spike` — an INVALID condition there means a
   grammar or form-resolution miss, and every later observation would be meaningless.
4. Bind two fire-and-forget aimed spells: one assigned **Left**, one assigned **Right**.
   Keep an ordinary spell equipped in the **right** hand for the double-fire test.
5. Enable `logger::debug`/`trace` for the SH2 log.

### Q1 — does OAR see the globals, in both hosting graphs?

The two hosting graphs are entered by weapon state, so run every cast twice:

- **`magicbehavior`** — weapon **sheathed** (or spells equipped / magic stance).
- **`1hm_behavior`** — a **one-handed weapon drawn**.

| Cast | OAR Animation Log line proving selection |
|---|---|
| right-assigned, sheathed | replacement of `MSCO_left1.hkx` by submod **`SH2 Spike - Right Probe (CastingSource)`** |
| right-assigned, 1H drawn | same submod name, same line |
| left-assigned, sheathed | submod **`SH2 Spike - AnimType Probe (SpellAnimationType)`** |
| left-assigned, 1H drawn | same submod name |

Read the **submod name**, not the file name — both probes ship the identical clip, so the name is
the only discriminator.

Verdict:

- Right probe wins in **both** graphs → `SpellHotbar_CastingSource` is visible while the shtb state
  holds the graph. Combined with the animtype probe winning on the left cast, **Q1 is yes and
  shape A is live**.
- Right probe never wins but animtype probe does → OAR sees `0x815` and not `0x835`; hand selection
  cannot be driven from casting source → **shape B**.
- Neither wins (log shows MSCO `Base - default` / `MSCO - Default Animations (Inquisitor)`) → OAR
  sees no SpellHotbar global during the shtb state → **shape B**, and record which graph failed.
- One graph selects and the other does not → that asymmetry is the finding; capture both logs.

Visual identity still needs a **frame**, per the acceptance matrix: a log line proves selection, not
appearance. Take one screenshot per hosting graph.

### Q2 — does a right SpellFire commit exactly once?

Right-assigned cast, ordinary spell **equipped in the right hand**, weapon sheathed:

- **Commit** — SH2 log at ~0.48 s into the clip:
  `SH2 cast: graph raised a right SpellFire event`
  (This line only prints when the arming mask accepts the hand, so it is simultaneously proof that
  piece 1 armed `fire_right` and that the Nemesis registration resolved the annotation.)
- **Isolation** — immediately before it:
  `SH2 cast: isolated right-hand caster before vanilla SpellFire`
- **Exactly once** — exactly **one** projectile/effect leaves the character for the hotbar spell.
  The equipped right-hand spell must **not** also fire. Two projectiles, or the equipped spell's
  own effect appearing alongside the hotbar spell's, is a Q2 failure.
- **No fallback** — `SH2 cast: no SpellFire event; delivering the payload anyway` must be **absent**
  for the right cast. Its presence means the annotation did not resolve in that graph (check the
  Nemesis registration for the graph you were in).

Repeat with a 1H weapon drawn to cover `1hm_behavior`.

**Dual control (optional but cheap):** a dual-assigned cast should log both
`…raised a left SpellFire event` and `…raised a right SpellFire event`, isolate both casters, and
still deliver **one** payload. Note that a dual cast writes `0x815 = 10016`, so **neither probe
matches** and it plays the stock MSCO clip with its left annotation — so in practice only the left
line will appear. That is expected; the dual arming path is proven by the unit tests, and a live
dual matrix belongs to the implementation ticket, not this spike.

### Regression sweep while you are in there

Four consecutive right casts should still walk `SH2_CastRight → SH2_Cast2 → SH2_Cast3 →
SH2_Cast4` in the graph trace (only clip 1 is replaced by the probes, so 2–4 keep their MLh
annotations and their left arming will now *not* match a right cast — expect the
no-SpellFire fallback on steps 2–4 of a right cast, and note it; it is a property of the spike's
one-clip scope, not of piece 1).

### Teardown

Delete `Dev - Spell Hotbar 2\meshes\…\OpenAnimationReplacer\SpellHotbar2Spike\` after the run. The
Nemesis registration and the two C++ pieces are meant to survive into the implementation ticket;
the probes are not.

---

## 10. What this session could not verify

- **Nothing runtime.** No game, MO2, or Nemesis run happened. Every claim above is static: source
  reads, a clean build, seven passing unit suites, `hkxc verify`, and JSON parse checks.
- **Whether OAR actually reads either global during the shtb state.** That is the whole question the
  spike exists to answer; it cannot be answered from disk.
- **Whether `MRh_SpellFire_Event` resolves in `1hm_behavior` after the patch.** The registration
  follows the working `MLh` pattern in the same block, but only a Nemesis run plus a live cast
  proves the annotation resolves.
- **That the MSCO `Base - default` (Inquisitor) submod is the runtime winner.** It is the highest
  `MSCO_left1.hkx` submod found on disk (`priority 6700`, `conditions: []`) and its path shadows the
  base MSCO mod's, but the OAR Animation Log during a control cast is the only proof.
- **Appearance.** The probe is a byte-faithful copy of a left-hand clip with a right-hand
  annotation; it will still *look* left-handed. That is intentional for the spike — it isolates the
  selection and commitment questions from art — but it means no visual-identity claim can be made
  from this artifact set.
