# 16 — Ship byte-unique art clips instead of pointing at the author's

Arts drop at random — one or two per launch, a different one each time — because our submod and a
stance submod point at the same bytes and OAR's duplicate filter keeps only one. Own the bytes and
the collision cannot happen.

**Blocked by:** the Ashes of War author's permission to redistribute stamped copies of the 57 art
clips. Requested 2026-08-25. **The local test below needs no permission** — copying and modifying
files on one machine is not redistribution — so the mechanism can be proven while the answer is
outstanding.

**Status:** ready-for-agent for phase 1 (the local proof). Phase 2 (ship it) waits on the answer.

**Settles:** [ticket 15](15-double-slash-plays-no-clip.md) for the shipped set.
**Decision:** [ADR-0017](../../../docs/adr/0017-the-shipped-art-pack-carries-its-own-clip-bytes.md).

## Why this and not the alternatives

Read ADR-0017 for the full argument. The three ideas it rules out, so nobody re-proposes them:

- **Renaming our copies.** The filter hashes content, not names. And OAR binds a replacement to
  its base animation by path, so the file must stay `AABL_Attack_A.hkx` regardless.
- **Turning `bFilterOutDuplicateAnimations` off.** Nolvus enables it deliberately — 8104
  animations collapsed in `DefaultMale` on one launch — and shipping cannot dictate a user's ini.
  One flip is a diagnostic; it is never the fix.
- **Fixing it in the generator.** The generator points; pointing is the problem. It stays for
  content we do not ship (see phase 3).

## Phase 1 — prove the mechanism locally, no permission needed

1. Copy each of the 57 art clips out of the author's folders into our own pack submods, replacing
   `overrideAnimationsFolder` with a real `AABL_Attack_A.hkx` in each submod.
2. Stamp each copy with one appended annotation — a namespaced name nothing registers for, e.g.
   `SH2_PackStamp` — so its hash differs from the original. `hkxc-anno-cli` reads and writes these,
   and `custom_ability_runtime` already drives an `update -a` pass over `.hkx` at runtime, so the
   tooling exists.

   **The stamp must be unique per submod, not one shared token** (measured 2026-08-25).
   Two of our own arts resolve to the same author file -- `Pirate's Slash` and `Wind Slice`
   are byte-identical -- so a single shared `SH2_PackStamp` would leave those two identical
   to each other and the filter would still collapse them. The stamp carries the submod's
   name: `SH2_PackStamp_<slug>`.
3. **Confirm the stamp is inert before trusting the result.** Annotations surface as animation
   events. Drive one art and read `SpellHotbar2.log` for anything reacting to the stamp; the
   graph-event trace in `animationeventhook.cpp` only prints tags this integration matches, so
   also confirm no behavior change against the same art's pre-stamp run.
4. Verify no two shipped clips share a hash after stamping, and that each differs from the
   author's original.

### Acceptance — live only

- [ ] Every one of the 57 arts binds a real clip across **three consecutive launches**, with the
      animmotion key count matching that art's own `hkxc-anno-cli` dump. No `latch 2
      (winopen=false hitframe=false)`, no "activated with no animmotion keys", no ~6 ms exit.
- [ ] The stance movesets still play normally — this must not win their clips away from them.
- [ ] The stamp raises no event any consumer reacts to.

Three launches is the bar because ticket 15's defect hides on any single launch: it takes one or
two arts at random, so a clean run proves nothing on its own.

## Phase 2 — ship it, once the author agrees

Commit the stamped clips and the static submods. The shipped pack stops being generated: 57
submods, byte-identical on every machine, no scan and nothing to resolve at load. Credit the
author per their modification permission. Roughly 7.5 MB.

## Phase 3 — the generator becomes the fallback

`art_pack_gen` (commit `d70cc09`) keeps its job for Ashes of War-style content we do **not** ship:
another pack a user installs, or arts added later. Those stay pointer-style, with the duplicate
collision as a known and now-understood risk. Its guard already does the right thing here — it
skips generation when a populated `SpellHotbar2Arts` exists, which after phase 2 is always true
for the shipped set.

Its own unresolved defect rides along: the generated pack has never registered with OAR, on any of
three launches, while the byte-identical shipped pack worked. If phase 1 clears that up, the
duplicate filter was the cause of both. If phase 1 passes and the generator still fails, they were
two problems and the generator's next step is OAR's in-game replacer-mod list — not another
restart bisect, which produced no convergence across five cycles.
