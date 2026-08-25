# The Ability art selector is a behavior-graph variable, not an ESP record

Date: 2026-08-24

Status: accepted

## Context

ADR-0008 settled that one selector value picks the Ability art, rather than a behavior path per
art. The value had to live somewhere both this mod's DLL and Open Animation Replacer could see.
It was implemented as `SpellHotbar_ArtSelector`, a `GlobalShort` read as form `0xD63` in
`SpellHotbar.esp`, with every generated OAR submod gating on `CompareValues` against that form.

That form is not in Spell Hotbar 2. Upstream's repository tracks no plugin file at all — 2761
files, no `.esp` in any form, binary or serialized — and its source never references `ArtSelector`
or `0xD63`; the highest form it loads from its own plugin is `0x850`. The record exists only in a
hand-edited copy of upstream's `SpellHotbar.esp` on the author's machine: the stock plugin in the
0.0.14 release archive is 11105 bytes, the installed one 11142, dated months later while its
sibling from the same archive kept its original date.

So the shipped art pack gated on a record no user would have. The failure is quiet by
construction: `load_form_from_game` logs one error, leaves the pointer null, and every call site
null-guards it, so on a clean install abilities simply never animate and nothing says why.

Three ways out were weighed. Redistributing the patched plugin is the reupload the upstream author
steered away from. Shipping an addon ESP with `SpellHotbar.esp` as master works but adds a plugin
to the load order for one integer. Asking upstream to add the record cannot be a pull request at
all, because the plugin is not in the repository — it could only ever be an issue against a future
release, leaving the feature broken until he acted.

## Decision

The selector is a **behavior-graph variable**, `SH2_ArtSelector`, declared by this mod's own
`shtb` Nemesis patch and written by the DLL. No ESP record, and no dependency this mod does not
already have.

OAR's built-in `CompareValues` accepts a graph variable as an operand, so no OAR addon and no
custom condition is needed either. Generated submods carry:

```json
"Value A": { "graphVariable": "SH2_ArtSelector", "graphVariableType": "Int" }
```

`generate_art_pack.py` emits that form, and it is the shape that matters — the generator is the
source, the per-machine packs are its output.

**The variable is declared in both `0_master` and `1hm_behavior`.** Declaring it only in the root
is not enough. ADR-0014 established that a nested graph keeps its own variable storage and links
to the root's by name; the art clip activates inside `1hm_behavior`, so the name has to exist on
both sides of that boundary for the write to be visible where the condition is evaluated.

An SKSE-registered custom OAR condition was the other candidate and was rejected. It would have
meant vendoring GPL-3.0-with-modding-exception headers into a plugin tree licensed MIT by
upstream, and OAR broke its condition API in both 3.0.0 and 3.1.0 — each break requiring a
recompile or the condition degrades to `! INVALID !` and art silently stops. The graph variable
depends on nothing beyond Nemesis, which this mod already requires.

## Consequences

- Adding or renaming the variable edits a base `#NNNN.txt` in the `shtb` patch, which invalidates
  the Nemesis engine cache. That needs Update Engine before Launch, not just a re-run.
- The three declaration arrays (`#0106`/`#0107`/`#0108` in `0_master`, `#0085`/`#0086`/`#0087` in
  `1hm_behavior`) are index-aligned by position. A future variable must be appended to all three
  or the graph's variable table is silently misaligned.
- The art pack is generated per machine from the user's own installed Ashes of War mods, so the
  generator and `arts_ashes.csv` are the source and the submod folders are output. A release ships
  the generator's rules, not one machine's pack.
- Verification of art selection is by fingerprint, not by inspection: each art's clip has its own
  annotation profile (`hkxc-anno-cli dump`), and the runtime's bound-clip key count and
  WinOpen/HitFrame flags must match the art that was requested. Identical fingerprints across
  different arts mean the selector is not steering, whatever the logs say about the value.
- The DLL keeps an in-process copy of the selector for the clip-translation path, which reads it
  off the animation thread. The graph variable is the half OAR sees; the atomic is the half this
  mod sees. Both are written together.
