# Contention table: ticket 39's node set against this load order

Ticket 39 step 1, following ticket 58's procedure. Generated 2026-08-29 by walking every
`Nemesis_engine/mod/<code>/<graph>/` directory under
`C:/Nolvus/Instances/Nolvus Awakening/MODS/mods/` (read-only) and indexing every node file
by `(graph, filename)`, then looking up the sixteen files this ticket edits.

**56 mod codes present.** 27 touch `1hm_behavior`; 14 touch `magicbehavior`:

| Graph | Mod codes present |
| --- | --- |
| `1hm_behavior` | aabl, adxp, amco, atkcnc, bcbi, block, cbbi, colis, evfmgo, fvpa, gpma, hotkey, jpatka, na1w, nemesis, pscd, rthf, sbeef, scar, **shtb**, sscb, tdmv, tkds, tkuc, tudm, turn, zcbe |
| `magicbehavior` | colis, evfmgo, gpma, hotkey, jpatka, **msco**, **sbeef**, **shcr**, **shtb**, tdmv, tkds, tkuc, tudm, zcbe |

**Result: zero contested nodes.** Every file this ticket edits is a `#shtb$NN` node — a new
node shtb itself defines — and the `#shtb$` name is unique to this mod code by construction.
No `#NNNN` vanilla base file is touched, so nothing is exposed to Nemesis's
last-checked-wins resolution of single-value conflicts.

| Graph | Node file | Class | Serves | Also owned by |
| --- | --- | --- | --- | --- |
| `1hm_behavior` | `#shtb$11.txt` | `hkbVariableBindingSet` | SH2_CastRight_State | clear |
| `1hm_behavior` | `#shtb$12.txt` | `BSIsActiveModifier` | SH2_CastRight_State | clear |
| `1hm_behavior` | `#shtb$14.txt` | `hkbVariableBindingSet` | SH2_Cast2_State | clear |
| `1hm_behavior` | `#shtb$15.txt` | `BSIsActiveModifier` | SH2_Cast2_State | clear |
| `1hm_behavior` | `#shtb$17.txt` | `hkbVariableBindingSet` | SH2_Cast3_State | clear |
| `1hm_behavior` | `#shtb$18.txt` | `BSIsActiveModifier` | SH2_Cast3_State | clear |
| `1hm_behavior` | `#shtb$20.txt` | `hkbVariableBindingSet` | SH2_Cast4_State | clear |
| `1hm_behavior` | `#shtb$21.txt` | `BSIsActiveModifier` | SH2_Cast4_State | clear |
| `magicbehavior` | `#shtb$13.txt` | `hkbVariableBindingSet` | SH2_CastRight_State | clear |
| `magicbehavior` | `#shtb$14.txt` | `BSIsActiveModifier` | SH2_CastRight_State | clear |
| `magicbehavior` | `#shtb$16.txt` | `hkbVariableBindingSet` | SH2_Cast2_State | clear |
| `magicbehavior` | `#shtb$17.txt` | `BSIsActiveModifier` | SH2_Cast2_State | clear |
| `magicbehavior` | `#shtb$19.txt` | `hkbVariableBindingSet` | SH2_Cast3_State | clear |
| `magicbehavior` | `#shtb$20.txt` | `BSIsActiveModifier` | SH2_Cast3_State | clear |
| `magicbehavior` | `#shtb$22.txt` | `hkbVariableBindingSet` | SH2_Cast4_State | clear |
| `magicbehavior` | `#shtb$23.txt` | `BSIsActiveModifier` | SH2_Cast4_State | clear |

Every row's only other hit in the installed tree is `Dev - Spell Hotbar 2`, which is the
deployed copy of this same patch — the same bytes, not a competing owner.

Ownership was traced by wiring, not by filename: each state's `generator` is its `_MG`
`hkbModifierGenerator`, whose `modifier` is the `BSIsActiveModifier`, whose
`variableBindingSet` is the set. `python_scripts/validate_shtb_commitment.py` re-walks that
chain on every run, so a renumbering cannot silently point this table at the wrong nodes.

## The one cross-mod dependency this ticket does not create, and does not remove

The four sets bind `HKSMoveON`, which **shtb does not declare in either graph** — Hot Key
Skill (`hotkey`) declares it, into `1hm_behavior/#0085.txt` and `magicbehavior/#0077.txt`.
`magicbehavior` has two more of these: `bAllowRotation` (declared by `hotkey`, and by our
own `shcr`) and `MSCO_attackspeed` (declared by `msco`). These are pre-existing latent
dependencies on other mod codes staying installed and ticked, not something ticket 39
introduces; dropping the `bAnimationDriven` bind removes one such reference in
`magicbehavior` (where `hotkey`, not vanilla, declares it) without closing the general gap.
Recorded in the validator's `EXTERNAL` table so the list stays honest as the patch changes.

`HKSMoveON` is written by `BSIsActiveModifier` binding sets across `hotkey` and `shtb` and
read by no condition, expression, or transition anywhere in either graph — it is
bookkeeping for consumers outside the graph, not a movement gate. Left untouched here.

## What is deliberately NOT in the table

`SH2_Channel_*` and `SH2_Art_*` keep their binding sets and modifiers unchanged (ticket 39's
scope: the owner accepted the channel's root, and art clips must keep consuming animmotion).
`nemesis/Nemesis_Engine/mod/shcr/` is untouched — ticket 58's shipped patch, owner-confirmed
live. `0_master` is untouched: shtb's three files there are declaration nodes only and carry
no binding set at all. All four are asserted by the validator rather than left to care.
