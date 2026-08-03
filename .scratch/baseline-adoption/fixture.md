# Baseline Adoption fixture

Status: established. The environment is activated, fingerprinted, and owned; every runtime
acceptance cell remains open.

Captured: 2026-07-29, superseded and re-captured 2026-08-03 (America/Chicago).

Nothing in this file is runtime proof. A successful build, a hash, and a load-order position
support attribution only; acceptance belongs to tickets 02–07.

## Runtime ownership

Exclusive ownership of the `Nolvus Awakening` MO2 instance and the Skyrim runtime was granted by
the owner on 2026-08-03 for this ticket, with mutation allowed. At capture:

- Skyrim was not running; DevBench reported offline at `http://127.0.0.1:8920/mcp`.
- MO2 2.4.4 was running (PID 72072) with `selected_profile=Nolvus Awakening`.
- The owner enabled `Spell Hotbar 2` and `Dev - Spell Hotbar 2` in that profile through MO2.

The 7/29 capture could not claim ownership: MO2 then reported `Dev - Skeleton` and another task
held the runtime. That blocker is cleared.

## Source and binary identity

- Core Fork repository: `C:\Nolvus\Projects\spell-hotbar-2`
- Ticket branch: `codex/baseline-01-validation-fixture`
- Integration target: `codex/integration`
- Review baseline: `7802b0f85cf62a89add7ff9a6344a5cfba77c3b1`
- Upstream base: `f203cd26747e875336caed91f7d1453ca9a8a808` (`0.0.14`)

**Tested binary** — built for this ticket from a clean worktree at
`a50bda1c17c8c13acf955283dc7ec79b5fd6ef98`:

- Build: `cmake --preset release` then `cmake --build build/release --config Release`, MSVC
  14.41.34120, vcpkg triplet `x64-windows-static-md`, staged to a scratch `OUTPUT_FOLDER`
  outside the MO2 instance. Exit 0; the only diagnostic was `C4100` on an unused parameter in
  `advanced_bind_menu.cpp:507`.
- `SpellHotbar2.dll` size `1713664`, SHA-256
  `9846FB9B79E0D5C352F24D32FA354B7C98F9EE9A1384131719A1D0F2AA6D311D`
- `git status --porcelain` was empty before and after the build.

Source provenance is checkable rather than asserted: `git diff f203cd2...HEAD` over
`skse_plugin/ papyrus/ SWF_Generator/ python_scripts/` touches only `CMakeLists.txt`,
`build-release.bat`, and `configure-release.bat` — all build plumbing added in `7b40a68`, before
the review baseline. **No C++ under `skse_plugin/src` differs from upstream `0.0.14`.** The tested
binary is therefore a local build of unmodified upstream source, which is what a compatibility
baseline requires.

Superseded candidates, retained for attribution:

- Installed release DLL (`Spell Hotbar 2`, shipped in the FOMOD): size `4871168`, SHA-256
  `AB8F82DBA9F8673E3486C783B5910C82B40A5E6630E24B120CFD9936E4113E4B`. Not the tested binary; it
  is overridden at runtime (see priorities below).
- Prior local build of 2026-07-23: size `1713664`, SHA-256
  `93357D4414A3576C1ADDB0B684FF5562B818AE44EC32354E1FE5389DC094756F`. **Not** the tested binary —
  its producing commit was never proven. Its size matching the new build is consistent with an
  unchanged source tree, but that is corroboration, not provenance. Preserved as the rollback
  artifact.

## MO2 environment

- Instance: `C:\Nolvus\Instances\Nolvus Awakening`
- Profile: `Nolvus Awakening`
- MO2: `2.4.4`; game path `...\Nolvus Awakening\STOCK GAME`
- Runtime: Skyrim SE `1.5.97.0`, `SkyrimSE.exe` SHA-256
  `693E5A51EA2680119A68620BCF5080E81745549872B5D06BBD3F51131B67ABAB`; SKSE loader `0.2.0.20`,
  SHA-256 `FECC9F5D9A8F19B5244D514DD2649FEACDC4F0CA1EF89B390C571D861F4605FD`

Profile fingerprints **after** activation, captured twice with identical results while MO2 was
open:

| File | Size | SHA-256 |
| --- | --- | --- |
| `modlist.txt` | 147537 | `DB5942D0D7704B3F42D1D30E554B092FE1C722791F2361C0677FCD9B967C9489` |
| `plugins.txt` | 131348 | `9B18AFAF9A317FEAC0CDF37FD4D5BE147FB0B5F5CF3AF4A6E30E7389BA9B70B5` |
| `loadorder.txt` | 129703 | `B441CB4757D71C1CB3D65126C83B3F429223FD1CE5087164D96B7BB41B30A1D8` |

- Enabled mods `3911`, enabled plugins `3735` (7/29: `3906` / `3733`). The plugin delta of `+2` is
  exactly this ticket's two plugins. The mod delta is `+5`: two are this ticket's, and **the other
  three are unattributed** — the 7/29 capture recorded only a hash of `modlist.txt`, not its
  contents, so the intervening changes cannot be identified from this record. They were not made
  by this ticket.
- Mod priority: `Dev - Spell Hotbar 2` at `4353`, `Spell Hotbar 2` at `4352` — both at the top of
  the list, so **the development DLL override wins over the FOMOD-installed release DLL**. That
  arrangement is deliberate: `Dev - Spell Hotbar 2` contains exactly one file, the DLL.
- Plugins: `SpellHotbar.esp` and `SpellHotbar_BattleMage.esp` at enabled indices `3591` and
  `3592` of `3735`, loading immediately after `Gamepad++.esp`.
- Plugin hashes: `SpellHotbar.esp` `E7064A7EFAFB1C8D853C64E28B42A9B3E2653B95F32836634BFED6179DC0C85F`;
  `SpellHotbar_BattleMage.esp` `69C111BC7EC2ECF03780D4BF3243DA9B5D3A45A3F8FEC8EDCD7A0478A862B7EA`.

Because MO2 was open during capture, re-verify these three fingerprints at the start of ticket 02
before trusting them.

## Installed Configuration

The FOMOD answers could **not** be observed directly and were not guessed. MO2's `meta.ini` for
the mod records an empty `[installedFiles]` and no installer choices, and the source archive
`Spell.Hotbar.2.-.0.0.14.zip` is gone from the recorded Downloads path. The owner directed that
the installed payload be treated as authoritative.

The derivation is not a payload guess. `python_scripts/create_fomod_installer.py` in this
repository *generates* the 0.0.14 `ModuleConfig.xml`, so it is the authoritative option→file
mapping for this exact release. Each installer group was inverted against the files actually on
disk. Every group below resolved to exactly one answer.

| Installer group | Answer | How it was determined |
| --- | --- | --- |
| Battle Mage Perks (SelectAny) | **CSF 2 (1.5.97-compatible)** only | `NetScriptFramework/Plugins/CustomSkill.SpellHotbar_Battlemage.config.txt` and `Interface/MetaSkillsMenu/Battlemage SpellHotbar.dds` present; the CSF3 markers `SKSE/Plugins/CustomSkills/SpellHotbar_Battlemage.json` and `Interface/MetaSkillsMenu/SpellHotbar_Battlemage SpellHotbar.dds` absent |
| Perk Overhaul (SelectExactlyOne) | **Vokriinator Black** | The only option installing all three of the `ordinator`, `sperg`, and `path_of_sorcery` folders; all three `spells_*.csv` and `icons_*.dds` are present, and `perkdata/dual_cast_perks.csv` is the VANILLA variant that option sets |
| Spell Packs (SelectAny) | **22 of 31** (listed below) | Per-pack `spelldata/spells_<name>.csv` + `images/icons_<name>.dds` |
| UI Textures (SelectExactlyOne) | **Nordic UI** | `images/default_icons_nordic.csv` and `.dds` present; installed by no other option |
| Text Font (SelectExactlyOne) | **Sovngarde** | `fonts/text_font.ttf` name table reads Family `Sovngarde`, Subfamily `Light`, Version 8.9 — decisive against the Default/PL options that write the same filename |
| Auto Profile (SelectExactlyOne) | **Controller with Bindmenu** | `presets/auto_profile.json` is byte-identical to `presets/controller_bindmenu.json` (both SHA-256 `04794566F67F4C1A67A328610B47352422721BD8DDD0904E07979D613D1CE553`) and matches no other preset |

Two of these materially shape the acceptance model and should be read before ticket 02 plans a
single keystroke:

- **The auto-loaded profile is the controller preset with the dedicated Bind Menu**, not a
  keyboard preset. Its own installer text describes it as binding DPad + ABXY with RS/LS as
  modifiers, with the non-modifier bar disabled, using the mouse-operated Bind Menu to avoid
  controller menu conflicts. The keyboard cells (`KB-*`) are therefore not testing the shipped
  default binding set; they need bindings established deliberately, and that fact must be
  recorded in each cell rather than discovered mid-run.
- **Dual-cast perk data is the vanilla five** (`Skyrim.esm` `0x0153CD`–`0x0153D1`,
  `perkdata/dual_cast_perks.csv` SHA-256
  `38B7EE046F33746D4D6AA3835F1D9F75A3C329E3F938931202B2C5507A98127F`), which is what Vokriinator
  Black selects. `KB-DUAL-1` is gated on the character actually holding one of those five perks.

Selected spell packs, with each pack's gating plugin checked against the profile's enabled list:

| Active in this load order (18) | Installed but inert (4) |
| --- | --- |
| abyss, ancient_blood_magic_2, apocalypse, arclight, astral_magic_2, constellation_magic, dark_hierophant_magic, desecration, elemental_destruction_magic_redux, holy_templar_magic, mysticism, obscure_magic, sacrosanct, sonic_magic, storm_calling_magic2, thunderchild, triumvirate, vulcano | elemental_mastery_magic (`KittySpellPack02.esl`), odin (`Odin - Skyrim Magic Overhaul.esp`), shadow_spell_package (`ShadowSpellPackage.esp`), stellaris (`Stellaris.esp`) |

The four inert packs will log `Skipping Plugin '<name>', not loaded` at startup. That is expected,
is not a Material Interaction, and **excludes them from ticket 05's FOMOD smoke coverage** — a
feature whose gating plugin is absent cannot be smoke-tested and must not be recorded as passing.

Not selected in the installer (9): `abyssal_tides_magic`, `abyssal_wind_magic`, `andromeda`,
`miracles_of_skyrim`, `star_wars_spell_pack`, `star_wars_spell_pack_esl`, `undead_horse`,
`winter_wonderland_magic`, `witcher_signs`. Out of scope per the spec.

### Battle Mage component and Custom Skills Framework

The CSF2 choice looks anachronistic beside a modern load order, and is not. The profile enables
`Custom Skills Framework` **2.0.2.0**, installed from `Custom Skills Framework for
1.5.97-41780-2-0-2`, matching the Skyrim `1.5.97` runtime. Ten other enabled Nolvus mods ship the
same `NetScriptFramework/Plugins/CustomSkill.*.config.txt` convention (Vigilant, Legacy of the
Dragonborn, Companions, the Lich and Dragonborn Shouts trees, Ascension 2, Ancient Magick, the
Stances perk system, and the UTF8 fix). Spell Hotbar's Battle Mage component follows the same
convention as every other custom skill here.

This is consistency, not proof that the perk tree opens. Whether the Battle Mage menu actually
appears belongs to ticket 05's `FEAT-FOMOD-1` cell.

## Input stack

Enabled in the profile: Gamepad++ `1.2.1`, Complete Controller Setup `5.3.5.0`, Auto Input Switch
`1.1.2`, Wheeler `1.2` with CTD Fix `1.0.1` and Nolvus Settings `6.1`, Dynamic Activation Key
`1.02`, Extended Hotkey System `1.1`, dTry's Key Utils `1.1`, ENB Input Disabler `1.1.1`.

Of these only `Gamepad++.esp` carries a plugin, and `SpellHotbar.esp` loads directly after it. The
rest are SKSE-side and contend for input at runtime rather than in the load order — which is where
`SEAM-INPUT-1` has to look.

No reWASD process was present at capture. Record only the representative mappings exercised in
ticket 04; do not archive the full profile.

## Save fixtures

A save fixture here is **two files**, not one. The `.ess` holds the game save; the `.skse` co-save
holds serialized SKSE state — including, once the mod runs, the `GDAT` record carrying this mod's
bindings and settings. A fixture identified by its `.ess` alone cannot prove it was unchanged, and
cannot tie later evidence to the exact state a run started from. Both halves are fingerprinted.

Controlled disposable — `Save20_EBCD0A92_0_5861656C6C65_QASmoke_000547_20260723170328_17_1`:

| Part | Size | SHA-256 |
| --- | --- | --- |
| `.ess` | 9728814 | `21A5BCCA9D90769C6FF757A1C51866CD9AAE1444C8917C8113FEDE4F196DC726` |
| `.skse` | 617190 | `A2FB787B1C92BCC0F3DEDB3386B767CC3D8543C498E1B93B71F1BD591B946867` |

Representative playthrough —
`Save2_97ED70F8_0_4D61656C6C65_WhiterunDragonsreach_000856_20260705191607_11_1`:

| Part | Size | SHA-256 |
| --- | --- | --- |
| `.ess` | 10842171 | `F1F8322DD336AC9EC2783F9F69CB518A3B90E2FA2AE4AEF68DC4D1BE13B8902A` |
| `.skse` | 1694923 | `BB807C9C4DB08BA95DA1B77307BCED537AF89F86F9B7FE38680FB63FB318032A` |

All four files were read and hashed only. Both `.ess` hashes are byte-identical to the 7/29
capture despite heavy unrelated use of the same character by other work on 2026-08-03.

All live under `...\MODS\profiles\Nolvus Awakening\saves`. Other tasks autosave the same
disposable character; re-verify all four hashes before each run, reload rather than overwrite, and
save derived test states under distinct names.

Neither save has ever run with Spell Hotbar 2 active — the mod was disabled in this profile until
2026-08-03. Bindings and settings are serialized into the SKSE co-save (`storage.cpp`, record
`GDAT`), so **no bindings, MCM state, or mode state exist for these fixtures yet.** They are
established at first activation, which is ticket 02's opening act and belongs in its evidence, not
here.

## Deployment and rollback (applied)

Previewed, then applied on 2026-08-03 under the ownership grant above.

- Source: the staged build output, SHA-256 `9846FB9B…`
- Destination: `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Dev - Spell Hotbar 2\SKSE\Plugins\SpellHotbar2.dll`
- Replaced: SHA-256 `93357D44…`
- Verified after the copy: destination SHA-256 equals the staged source.
- Untouched: the installed `Spell Hotbar 2` mod and every profile file.

### Durable artifact store

Rollback artifacts are useless in a session-scoped temp directory. Everything needed to restore
this environment lives at an absolute, session-independent path:

`C:\Nolvus\_artifacts\spell-hotbar-2\baseline-adoption\01\`

| File | Size | SHA-256 |
| --- | --- | --- |
| `dll\SpellHotbar2.dll.tested-a50bda1` | 1713664 | `9846FB9B79E0D5C352F24D32FA354B7C98F9EE9A1384131719A1D0F2AA6D311D` |
| `dll\SpellHotbar2.dll.replaced-20260723` | 1713664 | `93357D4414A3576C1ADDB0B684FF5562B818AE44EC32354E1FE5389DC094756F` |
| `profile-activated\modlist.txt` | 147537 | `DB5942D0D7704B3F42D1D30E554B092FE1C722791F2361C0677FCD9B967C9489` |
| `profile-activated\plugins.txt` | 131348 | `9B18AFAF9A317FEAC0CDF37FD4D5BE147FB0B5F5CF3AF4A6E30E7389BA9B70B5` |
| `profile-activated\loadorder.txt` | 129703 | `B441CB4757D71C1CB3D65126C83B3F429223FD1CE5087164D96B7BB41B30A1D8` |
| `profile-pre-activation\modlist.txt` | 147537 | `0222C9FE5B281E3115E4EA83625CB2C67762C8A3AD38E6AB934911800B38ED1C` |
| `profile-pre-activation\plugins.txt` | 131301 | `DB153B263FEA8CD2AF4DD9D06B6541B4278C2656CBDEDACAA4B8A085E9D0D46A` |
| `profile-pre-activation\loadorder.txt` | 129658 | `18B23C5749E036F9E7CD30682C74E1C9EF34E40D490185C0806B001D457E44EC` |

`profile-activated` was copied from the live files and each copy hashes equal to its source.
`dll\...tested-a50bda1` hashes equal to the DLL currently deployed at the destination.

### The pre-activation snapshot is reconstructed, and the reconstruction is proven

There is no captured pre-activation snapshot, because the owner activated the mods before this
ticket's work began; a copy taken afterwards is a copy of the activated state and cannot restore
what preceded it. The pre-activation files above were therefore **reconstructed** by inverting the
activation, and the inversion is proven rather than asserted: each reconstructed file hashes
byte-exactly to the hash captured from the live file *before* activation.

The inversion is not a single uniform edit, which is why asserting it would have been wrong:

- `modlist.txt` — both entries already existed, disabled. Activation flipped `-` to `+`; the
  inversion flips them back. Size is unchanged at `147537`, which is itself the tell.
- `plugins.txt` and `loadorder.txt` — the entries did **not** exist. Activation *added* two lines
  to each; the inversion removes those whole lines. Sizes drop by `47` and `45` bytes. Merely
  dropping the `*` prefix would leave the plugins listed-but-disabled, which is a different state
  from absent.

Restoration paths, in increasing scope:

1. Copy `dll\SpellHotbar2.dll.replaced-20260723` back over the destination.
2. Disable `Dev - Spell Hotbar 2` in the profile, falling back to the installed release DLL
   `AB8F82DB…`.
3. Copy the three `profile-pre-activation` files over the live profile files. Do this with MO2
   closed, or MO2 will flush its in-memory state over them on exit.

## Prior-run evidence that does not transfer

`Documents\My Games\Skyrim Special Edition\SKSE\SpellHotbar2.log` records a run on 2026-07-30
21:24. It is **not** evidence for this baseline. That run was in the `Dev - Skeleton` profile — the
only profile where both Spell Hotbar mods were enabled at the time — and its log shows `Abyss.esp`,
`Arclight.esp`, `AncientBloodII.esl`, and others as `not loaded`, precisely the packs that *are*
active here. It exercised a different DLL, a different load order, and a different feature set.

The log is also shared: SKSE writes to the real `Documents` tree, not a per-profile one. Truncate
or timestamp-bound it at the start of ticket 02 so `LOG-SKSE-1` reads only its own run.

## What remains open

Every row of `acceptance-matrix.csv` is `open`, and this ticket closes none of them. Ticket 02
inherits: an activated and fingerprinted profile, a tested binary with a proven producing commit,
a confirmed Installed Configuration, and two untouched save fixtures. It owes the first live
observation — bindings, active input mode, and the mod's own ImGui settings — because none of that
exists until the mod runs here.
