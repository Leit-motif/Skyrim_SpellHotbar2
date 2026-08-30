# Nexus page draft — Spell Hotbar 2 NG

> **NOT THE MOD PAGE.** Lives in `.scratch/` because a draft in `docs/` reads as finished and got
> cited that way twice in the sibling repo. Ticket 08 owns when this becomes a page.
>
> **The name is provisional.** `deploy/release/release.json` says `Spell Hotbar 2 NG` with
> `identity_frozen: false`.
>
> **REWRITTEN 2026-08-29 ON THE OWNER'S RULING. The first draft is struck in full.** Owner: *"you
> keep treating this as coding documentation. this is end user copy. users will perhaps need a brief
> introduction of what sh2 is and why it's so cool. but then an explanation of what my mod adds to
> it (features). that's all!"*
>
> **The page is two sections: what Spell Hotbar 2 is, then what this adds.** Everything else —
> requirements, install, credits — is short boilerplate at the bottom, not part of the argument.
>
> The first draft failed three ways, all of them the same instinct. It **never said what Spell
> Hotbar 2 is**, so a reader who does not already own it learned nothing. It put **"Known limits"
> second**, handing the reader the overwrite coupling and the version pin before it had told them
> what the mod does. And it explained mechanism instead of play — *"the state routes around
> locomotion rather than blending with it"*. Anyone restoring prose from git history restores all
> three.

---

**Title:** Spell Hotbar 2 NG

**Summary field:**

> Hotbar casting that behaves like a real MCO attack: proper animations, combos that survive a
> cast, and weapon arts you can bind to a slot. Requires Spell Hotbar 2.

---

## Description body

### What Spell Hotbar 2 is

Skyrim makes you hold a spell in a hand. That one rule shapes everything about playing a mage — you
open a menu to change spells, you put your weapon away to cast, and a fight turns into inventory
management with fireballs in it.

Spell Hotbar 2 gives you a hotbar. Twelve slots, bound to keys, holding spells, shouts, scrolls and
potions. Press one and it casts. The spell is never equipped and never takes a hand, so your sword
stays in it. Separate bars swap themselves in as you sneak, draw a weapon or go to magic, so what is
in front of you matches what you are doing.

It is the largest change you can make to how casting feels in this game.

### What this adds

Spell Hotbar 2 borrows the shout animation for its casts. That is a reasonable choice and it works,
but it means every spell you fire looks like a Thu'um, and casting drops you out of MCO the same way
shouting does.

**Real casting animations.** Every hotbar cast plays an MCO animation picked for that kind of spell
instead of the shout inhale.

**Your combo survives the cast.** Swing, cast, swing — the second swing carries on from where the
first left off rather than resetting to the first hit. A cast is a step in the chain, not a break
in it.

**Weapon arts on the bar.** A slot can hold a special attack. Bind one the way you would bind a
spell and it fires from the hotbar, with its own cost and cooldown, instead of living behind a
separate mod's hotkey and an item you have to be wearing. 57 of Ashes of War's arts are set up out
of the box, and there are drop-in folders for adding your own clips.

**Both hands.** Casting from the left while your right hand holds a weapon looks like it should,
rather than like the right-hand animation mirrored.

**Held spells.** Concentration spells run for as long as you hold the key, and attacking out of one
ends the cast and becomes the swing. The hold still counts toward your combo, however long it ran.

**Casts commit.** Once a cast starts you are in it — no steering out of it with WASD halfway
through. Casting costs you a moment, and it is a moment you can be punished in.

**A cooldown you can feel.** The global cooldown starts on the press. Set it low and the bar plays
like an action game; set it high and each cast is a decision. I play at 0.5.

---

### Requirements

*(One tier. Each entry brings its own requirements with it — several of these have long chains of
their own, listed on their pages.)*

- **Spell Hotbar 2** — the base mod. Install it first; this replaces its plugin.
- **Thu'um Reborn** — every hotbar shout routes through it.
- **(SE) Ashes of War Weapon Art Via Additional Attack** (100174) — take the FULL SUITE OAR main
  file. The shipped weapon arts point at its clips.
- Address Library for SKSE Plugins (32444)
- Nemesis Unlimited Behavior Engine (60033)
- Open Animation Replacer (92109)
- Payload Interpreter (65089)
- Behavior Data Injector (78146)

Two Nexus cannot link: **SKSE64** (skse.silverlock.org) and **ADXP | MCO** (Distar's skyrim-guild /
Discord). MCO is not optional — there is no combo to chain into without it.

You also need a current **Visual C++ redistributable**. Every SKSE plugin here fails without it,
usually silently.

### Installation

1. Install Spell Hotbar 2 first. This replaces its plugin, so it has to sit underneath.
2. Install this and let it overwrite.
3. Run Nemesis, tick the patches, Launch.

Built against Spell Hotbar 2 0.0.14. Nemesis only — Pandora is not supported.

### Compatibility

*(To be written against what the video and the playtest establish. Do not guess it.)*

### AI disclosure

*(Owner's own words. The shape that works, from the model page: first person, specific about what
the tools did and what you did, credentials plainly stated, no defensiveness, one joke. The only
section on the page with a voice.)*

### Credits

Spell Hotbar 2 by **pWn3d1337**. This is a fork, not an official continuation — they were not
involved in it.

---

## Not page text — check before upload

- **The GCD default.** The copy says it is tunable and that the owner plays at 0.5; the shipped
  default was not found in `game_data.cpp` on the 2026-08-29 read. Get the number or cut the line.
- **"57 of Ashes of War's arts."** Row count of `arts_ashes.csv`. Confirm every row resolves, and
  confirm the pointer-not-redistribution wording is right — it is a permissions statement as much
  as a feature one.
- **"an MCO animation picked for that kind of spell."** Confirm the per-family selection covers
  every school a reader would try, or soften the claim.
- **Runtimes.** Read them off the built DLL.
- **Thu'um Reborn's Nexus id**, once that page exists.
- Body above (intro plus features): about 480 words. Deliberately shorter than the 700–900 in
  ticket 08 — that target was set for a page carrying a known-limits section and an overwrite
  essay, both of which the owner cut.
