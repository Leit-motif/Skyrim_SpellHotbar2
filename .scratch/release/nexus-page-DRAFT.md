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

- **MSCO's permissions are the open question, and they are not settled.** See the block below.
  Nothing ships until they are.
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

## MSCO permissions — the finding, 2026-08-29

**We ship 32 of MSCO's animation files, modified.** This was assumed otherwise and checked instead.

- `data/meshes/.../OpenAnimationReplacer/SpellHotbar2Casts/**/MSCO_*.hkx` — 32 files, and they are
  every `.hkx` in the repo.
- `build_mod_release.py` ships them: line 416 is `for rel in tracked_files("data")`, with no
  animation exclusion.
- They are derived, not copied. MSCO's own root clips are all one hash (`95b61e1…`); our eight
  per-context variants of `MSCO_left1.hkx` are eight distinct hashes, none of them MSCO's. Ours is
  15,520 bytes against MSCO's 13,152 for the real submod clip — MSCO's animation data with our
  annotations stamped into it.

**Read of the two clauses.** They do different jobs, and the distinction is the whole answer.

*Modification permission* — granted with credit — governs releasing a modified version of **their
mod**: a bug fix, a patch, an improved MSCO. *Asset use permission* — not granted, ask first —
governs taking assets **out of their file and into yours**.

We are doing the second one. The clips leave MSCO's package and ship inside ours, under our own
folder name. Editing them first does not move that into the modification bucket; the modification
clause is permission to improve MSCO, not permission to reuse MSCO's animation data elsewhere
because you changed it on the way.

**The instinct behind "it's a requirement, so we're not using the assets" is right, but it is not
what makes it true.** Listing a mod as a requirement grants nothing by itself. What would make it
true is the archive containing none of their bytes — requiring MSCO and driving its installed clips
at runtime needs no permission at all. Right now the archive contains 32 of them.

**The Ashes of War precedent is real but it does not transfer cleanly. Corrected 2026-08-29 after
checking the code rather than the memory of it.**

What was actually ruled and shipped there, from `../weapon-arts/issues/18-ship-the-stamped-art-clips.md`:
the author never answered, the owner ruled *"i want to ship. that would mean we defer this effort to
a future enhancement"*, and the pack stays **pointer-style** — `art_pack_gen.cpp` writes
`config.json` files with `overrideAnimationsFolder` and copies no clip bytes at all. That is clean
precisely because Ashes of War's clips are used **as they are**.

MSCO's are not. Our 32 clips carry our own annotations, and those annotations are the feature —
cast timing and the per-hand payloads. A pointer at MSCO's unmodified clip does not carry them, so
the shipped Ashes of War answer cannot simply be repeated here.

**What it would actually take.** `python_scripts/stamp_art_clips.py` does the copy-and-annotate, but
it is a build-time script on this machine, and ticket 18 is explicit that running it locally *"needed
no permission because copying locally is not redistribution. Shipping them does."* Getting the same
property for the cast clips means doing that stamping **on the user's machine, against their own MSCO
install** — porting the copy-and-annotate into the DLL. `art_pack_gen.cpp` is the natural home and
today it writes configs only, never bytes. That is new work, not a reuse.

**So the honest options are two, not one.** Ask VVVK-distar-xing-adri (mod 168499) and ship the 32
clips if the answer is yes; or build runtime stamping and ship no clips at all. Asking is cheap and
worth starting now either way — the sibling case says silence is the likely reply, and it is better
to learn that before the second option is needed rather than after.
