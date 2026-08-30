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

## Structure

Sections are the features, one per gallery image (ticket 07), matching the model page's shape.

**Summary field.** One sentence. It appears in search results and under the title, and it is the
only text some readers ever read. It must say that base Spell Hotbar 2 is required.

**Opening paragraph.** Dense and factual, no hook. What it is: an overwrite for Spell Hotbar 2 that
makes a hotbar press a first-class MCO action — the spell fires from the slot without occupying a
hand, plays a real MCO clip, and hands the combo back where it found it. Then the hard requirement
and the Nemesis run, in the same breath.

**Known limits, immediately after.** Not at the bottom. The overwrite coupling belongs here and it
is the honest one: our DLL is built from upstream's source at our fork point, so a future base-mod
fix is reverted by our overwrite until we rebase. Name the base version this build is pinned to.

**Then a section per feature.** Prose paragraph on the mechanic, bullets with the numbers.

| Section | The claim, and where it is established |
|---|---|
| Direct Cast | A slot casts without equipping and without occupying a hand. `CONTEXT.md`, Direct Cast |
| Casting inside a combo | The swing after a cast continues the combo at its index rather than restarting at hit 1. ADR-0014 |
| Abilities on the bar | A slot can hold an MCO special attack, not only a Spell, Scroll, Shout or potion. ADR-0011, weapon-arts spec. Give the shipped ability count |
| Per-hand casting | Left and right hands present differently over one neutral graph. ADR-0018 |
| Held casts | A concentration spell holds for the hold and chains out into an attack. ADR-0013 |
| Commitment and the GCD | A cast commits; the press-anchored GCD is the clock. Name the default and say it is tunable — the owner plays at 0.5 |
| Configuration | The SkyUI MCM. SMF is post-release, so do not describe a Mod Control Panel |

**Do not list Equip mode or Oblivion mode.** Owner ruling 2026-08-29: *"i only designed this for
direct cast. i dont even know what equip and oblivion mode are — and frankly, i dont care."* They
are upstream's, inherited by our build, untested here. Advertising a mode nobody has driven is how
a support thread starts. Direct Cast is the product.

**Requirements are one tier: REQUIRED, each one with everything it requires in turn.** Owner
ruling 2026-08-29. Base Spell Hotbar 2 first with its version pinned. Then **Thu'um Reborn** — the
author's own shout mod, ONE entry, engine and animations in one file per that repo's owner
ruling 2026-08-28 (*"no the animations are not a separate file. they will be shipped with the
mod"*). Do not list "Shouts for MCO"; that is the old name, still on the release zip and the
MO2 folder pending a rename. Then Ashes of War (Nexus 100174, FULL SUITE OAR file).
None of these is a recommendation. Do not borrow the model page's OPTIONAL and RECOMMENDED
tiers just because it has them. Several entries carry long chains of their own, so the "and all
of its requirements" phrasing is load-bearing, not filler. Anything Nexus cannot link — SKSE, ADXP | MCO — gets
named again in the body, because a reader scanning green links will not notice the one they
cannot click.

**Installation.** The overwrite is the part support tickets will come from. Spell it out: install
base SH2, install this after it, let it win the conflict, run Nemesis (Launch — end users need no
Update Engine for a selection-only change), tick the patch.

**Compatibility.** What it sits with, and what it does not.

**AI disclosure.** This is the one section allowed a voice, and copying the *shape* of the model
page's is deliberate: first person, specific about what the tools did and what the author did,
credentials stated plainly, no hedging, one joke. It works because it is candid rather than
defensive. Do not write a defence. Check the disclosure flag's wording on the upload form and tag
honestly.

**Credits.** pWn3d1337 for Spell Hotbar 2, and anyone whose clips or icons the package carries.
Say plainly that this is not an official continuation.

## Acceptance

- [ ] Draft at `../nexus-page-DRAFT.md`, carrying the "not the mod page" banner the sibling repo's
      draft carries.
- [ ] Every factual claim is traceable to an ADR, a ticket, or a measured number. No claim the
      video does not support.
- [ ] 700–900 words, counted.
- [ ] Known limits appear above the fold, not at the bottom.
- [ ] Base SH2 named as required in the summary field, the first paragraph, and the requirements
      list. Three surfaces, because the support burden if it is missed is every install.
- [ ] Ashes of War (100174) in REQUIRED, and the FULL SUITE OAR file named. A reader who takes
      a different main file gets an ability catalogue that resolves to nothing.
- [ ] Thu'um Reborn in REQUIRED as a single entry, Nexus id filled in once that page exists.
      Not two entries, and not under the old "Shouts for MCO" name.
