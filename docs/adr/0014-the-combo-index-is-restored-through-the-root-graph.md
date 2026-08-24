# The combo index is restored through the root graph, and the successor is learned

Date: 2026-08-24

Status: accepted

## Context

A hotbar cast that interrupts an MCO swing has to hand the combo position on, so the swing after
the cast continues the chain instead of restarting it. Two questions had to be answered before
that could work, and both had been answered wrongly for several sessions.

**Where the index lives.** MCO selects the attack clip inside `MCO_Attack.hkb`, a nested behavior
graph whose `AttackNodes_StateMachine` binds `startStateId` to `MCO_nextattack`. That graph is
`VARIABLE_MODE_DISCARD_WHEN_INACTIVE`, and a hotbar cast always leaves the attack subtree, so the
next attack reactivates it from cold. Every nested graph owns a private variable-value set, and a
nested graph's variable names are linked to the **root** graph's ids by Havok's linking pass, not
to the intermediate graph's. `BShkbAnimationGraph` holds only the root graph, so
`SetGraphVariableInt` can reach root storage and nothing else. Writes from inside a nested graph
never appear at the root — `MCO_currentattack`, which `AttackNodesState<N>` sets in-graph, reads 0
from the root at all times, including mid-swing.

The consequence: what the nested selector consumes on reactivation is whatever the **root** holds
at that moment, so the root must both declare the name and hold the right value at the right time.
Declaring `MCO_nextattack` in `0_master` through the `shtb` Nemesis patch created the root storage
the link resolves against. Before that, no weapon honored a restore.

That left a race rather than a data problem. MCO stomps `MCO_nextattack` to 1 at ready-enter and
at `AttackState` exit. One-handed weapons emit `SBF_ReadyStart` / `MSCO_MagicReady` on the way out
of the cast state, which gave the restore a rewrite moment after the stomp and before the press —
so 1H appeared to work. Two-handed weapons emit neither, so the stomp always won and greatsword
and warhammer always played `attack1`. The graphs are node-for-node identical on both paths; the
difference was only which events the exit path raises.

**What value to hand on.** The advance is a clip annotation, `PIE.@SGVI|MCO_nextattack|N`, and its
time within the clip is pack data (Elder Creed's sword publishes at 0.63s of 1.8s; Mercenary
Greatsword at 0.82s of 2.17s; For Honor's warhammer at 0.0s). A cast landing before that moment
reads the index of the swing that is *playing*; restoring it replays the swing the player just
interrupted. The owner's ruling is that an interrupted swing still counts, because a combo is
something the player sees as animation rather than as a hit frame — so a pre-advance interrupt
must still hand on the successor. Nothing in the graph knows the successor before the clip writes
it, and deriving it as `index + 1` is not available: moveset lengths and wrap points are pack data
(Mercenary Greatsword chains 1,2,3,4,9,10 with `attack4 → 1`; Elder Creed's `attack5` publishes no
advance at all).

A separate defect masked all of this. The engine splits a clip annotation at the first `.` into an
event name and a payload, so the advance arrives as event `Pie` carrying payload
`@SGVI|MCO_nextattack|N`. The animation-event hook forwarded only the event name. The advance was
therefore arriving on every swing and being discarded, which was recorded as "these packs emit no
SGVI".

## Decision

**The restore is written to the root graph, at actor level, and re-asserted against every stomp.**
`Actor::SetGraphVariableInt` is not a compromise for want of a per-graph write; it is the only
storage the nested selector's variable link resolves against. Per-graph writes solve nothing and
are not attempted. The animation-event hook forwards the event payload alongside the tag, and a
stomp observed while a restore is pending is answered with an immediate rewrite in the same
dispatch. That closes the race on every weapon class, including the two-handed paths that raise no
ready events.

**The successor is learned from the clips, never derived.** A swing tracker records which index is
playing, taken from the variable read at `MCO_AttackInitiate`. When that swing's clip publishes its
advance, the pair is recorded in a successor table keyed by weapon type. At cast start, a live
sample equal to the playing index is a pre-advance interrupt and is replaced by the learned
successor; a sample that differs is already the successor and is kept as-is. Wrap points and short
movesets are therefore correct by construction, because every value in the table is one a clip
taught.

**A payload carrying 1 never teaches.** The ready-enter and `AttackState`-exit resets emit the same
annotation shape as a real advance, and when a swing is cut the exit notify reaches the sink
*before* `attackStop` — with the tracker still open and `IsAttacking` still true. Since a reset
always carries 1 and no clip can teach itself, the value 1 is refused at the payload edge. A
genuine wrap to 1 is still learned, one edge later, by the window-close sampler, which only follows
a swing that actually advanced.

**An unlearned successor keeps the sampled value.** The table is per-session, so the first
pre-advance interrupt at an index this session has never seen through to its advance falls back to
the previous behavior — the interrupted swing replays — and says so in the log. Guessing would
violate the rule above.

## Consequences

- The restore path depends on the `shtb` patch's `0_master` declaration of `MCO_nextattack`.
  Removing it returns every weapon to `attack1`. The neighbouring `MCO_currentattack` declaration
  is inert by the mechanism above and is retained only because editing it would cost a Nemesis
  engine update and full regeneration for no functional gain.
- `MCO_Attack.hkx` is never edited. It belongs to another mod, is outside Nemesis's patchable set,
  and the per-index entry-transition refactor once considered for it is closed permanently: the
  binding is the only selector on both the working and failing paths.
- Combo behavior now depends on the animation-event payload field. A future change to the hook's
  signature or to which events are forwarded will silently reintroduce the original defect, and the
  failure mode is a combo that restarts rather than an error.
- Clip identity in diagnosis comes from the advance payload itself — attack N is the only clip that
  teaches N+1 — which replaced the OAR Animation Log window as the oracle and made the whole test
  loop drivable without a human at the keyboard. Graph variable read-backs still do **not** name
  the playing clip and must not be used for it.
- Concentration holds are unaffected: the hold is credited against the sample's age, so a channel
  of any length still hands its position on.
