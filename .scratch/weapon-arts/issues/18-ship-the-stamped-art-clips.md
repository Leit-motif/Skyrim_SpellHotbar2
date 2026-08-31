# 18 -- Ship the stamped art clips

**Type:** task (packaging).

**Status:** DONE 2026-08-31 -- the clips are committed and verified inside the archive. One
owner cell remains: confirm in game that weapon arts still fire from a hotbar slot.

**PERMISSION GRANTED 2026-08-29. The author answered.** Owner: *"The author of Weapon Arts did
respond to me and said as long as I list Ashes of War as a requirement, I'm free to integrate the
mod, including how we are renaming the file."*

Integration is permitted, the rename is permitted, and there is one condition: **Ashes of War must
be listed as a requirement.** That listing is a permission term, not a courtesy — it never comes off
the page. Do not re-raise, re-verify, or caveat anything with this again.

**Status (superseded — see the top):** ~~deferred 2026-08-26 by owner ruling -- the author has not responded and the owner is
operating as if permission will not come.~~ **STRUCK.** That deferral, and every downstream claim
that the pack must stay pointer-style, is dead. It caused rework in the release effort by being read
as current.

## What to do

Ticket 16's **"Yes" branch is now the live one**: commit the 57 stamped clips and their static
submods (~7.5 MB), credit the author per their terms, and the shipped pack stops being generated per
machine. `python_scripts/stamp_art_clips.py` produces them; `--verify` re-checks on disk.

## Acceptance

- [x] **The 57 stamped clips and their submods are committed and reach the release archive.**
      Copied from the verified MO2 pack into `data/.../SpellHotbar2Arts/`, byte-identical across
      all 57, 57 distinct hashes, every submod carrying its `config.json`. The archive grew from
      156 files / 2.95 MB to **270 files / 7.91 MB**, and re-reading the zip confirms 57 clips,
      57 distinct hashes, zero byte mismatches, and the pack-level `config.json` present.
- [x] **The author is credited.** `(SE) Ashes of War Weapon Art Via Additional Attack` by
      **Gild** (mod 100174), named in the page's Credits.
- [x] **Ashes of War is listed as a requirement on the mod page**, and
      `deploy/release/release.json` carries `_requirements_comment` recording that the listing is
      a permission condition, next to the data the packaging build actually reads.
- [ ] **Owner cell.** Bind a weapon art and fire it from a hotbar slot. The shipped pack is the
      same bytes the machine has been running on, so nothing should change -- but the DLL now
      self-skips generation because the pack arrives populated, and that path is what a user gets.

## Two things found on the way

**`.gitattributes` did not exist, and `core.autocrlf` is true.** The 32 MSCO clips already tracked
survived on git's NUL-byte auto-detection alone. Adding 57 more binaries on that basis was a bad
bet, so `*.hkx binary` is now pinned explicitly. Scoped to `.hkx` deliberately: the packaging build
*depends* on Nemesis `.txt` files still getting CRLF from autocrlf, so a global `text=auto` would
have broken the thing it was meant to protect. Staging the 57 clips was then proved byte-exact
against the working files.

**The pack-level `config.json` description was stale.** `build_pack_config_json()` in
`art_pack_gen.h` still says *"Animation files stay in the author's folders"*, which was true of the
pointer-style pack and is false now. The shipped copy says what the pack actually contains. The
generator's string is untouched because generation only runs when the pack arrives empty, which no
longer happens for a released install -- worth tidying, not worth a behavior change here.

Ticket 16 built the stamped clips and they are live on this machine, which needed no permission
because copying locally is not redistribution. Shipping them does. The author's stated modification
permission allows released improvements with credit; that is the clause being confirmed.

~~When the answer lands: Yes / No branches.~~ The answer landed and it is yes. The "No" branch --
pointer-style, generated per machine -- is dead; do not restore it from history.

~~Still worth checking once: whether the owner's animation-load mod removed the duplicate-filter
problem entirely, which would make the stamping unnecessary rather than merely permitted.~~ **Not
checked, and deliberately not chased.** The stamped clips are verified working on this machine and
in the archive; establishing that a different mod makes them unnecessary would be an optimisation
against a shipping deadline, and dropping them later is a one-commit revert if it ever proves out.
