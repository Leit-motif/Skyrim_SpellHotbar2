# 09 — Upload session

**Type:** task
**Status:** ready-for-agent
**Blocked by:** 01, 02, 04, 05, 06, 07, 08

The owner's action, start to finish. This ticket is the checklist they work from, and it is the
only place in this effort where "the owner has playtested" appears as acceptance.

## The playtest gate

One playthrough stands between complete and published. Agents build to UAT and do not park work
pending owner feedback, so no other ticket in this effort may carry an owner-playtest cell. This is
that cell.

- [ ] One playthrough on the packaged archive — not the dev deployment — installed the way the
      page tells a user to install it, base mod first, Nemesis run.

## Before the form

- [ ] The archive is cut from a clean tagged commit and its version stamp matches the tag.
      **Read the version out of the build rather than trusting this ticket**; the sibling repo's
      checklist said 1.0.1 while the source had moved to 1.0.3.
- [ ] Public source pushed and reachable, if ticket 01 ruled that it must be. Source correspondence
      is the commit plus the build recipe, not a reproducible hash — the builds are not byte
      reproducible and no user needs to reproduce them.
- [ ] `nemesis/Nemesis_Engine/mod/shtb/info.ini` and `mod/shcr/info.ini` carry no real name.
      Nemesis shows that field in its own mod list.
- [ ] The DLL's version resource does not stamp a copyright line that contradicts the licence in
      the same archive.

## The form

- [ ] Category and runtimes per ticket 01.
- [ ] Requirements entered by Nexus id, with base Spell Hotbar 2 first. Off-site dependencies by
      name and URL.
- [ ] Permission fields set consistently with the licence inside the archive. A "no" that
      contradicts the shipped LICENSE puts the page at odds with itself.
- [ ] Main image (06), header banner (06), gallery (07), each gallery image given its caption in
      the caption field rather than burned into the pixels.
- [ ] Video attached or linked (04).
- [ ] AI content flag set honestly, per the wording the form actually uses.

## Day one

- [ ] Sticky comment naming the overwrite coupling and the pinned base version. That is the one
      thing a user cannot discover from the page fast enough to avoid the support ticket.
- [ ] Re-check the base mod's page afterwards. An upstream release silently reverts under our
      overwrite, and we will not be told.
