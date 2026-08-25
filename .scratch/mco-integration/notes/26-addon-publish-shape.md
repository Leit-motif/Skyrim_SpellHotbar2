# Publishing SH2-MCO as a Nexus add-on

Explored 2026-08-22. No implementation. Governs how a public release would sit on top of pWn3d1337's GitHub Spell Hotbar 2, after his 2026-08-12 reply (recorded in ADR-0002).

## Verdict

The add-on he blessed is **a second MO2 mod**, not a second SKSE plugin sitting beside his DLL.

Users install his GitHub package first (ESP, icons, fonts, images, original `SpellHotbar2.dll`, Papyrus, his OAR). They then install this Nexus mod **below** it. This mod **must overwrite** `SKSE/Plugins/SpellHotbar2.dll` (same filename, same Papyrus class `SpellHotbar`, same serialization unique id `0xB8498471`). Almost everything else this fork owns can be **additive**: Nemesis `shtb`, BDI json if the filename is unique, `SpellHotbar2Arts`, `artdata`, a tiny ESP for `SpellHotbar_ArtSelector` if that form is not in his plugin.

A companion DLL with a new name that leaves his DLL loaded is **not viable today**. SH2 exposes no SKSE messaging API and no C ABI. Both plugins would register natives on `SpellHotbar`, both would hook input/animation/ImGui, and both would claim the same co-save id. That is a double-load, not an add-on.

## What "plugin way" meant

His sentence about icons: *If you do the plugin way everything should be super clear*. In that thread it means **ship your own SKSE plugin and do not reupload his assets**. It does not mean a Bethesda `.esp` patch, and it does not mean two DLLs in `SKSE/Plugins` at once.

MIT on `skse_plugin/LICENSE.txt` already covers redistributing a modified build of the C++. The add-on shape still matches his preference: he keeps the SH2 package; you do not host his icons, SkyUI bits, or a full reupload.

## File-by-file

| Asset | Collision with stock SH2 | Ship as |
|---|---|---|
| `SKSE/Plugins/SpellHotbar2.dll` | Same name. CMake `project(SpellHotbar2)`. Papyrus class `SpellHotbar`. Serialization id `0xB8498471`, save format **6** (ability binds) vs upstream **5**. | **Overwrite.** Required. |
| `SpellHotbar.psc` extras (`castSlot`, `slotArt`, `getArtSelector`) | Natives can be registered without a pex overwrite if no stock script calls them. | Prefer leave his pex. Overwrite only if a script must call the new natives. |
| `SpellHotbar.esp` | Stock package. Fork currently resolves `SpellHotbar_ArtSelector` as form `D63` **in that file**. | **Do not overwrite** if avoidable. Add-on ESP with SH2 as master, or PR the global into his plugin, then point the DLL at the new plugin name. |
| Icons, fonts, `images/`, his OAR cast packs | His assets. | **Do not ship.** |
| Nemesis `mod/shtb` (`info.ini` author Amrit Chana) | New folder. | **Additive.** |
| `SpellHotbar2_BDI.json` | Collides only if he already ships that filename. | Additive if unique; otherwise rename. |
| `OpenAnimationReplacer/SpellHotbar2Arts` | New pack. | **Additive.** Keep his cast OAR untouched. |
| `SKSE/Plugins/SpellHotbar/artdata/` and custom-ability folders | New data roots in this fork. | **Additive** if those paths are empty in his package. |

## Why overwrite is not optional for the DLL

1. SKSE loads every DLL under `SKSE/Plugins`. Two copies both run.
2. `SKSEPluginLoad` installs animation hooks, a game-loop hook, input trampoline, ImGui, Papyrus natives on class `SpellHotbar`, and a serialization callback on a fixed unique id. A second plugin doing any of that fights the first.
3. Driver Cast, Ability slots, combo restore, plant, and the Ability Editor are edits **inside** `casting_controller`, `animationeventhook`, `plugin.cpp`, storage, and the bind menu — not a layer that can subscribe from outside.
4. Save format 6 lives in this binary. Stock SH2 will ignore or drop `WART` / kind-byte slots.

The user-facing install is still "add-on": two mods, his files remain the source of ESP and art, yours wins only on the binary and on files he does not ship.

## What a true companion plugin would take

The ShoutMCO cast-intent header in `skse_plugin/src/extern/ShoutMCO_CastIntent.h` is the pattern: versioned C ABI, `GetProcAddress`, fail-open. SH2 has no equivalent export.

His PR invitation is the path if you want that later: a small public seam in his DLL (cast begin/end, slot payload, maybe a hook for Ability start), then a Nexus DLL that **only** contains MCO/Ability code. That is a large upstream ask, not a "fix" PR, and it is not required to publish.

Useful small PRs that stay in "fixes / missing records" territory:

- Add `SpellHotbar_ArtSelector` to `SpellHotbar.esp` so add-ons do not need a second plugin for one global.
- Any generally applicable bug you already found.

Do not PR the whole Driver Cast / Ability product into his repo unless he asks. He steered to add-on specifically because there is no SH2 Nexus page yet and he is retaining SH2.

## Nexus page mechanics

- **Requirements text** (not a Nexus file requirement): install SH2 from GitHub first, pin the tested version (rebase target, currently the 0.0.14 line unless you retarget), then this mod, then run Nemesis with `shtb` enabled. Link ShoutMCO / MCO / OAR as hard requirements of the add-on, not of SH2.
- GitHub mods cannot be a Nexus "required file" checkbox. Manual instructions are the normal pattern (same as SKSE historically).
- Credit him; MIT notice in the DLL package; do not list his icons as yours.
- Version coupling: every SH2 GitHub release that changes the DLL needs a rebase of this binary. State that on the page. Treat his ESP as stable API: form ids you already resolve (`0x815` animation type, etc.) plus whatever you add.

## Install order (MO2)

1. Spell Hotbar 2 (GitHub) — enabled.
2. This add-on — **below** so `SpellHotbar2.dll` wins.
3. ShoutMCO and MCO/OAR as already required by the integration.
4. Nemesis output after `shtb` is ticked.

Disable his DLL by overwrite, not by deleting his mod. Users who update SH2 from GitHub will restore his DLL until this add-on is reinstalled or the overwrite wins again — worth a sticky comment.

## Out of scope for this shape

- Full-package Nexus upload of SH2 + this work (not granted; he steered away).
- Shipping two SKSE plugins both named as SH2.
- Changing ADR-0001's split: Nolvus-only records still belong in a personal compatibility package, not in the public add-on.
