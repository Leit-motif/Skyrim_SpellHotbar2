# {{name}} {{version}}

MCO/ADXP casting for Spell Hotbar 2. Cast from a hotbar slot out of a drawn weapon stance,
mid-combo, without dropping the moveset — plus bindable Abilities, a press-anchored global
cooldown, and per-hand cast animations.

This is an **overwrite** over Spell Hotbar 2, not a standalone mod. Base Spell Hotbar 2 is
required and must be installed first.

## Requirements

All of these are required. Install them in this order.

1. **Spell Hotbar 2 {{base_version}}** — the base mod. This add-on overwrites it, so it has
   to be installed and working first. Distributed through GitHub releases, not Nexus:
   <https://github.com/pWn3d1337/Skyrim_SpellHotbar2>. Read the version pin below before
   using any other release.
2. **(SE) Ashes of War Weapon Art Via Additional Attack** — Nexus 100174, by Gild. Take the
   **FULL SUITE ... OAR** main file. The shipped Ability catalogue points at its clips; the
   Abilities do not resolve without it. It brings its own requirement chain, which Nexus
   lists on its page.
3. **SKSE64** — <https://skse.silverlock.org>. Match your game runtime.
4. **Address Library for SKSE Plugins** — Nexus 32444.
5. **Nemesis Unlimited Behavior Engine** — Nexus 60033. Two behavior patches ship here and
   have to be generated. Pandora is untested.
6. **Open Animation Replacer** — Nexus 92109. Picks the per-hand cast clips and the Ability
   animations.
7. **Payload Interpreter** — Nexus 65089. Carries the annotation payloads the cast and
   Ability clips fire.
8. **Behavior Data Injector** — Nexus 78146. A `SpellHotbar2_BDI.json` ships here and needs it.
9. **ADXP | MCO** — Distar's moveset system, off Nexus (skyrim-guild). Not optional: without
   it there is no combo to cast out of, which is what this add-on exists to do.

Tested on Skyrim Special Edition 1.5.97.

## Install

1. Install every requirement above, base Spell Hotbar 2 {{base_version}} first.
2. Install this archive **after** base Spell Hotbar 2, and let it overwrite. In Mod Organizer
   2 that means a lower position in the left pane; in Vortex, choose to load this one after.
3. Run Nemesis. Tick **Spell Hotbar 2 Cast States** and **Spell Hotbar 2 - Casting
   Commitment**, then press **Launch Nemesis Behavior Engine**.
   You do not need **Update Engine** — ticking a patch whose files have not changed is a
   selection-only change.
4. Start the game. The MCM sits where base Spell Hotbar 2's does.

Nemesis has to be re-run whenever you add or remove a behavior mod, this one included.

## What this overwrites

This is an add-on that wins the conflict against its own base mod. It replaces three files
from Spell Hotbar 2 and nothing else:

- `SKSE/Plugins/SpellHotbar2.dll`
- `Scripts/SpellHotbar.pex`
- `Scripts/SpellHotbarMCM.pex`

Everything else in this archive is new. The base mod's icons, fonts, presets, spell data and
plugin come from your base install and are untouched — that is checked at build time, so a
base asset cannot ride along in here. Nothing from Ashes of War is redistributed either; the
catalogue points at the clips your install already has.

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
