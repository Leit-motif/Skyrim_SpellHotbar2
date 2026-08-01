# Spell Hotbar 2 Integration

This context defines the language used while adapting Spell Hotbar 2 for the user's Nolvus mod list.

## Language

**Nolvus Integration Fork**:
A maintained fork that preserves Spell Hotbar 2's core product while adding the configuration, compatibility, fixes, and selective improvements required by the user's Nolvus mod list.
_Avoid_: Rewrite, successor, total redesign

**Baseline Adoption**:
The first integration milestone: a compatibility check that confirms the base mod behaves normally in the active Nolvus load order and identifies unintended interactions with that environment before customization begins.
_Avoid_: Experimental proof of concept, reduced feature trial, customization milestone

**Direct Cast**:
The fork's primary casting mode, where activating a hotbar slot casts its bound spell directly instead of first equipping it. This behavior is the central reason for adopting Spell Hotbar 2.
_Avoid_: Equip-first casting, secondary test mode

**Installed Configuration**:
The exact set of components and options selected in the user's current FOMOD installation. Compatibility acceptance covers this configuration, not unselected installer alternatives.
_Avoid_: Every installer permutation, theoretical support matrix

**Core Fork**:
The maintained Spell Hotbar 2 source that owns generally applicable native behavior, fixes, and improvements.
_Avoid_: Nolvus patch, load-order bundle

**Compatibility Package**:
A separate integration layer that owns records, presets, configuration, and other adaptations specific to the user's Nolvus load order.
_Avoid_: Core fork, upstream source

**Personal Integration**:
The private runtime result maintained for the user's own Nolvus installation, without a public release or compatibility claim for other users.
_Avoid_: Community release, supported mod-list distribution

**Accepted Baseline**:
The independently validated Installed Configuration before any behavior customization, retained as the known-good reference for attributing later regressions.
_Avoid_: Build success, provisional smoke test

**Dual-Input Compatibility**:
Compatibility of the Installed Configuration with both native keyboard input and native gamepad input, including the user's reWASD mappings between them.
_Avoid_: Controller-only support, keyboard-only support

**Material Interaction**:
A repeatable Nolvus-specific conflict, regression, stability problem, or meaningful behavioral deviation from normal upstream expectations that prevents acceptance.
_Avoid_: Harmless warning, unselected feature behavior, theoretical conflict

**Compatibility Evidence**:
The traceable runtime record supporting acceptance, tied to the tested source and binary, MO2 environment, Installed Configuration, saves, input paths, logs, and visible results. For reWASD, record the mappings exercised rather than archiving the full profile unless a defect depends on it.
_Avoid_: Build success, undocumented smoke test

**Shout-Graph Cast**:
The mod's casting mechanism: a hotbar cast is driven entirely by notifying the vanilla shout behavior graph, with the played clip chosen by OAR. The mod owns no casting graph of its own.
_Avoid_: Shout, spell casting animation, magic behavior

**Chain Window**:
The interval late in a shout exhale during which attack input is honored, letting the animation hand off to an MCO attack. Owned by the separate MCO shout behavior engine, not by this mod.
_Avoid_: Attack cancel, animation blend

## Findings

Verified 2026-07-31 by static inspection of this repository unless noted. The vanilla
`shout_behavior` facts are cross-referenced from the sibling MCO shout behavior engine project,
where they were dumped and verified against the graph itself.

### 1. Casting rides the shout behavior graph, through exactly three events

The mod has no casting graph. Every hotbar cast is three notifications into the vanilla shout
graph: `ShoutStart` to begin, `MT_BreathExhaleShort` to release, `ShoutStop` to cancel. An
exhaustive search of the plugin finds these three literals and no others; nothing overrides the
virtuals that return them.

This is why Direct Cast can cast without equipping, and therefore without occupying a hand —
the property the whole MCO integration exists to preserve.

### 2. The release event is stance-blind, so drawn-weapon casts play the sheathed clip

The vanilla graph's six exhale events encode word count **and stance** together.
`MT_BreathExhaleShort` selects the sheathed short exhale; the `CombatReady_*` events select the
weapon-drawn branch. The mod fires the sheathed event unconditionally.

Two consequences. A cast with a weapon drawn plays the sheathed shout clip, which is a plain
animation defect on its own terms. And every cast lands outside the `CombatReady_*` branch that
MCO chaining is designed around.

### 3. Only the short exhale is ever used

Because the release event never varies, the mod reaches one exhale clip per stance rather than
the twelve-name shout animation API a normal shout pack must cover. The animation surface to
author is correspondingly small.

### 4. Release is timer-owned, not graph-owned, and the cast dies with the shout state

The mod advances its own cast timer and fires the spell itself. It does not use the graph's own
`Voice_SpellFire_Event`, which vanilla places 0.1s into every exhale and which is what makes
vanilla shouts robust to varying clip durations.

It also treats the `IsShouting` graph variable as a liveness check: the moment `IsShouting` goes
false, the cast instance is torn down. Anything that ends the shout state early — including an
MCO chain-out during a Chain Window — destroys the cast and the spell never fires.

### 5. Which animation plays is data, not code

A global set per cast (`SpellHotbar_SpellAnimationType`, `SpellHotbar.esp`) is read by OAR
conditions to pick the clip. Animation id `0` is named "Skyrim Shout" — the vanilla fallback.

A cast that looks like an unmodified vanilla shout may therefore be an unmapped spell rather
than a design limit. Check the in-game spell editor's animation assignment before treating it
as a defect.

### 6. The MCO shout behavior engine needs no changes to serve this mod

That engine's DLL is a pure animation-event observer, and its ADR-0002 forbids it from reading
shout cooldown or availability state. It cannot distinguish a Shout-Graph Cast from a real
shout. All integration work is therefore on this side of the line.

Its documentation frames consumers as animation *packs*. This mod is a second consumer class —
a *driver* that enters the shout graph programmatically with no shout equipped — which is worth
a note in that project but implies no code change there.

### 7. Battlemage separates cleanly except for the part that pays out

The perks, configuration globals and proc magic effect live in the main plugin; only the Custom
Skill Framework menu power lives in the optional battlemage plugin. The proc triggers are
ordinary hit and game-loop event sinks, portable to a standalone plugin.

The proc's benefit is not portable. Near-instant cast and reduced cost exist only inside this
mod's own cast pipeline, where cast time is a synthetic timer it owns and magicka is deducted
manually. Vanilla casting offers neither lever: cost is recoverable through a perk entry point
conditioned on the proc effect, but cast time has no vanilla mechanism and would need its own
hook.

### Open question

Whether `ShoutStart` is honored while the player is inside an MCO attack state. If the graph
refuses the transition, the liveness check in finding 4 fails on the first update and the cast
is swallowed silently. Not answerable by static inspection.
