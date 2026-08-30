# 04 — Record the demo video

**Type:** task
**Status:** ready-for-agent
**Blocked by:** 03

A still frame cannot show this mod. Every claim on the page is about what happens between two
inputs — the weapon staying in the hand, the combo index surviving, the cast committing. The video
is the evidence and the GIFs (ticket 05) are cut from it.

Target: **40–60 s**, no voiceover, no music decision made here.

## The A/B is the spine, and it is a runtime flip

`Input::set_input_mode` takes Cast (0), Equip (1), Oblivion (2), and Equip mode is exactly the
equip-first behaviour Direct Cast replaces (`skse_plugin/src/input/modes.cpp:31`). So the "before"
and the "after" are the same save, the same light, the same camera and the same combo, seconds
apart, differing by one setting. Two separate sessions could never match that and a viewer would
be right not to trust them.

Open the take on ~8 s of Equip mode so the contrast is established before anything new is shown.
Do not caption it. The hands swapping and the combo dropping read on their own.

## Shot list

Ordered by what each shot proves, not by what looks best. Cut for length from the bottom.

| # | Shot | What it proves | Why video and not a still |
|---|---|---|---|
| 1 | Equip mode: hotbar press mid-combo. Weapon goes away, spell comes up, combo resets | The problem | The swap is the whole point and it is three frames |
| 2 | **Cast mode: swing → hotbar cast → swing, weapon never leaves the hand, combo continues at the same index** | Direct Cast plus combo-index restore (ADR-0014) | This is the money shot. It is the one thing no still frame can show, and it is the reason the video exists |
| 3 | Left-hand cast while the right hand keeps the weapon | Per-hand presentation (ADR-0018) | The two hands doing different things at once |
| 4 | A named weapon art fired from a slot, mid-combo | Abilities are hotbar actions (weapon-arts spec, ADR-0011) | The bind is invisible in a still; the swing is not |
| 5 | A concentration spell held from a slot, then released into an attack | Cast Channel and its chain-out (ADR-0013) | The hold has a duration |
| 6 | Cast, then try to walk out of it | Commitment (ADR-0015) | Rooting is a non-event in a still |
| 7 | Two presses inside the GCD, second one refused, the bar showing it | Press-anchored GCD, slot-strip tint tiers (ticket 17) | The sweep is animation |

Shots 2 and 4 carry the pitch. If the take has to be cut to 30 s, keep 1, 2, 4 and 7.

## Setting

Open ground, clean horizon, forced clear weather. The readable thing is the silhouette mid-chain,
so open matters more than pretty — the tundra outside Whiterun over anything interior. A live
target is required (shots 1, 2, 4 all need something to swing at), so place one rather than
shadowboxing; a viewer reads a swing at nothing as a test scene.

Lock the camera distance across the A/B pair. If shot 1 and shot 2 are framed differently the
comparison is worthless.

## Redeploy before recording

Confirm the enabled MO2 mod is the one that was just built, and that no older folder wins over it.
The sibling repo's recording rig throws with that specific warning rather than silently reading the
wrong deployment; do the equivalent check here.

## Acceptance

- [ ] A single take per shot, driven by `drive-input.ps1`, recorded by `record-demo.ps1` under the
      ticket-03 constraints.
- [ ] The A/B pair (shots 1 and 2) is the same save, same camera, same weapon, differing only by
      the input mode, and the flip is a runtime call rather than a restart.
- [ ] Shot 2's combo index is visibly continuous — the swing after the cast is not hit 1.
- [ ] Master mp4 and per-shot clips committed under `.scratch/release/media/`, with the input
      script that produced each one, so a re-shoot is a re-run and not a re-derivation.
