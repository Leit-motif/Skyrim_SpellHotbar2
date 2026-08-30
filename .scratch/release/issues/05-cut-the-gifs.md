# 05 — Cut the GIFs

**Type:** task
**Status:** ready-for-agent
**Blocked by:** 04

GIFs are not a fallback for the video, they are a different job: the video is watched by people
already interested, the GIFs are seen by people deciding whether to be. They go inline in the
description body next to the claim each one proves, which is how a skimming reader ends up
convinced without pressing play.

## What to cut

One per claim, from the ticket-04 master. Three or four total, not seven — a page of GIFs is a
page nobody reads.

1. **Shot 1, the mid-combo cast.** Non-negotiable. Sits next to the Direct Cast paragraph.
2. **Shot 2, the weapon art from a slot.** Sits next to the Abilities section.
3. **Shot 6, the GCD refusal and the slot tint.** Sits next to the cooldown paragraph, and does
   the job a sentence about tint tiers cannot.
4. Optional: **shot 3, per-hand casting.** Weapon right, spell left, in one loop.

**No A/B GIF.** An earlier version of this ticket offered a before-and-after loop against Equip
mode. Struck with the owner's 2026-08-29 ruling — those modes are upstream's and untested here, and
ticket 04 stages no alternative at all.

## Constraints

- **Crop to the action and the bar.** A 3440x1440 frame scaled to a description-column width makes
  the hotbar icons illegible, which defeats the point. Crop so the bar occupies real pixels.
- Two to four seconds each, looping cleanly. A GIF that stutters at the loop reads as broken.
- Check the file size the description embed tolerates before generating final ones; a 12 MB GIF is
  a slower page than no GIF.
- Consider webm/mp4 loops if the page will take them — same clip, a fraction of the weight and no
  colour banding. Check what the upload form actually accepts rather than assuming GIF.

## Acceptance

- [ ] Three to four loops committed under `.scratch/release/media/`, each named for the claim it
      supports.
- [ ] Each one is legible at the description column's real width, checked at that width and not at
      full size.
- [ ] No burned-in captions.
