# 01 — Establish the reproducible validation fixture

**What to build:** A reproducible Baseline Adoption fixture for the exact Installed Configuration in Nolvus Awakening, with every later runtime result attributable to a known source, binary, load order, save, and input configuration.

**Blocked by:** None — can start immediately.

**Status:** resolved

- [x] Record the exact selected FOMOD components, relevant MCM options, enabled compatibility data, hotbar bindings, and enabled input stack, including Gamepad++, Input Switcher, reWASD, and other input-affecting mods present. — FOMOD components and compatibility data derived decisively; input stack recorded. **MCM options and hotbar bindings do not exist yet**: they serialize into the SKSE co-save and the mod had never been active in this profile, so the fixture records their absence and assigns their first capture to ticket 02. reWASD was not running at capture; its mappings are recorded per-mapping in ticket 04.
- [x] Record the Core Fork commit and immutable identity of the tested DLL.
- [x] Inspect and record the active Nolvus Awakening MO2 instance, profile, mod list/load order, and relevant runtime version information.
- [x] Identify a controlled disposable save and a representative playthrough save without mutating either during fixture preparation.
- [x] Confirm exclusive ownership of Skyrim and the active MO2 profile before any runtime mutation.
- [x] Preview any required deployment or rollback operation and record the intended source, destination, and restoration path before applying it.
- [x] Create an acceptance matrix that can record spell or feature, input path, camera, hand mode, save fixture, persistence transition, expected and actual results, evidence, and pass/fail/open status.
- [x] Leave every runtime acceptance cell open; static inspection, fixture preparation, and a successful build do not count as runtime proof.

## Comments

### 2026-07-29 — Prepared through the runtime ownership gate

Claim checkpoint: `f3a380ff10c922b1487bcbe8a7d6be125a60c716`.

Created `../fixture.md` and `../acceptance-matrix.csv`. Read-only inspection identified immutable candidates for the installed and Core Fork DLLs, fingerprinted the intended profile and runtime, identified and hashed two save candidates without loading them, inventoried the installed input stack, and left all runtime matrix rows open.

Stopped before activation, deployment, or in-game testing as requested. Skyrim was already running for another task and the live MO2 bridge reported `Dev - Skeleton`, not `Nolvus Awakening`; exclusive ownership therefore could not be confirmed. The intended profile also does not currently include Spell Hotbar in its mod list or enable its plugins. Exact FOMOD answers, applied MCM options, hotbar bindings, active input mode, and representative reWASD mappings remain unobserved. The ticket stays `claimed`.

### 2026-08-03 — Fixture established

The 7/29 ownership blocker cleared. The owner granted exclusive ownership of the `Nolvus
Awakening` MO2 instance and the Skyrim runtime with mutation allowed, and enabled `Spell Hotbar 2`
and `Dev - Spell Hotbar 2` in that profile through MO2. Skyrim was not running and DevBench was
offline throughout. No in-game session was run; every acceptance cell stays `open` and the
matrix is unchanged.

`fixture.md` is rewritten from the ground up. What changed materially:

**Tested binary now has a proven producing commit.** Built from a clean worktree at `a50bda1`,
staged outside the MO2 instance, SHA-256 `9846FB9B…`. The 7/23 artifact `93357D44…` is explicitly
demoted — its producing commit was never proven — and preserved as the rollback file. Deployment
to `Dev - Spell Hotbar 2` was previewed, applied, and hash-verified at the destination; the
installed release mod and all profile files were untouched.

Source provenance is mechanically checkable rather than asserted: `git diff f203cd2...HEAD` over
the source directories touches only `CMakeLists.txt` and the two build `.bat` files, all added in
`7b40a68` before the review baseline. No C++ under `skse_plugin/src` differs from upstream
`0.0.14`, so the tested binary is a local build of unmodified upstream source.

**FOMOD answers were derived decisively, not guessed.** MO2 recorded no installer choices
(`[installedFiles]` empty) and the source archive is deleted, so direct observation was
impossible; the owner directed that the payload be authoritative. Rather than describe files,
the derivation inverts `python_scripts/create_fomod_installer.py` — the script that *generates*
this release's `ModuleConfig.xml` — against the files on disk. All six installer groups resolved
to exactly one answer each: Battle Mage **CSF2**, Perk Overhaul **Vokriinator Black**, **22 of 31**
spell packs, **Nordic UI**, **Sovngarde** font, Auto Profile **Controller with Bindmenu**. The
sharpest evidence is a byte-exact hash match between `auto_profile.json` and
`controller_bindmenu.json`, and the `Sovngarde` family name read out of the installed TTF's name
table, which separates it from the two other options that write the same filename.

Three findings change how later tickets must be run, and are the reason this was worth doing
before ticket 02 rather than during it:

1. **The auto-loaded profile is the controller preset with the Bind Menu**, so the keyboard cells
   (`KB-*`) are not exercising a shipped default binding set. Bindings must be established
   deliberately and stated in each cell.
2. **Four selected spell packs are inert here** because their gating plugins are not enabled
   (`odin`, `stellaris`, `shadow_spell_package`, `elemental_mastery_magic`). They will log
   `Skipping Plugin … not loaded`, which is expected and not a Material Interaction, and they are
   excluded from ticket 05's smoke coverage — a feature whose plugin is absent cannot pass.
3. **The 2026-07-30 `SpellHotbar2.log` is not evidence for this baseline.** It came from the
   `Dev - Skeleton` profile, and its "not loaded" list contains exactly the packs that *are* active
   here. The SKSE log path is shared across profiles, so `LOG-SKSE-1` needs the log bounded at the
   start of its own run.

Ownership-gated work applied: profile activation (by the owner), the DLL deployment above, and a
profile snapshot held beside the live files as `*.bak-sh2-baseline01-20260803`. Rollback paths are
recorded at three scopes. Save fixtures were read and hashed only and are byte-identical to the
7/29 record despite heavy unrelated use of the same character that day.

Open by construction: no bindings, MCM state, or mode state exist for either save fixture, because
they serialize into the SKSE co-save and the mod has never run in this profile. Their first
capture is ticket 02's opening act.

### 2026-08-03 — Cold review and closure

Independent review by Codex GPT-5.6 Sol at `high` reasoning effort, over
`7802b0f...b4ce74b`, against this ticket, the spec, `CONTEXT.md`, and `AGENTS.md`.

Verdict: **not ACCEPT** — three P1 blocking findings, all `RECORD`, no `PRODUCT` or `TEST`
findings. The reviewer confirmed independently that all six FOMOD inversions rest on unique
installed-payload markers, that the MCM/binding limitation is stated clearly enough not to read as
observed data, and that all 24 runtime cells remain `open`.

All three closures changed only the fixture record, so no second model pass is owed. Each is
proven below by the command output that closes it.

**Finding 1 — rollback artifacts sat in a session-scoped temp directory.** Correct: the staged
build and the replaced DLL were under `%TEMP%\claude\…\scratchpad`, which cleanup can delete, and
the fixture referred to them as `<scratch>`, which names no reproducible location. Closed by
copying every artifact to `C:\Nolvus\_artifacts\spell-hotbar-2\baseline-adoption\01\` and
recording absolute paths with sizes and hashes. Verified: the durable tested DLL hashes equal to
the DLL now deployed at the destination, and the durable activated-profile copies hash equal to
their live sources.

**Finding 2 — only the `.ess` half of each save fixture was fingerprinted.** Correct, and the
omission was self-contradictory: the same fixture argues that bindings and settings serialize into
the SKSE co-save. Closed by fingerprinting both halves of both fixtures —
`Save20…QASmoke….skse` size `617190` SHA-256 `A2FB787B…`, and
`Save2…WhiterunDragonsreach….skse` size `1694923` SHA-256 `BB807C9C…`.

**Finding 3 — the "snapshot taken before this ticket's work" was nothing of the kind.** This was a
real error, not a wording problem. The owner activated the mods before the snapshot was taken, so
all three files were copies of the *activated* state and hashed identically to the live files; and
the recorded reversal ("drop the `*`") was wrong for two of the three, because activation had
**added** those lines rather than disabling them. Dropping the prefix would have left the plugins
listed-but-disabled, which is not the pre-activation state.

Closed by reconstructing the pre-activation profile and proving the reconstruction against the
hashes captured from the live files before activation. The read/write path was first shown
byte-faithful by round-tripping an untouched file to an identical hash, so a mismatch could only
come from the transformation itself:

| File | Transformation | Reconstructed size | Reconstructed SHA-256 | vs pre-activation |
| --- | --- | --- | --- | --- |
| `modlist.txt` | flip 2 lines `+` → `-` | 147537 | `0222C9FE…` | MATCH |
| `plugins.txt` | remove 2 lines | 131301 | `DB153B26…` | MATCH |
| `loadorder.txt` | remove 2 lines | 129658 | `18B23C57…` | MATCH |

The check discriminates: the first attempt at `modlist.txt` anchored `$` before a trailing `\r`,
replaced nothing, and reproduced the *activated* hash `DB5942D0…` — a visible failure, corrected
and re-verified. The three mislabelled `*.bak-sh2-baseline01-20260803` files were removed from the
profile directory; the durable store supersedes them. The live profile is unchanged.

One further correction made unprompted: the fixture had described the mod-count delta as "three
unrelated changes made by other work", which asserted more than the evidence supports. The 7/29
capture recorded only a hash of `modlist.txt`, not its contents, so those three are now recorded as
unattributed and merely not made by this ticket.
