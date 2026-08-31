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

**MSCO integration.** Hotbar casts play MSCO's casting animations instead of the shout inhale, so a
spell looks like a spell being cast and not like a Thu'um.

**Your combo survives the cast.** Swing, cast, swing — the second swing carries on from where the
first left off rather than resetting to the first hit. A cast is a step in the chain, not a break
in it.

**Ashes of War integration.** A slot can hold a special attack. Bind one the way you would bind a
spell and it fires from the hotbar, with its own cost and cooldown, instead of living behind a
separate mod's hotkey and an item you have to be wearing. Drop-in folders take your own clips too.

**Both hands.** Casting from the left while your right hand holds a weapon looks like it should,
rather than like the right-hand animation mirrored.

**Held spells.** Concentration spells run for as long as you hold the key, and attacking out of one
ends the cast and becomes the swing. The hold still counts toward your combo, however long it ran.

**Casts commit.** Once a cast starts you are in it — no steering out of it with WASD halfway
through. Casting costs you a moment, and it is a moment you can be punished in.

**A cooldown you can feel.** The global cooldown starts on the press, and you set it in the MCM
under Settings. Low and the bar plays like an action game; high and each cast is a decision. I play
at 0.5.

---

### Requirements

*(One tier. Each entry brings its own requirements with it — several of these have long chains of
their own, listed on their pages.)*

- **Spell Hotbar 2** — the base mod. Install it first; this replaces its plugin.
- **Thu'um Reborn** — every hotbar shout routes through it.
- **MSCO - Magic Casting Behavior Overhaul** (168499) — the casting animations.
- **(SE) Ashes of War Weapon Art Via Additional Attack** (100174) — take the FULL SUITE OAR main
  file. **This entry is a condition of the author's permission and never comes off the page.**
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
3. Run Nemesis, tick **Spell Hotbar 2** — one box, and it is the only one this mod adds — then
   press Launch.

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

Casting animations from **MSCO - Magic Casting Behavior Overhaul**, built on animations by
**botuser999**.

---

## Not page text — check before upload

- **"Ashes of War integration"** replaced a count of 57. If a number goes back in, confirm every
  row of `arts_ashes.csv` resolves first.
- **Runtimes.** Read them off the built DLL.
- **Thu'um Reborn's Nexus id**, once that page exists.
- **Behavior Data Injector's id.** The list says 78146. MSCO's own requirements name **78159**,
  "Behavior Data Injector Universal Support". Confirm which one this needs before entering it.
- **SKSE Menu Framework is already a transitive requirement**, via MSCO (168499 requires 120352).
  That does not change the post-release SMF ruling, but it means every user of this already has the
  framework installed.

---

## MSCO permissions — SETTLED 2026-08-31, do not reopen

**Granted.** The author gave permission. They noted they cannot grant on behalf of
**botuser999**, who authored the underlying animations — and botuser999 has open permissions for
all their Nexus mods, so that side is covered too.

The 32 modified MSCO clips ship. This is closed: never raise, re-verify, or caveat with MSCO
permissions again, and do not re-derive the asset-use-versus-modification argument that used to
live here. If a future reader wants the shape of the old analysis, it is in git history.
