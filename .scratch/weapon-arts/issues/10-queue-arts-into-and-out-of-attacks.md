# 10 — Queue Weapon Arts into and out of attacks

A bound ash currently needs a full stop to idle before it will play, and it does not chain into
the next attack. Owner playtest 2026-08-19 (ticket 07): “ashes of war don't combo into and out of
attacks… ideally we give it a bit of queuing.”

**Blocked by:** 01 (resolved) — idle play already works

**Status:** needs-triage

## You would test this

Profile `Nolvus Awakening`. Weapon drawn, a named ash bound.

1. Press the art **during an MCO swing** (not idle). The art starts at a sensible window (recovery
   / hit / similar), not only after a full return to idle.
2. After the art clip, left-click attack continues the chain in a way that feels like MCO, not a
   hard reset that requires standing still first.

If the only reliable trigger is still “completely stop all attack state (idle)”, it fails.

## What this is

**Queue / combo membership for arts**, analogous to SH2 casts riding MCO (see
`.scratch/mco-integration/issues/04-hold-hotbar-input-during-an-mco-swing.md` and
`20-chain-a-hotbar-cast-in-during-mco-recovery.md`) but for the art selector path, not Driver Cast.

## What this is not

Not Art Class gray-out (07, resolved). Not icons (06). Not Custom Art Folders (08). Not a request
to copy Ashes-of-War fall-through.

## Notes

Ticket 01 acceptance is **drawn idle only**, and “next left-click is combo hit 1” after an art.
This ticket is the mid-swing / recovery queue and the out-of-art attack chain the owner asked for
as a follow-up.

Needs a grill on window (HitFrame vs recovery vs WinClose), whether arts restore `MCO_nextattack`,
and whether queue is press-and-hold vs one-shot.

## Comments

Filed from ticket 07 close-out. Owner: not for work in that session; capture only.
