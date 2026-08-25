# 39 — Stepping fire-and-forget cast animations

**Type:** enhancement (animation assets)

**Status:** parked — needs new animation assets, same wall as tickets 32 and 34

**Blocked by:** asset procurement or authoring; owner decision on source.

## The ideal the owner named

2026-08-25: fire-and-forget casts should move the feet when the body translates, instead of
rooting (ticket 38's fallback). The MSCO clips (`MSCO_left1`–`4`) carry `animmotion` root
translation but no stepping in the leg tracks, so consuming the motion glides the character.

## What unparking takes

Cast animations whose leg tracks actually step through the lunge — sourced from an animation
pack with permission, or authored (Blender-grade keyframing; not agent work). SH2 already owns
the machinery to play them: the shtb states name the clip files, OAR picks the winner, and
ticket 38 leaves `ClipTranslationDriver` able to consume `animmotion` again by re-adding the
cast clips to the motion filter — a one-line revert of 38's filter plus the `apply()` gate arm.

## Comments
