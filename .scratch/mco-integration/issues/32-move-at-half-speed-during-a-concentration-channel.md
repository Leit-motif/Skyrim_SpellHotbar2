# 32 — Move at half speed during a concentration channel

**Type:** spike, then feature (Nemesis patch + driver)

**Status:** claimed — **UN-PARKED by owner ruling, 2026-08-28.** Two decisions taken the same
day: the blend goes **route B** (keep ticket 28's shipped held state, author the locomotion
blend inside it — route A is dead: it would un-ship an accepted feature, re-open ADR-0006, and
collide with ShoutMCO's root), and the **uniform half-speed rule is revived** (ticket 33 flips
back to the conditioned-`SpeedMult`-record shape; its root-everyone build is not written). The
owner's working model of the blend: the inhale pose layered over the live locomotion set per
stance — legs come from the ordinary walk/run clips, nothing new authored. Plan at
`C:\Users\Rando\.claude\plans\stateless-weaving-papert.md`.

**Stale-section warning:** the "Where the root actually comes from today" section below predates
ticket 35 (`c73b4f1`), which deleted the WASD capture and the whole `blocks_movement()` chain.
Rooting is graph-only now: the `bAnimationDriven` modifier pairs on the channel state
(`#shtb$34/$35` in `1hm_behavior`, `#shtb$29/$30` in `magicbehavior`).

Original parking record, kept for history:

> parked — **TABLED by owner ruling, 2026-08-24, hours after being upgraded to a
> requirement.** On learning the path here requires new animation assets: "movement is out of
> scope. As long as both players and NPCs are rooted during concentration casts, I think we're
> good to proceed, and we can table the movement and blend work for a future endeavor."

Concentration therefore ROOTS for every actor — [ticket
33](33-commit-npc-concentration-casts.md) carries that, and the player's channel is already
rooted today by the held `shtb` state. Everything below — the duration rationale, the two blend
routes, the route-A recommendation, the conditioned-slow mechanism pointer — is the future
endeavor's starting brief, preserved as written. It was a requirement for a few hours and is a
want again; the intermediate rulings live in ticket 33's history section and ADR-0015's two
amendments. Ticket 34 (movement ritual animations) parks alongside this.

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

## The 50% — superseded in mechanism by ADR-0015's amendment, same day

The paragraphs below predate the uniform-rules ruling and describe an SH2-applied effect. The
ruling moves the slow to **one conditioned ability record shared with NPCs** ([ticket
33](33-commit-npc-concentration-casts.md) owns it): `SpeedMult` −50 conditioned on
concentration-casting, so the player's hotbar channel, the player's equipped-hand channel, and
the enemy mage all read the same number from the same record. This ticket keeps only what is
player-specific: the animation blend (the routes above) and releasing the WASD capture for a
channel. The magic-effect preference and the −50-points caveat below carry over to ticket 33's
record unchanged.

## The 50%, once movement is open (superseded — see above)

Apply a `SpeedMult` slow for the hold and drop it at `end_channel` / `on_reset` / cancel, on the
same edges that already tear the channel down (`casting_controller.h`, `CastingInstanceSpellConcentration::end_channel`).

- **Prefer a magic effect** — a fork-owned MGEF applied at channel start and dispelled at the
  end — over a raw actor-value write. A raw `SpeedMult` change is known not to take effect until
  the actor's movement data is refreshed, and it leaks if the channel dies on a path that misses
  the clear. Verify the refresh behavior rather than assuming either way.
- **"50%" means −50 points of `SpeedMult`**, which is half speed for an unbuffed player and less
  than half for a buffed one. Say so out loud; a true proportional halving would have to re-read
  the value and would fight anything else modifying it mid-channel.
- Ritual (two-handed) concentration: **RESOLVED 2026-08-24 — rituals stay fully rooted, out of
  this ticket's scope.** Owner: "i think it was intended that ritual spells should be rooted and
  it's less of a headache for us now." The evidence agrees on both halves: upstream authored the
  root deliberately (`CastingInstanceSpellRitualConcentration::blocks_movement()` returns
  `!m_casted`, `casting_controller.cpp:1402`), and the `cast_ritual_aimed_conc` submod replaces
  idles, turn-in-place clips, and shout clips — **no walk or run clip anywhere in it**. The
  author built a caster who plants and pivots. This is a per-action-class rule, so it holds for
  every actor and does not breach the consistency ruling. A future movement-designed ritual set
  (floating/hover) is [ticket 34](34-ship-movement-ritual-animations.md), parked.

## Spike result 2026-08-28 — layered blend confirmed; two ticket premises corrected

Full evidence: [spike-vanilla-shout-blend.md](../evidence/t32/spike-vanilla-shout-blend.md).
The verdict: vanilla walks while shout-charging (and while casting) via a **bone-weighted
layer** — the same static inhale pose on the upper body, an ordinary locomotion sub-graph
underneath. The new-assets wall this ticket was tabled against does not exist for that route.
The minimal build is three new nodes plus one repointed generator per graph
(`BSBoneSwitchGenerator` with `spBoneWeight` bound to the character property `UpperBody`,
default generator = the graph's existing standing/locomotion machine, Nemesis `#4184` in
`1hm_behavior` / `#1007` in `magicbehavior`), plus dropping the `bAnimationDriven` binding from
the channel's own binding sets (`#shtb$33` / `#shtb$28`) — the modifier, not the clip, is what
plants the feet.

Two corrections to this ticket's own text, so nobody re-inherits them:

1. **"The clips for moving while channeling already exist" is false.** Vanilla's
   `mt_/1hm_/sneak1hm_shout_inhale.hkx` are stance variants (1.03 s, zero annotations,
   `extractedMotion` null — no movement in any of them), and the SH2 submod's five files are
   **byte-identical** copies of one 3.17 s standing pose. Playing `mt_shout_inhale` would have
   changed nothing.
2. Route A's real value was never the clips — it was the `ShoutStandingLocomotionBehavior`
   machinery, which route B transplants without touching the shout graph or ADR-0006.

Progress the same day: the slow half is built — `SpellHotbar_ConcSlow.esp` (MGEF + ability,
SpeedMult −50, houseCARL-authored, mod folder `houseCARL - SpellHotbar_ConcSlow`, not yet
enabled) and the DLL edges (`apply_conc_slow`/`remove_conc_slow` at channel start / end_channel /
on_reset / save load, rituals gated out via `applies_conc_slow()`), commit `ae6d174`, build
43/43, 7/7 test binaries.

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
