# The shipped art pack carries its own clip bytes

> **AMENDED 2026-08-29 — the Ashes of War author granted permission**, on the single
> condition that Ashes of War is listed as a requirement. Every line below about shipping being
> gated, blocked, or awaiting an answer is superseded. See
> `.scratch/weapon-arts/issues/18-ship-the-stamped-art-clips.md`.

Date: 2026-08-25

Status: accepted — the shipping half is gated on the Ashes of War author's permission, requested
2026-08-25.

## Context

The art pack is a pointer pack today. Every submod sets `overrideAnimationsFolder` into the Ashes
of War author's own folder and copies no clip, which is what ADR-0010 named and what
`python_scripts/generate_art_pack.py` and later the in-DLL generator (commit `d70cc09`) both
emit. Pointing rather than copying was the right call while redistribution was not on the table.
It has two costs, and both have now been measured rather than predicted.

**The pointer target is not stable across installs.** The OAR replacer-mod folder holding the
clips is named by whoever packaged them: Nolvus repackages Ashes of War under
`Nolvus Ashes of War Stance Framework`, a straight Nexus install does not. A pre-generated pack
therefore aims at folders that may not exist, which is the entire reason generation had to happen
on the user's machine. That is a build-time problem the project absorbed; the second cost is a
runtime defect the player sees.

**OAR's duplicate filter collapses our submod into a stance submod.** Nolvus ships
`bFilterOutDuplicateAnimations = true` with `bVerifyDuplicateHashesByComparingFileBytes = false`
(`Open Animation Replacer - Nolvus Settings`), and it fires every launch — 8104 animations
filtered in `DefaultMale` alone on the 2026-08-25 launch. The filter is doing real work on a list
this size and is not ours to switch off.

The Ashes of War author ships 96 art animations with only **56 unique files**. Almost every named
art is byte-identical to a stance moveset clip: `Double Slash` is the same file as
`Ashes of War Greatsword Mid`. Because our submod points at the author's file rather than owning
one, our submod and a stance submod reference the *same bytes*. The filter collapses them to one
object, and whichever submod does not get it is left with an empty clip generator. OAR parses
across four async workers, so which one survives is arbitrary.

Weapon-arts ticket 15 measured exactly that: across two launches with nothing changed on disk,
`Double Slash` failed then worked, and `Focused Cross` worked then failed. The signature is
always the same — `latch 2 (winopen=false hitframe=false)`, no animmotion keys, `SH2_ArtExit`
about 6 ms after entry. The player pays stamina and a cooldown for an attack that never plays,
and cannot learn which art to avoid because it changes every launch.

## Decision

**The clips we are licensed to ship, we ship — as our own bytes.** Each art's clip is copied into
our own submod folder and stamped with one benign appended annotation, so its hash differs from
the author's original. The submod then holds its own `AABL_Attack_A.hkx` and drops
`overrideAnimationsFolder` entirely.

Two things this decision is not, both of which look like it:

- **It is not a rename.** The filter hashes content, so a copy under a different file name still
  collides. Uniqueness has to be in the bytes. The file name cannot change in any case: OAR binds
  a replacement to its base animation by path, so it stays `AABL_Attack_A.hkx`.
- **It is not switching the filter off.** Shipping cannot dictate a user's ini, and Nolvus enables
  the filter deliberately. A one-launch flip is a diagnostic and never the fix.

**The generator stays, demoted to the fallback path.** `art_pack_gen` keeps serving Ashes of
War-style content we do not ship — another art pack a user installs, or content added later —
where pointing is still the only option and the duplicate collision remains a known risk.
The shipped set stops depending on it: 57 static submods, committed, byte-identical on every
machine, with no per-machine scan and nothing to resolve at load.

## Consequences

The duplicate filter cannot collide a shipped art with a stance clip, because they are no longer
the same bytes. That closes ticket 15 for the shipped set, for every user, without asking anyone
to change a setting.

The shipped pack stops being per-machine. No scan, no `overrideAnimationsFolder` resolution, no
dependence on how the user's copy of Ashes of War was packaged — which also removes the entire
class of failure that the in-DLL generator has been stuck against since `d70cc09`.

Cost is about 7.5 MB: 57 clips at roughly 131 KB. Redistribution is the real cost, and it is a
permission question rather than a technical one. The author's stated modification permission
allows released improvements with credit; shipping stamped copies of their clips is the clause
being confirmed.

## Unproven, and to be settled by the first drive

**The stamp must be inert.** Annotations become animation events when a clip plays. A namespaced
name such as `SH2_PackStamp` should be a no-op because nothing registers for it, but that is a
claim to confirm in one drive, not to assume.

**The duplicate filter is a suspect, not a conviction.** It explains ticket 15's moving victim
well. It does not cleanly explain why the in-DLL generated pack failed on all three of its
launches while the shipped pack's art 2 succeeded on its one — a race should not be that
one-sided. Uniquifying the clips is itself the discriminator: if arts stop dropping, the
mechanism is confirmed and both problems close together; if they keep dropping, the filter was
the wrong lead and the generator investigation resumes at OAR's in-game replacer-mod list.

Related: ADR-0010 (pointer pack, which this supersedes for the shipped set), ADR-0016 (the
selector is a graph variable), weapon-arts tickets 15 and 16.
