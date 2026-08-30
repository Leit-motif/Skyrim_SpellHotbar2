# 04 — Record the demo video

**Type:** task
**Status:** ready-for-agent

**Blocked by:** 03

A still frame cannot show this mod. Every claim on the page is about what happens between two
inputs — the weapon staying in the hand, the combo index surviving, the cast committing. The video
is the evidence and the GIFs (ticket 05) are cut from it.

Target: **40–60 s**, no voiceover, no music decision made here.

## No A/B, and no Equip or Oblivion mode on camera

**Owner ruling, 2026-08-29:** *"i only designed this for direct cast. i dont even know what equip
and oblivion mode are — and frankly, i dont care."*

The first version of this ticket opened on ~8 s of `Input::set_input_mode`'s Equip mode as a
same-session "before". Struck in full. Those modes are upstream's, inherited by our build and never
designed for or tested here, so filming one shows untested behaviour under our name — and it is not
base Spell Hotbar 2 either, so it would not even be an honest comparison.

**The video shows the product working and does not stage the alternative.** No before-and-after, no
disabled-first beat, nothing that argues the viewer had a problem. Open on shot 1 and let the
weapon staying in the hand do the work. Anyone reaching back through git history for the A/B
framing is reaching for a bug.

## Shot list

Ordered by what each shot proves, not by what looks best. Cut for length from the bottom.

| # | Shot | What it proves | Why video and not a still |
|---|---|---|---|
| 1 | **Swing → hotbar cast → swing. The weapon never leaves the hand and the combo continues at the same index** | Direct Cast plus combo-index restore (ADR-0014) | This is the opener and the money shot. It is the one thing no still frame can show, and it is why the video exists |
| 2 | A named weapon art fired from a slot, mid-combo | Abilities are hotbar actions (ADR-0011, weapon-arts spec) | The bind is invisible in a still; the swing is not |
| 3 | Left-hand cast while the right hand keeps the weapon | Per-hand presentation (ADR-0018) | Two hands doing different things at once |
| 4 | A concentration spell held from a slot, then released into an attack | Cast Channel and its chain-out (ADR-0013) | The hold has a duration |
| 5 | Cast, then try to walk out of it | Commitment (ADR-0015) | Rooting is a non-event in a still |
| 6 | Two presses inside the GCD, the second refused, the bar showing it | Press-anchored GCD, slot-strip tint tiers (weapon-arts 17, owner-accepted) | The sweep is animation |

Shots 1 and 2 carry the pitch. If the take has to be cut to 30 s, keep 1, 2 and 6.

Shot 1 goes first because it is the strongest thing the mod does, not because it sets up a
contrast. Do not hold it for a reveal.

## Setting

Open ground, clean horizon, forced clear weather. The readable thing is the silhouette mid-chain,
so open matters more than pretty — the tundra outside Whiterun over anything interior. A live
target is required (shots 1, 2 and 6 all need something to swing at), so place one rather than
shadowboxing; a viewer reads a swing at nothing as a test scene.

Keep the camera distance consistent across shots. A gallery of six framings reads as six sessions.

## Redeploy before recording

Confirm the enabled MO2 mod is the one that was just built, and that no older folder wins over it.
The sibling repo's recording rig throws with that specific warning rather than silently reading the
wrong deployment; do the equivalent check here.

## Acceptance

- [ ] A single take per shot, driven by `drive-input.ps1`, recorded by `record-demo.ps1` under the
      ticket-03 constraints.
- [ ] Shot 1's combo index is visibly continuous — the swing after the cast is not hit 1.
- [ ] No frame of the finished cut shows Equip or Oblivion mode.
- [ ] Master mp4 and per-shot clips committed under `.scratch/release/media/`, with the input
      script that produced each one, so a re-shoot is a re-run and not a re-derivation.
