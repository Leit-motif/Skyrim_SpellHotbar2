# Contention table: Magelock's node set against this load order

Ticket 58 step 1. Generated 2026-08-29 by cross-referencing every vanilla `magicbehavior` node
Enemy Magelock patches against every `Nemesis_Engine/mod/<code>/magicbehavior` directory in the
installed mods (58 mod codes present; 20 touch `magicbehavior`).

**Result: 22 of 27 clear, and every structural node we actually need is among them.** Ticket 53
expected the opposite — "the integration risk is real and is most of this ticket's work". It
isn't. The territory this mechanism roots in is empty in this load order.

| Node | Vanilla name | Class | Also patched by |
| --- | --- | --- | --- |
| `#0077` | (graph string data) | `hkbBehaviorGraphStringData` | colis, evfmgo, gpma, hotkey, jpatka, msco, pscd, sbeef, shcc, shtb, tdmv, tkds, tkuc, tudm, zcbe |
| `#0078` | (variable value set) | `hkbVariableValueSet` | gpma, hotkey, jpatka, msco, sbeef, shcc, tdmv, tkds, tkuc, tudm, zcbe |
| `#0079` | (graph data) | `hkbBehaviorGraphData` | colis, evfmgo, gpma, hotkey, jpatka, msco, sbeef, shcc, shtb, tdmv, tkds, tkuc, tudm, zcbe |
| `#0281` | (MRh ready transitions) | `hkbStateMachineTransitionInfoArray` | **msco** |
| `#0521` | (clip trigger array) | `hkbClipTriggerArray` | **tudm** |
| `#0088` | LeftHandMagicCast_MSG | `hkbManualSelectorGenerator` | clear |
| `#0392` / `#0393` | MLh_PreTelekinesis | state info / clip | clear |
| `#0429` / `#0430` | MLh_SelfReleaseState / MLh_SelfRelease | state info / clip | clear |
| `#0457` / `#0458` | MLh_PreAimedCon_MSG / MLh_PreAimedCon | selector / clip | clear |
| `#0460` | StfMagic_PreAimCon | clip | clear |
| `#0496` / `#0497` | MLh_AimedPreReady_MSG / MLh_PreReady | selector / clip | clear |
| `#0499` | StfMagic_PreReady | clip | clear |
| `#0509` / `#0510` | MLh_AimedPreCharge_MSG / MLh_AimedPreCharge | selector / clip | clear |
| `#0512` | StfMagic_AimedPreCharge | clip | clear |
| `#0517` / `#0518` | MLh_AimedRelease_MSG / MLh_Release | selector / clip | clear |
| `#0520` | StfMagic_Release | clip | clear |
| `#0523` | (transition info array) | `hkbStateMachineTransitionInfoArray` | clear |
| `#0926` | **MagicCastingLocomotionState** | `hkbStateMachineStateInfo` | clear |
| `#0930` | **MagicCast_Standing** | `hkbStateMachineStateInfo` | clear |
| `#0965` | **MagicCast_TurnLeft_State** | `hkbStateMachineStateInfo` | clear |
| `#0998` | **MagicCast_TurnRight_State** | `hkbStateMachineStateInfo` | clear |

## Reading it

**`#0077` / `#0078` / `#0079` are not conflicts.** They are the graph's string data, variable
value set, and graph data — where a patch declares its own variables and events. Nemesis merges
list appends there, which is why fifteen mod codes coexist in them today. `shcc` already
declares into them. Our patch does the same and the crowd is irrelevant.

**Two nodes are real contention, and we may not need either.**

- `#0281` is the `MRh` ready transition array that MSCO gates behind `iMSCO_ON == 0` — the one
  place our change could fight the mod whose feel we are matching. Magelock patches it as part
  of reworking the right-hand ready flow.
- `#0521` is the clip trigger array Magelock points `#0458` at when it retimes that clip; TUDM
  also patches it.

Both belong to Magelock's clip-level rework, not to the structural root. **If our patch replaces
only the generators behind the four `MagicCast*` state nodes, it touches zero contested nodes**
beyond the three declaration nodes everyone shares. That is the design target, and it is worth
protecting: a patch confined to clear nodes needs no compatibility work and cannot silently
displace `msco`, `pscd`, `sbeef`, or `tudm` the way `shcc` displaced `sbeef` and `pscd` on
ticket 33.

Departing from that target — reworking clips, retiming triggers, touching the ready flow — is
the point at which contention becomes real. Do it only against a stated reason.
