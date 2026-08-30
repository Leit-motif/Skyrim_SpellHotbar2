# Nexus page draft — Spell Hotbar 2 NG

> **NOT THE MOD PAGE.** This lives in `.scratch/` because a draft in `docs/` reads as finished and
> got cited that way twice in the sibling repo. Ticket 08 owns when this becomes a page. Do not
> cite this file as evidence of anything.
>
> **First draft, 2026-08-29.** Written before the demo video exists, so every claim here is
> traceable to an ADR or a measured number rather than to footage. The list at the bottom names
> what still has to be checked before upload.
>
> **The name is provisional.** `deploy/release/release.json` says `Spell Hotbar 2 NG` with
> `identity_frozen: false`. Release ticket 01 owns it.

---

**Title:** Spell Hotbar 2 NG

**Summary field:**

> An overwrite for Spell Hotbar 2 that makes a hotbar press a real MCO action. Requires the base
> mod. Nemesis only.

---

## Description body

An overwrite for Spell Hotbar 2. A slot casts its spell without equipping it and without occupying
a hand, the cast plays a real MCO clip instead of the shout animation, and the swing afterwards
picks your combo up where it left off. Twelve slots a bar, and a slot can hold a special attack as
well as a spell.

Base Spell Hotbar 2 is required and this installs on top of it. Run Nemesis afterwards.

### Known limits, before anything else

This replaces base Spell Hotbar 2's DLL rather than sitting beside it, because 25 of its own source
files are modified here. Two things follow. **You must install the base mod first** — the icons,
fonts and presets all come from it, and this archive deliberately does not carry them. And **this
build is pinned to base version 0.0.14**: if the base mod updates, this overwrite silently reverts
whatever changed until it is rebuilt against the new source. The base mod has not moved since June
2025, so the risk is small, but it is real and it is quiet.

Nemesis only. Pandora is not supported.

### Casting

A hotbar press casts directly. Nothing is equipped, no hand is occupied, and your weapon stays
where it is — which is the entire reason this fork exists.

The cast plays through this mod's own behaviour states rather than borrowing the vanilla shout
graph, so the animation is an MCO clip chosen by OAR and not a repurposed inhale.

### Casting inside a combo

Attacking after a cast continues your combo at the index it was on, rather than restarting from the
first hit. The cast is a step in the chain, not an interruption of it.

### Abilities

A slot can hold a special attack. Not a Spell, Scroll, Shout or potion — an actual MCO attack
animation, bound and fired the same way a spell is, with its own cost and cooldown owned here
rather than by a separate mod's hotkey.

The catalogue ships 57 entries keyed to Ashes of War's animations, and it does not redistribute
them: the rows point at the clips your own install already has. Any MCO-annotated clip your load
order can see will work — there are drop-in folders for adding your own without touching anything
else.

An ability is a member of the combo chain too. The swing after it continues the count.

### Both hands

The left and right hands present differently over one neutral graph, so casting from the left while
the right keeps a weapon looks like what it is instead of a mirrored guess.

### Held casts

A concentration spell held from a slot holds for as long as you hold it, and an attack during the
hold ends the cast and becomes the swing. The hold counts toward the combo, so a channel of any
length still hands its position on.

### Commitment and the cooldown

A cast commits. You cannot steer out of it with WASD partway through, because the state routes
around locomotion rather than blending with it.

The global cooldown is anchored to the press, not to the animation, and it is tunable. Cranking it
down makes the bar feel like an action game's; leaving it up makes casting a decision. I play at
0.5.

### Configuration

The SkyUI MCM, as in the base mod, plus this fork's own in-game editors and bind menu.

---

## Requirements

**Required.** All of them, in this order.

- **Spell Hotbar 2**, version 0.0.14 — the base mod. This overwrites it, so install it first.
  GitHub releases, not Nexus.
- **(SE) Ashes of War Weapon Art Via Additional Attack** (100174) — take the FULL SUITE OAR
  main file. The shipped ability catalogue points at its clips.
- Address Library for SKSE Plugins (32444)
- Nemesis Unlimited Behavior Engine (60033)
- Open Animation Replacer (92109)
- Payload Interpreter (65089)
- Behavior Data Injector (78146)

Two Nexus cannot link: **SKSE64** (skse.silverlock.org) and **ADXP | MCO** (Distar's
skyrim-guild / Discord). MCO is not optional — there is no combo to chain into without it.

## Installation

1. Install base Spell Hotbar 2, and the rest of the requirements.
2. Install this after Spell Hotbar 2 and let it win the conflict. It is an add-on that
   overwrites its base mod.
3. Run Nemesis, tick the patches, Launch. You do not need Update Engine.

## Compatibility

*(To be written against what the video and the playtest actually establish. Do not guess it.)*

## AI disclosure

*(Owner's own words. The shape that works, from the model page: first person, specific about what
the tools did and what you did, credentials stated plainly, no defensiveness, one joke. This is the
only section on the page with a voice — do not write a defence, and do not spread the register into
the rest of the page.)*

## Credits

Spell Hotbar 2 by **pWn3d1337**. This is a fork, not an official continuation — they were not
involved in it.

---

## Claims to verify before upload — NOT page text

- **The GCD default number.** The copy says "tunable" and cites the owner playing at 0.5, but the
  shipped default was not found in `game_data.cpp` on the 2026-08-29 read. Get the number or cut
  the sentence.
- **"57 entries."** That is the row count of `data/SKSE/Plugins/SpellHotbar/artdata/arts_ashes.csv`.
  Confirm every row resolves, and confirm the pointer-not-redistribution wording is exactly right —
  it is a permissions statement as much as a feature one.
- **Runtimes.** Read them off the built DLL. Not stated above because they were not checked.
- **"Pandora is not supported."** Inherited from the sibling repo's page. Confirm it holds here.
- **Payload Interpreter and OAR as hard requirements.** Confirm against what the archive actually
  needs, rather than against what the dev profile happens to have.
- Word count of the body above: roughly 640. Ticket 08 targets 700–900, so the Compatibility
  section has room and does not need trimming to fit.
