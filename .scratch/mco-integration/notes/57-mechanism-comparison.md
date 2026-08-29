# Casting commitment: what the two references actually do

Read from sources on 2026-08-29 — CARIM's three `.psc` files plus its ESP records, Magelock's
228 Nemesis patch files. This supersedes ticket 53's first-pass description of both, which was
wrong about each in a way that matters.

## CARIM — a proportional slow, and it can never root

The moving part is not the script. It is a hidden perk.

`CARIM_Perk_Control` (`0x804`, non-playable, hidden) carries four entries. Three are
`PerkEntryPointModifyActorValue` on entry point **`ModIncomingSpellMagnitude`**, ActorValue
**`SpeedMult`**, Modification **`SetToAVMult`**, Value `0.95`, priorities 255/254/253. Each is
conditioned `GetIsID(<one of the three debuff spells>) == 1` on tab index 1 — the incoming
spell. The fourth entry grants `CARIM_Spell_Passive`, which carries the detection script. SPID
hands the whole perk to `ActorTypeNPC` at 100%.

So the debuff spell itself is a shell: `CARIM_Spell_MovespeedDebuffCast` is fire-and-forget,
self-targeted, **magnitude 1**, duration 86400, ignoring resistance. Its effect
`CARIM_MGEF_MovespeedDebuff` is a `PeakValueModifier` on `SpeedMult` with **no conditions**.
The magnitude of 1 never applies — the perk rewrites it at apply time to *the target's own
current `SpeedMult`, times 0.95*.

That is the piece ticket 53 missed, and it is exactly the defect that sank our own trial. We
applied a flat −100 against a fixture whose buffed base was 117 and got a crawl. CARIM never
uses a flat number: the debuff is a proportion of whatever that actor's speed happens to be, so
a buffed NPC and a vanilla one both lose the same fraction. Ticket 53's post-mortem credited
"edge-driven scripting instead of conditions". Edge-driven scripting is real and necessary, but
the magnitude fix is the perk entry point.

`CARIM_MaintenanceScript` rewrites those three entry values from the JSON on every load
(`SetNthEntryValue`), clamped by `ClampValues` to **0.95 maximum**.

**That clamp is a design statement, and it decides the fork.** CARIM at its most aggressive
setting leaves 5% of base speed. It is a slow by construction and cannot be turned into a root
by configuration. If the owner wants rooted, CARIM's shape is the wrong shape — not
under-tuned, structurally incapable.

The rest is plumbing, and the plumbing is worth stealing:

- `CARIM_DetectionScript` registers for `BeginCastRight` / `BeginCastLeft` / `CastStop` /
  `MRh_SpellFire_Event` / `MLh_SpellFire_Event` / `InterruptCast` / `weaponSheathe`, plus the
  bow and crossbow events. Cast-begin applies the debuff; every end path dispels it.
- Concentration is handled explicitly: on SpellFire with `GetCastingType() == 2` it polls
  `IsCastingRight`/`IsCastingLeft` at 0.1 s until the channel truly ends, then dispels.
  Fire-and-forget (type 1) dispels at SpellFire.
- `CARIM_SpeedmultAdjustScript` does `ModActorValue("CarryWeight", ±0.01)` on effect
  start/finish. That is the refresh kick that makes a `SpeedMult` change take effect on an actor
  already in motion. Its comment scopes it to first person.
- `ArrestMovement()` kills entry momentum: for the player in third person, toggle
  `SetPlayerAIDriven(GetPlayerControls())`, `EnableAI(false)`, wait 0.25 s, `EnableAI(true)`,
  toggle back; for NPCs the same without the AI-driven toggle, at 0.1 s. The author's own
  comments record that `DisablePlayerControls` does not work until the animation completes, and
  that this path has a known TDM camera issue.

## Magelock — commitment by replacing the casting locomotion state

Not a modifier on vanilla states, and not new animations either.

The mod is 199 new nodes across 29 patched vanilla ones, all in `magicbehavior`. The clips it
appears to add — `MLh_PreAimedCon`, `MLh_PreCharge`, `MLh_PreReady`, `MLh_PreTelekinesis`,
`MLh_Release`, `MLh_SelfRelease`, the `StfMagic_*` staff set — are **vanilla clip generators**
that already exist; `#0458 MLh_PreAimedCon` is a stock node the patch touches only to swap its
trigger array. The `.hkx` files shipped under DAR `_CustomConditions\94010` with condition
`Random(1)` are file-level overrides of those vanilla animations, using DAR purely as a carrier
so the registered animation names stay the same.

**Where it stops translation:** it replaces the generator of the vanilla nodes where casting and
locomotion meet — `MagicCastingLocomotionState` (`#0926`), `MagicCast_Standing` (`#0930`),
`MagicCast_TurnLeft_State`, `MagicCast_TurnRight_State` — pointing each at one of its own state
machines instead. Casting no longer routes through a state that blends locomotion, so there is
nothing left to translate the actor. The commitment is structural.

**The collapse that made ticket 58's build small (found during the build, recorded here so the
next reader doesn't re-derive it from 199 nodes):** all four of Magelock's per-state replacement
machines route their NPC branch to ONE identical node — a `hkbModifierGenerator` wrapping the
`bAllowRotation` modifier (`#altmag$51/$52`) around `#altmag$86`, which is a copy of vanilla
`#0088 LeftHandMagicCast_MSG` whose only delta is a staff-flavor rework (`#altmag$87` at
selector indices 0 and 8). Everything else in the 199-node set is the player branch, the split
machinery, or clip retiming. Drop the split and the staff rework and the whole structural root
is: one binding set, one modifier, four thin wrappers over vanilla `#0088` — the 14-file `shcr`
patch.

**How it separates player from NPC** — ticket 53 listed this as an open engineering question,
and the graph answers it. Each splice point lands on a two-state machine such as
`#altmag$174 BNPCBehavior`, whose **`startStateId` is bound to graph variable index 4**. State 0
is `NPCMagicBehavior`, state 1 is `PlayerMagicBehavior`. One variable picks the treatment, at
every splice. The variable's name is the one thing still unread — it needs the vanilla
`magicbehavior` graph's variable table, not the patch files.

**What it does not use:** `bAnimationDriven` appears nowhere in the mod. The single
`BSIsActiveModifier` (`#altmag$51`) binds `bAllowRotation` only — a rotation lock, not a
translation lock.

## The measured fact that agrees with both readings

One calibration capture ran before the protocol was abandoned (evidence
`.scratch/mco-integration/evidence/t54/01-msco-ff-moving-entry.json`): seven owner-performed
MSCO fire-and-forget casts from a moving entry, the root the owner certifies as correct.
**`bAnimationDriven` never rose. Not on any of the seven.** `IsCastingRight` moved cleanly, in
seven spans of about 2.5 s.

The playbook records `bAnimationDriven` as reading "the root plant directly" and calls it the
cheapest proof available for commitment mechanics. That is true for SH2's own hotbar channel
(the standstill check in the same directory shows it rising) and false for the behavior the
owner points at as the target. Magelock, independently, roots without ever touching that flag.

The lesson is consistent across all three: **a correct root is a state you enter, not a flag you
set.** Every mechanism this project has tried that planted a flag on an existing state has
failed, and neither reference implementation plants one.

## The fork, and a recommendation

- **Hard commit** wants Magelock's shape: replace the generators behind the casting-locomotion
  nodes with owned state machines. It satisfies the no-Papyrus constraint natively. The cost is
  contention — those nodes sit in the territory `msco`, `pscd`, `sbeef`, and `shtb` already
  patch, and Nemesis resolves single-value conflicts last-checked-wins.
- **Graded slow** wants CARIM's shape, and CARIM's shape tops out at 95%. Ported to SH2's DLL it
  is small: the cast edges are already sunk in `animationeventhook.cpp`, and the CarryWeight kick
  and the `EnableAI` blip are one-line native calls. What does not port for free is the perk
  entry point — the proportional magnitude lives in records, so the shipped mechanism needs
  either that perk or a native equivalent that reads the actor's current `SpeedMult` at apply
  time.

They are not exclusive. The composition the owner has circled twice from different directions —
root concentration, slow everything else — takes Magelock's structure for the channel and
CARIM's proportional debuff for the rest, and each half is already the right tool for its half.

The owner has ruled both ways on different days ("rooted is preferable" on 08-28, "slowed was
closer to what I envisioned" on 08-29). Nothing here resolves that; it is a feel question and it
needs the trials in tickets 55 and 56. What this read does settle is that the two mechanisms are
not competing implementations of one idea — one cannot produce what the other produces.
