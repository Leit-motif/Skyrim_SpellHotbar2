# 40 — Stepping fire-and-forget cast animations

> Renumbered from 39: that number went to `39-rooting-should-block-input-not-lower-body-animation.md`,
> filed off the owner's acceptance pass. If ticket 39 finds the MSCO clips already carry stepping
> and SH2's graph or a DLL holdover suppresses it, this ticket dissolves — no new assets needed.

**Type:** enhancement (animation assets)

**Status:** resolved — dissolved 2026-08-29 by ticket 39's live outcome, exactly the
condition the header names. Ticket 39 removed the `bAnimationDriven` plant from the four
fire-and-forget cast states and the owner confirmed live that "legs animate" — SH2's graph
was suppressing motion the clips do carry, so no new assets are needed. The other half of
this ticket's premise (consuming the animmotion glides the character) fell with it: with
legs animating, the traverse is authored motion, and the owner asked for it back. The
one-line revert this ticket documented was applied the same evening
(`skse_plugin/src/casts/clip_translation_driver.cpp`, commit `88cff9c` on ticket 39's
branch): the four `SH2_Cast*_Clip` names are back in the motion filter and `apply()` arms
during a driver cast. Verified live: `bound SH2_CastRight_Clip (4 animmotion keys)`, ~5.7
units of front-loaded XY traverse through the cast. The "no stepping in the leg tracks"
claim recorded below did not survive the owner's eyes; kept as history.

**Blocked by:** nothing — closed.

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
