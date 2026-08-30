# {{name}} {{version}}

MCO/ADXP casting for Spell Hotbar 2. Cast from a hotbar slot out of a drawn weapon stance,
mid-combo, without dropping the moveset — plus bindable Abilities, a press-anchored global
cooldown, and per-hand cast animations.

This is an **overwrite** over Spell Hotbar 2, not a standalone mod. Base Spell Hotbar 2 is
required and must be installed first.

## Requirements

Base Spell Hotbar 2 carries its own requirements (SKSE, Address Library, Open Animation
Replacer, and the rest). Install and verify it before this. On top of those:

- **Spell Hotbar 2 {{base_version}}** — the exact base version this build was compiled from.
  Read the pin below before using any other version.
- **Nemesis Unlimited Behavior Engine** — two behavior patches ship here and must be
  generated. Pandora is untested.
- **ADXP / MCO** — the moveset system this integrates with. Without it the casting states
  still work, but the combo integration has nothing to talk to.
- **Payload Interpreter** — carries the annotation payloads the cast and Ability clips use.
- **Behavior Data Injector** — a `SpellHotbar2_BDI.json` ships here and needs it.

Tested on Skyrim Special Edition 1.5.97.

## Install

1. Install base Spell Hotbar 2 {{base_version}} and let it finish.
2. Install this archive **after** it, and let it overwrite. In Mod Organizer 2 that means a
   lower position in the left pane; in Vortex, choose to load this one after.
3. Run Nemesis. Tick **Spell Hotbar 2 Cast States** and **Spell Hotbar 2 - Casting
   Commitment**, then press **Launch Nemesis Behavior Engine**.
   You do not need **Update Engine** — ticking a patch whose files have not changed is a
   selection-only change.
4. Start the game. The MCM sits where base Spell Hotbar 2's does.

Nemesis has to be re-run whenever you add or remove a behavior mod, this one included.

## What this replaces

Three files from base Spell Hotbar 2, and nothing else:

- `SKSE/Plugins/SpellHotbar2.dll`
- `Scripts/SpellHotbar.pex`
- `Scripts/SpellHotbarMCM.pex`

Everything else in this archive is new. The base mod's icons, fonts, presets, spell data and
plugin come from your base install and are untouched — that is checked at build time, so a
base asset cannot ride along in here.

## The version pin, and why it matters

This DLL is compiled from base Spell Hotbar 2's own source at release
**{{upstream_tag}}** (upstream commit `{{upstream_commit_short}}`), with our changes on top.
It replaces the base DLL rather than sitting beside it.

The consequence: on a **newer** base Spell Hotbar 2, this DLL silently reverts whatever
upstream fixed after {{upstream_tag}}, while the newer base assets stay in place. Nothing
warns you. Stay on base {{base_version}} unless a release here says otherwise.

SHA-256 of the DLL in this archive:

```
{{dll_sha256}}
```

## Credits

Spell Hotbar 2 is by pWn3d1337 — <https://github.com/pWn3d1337/Skyrim_SpellHotbar2>. This is
a fork of that mod's source with MCO/ADXP integration built in; the base mod's own assets are
not redistributed here.

Fork by {{author_public}}. Built {{built_utc}}.
