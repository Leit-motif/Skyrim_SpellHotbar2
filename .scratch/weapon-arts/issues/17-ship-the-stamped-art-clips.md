# 17 -- Ship the stamped art clips once the author answers

**Type:** task (packaging), blocked on an external answer.

**Status:** blocked -- waiting on the Ashes of War author's permission, requested 2026-08-25.

**Blocked by:** that answer. Nothing technical.

Ticket 16 built the stamped clips and they are live on this machine, which needed no permission
because copying locally is not redistribution. Shipping them does. The author's stated modification
permission allows released improvements with credit; that is the clause being confirmed.

When the answer lands:

- **Yes** -- commit the 57 stamped clips and their static submods (~7.5 MB), credit the author per
  their terms, and the shipped pack stops being generated per machine. `python_scripts/stamp_art_clips.py`
  produces them; `--verify` checks the result.
- **No** -- the pack stays pointer-style and generated, and the duplicate collision stays a known
  risk. Ticket 16's local fix remains valid for this machine only.

Check first whether the owner's animation-load mod has removed the problem entirely, in which case
neither branch matters.
