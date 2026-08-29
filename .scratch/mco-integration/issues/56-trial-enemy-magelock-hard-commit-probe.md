# 56 — Trial-install Enemy Magelock: does the hard commit work and feel right here?

**Status: CLOSED superseded 2026-08-29.** Trial-installing a third-party behavior patch is not
the path -- SH2 owns its own mechanism, and a dependency that contests `msco`/`pscd`/`sbeef`/
`shtb` is not something this mod ships. The mechanism was read from the reference's source
instead (`notes/57-mechanism-comparison.md`).

**Its one surviving half is the contention read**, which moves into ticket 58 as a build step:
the nodes to check are named in the comparison note, and `sbeef` is now both a thing not to
break and a framework already installed.

**Type:** trial install + contention read + owner feel test (adopt/adapt decided later, in 57)

**Status:** ready-for-agent. **Blocked by:** 54. Part of the ticket 53 umbrella.

## Why

The owner ruled commitment "can only happen through rooting the character in the manner that
it's done for msco animations" and supplied this mod as the done-before reference. Trialing it
unmodified is the fastest possible evidence that the state-machinery approach roots in THIS
load order — before we invest in dissecting or adapting it. It is also behavior-only, which
satisfies the no-Papyrus constraint natively.

## Protocol

1. Source: `C:\Users\Rando\Downloads\Enemy Magelock-49378-1-0-0-1619990342.rar`. Install as an
   MO2 mod (mod code `altmag`; content = a `magicbehavior` Nemesis patch + DAR
   `_CustomConditions\94010` files + `animationdatasinglefile` sections). Enable via the
   modlist-edit standing rule. Requirement DAR: this load order runs OAR, which reads DAR
   folders — verify the 94010 folder resolves (OAR log or cliplog on its clip names).
2. **Before launching**: tick `altmag` + `run-nemesis.ps1 -Tick altmag -Apply -UpdateEngine`
   (new file set), then read the merged `temp_behaviors/magicbehavior.txt` and the compiled hkx
   for every contended node — altmag patches `#0281` (MSCO's gated transition array), `#0088`,
   and ~30 more vanilla nodes in msco/pscd/sbeef/shtb territory; Nemesis resolves single-value
   contention last-checked-wins (measured, ticket 33). Produce the contention table FIRST; if a
   displacement would break MSCO's FF diversion or shtb's states, say so before the owner
   launches.
3. Owner feel cells (same fixtures as 55): enemy mage channel + FF casts; player equipped-hand
   both entries — establish whether Magelock's commitment covers the player at all (its title
   says NPC; its behavior patch is graph-wide; the truth is a measurement).
4. Instrument with the 54 recipe; diff against the known-good signatures.
5. Restore point: untick `altmag`, regen (selection-only change — no Update Engine).

## Kill criteria

- Unmodified Magelock cannot root an NPC channel here → the approach needs rework, not
  adoption; 57 re-specs with that evidence.
- It roots but breaks msco/shtb behavior (contention) → adaptation (own mod code, our node
  choices) becomes 57's default instead of adoption.

## Acceptance

- [ ] Contention table for every vanilla node altmag touches vs msco/pscd/sbeef/shtb, written
      BEFORE the first launch.
- [ ] Owner verdict on NPC channel + player cells, verbatim.
- [ ] Telemetry diff vs ticket 54 signatures stored under `.scratch/mco-integration/evidence/t56/`.
- [ ] Load order restored (or deliberately left, owner's call) at session end.
