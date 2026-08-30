# 08 — Write the Nexus page

**Type:** task
**Status:** ready-for-agent
**Blocked by:** nothing structural. Informed by 04 — write it after the video exists, because the
video settles which claims are actually demonstrable. Ticket 01's name is needed only for the title.

The draft lives at `../nexus-page-DRAFT.md` when it is written. **Not** in `docs/`: in the sibling
repo a draft under `docs/release/` was twice cited as though it were the finished page.

## Voice, and why these rules exist

Every rule below is a draft that was rejected in the sibling repo. They are not preferences.

- **No trailer voice.** No dramatic arc, no setup-pause-reveal, no paragraph with a punchline
  position. Specimens that were killed: *"Thu'um is back, and this time it speaks MCO"*,
  *"237,000 downloads, and then it stopped"*, *"which is all it ever needed to be."*
- **Feature reference, not essay.** State what it does, then list what it does. Three drafts died
  as essays arguing a thesis about combat paradigms.
- **Do not open on invented player experience.** No "you have done this a hundred times and blamed
  it on lag." Argue from the system.
- **Numbers instead of adjectives.** The model page says 458 dyes and 10 outfit slots. Ours says
  the slot count, the GCD default, the number of bindable ability clips.
- **Length.** Target 700–900 words. The rejected draft ran 1,400 against an accepted 813.
- **Assume the reader knows what MCO is.** They do, or the page is not for them.

One dry line lands better than five. Understatement, not build-up.

## The rule that matters most, added 2026-08-29

**Write every sentence from the player's side, not the plugin's.** Owner, rejecting the sibling
repo's draft: *"this is a technical documentation, not user facing copy. i even gave you a
reference of what good looks like."*

That draft had almost no AI tells. It was flat, it had numbers, it followed every rule above — and
it was still wrong, because it explained how the mod worked where it should have said what happens
when you play. The reference pages never do this: Fury says 25% more damage and five stacks, Mern
says ten outfit slots and that stats never change. Neither says how it is implemented.

Two mechanical checks before this draft ships:

- **No config identifier appears outside a Settings section.** Numbers belong in the feature
  bullets; variable names do not. The sibling draft had seven scattered through its body.
- **Every sentence names something the player does or sees.** "You plant where you stand" passes.
  "The state routes around locomotion rather than blending with it" does not, and that sentence is
  in the current SH2 draft.

The house rule "name the mechanism" governs internal docs. On a mod page the mechanism *is* the
internal detail.

## Structure — OWNER RULING 2026-08-29, and it replaces what was here

*"you keep treating this as coding documentation. this is end user copy. users will perhaps need a
brief introduction of what sh2 is and why it's so cool. but then an explanation of what my mod adds
to it (features). that's all!"*

**The page is two sections.** What Spell Hotbar 2 is, then what this adds. Requirements, install,
compatibility and credits are short boilerplate underneath, not part of the argument.

**1. What Spell Hotbar 2 is.** Assume the reader has never used it. Say what the base mod does and
why it changes how a mage plays — the hotbar, the twelve slots, casting without occupying a hand,
the bars that swap with your stance. This is the only place the page sells anything, and it is
allowed to. Roughly 150 words.

**2. What this adds.** The features, one bold lead-in each, player-facing. One short paragraph
naming what the base mod does today (it borrows the shout animation, so casts look like Thu'ums and
break your combo) is enough setup; do not write an essay about it.

| Feature | Established by |
|---|---|
| Real casting animations instead of the shout inhale | ADR-0013, ADR-0018 |
| The combo survives a cast | ADR-0014 |
| Weapon arts bindable to a slot | ADR-0011, weapon-arts spec. Give the count |
| Per-hand casting | ADR-0018 |
| Held / concentration casts, chaining out into an attack | ADR-0013 |
| Casts commit | ADR-0015 |
| Press-anchored global cooldown, tunable | Owner plays at 0.5 |

### Three things the first draft did that are now forbidden

Each one was in the 2026-08-29 draft and each was struck by the ruling above.

- **It never said what Spell Hotbar 2 is.** A reader who does not already own the base mod learned
  nothing from the page.
- **It put "Known limits" second.** The overwrite coupling, the base-version pin and "25 of its own
  source files are modified here" were handed to the reader before the page had told them what the
  mod does. That is a release document's instinct. The install note carries what a user actually
  needs — install the base mod first, built against 0.0.14 — and nothing else needs a section.
- **It explained mechanism instead of play** — *"the state routes around locomotion rather than
  blending with it"*. See the register rule above.

### Length

~~**No target.** The old 700-900 was set for a page carrying a known-limits section and an overwrite
essay.~~ **STRUCK 2026-08-29 by owner ruling** — *"let's not worry about that... There's no need to
be so prescriptive right now."* No word count, no target, and do not reintroduce one.

### The rulings that survive unchanged

**Do not list Equip mode or Oblivion mode.** Owner, 2026-08-29: *"i only designed this for direct
cast. i dont even know what equip and oblivion mode are — and frankly, i dont care."* Upstream's,
untested here. Advertising a mode nobody has driven is how a support thread starts.

**Requirements are one tier, each entry carrying its own chain.** Owner ruling 2026-08-29. Base
Spell Hotbar 2 first. Then **Thu'um Reborn** as ONE entry — engine and animations in one file; do
not write "Shouts for MCO", which is the old name. Then Ashes of War (100174, FULL SUITE OAR file).
None of these is a recommendation, so do not borrow the model page's OPTIONAL and RECOMMENDED
tiers. Anything Nexus cannot link — SKSE, ADXP | MCO — gets named in the body too, because a reader
scanning green links will not notice the one they cannot click.

**Installation** spells out the overwrite, because that is where support tickets come from: base SH2
first, this over it, run Nemesis (Launch — no Update Engine needed for a selection-only change).

**AI disclosure** is the one section allowed a voice, and copying the *shape* of the model page's is
deliberate: first person, specific about what the tools did and what the author did, credentials
plainly stated, no hedging, one joke. Candid, not defensive. Check the flag's wording on the upload
form and tag honestly.

**Credits.** pWn3d1337 for Spell Hotbar 2, and anyone whose clips or icons the package carries. Say
plainly that this is not an official continuation.

## Acceptance

- [ ] Draft at `../nexus-page-DRAFT.md`, carrying the "not the mod page" banner the sibling repo's
      draft carries.
- [ ] Every factual claim is traceable to an ADR, a ticket, or a measured number. No claim the
      video does not support.
- [ ] The page opens by saying what Spell Hotbar 2 is, for a reader who has never used it.
- [ ] No section is a known-limits or overwrite essay. The install note carries what a user needs.
- [ ] Base SH2 named as required in the summary field, the first paragraph, and the requirements
      list. Three surfaces, because the support burden if it is missed is every install.
- [ ] Ashes of War (100174) in REQUIRED, and the FULL SUITE OAR file named. A reader who takes
      a different main file gets an ability catalogue that resolves to nothing.
- [ ] Thu'um Reborn in REQUIRED as a single entry, Nexus id filled in once that page exists.
      Not two entries, and not under the old "Shouts for MCO" name.
