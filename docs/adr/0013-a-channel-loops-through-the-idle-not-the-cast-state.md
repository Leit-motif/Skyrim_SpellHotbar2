# A concentration channel needs a held state; the idle-loop shape does not work

**Original premise withdrawn 2026-08-23, on an owner observation the same day.** The first
version of this ADR said the concentration loop is built by replacing the idle and locomotion
set, and that a channel therefore sustains itself by leaving the `shtb` cast state and falling
into the drawn idle. That was implemented, and the owner tested it: the fire-and-forget throw
animation plays and no concentration loop follows. The premise was wrong. What is written below
replaces it.

## What the concentration submods actually replace

Counted across all 24 concentration submods under
`meshes/actors/character/OpenAnimationReplacer/SpellHotbar2/`:

| submod | idle clips | shout clips |
|---|---|---|
| `cast_1h_left_conc` | 0 | 5 |
| `cast_1h_right_conc` | 0 | 5 |
| `cast_1h_left_conc_idle` | 9 | 0 |
| `cast_ritual_aimed_conc` | 12 | 5 |

22 of the 24 replace **shout** clips — `1hm_shout_inhale`, `mt_shout_inhale`,
`mt_shout_exhale`, and the sneak variants. `cast_1h_left_conc`, the plain aimed concentration
loop that Flames uses, replaces five shout clips and **no** idle clips at all. It cannot loop
through an idle it does not touch.

The `_idle` submods are a supporting layer, not the loop. They exist so that turning and
standing during a channel keep the casting pose instead of snapping to a normal idle. Only 13
of the 24 carry any, and the two that carry nothing else are exactly the ones the failed shape
depended on.

## Where the loop really comes from

**Vanilla's shout graph loops the inhale.** That is how a shout charges through three words
while the key is held: the inhale state holds. Upstream Spell Hotbar 2 drove concentration
through that graph (`ShoutStart`, hold, then a release event), so the channel sustained for
free and the mod never needed a looping clip generator of its own. Replace the inhale clip and
you have a concentration channel that lasts exactly as long as the player holds the key.

This is why there is no `MODE_LOOPING` generator anywhere in the mod, and the absence is not
evidence that the loop is built some other way — it is evidence that the loop was borrowed.

ADR-0006 retired that voice path, on three findings: this load order's shout-path clips
T-posed, MSCO's own entry events are combo-chain-only and unreachable from idle, and MCBO
animates only real hand casts. The concentration loop was collateral. Nothing in the fork
holds a state open for a hold any more, so no clip can loop, whichever clip it is.

Worth recording against a future attempt: mco-integration ticket 05 was **closed** on
2026-08-23 by owner ruling — *"this isn't relevant any more. no t-pose anywhere."* One of
ADR-0006's three reasons for retiring the voice path is therefore an observation the owner has
since contradicted. That does not reinstate the path, but it does mean the retirement should be
re-argued rather than assumed if concentration goes back to it.

## What this decides

A concentration channel needs a **state that is held for the hold**. An animation that plays
once and exits cannot represent a channel, and no arrangement of OAR submods changes that,
because OAR chooses which file plays, never how long a state lasts.

Two shapes remain open, and neither is chosen here:

- **Re-enter the shout graph for concentration only**, reaching the inhale loop the submods are
  already authored against. This reopens ADR-0006 for one cast type.
- **Give the `shtb` patch its own looping generator on the shout-inhale path**, so the state is
  the fork's but the clip is still the one OAR's concentration submods replace. No new
  animation asset; the patch is the fork's own file.

## What survives from the withdrawn version

Three things in the same change are independent of the loop mechanism and stand on their own
evidence:

- A channel does not walk the cast-combo index and does not open the follow-up-press window.
  Both are properties of a clip that is still playing. `CastShape` in `combo_cache.h` carries
  the distinction.
- An attack during a streaming channel ends the channel rather than swinging over the top of a
  beam that keeps running.
- The animation-family table in `skse_plugin/src/game_data/cast_anim_ids.h`: ritual
  concentration keeps `11001` in both slots instead of borrowing the plain dual concentration
  loop at `11003`, and ward keeps `1003` in both because no dual ward submod exists.
