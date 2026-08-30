# Release: publishing this fork to Nexus

Status: in progress — created 2026-08-29 on the owner's ruling that release planning starts now.

The product's first public release. Model page for presentation quality is
[Fitting Room - ESO Style Transmog](https://www.nexusmods.com/skyrimspecialedition/mods/185342)
by Mern (owner's pick, read off the live page 2026-08-29). Model page for *register* stays
[Fury - an Adamant Addon](https://www.nexusmods.com/skyrimspecialedition/mods/169966), inherited
from the sibling repo's ticket 18 where three drafts were rejected before that ruling landed.

This spec owns strategy and sequencing. Tickets own the work.

## What is being released

An **overwrite** over base Spell Hotbar 2 (owner ruling 2026-08-29, recorded in
`../mco-integration/issues/59-release-packaging-ships-no-nemesis-tree.md`). Base SH2 is a hard
requirement; ours installs on top and wins the conflict. Our DLL is a modified build of
upstream's, so it cannot sit beside it. Prospective name: **Spell Hotbar 2 NG**.

Upstream permissions are settled. Do not raise, re-litigate, or caveat any deliverable in this
effort with them — ticket 59's acceptance still carries a stale "publication deferred until
permissions are settled" line, and ticket 01 here retires it.

## SMF is not in this release, and that is not an open question

**Owner ruling, 2026-08-29:** *"i've said this so many times. this is out of scope until after
release."* The `../skse-menu-framework/` effort ships after the first release. Media and copy shoot
against the currently deployed surface — the SkyUI MCM plus the current ImGui editors, bar drag and
bind menu.

The five tickets there carry `deferred — post-release` so a frontier scan does not surface them as
shippable work. That marking is the point: the ruling has been given several times and restating it
more firmly has not held, so it is now mechanical. Any agent that reads those tickets as live is
reading a status line that says otherwise.

## The two calls that shape everything else

**1. The A/B is a runtime setting flip, in one session, on one save.** `Input::set_input_mode`
takes Cast (0), Equip (1), Oblivion (2) — Equip mode *is* the equip-first behaviour our Direct
Cast replaces (`skse_plugin/src/input/modes.cpp:31`). So the demo can show the old behaviour and
the new one seconds apart, same light, same camera, same combo. Two separate recording sessions
could never match that, and a viewer would be right not to trust them. This is the single
highest-value fact in the plan; the video is built around it.

**2. The hotbar is what the mod *is*, and the engine's own screenshot path does not draw it.**
`capture kind=native` produced a 3440x1440 frame with the world and no hotbar, because SH2 draws
its bar through its own ImGui present hook (measured 2026-08-25, memory
`sh2-imgui-hotbar-is-uncapturable`). Desktop-level capture composites the presented frame and
should carry the overlay, but that is an expectation, not a measurement. Ticket 03 proves it with
one three-second take before any shot list is planned around it.

## Presentation: what Mern actually does

Read off the live page 2026-08-29, so the copy below is checkable rather than impressionistic.

- **Main image is key art, not a screenshot.** 2560x1440, flat saturated purple field, a large
  ghosted coat-hanger watermark, a blurred in-game shot behind it at low opacity, and a
  background-removed character render occupying the right third. Text lives on flat colour in the
  left third and is therefore legible at tile size.
- **The title lockup is two weights of one family.** Thin light "Fitting Room", a small stacked
  qualifier "ESO / STYLE" divided by a rule, then heavy bold "TRANSMOG" in a lighter tint. At
  Nexus tile size only the heavy word survives the shrink. Pick that word deliberately.
- **A header banner, separate from the main image.** A wide cropped strip of the actual UI. Most
  pages do not set one; it reads as care.
- **The other eight gallery images are honest, unannotated UI shots** at 2560x1440 — panel left,
  character right, blurred depth-of-field background. No callouts, no arrows, no captions burned
  in. The polish is in the UI and the framing.
- **Sections are named after the UI pages** — OUTFITS PAGE, DYE PAGE, RULES PAGE. Feature as
  section, prose paragraph then bullets.
- **Requirements are tiered**: REQUIREMENTS, OPTIONAL REQUIREMENTS, HIGHLY RECOMMENDED.
- **The AI disclosure has a voice** and is the one place on the page that does. First person,
  names what the LLM did and what he did, states his credentials, ends on a joke about
  technofeudalism. This is the part the owner specifically noted works, and copying its *shape* —
  candid, specific, unhedged, funny once — is deliberate.

## Voice, and the four rules that cost the sibling repo three drafts

These were paid for in ticket 18 of `shouts-for-mco` and apply unchanged. They are not style
preferences; each one describes a draft that was rejected.

- **No trailer voice.** No dramatic arc, no setup-pause-reveal, no paragraph with a punchline
  position. Mod authors do not write advertising and readers recognise it instantly.
- **Feature reference, not essay.** State what it does, then list what it does. Do not argue that
  the reader had a problem. Do not open on invented player experience.
- **Numbers instead of adjectives.** Mern says 458 dyes and 10 outfit slots. We say the cast
  index, the GCD, the slot count.
- **Length is part of it.** The rejected draft ran 1,400 words against an accepted 813. A Nexus
  reader skims.

Known limits go near the top, under what it does. Volunteered early they read as credibility;
found at the bottom they read as a cover-up.

Mern's page departs from Fury on exactly one axis — the AI disclosure's personality — and that is
the departure to copy. Everywhere else, flat.

## Sequence

```
01 settle the public identity                 ← name and version only; needs the owner
02 package the archive (= mco-integration 59) ← needs 01's name and version stamp
03 prove the capture path                     ← can run now
04 record the demo video                      ← blocked by 03
05 cut the GIFs                               ← blocked by 04
06 key art and header banner                  ← blocked by 01 (the title lockup)
07 gallery screenshots                        ← blocked by 03
08 write the page                             ← informed by 04
09 upload session                             ← blocked by all
```

Ticket 03 can start today and is the cheapest thing on the list; it decides whether 04 and 07 are
possible as written. Ticket 01 needs the owner, and only the name and version block anything.

## The owner's playtest is a publication gate

Carried over from the sibling repo's ruling, which is a project-wide rule and not a per-repo one:
agents build to UAT and do not park work pending owner feedback. One playthrough stands between
"complete" and "published", and it lives in ticket 09 and nowhere else. No other ticket in this
effort may carry "owner has playtested" as acceptance.
