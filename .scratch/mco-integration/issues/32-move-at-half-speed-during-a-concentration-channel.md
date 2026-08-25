# 32 — Move at half speed during a concentration channel

**Type:** spike, then feature (Nemesis patch + driver)

**Status:** needs-triage — **a preference, not a requirement** (owner, 2026-08-24). The spike
below is specified; the build depends on which route the owner takes, and the two routes are not
the same size of job. Weigh both against a want rather than a must.

**Blocked by:** None to spike. The build inherits [ticket 28](28-hold-a-looping-state-for-a-concentration-channel.md)'s
held state and re-opens [ADR-0013](../../../docs/adr/0013-a-channel-loops-through-the-idle-not-the-cast-state.md).

## What the owner asked for, 2026-08-24

> "for the concentration casts, i would like to open movement but at 50% movement speed."

And, asked directly how hard a requirement it is:

> "it isn't mandatory that concentration spells should allow movement at half speed, but it would
> feel better from a gameplay perspective because rooting for 1-2 seconds during an attack is fine,
> but being rooted for 6-10s channel would not feel great."

**That is the whole case, and it is a duration argument rather than a movement one.** A
fire-and-forget Driver Cast stays fully committed, as
[ticket 19](19-root-the-player-during-a-driver-cast.md) built it, and nobody minds — the clip is
over in 1.6 s. A channel runs as long as the key is held. Commitment that reads as weight at 2 s
reads as a loss of control at 8 s.

Two consequences for whoever picks this up. A route whose cost is out of proportion to a
quality-of-life want should be reported back rather than built. And a fix that shortens the felt
root without opening movement at all would answer the complaint as stated — worth a thought
before committing to either route below.

## Where the root actually comes from today — not where it looks like it comes from

`CastingInstanceSpellConcentration::blocks_movement()` returns `m_blocks_movement && !m_casted`
(`casting_controller.cpp:824`), and `m_blocks_movement` is the **dual-cast** flag at the
construction site (`casting_controller.cpp:940`). For a single-hand channel it is already
`false`. Turning it off changes nothing, because the root is coming from the other layer:

```
is_movement_blocking_cast()            // casting_controller.cpp:1298
  -> shtb_state_blocks_movement(MscoCastDriver::is_active() || ArtDriver::is_active())
```

and ticket 28's `SH2_CastChannel` holds that state open for the entire hold, so the WASD capture
in `input.cpp:468` swallows Forward/Back/Strafe for the whole channel. **A channel is rooted
today as a side effect of being held, not by a decision anyone recorded.**

## The blocker is the animation, not the movement code

`SH2_Channel_Clip` is one `MODE_LOOPING` `hkbClipGenerator` on `Animations\1HM_Shout_Inhale.HKX`
(`nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$30.txt`, and its twin in `magicbehavior`).
One static clip, no locomotion blend, no movement sub-machine. Stop capturing WASD and the
player translates with their feet planted — a slide, not a walk. That is the reason the current
root cannot simply be deleted, and it is the whole of this ticket's difficulty. The 50% itself
is the easy half.

**The clips for moving while channeling already exist and are currently dead weight.** The plain
concentration submod ships five files:

```
cast_1h_left_conc/animations/  1hm_shout_inhale.hkx   mt_shout_inhale.hkx   mt_shout_exhale.hkx
                               sneak1hm_shout_inhale.hkx   sneak1hm_shout_exhale.hkx
```

`mt_shout_*` are the movement-type shout clips — the ones vanilla plays when you shout *while
walking*. Upstream Spell Hotbar 2 ran concentration through the vanilla shout graph, where those
clips are reachable, which is exactly why the author authored them. The fork's held state only
ever plays `1HM_Shout_Inhale`, so four of the five files never play. The `_idle` submod's nine
clips (including `mt_idle.hkx`) cover the standing-and-turning case in the same way.

## The two routes

Both are answers to the same requirement: **movement during a channel needs a blend, not a
state.** That is the general shape of it — vanilla lets you walk while hand-casting a
concentration spell because vanilla magic casting is an upper-body layer over locomotion, and it
lets you walk while charging a shout because the shout machine carries its own `mt_` movement
set. A root-level state playing one clip has neither, which is where the fork's channel sits.

**A — drive concentration back through the vanilla shout graph.** ADR-0013's first open shape.
The graph blends locomotion natively and picks `mt_shout_inhale` on its own; nothing new is
authored. It re-opens [ADR-0006](../../../docs/adr/0006-own-nemesis-state-with-a-timer-floor.md)
for one cast type — and ADR-0013 already flags that one of ADR-0006's three reasons (T-posed
shout clips) is an observation the owner contradicted on 2026-08-23, so the retirement is meant
to be re-argued rather than assumed. It also puts concentration back on the graph ShoutMCO is
holding a root over, which is the collision to think about before starting.

**B — give the `shtb` channel state its own locomotion blend.** A blend generator or nested
state machine inside the held state, selecting `1hm_shout_inhale` against `mt_shout_inhale` on
the movement variables. The state stays the fork's and ADR-0006 stands. This is a larger Nemesis
authoring job than any patch this fork has written — every existing `shtb` state is one clip
generator and a transition.

**Recommendation: spike A first.** It is the shape the assets were authored for, and its risk is
a decision to re-argue rather than an unknown to discover. B is the fallback if the shout-graph
collision with ShoutMCO's root turns out to be real.

### The spike, either route

Smallest thing that answers it: **can the player walk during a channel with legs that move?**
Drive one channel on the chosen route, hold a movement key, and read the OAR Animation Log for
which clip wins plus `FootLeft`/`FootRight` cadence from the event stream. A frame is not
enough — a slide and a walk look the same in a still.

## The 50%, once movement is open

Apply a `SpeedMult` slow for the hold and drop it at `end_channel` / `on_reset` / cancel, on the
same edges that already tear the channel down (`casting_controller.h`, `CastingInstanceSpellConcentration::end_channel`).

- **Prefer a magic effect** — a fork-owned MGEF applied at channel start and dispelled at the
  end — over a raw actor-value write. A raw `SpeedMult` change is known not to take effect until
  the actor's movement data is refreshed, and it leaks if the channel dies on a path that misses
  the clear. Verify the refresh behavior rather than assuming either way.
- **"50%" means −50 points of `SpeedMult`**, which is half speed for an unbuffed player and less
  than half for a buffed one. Say so out loud; a true proportional halving would have to re-read
  the value and would fight anything else modifying it mid-channel.
- A ritual (two-handed) concentration keeps its full root —
  `CastingInstanceSpellRitualConcentration::blocks_movement()` returns `!m_casted`
  (`casting_controller.cpp:1402`) and nothing here changes it. Unless the owner says otherwise,
  the half-speed rule is the single-hand channel only.

## Acceptance

- [ ] A single-hand concentration channel translates the player while held, with the moving
      concentration pose playing — named clip from the OAR Animation Log, plus footfall cadence.
- [ ] Measured translation over a fixed hold is half the unchannelled rate for the same input.
- [ ] The slow is gone on every end path: key release, an attack cut
      (`cut_channel_for_attack`), an interrupt, and a load. No `SpeedMult` residue in the save.
- [ ] A fire-and-forget Driver Cast is still fully rooted — ticket 19 unregressed.
- [ ] Ritual concentration is still fully rooted.
- [ ] Evidence names the commit, the DLL hash, the save, and the profile.

## Scope note: NPCs

Nothing in this ticket reaches an NPC. The WASD capture reads the player's own input dispatch,
and no NPC is ever sent an `SH2_CastChannel` notify, so no NPC enters the state.

The wider picture, per the owner 2026-08-24: **NPC casts are rooted by MSCO, and MSCO does not
cover concentration.** So an NPC channelling is unrooted today, by omission rather than by
decision, and this ticket does not change that either way. If NPC concentration commitment is
ever wanted it is MSCO's, not this fork's.
