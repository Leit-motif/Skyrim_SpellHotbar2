# Baseline Adoption fixture

Status: prepared; runtime ownership and Installed Configuration confirmation remain open.

Captured: 2026-07-29 (America/Chicago)

## Source and binary identity

- Core Fork repository: `C:\Nolvus\Projects\spell-hotbar-2`
- Ticket branch: `codex/baseline-01-validation-fixture`
- Branch point: `7802b0f85cf62a89add7ff9a6344a5cfba77c3b1`
- Upstream base: `f203cd26747e875336caed91f7d1453ca9a8a808` (`0.0.14`)
- Installed release DLL candidate:
  - Path: `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Spell Hotbar 2\SKSE\Plugins\SpellHotbar2.dll`
  - Size: `4871168`
  - SHA-256: `AB8F82DBA9F8673E3486C783B5910C82B40A5E6630E24B120CFD9936E4113E4B`
- Local Core Fork DLL candidate:
  - Source path: `C:\Nolvus\Projects\spell-hotbar-2\skse_plugin\build\release\SpellHotbar2.dll`
  - MO2 development copy: `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Dev - Spell Hotbar 2\SKSE\Plugins\SpellHotbar2.dll`
  - Size: `1713664`
  - SHA-256 at both paths: `93357D4414A3576C1ADDB0B684FF5562B818AE44EC32354E1FE5389DC094756F`

Neither DLL is designated the tested binary yet. The local artifact predates the commit that checked in its build tooling, so its hash is immutable but its producing commit is not adequately proven. Rebuild into a staging directory from the eventual evidence commit before deployment.

## Intended MO2 environment

- Instance: `C:\Nolvus\Instances\Nolvus Awakening`
- Intended profile: `Nolvus Awakening`
- Profile snapshot:
  - `modlist.txt`: SHA-256 `21995CB7A963C30348C0201546996F087D5025299CB9B02C7523F6F4CC137DD6`
  - `plugins.txt`: SHA-256 `DB153B263FEA8CD2AF4DD9D06B6541B4278C2656CBDEDACAA4B8A085E9D0D46A`
  - `loadorder.txt`: SHA-256 `18B23C5749E036F9E7CD30682C74E1C9EF34E40D490185C0806B001D457E44EC`
  - Enabled mods: `3906`
  - Enabled plugins: `3733`
- Runtime:
  - Skyrim SE `1.5.97.0`; `SkyrimSE.exe` SHA-256 `693E5A51EA2680119A68620BCF5080E81745549872B5D06BBD3F51131B67ABAB`
  - SKSE loader `0.2.0.20`; SHA-256 `FECC9F5D9A8F19B5244D514DD2649FEACDC4F0CA1EF89B390C571D861F4605FD`

At capture time, MO2 and Skyrim were running for another task. The live MO2 bridge and `ModOrganizer.ini` both reported `Dev - Skeleton`, not the intended profile. No exclusive ownership was claimed and no live state was changed.

Spell Hotbar 2 is installed on disk but is absent from the intended profile's `modlist.txt`. `SpellHotbar.esp` and `SpellHotbar_BattleMage.esp` are absent from its enabled plugin list. This must be corrected through an ownership-gated MO2 operation before runtime validation, then the profile fingerprints above must be recaptured.

## Installed payload observation

MO2 metadata identifies `Spell Hotbar 2` version `0.0.14.0` and the original installation filename `Spell.Hotbar.2.-.0.0.14.zip`; that archive is no longer present at the recorded path.

Observed installed payload includes:

- `SpellHotbar.esp`: SHA-256 `E7064A7EFAFB1C8D853C64E28B42A9B3E2653B95F32836634BFED6179DC0C85F`
- Battle Mage payload for the Skyrim 1.5.97-compatible Custom Skill Framework 2 path:
  - `SpellHotbar_BattleMage.esp`: SHA-256 `69C111BC7EC2ECF03780D4BF3243DA9B5D3A45A3F8FEC8EDCD7A0478A862B7EA`
  - `NetScriptFramework\Plugins\CustomSkill.SpellHotbar_Battlemage.config.txt`: SHA-256 `67599F2E21CBA6CDDB8B94CAD188FAA09D32DE0CDDFB438D455759E78D4C142E`
- Vanilla dual-cast perk data (`Skyrim.esm` perks `0x0153CD` through `0x0153D1`).
- First-person, concentration, staff, dual-cast, ritual, running, sneak, and Thu'um OAR animation payloads.
- Spell/icon data for the installed integrations, including Abyss, Ancient Blood Magic II, Apocalypse, Arclight, Astral Magic II, Constellation Magic, Dark Hierophant Magic, Desecration, Elemental Destruction Magic Redux, Elemental Mastery Magic, Holy Templar Magic, Mysticism, Obscure Magic, Odin, Ordinator, Path of Sorcery, Sacrosanct, Shadow Spell Package, Sonic Magic, SPERG, Stellaris, Storm Calling Magic II, Thunderchild, Triumvirate, and Vulcano.
- All distributed presets (`all_bars`, `auto_profile`, `controller`, `controller_bindmenu`, `oblivion_mode`, and `simple`).

This is a payload observation, not proof of the user's FOMOD answers or applied preset. The exact FOMOD selections, current MCM options, enabled compatibility data, hotbar bindings, and active input mode must be observed in the intended profile before the Installed Configuration criterion can close.

## Input stack

Enabled in the intended profile:

- Gamepad++ `1.2.1`
- Complete Controller Setup `5.3.5.0`
- Auto Input Switch `1.1.2`
- Wheeler `1.2`, CTD Fix `1.0.1`, and Nolvus Settings `6.1`
- Dynamic Activation Key `1.02`
- Extended Hotkey System `1.1`
- dTry's Key Utils `1.1`
- ENB Input Disabler `1.1.1`

No reWASD process was present during capture. Record only the representative mappings exercised once the user activates the intended profile; do not archive the full reWASD profile.

## Save fixtures

The files were only read and hashed.

- Controlled disposable candidate:
  - `Save20_EBCD0A92_0_5861656C6C65_QASmoke_000547_20260723170328_17_1.ess`
  - Size: `9728814`
  - SHA-256: `21A5BCCA9D90769C6FF757A1C51866CD9AAE1444C8917C8113FEDE4F196DC726`
- Representative playthrough candidate:
  - `Save2_97ED70F8_0_4D61656C6C65_WhiterunDragonsreach_000856_20260705191607_11_1.ess`
  - Size: `10842171`
  - SHA-256: `F1F8322DD336AC9EC2783F9F69CB518A3B90E2FA2AE4AEF68DC4D1BE13B8902A`

Both live under `C:\Nolvus\Instances\Nolvus Awakening\MODS\profiles\Nolvus Awakening\saves`. Confirm their intended roles before loading them. Preserve the original files; runtime work should reload rather than overwrite the fixture and should capture any derived test save under a distinct name.

## Ownership and activation gate

Before any runtime mutation:

1. Confirm the other task has released Skyrim, DevBench, and the MO2 profile.
2. Confirm Skyrim is stopped and the live MO2 bridge reports `Nolvus Awakening`.
3. Recapture hashes for the profile files.
4. Preview activation of `Spell Hotbar 2`, `SpellHotbar.esp`, and the installed Battle Mage component in the intended profile.
5. Record the exact priority/plugin positions and the rollback snapshot.
6. Rebuild the Core Fork DLL from the named evidence commit into staging, hash it, and preview deployment to `Dev - Spell Hotbar 2`.
7. Apply only after the preview names the intended profile and the rollback path.

Intended source: a staged Data-shaped Core Fork artifact. Intended destination: `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Dev - Spell Hotbar 2`. Restoration path: disable/remove only that development override and restore the ownership-gated MO2 snapshot, leaving the installed `Spell Hotbar 2` mod untouched.

No activation, deployment, rollback, launch, DevBench call, or in-game test was performed while the runtime was occupied.
