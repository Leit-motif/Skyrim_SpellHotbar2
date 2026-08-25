# 15 — An arbitrary Ash plays no clip, and which one changes every launch

Reported as "Double Slash does nothing but goes on cooldown". It is not Double Slash. On any given
launch one Ash or another plays no animation while still spending stamina and starting its
cooldown, and **which Ash it is changes every time the game starts**. The player pays for an attack
they never see, and cannot learn which one to avoid.

**Blocked by:** nothing

**Status:** PARKED for a post-launch refinement pass (owner ruling, 2026-08-25). See ticket 16's parked note; the byte-unique fix is live and did not reproduce this defect across 51 arts on one launch, but the three-launch bar was deliberately not pursued.

## What is actually happening

The fork's side of the press is correct all the way through. The Ability Class check passes
(Double Slash is Generic), `try_start_art` sets the Art Selector, and the `shtb` art state enters.
What never arrives is the clip:

```
14:36:45.030  art_driver:59    latch 2 (winopen=false hitframe=false)
14:36:45.031  clip_translation SH2_Art_Clip activated with no animmotion keys
14:36:45.037  animationeventhook SH2_ArtExit          <- 6 ms after entry
```

A working art reads the opposite way — `latch 0 (winopen=true hitframe=true)`, a bound clip with
real motion keys, and an exit at the end of the animation rather than 6 ms in. So the art state is
running against an empty clip generator: OAR is not applying the fork's submod for that selector.

## What was ruled out

Evidence gathered 2026-08-23, profile `Nolvus Awakening`, `Dev - Spell Hotbar 2` matching a fresh
build of `8a25cac`.

- **The fork.** Seven other arts bound and fired through the `castSlot` seam under identical
  conditions — Aimed Blow (2), Akatosh Charge (3), Crane Style (8), Cyclone Spin (10), Dash Slam
  (11), Disengage (12), Dragon Strike (16) — every one bound a real clip (15–200 animmotion keys).
  Within a single launch the failure is perfectly repeatable; across launches it moves.
- **The animation file.** `hkxc-anno-cli dump` reads it cleanly: 2.666667 s, 99 transform tracks,
  171 annotations including `HitFrame`, `MCO_WinOpen`, `MCO_PowerWinOpen` and a full animmotion
  track. It is not corrupt and it is not empty.
- **Extension case.** The pack mixes `.HKX` (58 files) and `.hkx` (38). Disengage is `.HKX` and
  works, so OAR is not skipping the uppercase ones.
- **The submod config.** Byte-identical to Flurry Strike's and Disengage's apart from name,
  priority, selector value and folder.
- **VFS ambiguity.** Four mods provide `Nolvus Ashes of War Stance Framework/Double Slash`; all
  four ship the same 131088-byte file with the same MD5, so the winner does not matter.
- **The equipped weapon.** Fails identically on dual wield and on a greatsword.

## The remaining lead

Of the 96 Ashes of War art animations, only **56 hashes are unique**. Almost every named art is
byte-identical to a stance art — Double Slash is the same file as `Ashes of War Greatsword Mid`.
Nolvus ships OAR with duplicate filtering on:

```ini
[Filtering]
bFilterOutDuplicateAnimations = true
bVerifyDuplicateHashesByComparingFileBytes = false
```

and OAR's own log confirms it fires: `Filtered out 8115 duplicate animations in project
DefaultMale`. Because every fork art borrows the author's files through
`overrideAnimationsFolder`, the fork's submod and a stance submod point at the same bytes. If the
collapse keeps the stance submod's object, the fork's replacement is left with nothing — which is
exactly the symptom. OAR parses across four async workers, so which submod survives would be
arbitrary — which is what the restart below actually showed.

## Confirmed 2026-08-23: the victim changes across a restart

The game was quit and relaunched with **no config change of any kind**, then every Generic art was
bound and fired through the `castSlot` seam, and each result compared against the art's own
`hkxc-anno-cli` dump.

| | Launch A (pid 26976 predecessor) | Launch B (pid 26976) |
|---|---|---|
| Double Slash (15) | empty clip, `latch 2`, 0 keys | **works** — `latch 0`, 160 keys |
| Focused Cross (24) | not tested | **empty clip**, `latch 2`, 0 keys |
| Disengage (12) | works, 140 keys | works, 140 keys |

The bound key counts match each file's dump exactly — Double Slash's dump holds 160 animmotion
keys and Disengage's 140, and those are the numbers the runtime reported. So the right clip is
reaching the right art when it works at all.

Same signature, different victim, nothing changed on disk. That rules out every static
explanation — the file, the submod config, the catalogue row, the VFS winner, the extension case,
the equipped weapon. What is left is a load-time resolution race inside OAR.

The duplicate filter stays the leading candidate: both confirmed victims, Double Slash and Focused
Cross, are hash-duplicates of a stance art, and 40 of the 96 art files are duplicates of another.
The mechanism is not yet pinned to that setting.

**Scale:** 1 art out of 26 Generic arts on launch B. Small, but the player cannot predict which
one, and a dead art still charges stamina and cooldown.

**Beware the false signal.** `SH2_Art_Clip activated with no animmotion keys` is not by itself a
fault. Eldritch Beam, Enrage (F) and Enrage (M) are genuinely stationary clips with zero
animmotion keys, and they report that warning while working correctly. A real failure is the
runtime's `latch` *and* key count disagreeing with the file's own dump.

## Separate, smaller finding

Crane Style (8), Dash Slam (11) and Simple Bash (51) bind the correct clip — key counts match
their dumps — but the driver reads `hitframe=false` where the file carries a `HitFrame`
annotation. The art plays; only the latch classification is wrong. Worth its own ticket rather
than folding into this one.

## Agent tests the rest

1. ~~Restart with no config change and re-fire art 15.~~ Done — see above. Confirmed
   nondeterministic.
2. Set `bFilterOutDuplicateAnimations = false`, restart, re-fire art 15 with arts 12 and 16 as
   controls. Art 15 playing a real clip confirms the cause. **Owner ruled 2026-08-23 that their
   OAR config stays untouched, so this cell needs their say-so before it runs.**
3. If confirmed, decide the fix: give the fork's submods physically distinct animation files
   (which changes what ADR 0007 promises about borrowing the author's path), or document the ini
   requirement, or find an OAR field that opts a submod out of the collapse.

## What this is

A pack/OAR resolution bug reached through the fork. Ticket 04 owns which clip an art plays.

## What this is not

Not the Ability Class gray-out (07, 13). Not clip translation (05) — the motion driver is
reporting the fault correctly, not causing it. Not the Binding Menu.

## Comments

Owner 2026-08-23: reported during the wa13 acceptance pass — "the AoW Double Slash does nothing
(but goes on cooldown)". Filed with the evidence rather than actioned; owner chose to leave the
OAR config alone for now.

## Direction settled 2026-08-25 — fixed by owning the bytes

The duplicate-filter lead above is now the accepted cause, and the fix is
[ticket 16](16-ship-byte-unique-art-clips.md): ship our own copies of the art clips, each stamped
with one benign annotation so its hash differs from the stance clip it currently collides with.
A submod that owns its bytes cannot be collapsed into another one. See
[ADR-0017](../../../docs/adr/0017-the-shipped-art-pack-carries-its-own-clip-bytes.md).

Ruled out as fixes, so they are not re-proposed here: renaming our copies (the filter hashes
content, and OAR binds a replacement to its base animation by path, so the file stays
`AABL_Attack_A.hkx`), and turning `bFilterOutDuplicateAnimations` off (Nolvus enables it
deliberately — 8104 animations collapsed in `DefaultMale` on the 2026-08-25 launch — and shipping
cannot dictate a user's ini; one flip is a diagnostic, never the fix).

Corroboration from a second failure, 2026-08-24: the in-DLL generator (`d70cc09`) produced this
exact signature on all 57 arts across three launches, while the byte-identical shipped pack played
art 2 correctly. Same fingerprint, universal instead of intermittent. That is consistent with the
filter and not yet explained by it — a race should not be that one-sided — so ticket 16's phase 1
is the discriminator for both.

This ticket stays open until ticket 16's phase 1 acceptance passes: 57 arts binding real clips
across three consecutive launches. One clean launch proves nothing, because the defect takes one
or two arts at random.
