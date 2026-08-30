# 18 -- Ship the stamped art clips

**Type:** task (packaging).

**Status:** ready-for-agent -- unblocked 2026-08-29 by the author's answer.

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

- [ ] The 57 stamped clips and their submods are committed and reach the release archive.
- [ ] The author is credited per their terms.
- [ ] **Ashes of War is listed as a requirement on the mod page**, and a comment in the packaging
      config records that the listing is a permission condition so nobody trims it later.

Ticket 16 built the stamped clips and they are live on this machine, which needed no permission
because copying locally is not redistribution. Shipping them does. The author's stated modification
permission allows released improvements with credit; that is the clause being confirmed.

~~When the answer lands: Yes / No branches.~~ The answer landed and it is yes. The "No" branch --
pointer-style, generated per machine -- is dead; do not restore it from history.

Still worth checking once: whether the owner's animation-load mod removed the duplicate-filter
problem entirely, which would make the stamping unnecessary rather than merely permitted.
