# 02 — Package the archive

**Type:** build
**Status:** ready-for-agent

**This ticket does not restate the work.** It is owned in full by
`../mco-integration/issues/59-release-packaging-ships-no-nemesis-tree.md`, which carries the
package contents, the overwrite reasoning, the version-coupling cost, and the acceptance list.

Tracked here because it is the one release ticket that does **not** wait on ticket 01. The archive
contents do not change with the SMF ruling: SMF changes which UI hosts the settings, not which
files ship.

## What this ticket adds to 59

- A version stamp inside the archive that names **our** build, not upstream's `2.0.10`. Ticket 01
  picks the scheme; 59's build script writes it.
- The archive filename and the MO2 folder name must match the public name from ticket 01. The
  sibling repo shipped artifacts named for a title it had already changed, and renaming them
  afterwards touched the live playtest profile.

## Acceptance

- [ ] Ticket 59's acceptance list passes.
- [ ] The archive's version stamp is ours, and appears in both the README and the DLL's own
      version resource.
- [ ] Archive filename and MO2 folder name match ticket 01's recorded public name.
