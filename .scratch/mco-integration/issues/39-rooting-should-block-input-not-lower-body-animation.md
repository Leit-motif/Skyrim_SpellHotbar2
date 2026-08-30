# 39 — Rooting a cast should block input, not freeze the legs

Ticket 38 removed the forward glide from a driver cast, and the glide is gone. But it went too far:
the character now stands with a still lower body. The owner wants MSCO's own animation preserved —
the character visibly **steps forward, legs moving** — while the *player's* movement input is what
gets blocked.

**Blocked by:** nothing. Ticket 38's rooting change is the thing being corrected, so read it and
`docs/adr/0015-commitment-is-a-property-of-the-behavior-state.md` first.

**Status:** claimed — 2026-08-29, implementation session. Built, deployed, and live-verified
the same evening (see `## Live verification 2026-08-29`): cells 1 and 2 closed (owner eyes +
telemetry), cell 3 open pending an owner steering check. The scope also grew by owner ruling
mid-test: the slight forward traverse MSCO's animmotion authors is WANTED back, which
dissolves ticket 40 — see the live section.

## Triage 2026-08-29: the discriminator this ticket asked for already exists

The ticket wanted to know which of two layers freezes the legs, and said a frame comparison was
the way to find out. Ticket 54 answered half of it by measurement on a different question, and
the answer points hard at candidate (2), the `bAnimationDriven` state modifier:

- **The root the owner certifies as correct does not use the flag.** Ticket 54 measured
  `bAnimationDriven` never rising on the MSCO fire-and-forget cast the owner points to as the
  right feel — recorded in ADR-0015's 2026-08-29 amendment. The reference behavior gets its
  commitment from generator replacement, not from an animation-driven state.
- **Our shtb states still bind it, everywhere.** 24 files under
  `nemesis/Nemesis_Engine/mod/shtb/` bind `bAnimationDriven` (13 in `1hm_behavior`, 10 in
  `magicbehavior`, 1 in `0_master`), untouched by ticket 58.
- **Ticket 58's `shcr` did not change this path.** `shcr` replaces the generators behind the
  vanilla locomotion-adjacent casting nodes and binds `bAllowRotation` only — two binds, no
  `bAnimationDriven` anywhere in the patch. A hotbar driver cast still runs an animation-driven
  shtb state, so nothing about the commitment rebuild has already fixed this by accident.

So the shape of the fix is now plausible to state before the frame is captured: apply `shcr`'s
mechanism to the shtb states — commitment from what the state routes through, not from a flag
that freezes the actor — and the legs keep MSCO's authored motion while input stays blocked.
That is a hypothesis, not a finding. **The frame comparison below is still the acceptance
evidence**, and per the domain rule a visual claim needs a committed screenshot; a log line
proving a state is active never shows what the legs did.

One caution carried from ticket 58: `shcr` and `shtb` both patch behavior, and Nemesis resolves
single-value conflicts last-checked-wins. Produce the contention read before authoring, the way
ticket 58 did.

## The distinction this ticket exists to hold

Three separate things have been collapsed into one:

1. **Animation-driven root motion** — the clip's own animmotion track translating the actor.
2. **Lower-body animation** — the legs playing their authored stepping motion, in place or not.
3. **Player movement input** — WASD / stick actually steering the character during the cast.

The desired behavior is (2) **kept**, (3) **blocked**, and (1) is the knob ticket 38 turned. The
current result reads as (2) also being lost, which is what the owner is reporting.

## Owner report 2026-08-25

> Forward Glide is gone, but this was not necessarily the desired behavior. It was a fallback. What
> I wanted was a preservation of the animations as they are in MSCO — the character actually steps
> forward, there's leg movement. Whereas with our implementation of these casting animations
> through Spell Hotbar 2, the interpretation of rooting movement was taken as stopping all lower
> body movement, which was not correct. We just wanted to block input, and so maybe there's still a
> holdover that is blocking the legs from moving.

The owner's own hypothesis — a holdover from an earlier rooting attempt still suppressing lower-body
animation — is the first thing to check, not the last.

## Where to look

The owner suspects a leftover, and there are two plausible layers:

- **The behavior state.** ADR-0015 made commitment a property of the state, and the state-modifier
  `bAnimationDriven` path is what roots the actor. Whether it also suppresses the lower body is the
  question; a state that is animation-driven with no motion track can present as planted legs.
- **Ticket 38's own change.** `SH2_Cast*_Clip` no longer consuming animmotion is precisely (1). If
  the legs are still moving in the clip data, the freeze is coming from somewhere else and (1) is
  not the culprit — that is a strong discriminator worth running first.

Ticket 35 retired the DLL movement capture, so any surviving movement suppression in the plugin is
by definition a leftover. Check it against the tickets 32 and 34 parked work before assuming.

## Narrowed 2026-08-25 by ticket 38's own telemetry

The ticket-38 session ran both its cells on the deployed DLL right after the owner's pass, and the
second one is a discriminator for this ticket:

- **Disengage (art 12, stamped clip, 140 animmotion keys) leapt ~315 units XY**, matching ticket
  05's ~318. So `SH2_Art_Clip` still consumes animmotion and still translates the actor.
- A fire-and-forget driver cast froze XY to the last digit through the clip, with no
  `SH2_CastRight_Clip` motion bind.

Motion consumption is therefore alive and working where it is supposed to be. That weakens
candidate (1) — ticket 38's animmotion change — as the cause of the leg freeze, and points at the
**`bAnimationDriven` state modifier** side instead. Start there rather than re-running the
animmotion comparison.

This is telemetry from a peer session, not a frame. It says motion translates; it still does not
show what the legs do. The frame comparison below is unchanged as the acceptance evidence.

## Diagnose before building

Do not fix this by reverting ticket 38. The glide removal is wanted; only the leg freeze is not.
Establish which layer kills the lower body first — capture a frame mid-cast and compare against the
same clip played outside a driver cast (a plain MSCO swing, or the pre-38 build). A frame is the
evidence here; a log line proving a state is active never shows what the legs did.

## You test this

Profile `Nolvus Awakening`. Weapon drawn, cast a fire-and-forget spell from a hotbar slot.

1. The character's legs play their stepping animation as MSCO authored it.
2. The character does not glide or translate forward across the ground.
3. Holding a movement key during the cast does not steer the character.

## What this is not

Not a request to restore the forward glide (ticket 38 removed it deliberately and correctly). Not
the concentration channel's rooting, which the owner accepted in the same pass ("I was rooted") —
though if one mechanism serves both, a fix here must not regress that.

## Build 2026-08-29

**Built, static only. All three acceptance cells above are still open** — nothing was
deployed, Nemesis was not run, and no frame was captured. Live testing and the frame
comparison happen after this session, by the coordinator and the owner.

### The mechanism

On each of the four fire-and-forget cast states' binding sets, in **both** graphs, the
`bAnimationDriven` binding is dropped and the remaining three are reslotted:

| | before | after |
| --- | --- | --- |
| `bIsActive0` | `bAnimationDriven` | `bAllowRotation` |
| `bIsActive1` | `bAllowRotation` | `HKSMoveON` |
| `bIsActive2` | `HKSMoveON` | `bHeadTrackSpine`, **inverted** |
| `bIsActive3` | `bHeadTrackSpine`, **inverted** | — |

Sixteen files: eight `hkbVariableBindingSet` (`numelements` 4 → 3) and the eight
`BSIsActiveModifier` that read them (the `true` invert moves from slot 3 to slot 2). The
modifiers are also renamed `bAnimationDrivenIsActiveModifier` → `SH2_CastCommitIsActiveModifier`:
`name` is a debug string with no consumer anywhere in this repo or the DLL, and a modifier
named after a variable it no longer touches is a trap for the next reader.

What is left is what MSCO's certified cast state writes, minus MSCO's own bookkeeping.
`#msco$30`/`#msco$31` (`MSCO_IAM_LR`, the modifier on `MSCO_Casting_LR`) binds five:
`bIsMSCO`, raw 66 inverted, raw 65 inverted, `bAllowRotation`, `bMSCO_LRCasting` — and no
`bAnimationDriven`. Of those five, `bAllowRotation` (uninverted) and raw 65 (inverted) are
now exactly what we write. Three deliberate non-adoptions:

- **`bIsMSCO` and `bMSCO_LRCasting` are MSCO's own bookkeeping**, declared by mod code
  `msco`, read by MSCO's own selectors. Nothing in shtb declares them; binding a name the
  merged graph does not declare makes Nemesis fail or silently no-op, and writing MSCO's
  private flags from our states would be reaching into another mod's state.
- **Raw 66 (`NotCasting`) exists only in `magicbehavior`** — there is no such variable in
  the `1hm_behavior` table, so adopting it would make the two graphs diverge for a variable
  that is MSCO condition bookkeeping rather than any part of the root.
- **`HKSMoveON` stays.** It came from the Hot Key Skill template these states were copied
  from wholesale (`hotkey`'s `#hotkey$21` binds exactly `bAnimationDriven`, `bAllowRotation`,
  `HKSMoveON`, in that order, under a modifier also named `bAnimationDrivenIsActiveModifier`
  — that is where SH2's plant came from). It is written by binding sets and read by no
  condition, expression, or transition in either graph, so it is inert bookkeeping. Leaving
  it means this change moves exactly one variable, which is what keeps the live result
  attributable.

`SH2_Channel_*` and `SH2_Art_*` are untouched, `shcr` is untouched, `0_master` is untouched,
and ticket 38's animmotion change on `SH2_Cast*_Clip` is untouched. No DLL change.

### Resolving MSCO's raw indices 65 and 66

The session that triaged this ticket guessed `65 = MagicAimOffsetPitch, 66 = bHeadTrackSpine`.
**That is off by one. The answer is `65 = bHeadTrackSpine`, `66 = NotCasting`,** and the
design above depends on it.

Counted from the `variableNames` list in `nemesis/Nemesis_Engine/mod/shtb/magicbehavior/#0077.txt`,
which is the vanilla `magicbehavior` string data with shtb's additions marked: shtb adds
**events only** there, no variables, so the 81-entry variable table in that file *is* the
vanilla table. Nemesis appends new variables to the end, so vanilla indices 0..80 are stable
however many mods compose ahead of MSCO — and both of MSCO's raw indices are below 81, which
is why they were safe to write raw in the first place. 0-indexed, the file's list positions
63..67 are `MagicAimOffsetHeading`, `MagicAimOffsetPitch`, **`bHeadTrackSpine`**,
**`NotCasting`**, `bWantCastLeft`.

Two independent checks that the counting method is right, rather than the count being
trusted on its own:

- **Semantics.** `NotCasting = false` and `bHeadTrackSpine = false` are both coherent things
  for a cast state to write. The off-by-one reading would have MSCO writing a boolean into
  `MagicAimOffsetPitch`, a float aim offset.
- **A second graph, cross-checked by name.** Hot Key Skill's `1hm_behavior` binding set
  `#hotkey$21` mixes raw and named references: raw `65`, raw `27`, and
  `$variableID[HKSMoveON]$`. The same counting method against
  `shtb/1hm_behavior/#0085.txt` gives `65 = bAnimationDriven` and `27 = bAllowRotation` —
  which is precisely the trio SH2's own plant writes by name, in the same slot order. The
  method reproduces a known answer in a different graph with a different table.

### The structural-parallel finding: NOT siblings, and the asymmetry runs the safe way

The ticket asked whether SH2's cast states are structurally parallel to MSCO's, because the
routing argument for dropping the flag depends on it. They are not siblings — but SH2's sit
*higher*, not lower, so the argument holds a fortiori.

Traced from `magicbehavior.hkx` (the Nemesis engine's own `cached_behaviors` copy,
decompiled read-only to XML in a scratch directory; nothing in the instance was written):

```
#1347 MagicBehavior.hkb
  #1346 MagicBehavior            <- SH2_Cast{Right,2,3,4}_State are appended HERE,
    #1345 MagicRoot                 as siblings of MagicRoot itself
      #1342 MagicRoot_MG
        #1336 MagicRootBehavior
          #1023 CastingState
            #1021 MagicCasting_iStateGen
              #1020 Casting_MG
                #1010 MagicCastingRootBehavior   <- MSCO_Casting_LR (#msco$42) is
                  #1009 DefaultMagicCasting         appended HERE, as a sibling of
                    #1007 DefaultMagicCastingBehavior  DefaultMagicCasting
                      #0926 MagicCastingLocomotionState   <- what `shcr` replaces
                      #0930 MagicCast_Standing            <- what `shcr` replaces
```

MSCO commits by deselecting `DefaultMagicCasting`, and with it the whole
`MagicCastingLocomotionState` / `MagicCast_Standing` subtree — the same nodes ticket 58's
`shcr` reaches by replacing their generators, and the mechanism the owner has confirmed live
roots both player and NPC. SH2's states deselect `MagicRoot` entirely, six levels further
up, so **every locomotion-blending node MSCO's certified cast excludes, an SH2 cast state
also excludes, plus the rest of the magic tree.** There is no locomotion route open during
an SH2 cast that is closed during the cast the owner certifies as correct. On the
`magicbehavior` side, that is the answer to "what prevents held WASD from steering the actor
once the flag is gone": the same thing that prevents it during an MSCO cast.

`1hm_behavior` has no MSCO analogue at all — MSCO ships `magicbehavior` only. There, SH2's
four cast states are top-level siblings of `1HM_Behavior`'s thirteen vanilla states,
including `AttackState`: the same slot an MCO attack occupies, and the same full-body
exclusive routing. Not a proof, but not an open route either.

**So: not parallel, and not a structural reason input would steer.** Editing proceeded.

Where the honest uncertainty is, stated plainly rather than smoothed over: `bAnimationDriven`
makes the actor's motion animation-driven instead of controller-driven — it is a translation
mechanism, not a lower-body suppressor, and the ADR's withdrawn amendment notes that MCO
attacks (which visibly step) raise it. So the causal story "the flag is what freezes the
legs" is a hypothesis this build acts on because ticket 54's measurement and both reference
implementations point at it, not something the graph proves. If the legs are still frozen
after this change, the flag was not the cause and the next suspect is the clip content
itself — and that is decided by the frame comparison, not by more graph reading.

**The successor is already filed: `40-stepping-fire-and-forget-cast-animations.md`.** It
records that the MSCO clips `MSCO_left1`–`4` carry `animmotion` root translation but **no
stepping in their leg tracks**, which would mean there is no leg animation here for anything
to have suppressed — and it is parked on new animation assets, which is not agent work.
Ticket 40 dissolves only if 39 proves suppression. So the frame comparison decides which of
the two tickets is real: legs stepping after this change closes 39 and confirms 40 was
mis-scoped; legs still frozen after it closes the suppression hypothesis and hands the
problem to 40's asset wall.

### Verification done here

- `python python_scripts/validate_shtb_commitment.py` — **169 checks, 0 failures.** New
  lever, modelled on `.scratch/shcr-build/validate_shcr.py`. It walks each cast state by
  name through `generator` → `_MG` → `modifier` → `variableBindingSet` (so a renumbering
  cannot fool it), asserts no `bAnimationDriven` on any of the eight chains and that the
  binds, slots, `numelements`, invert flags, and modifier name are exactly as decided;
  content-hashes the Channel and Art chains end to end against the branch point `5b29e96`
  — state, `_MG`, modifier, binding set **and clip** — plus the binding set the four cast
  clips share, and re-walks the wiring so a frozen node nothing points at any more still
  fails; asserts every `$variableID[...]$` shtb references is either shtb-declared or
  listed with the mod code that provides it; and asserts `0_master` still declares only
  the four `MCO_*` variables plus `SH2_ArtSelector` and carries no binding set.
- Negative-tested five ways, each expected to fail and each restored: a name-only revert
  of one modifier (1 named failure), an edit to `SH2_Art_Clip` (1), rewiring
  `SH2_Channel_MG` to another modifier (3), re-planting `bAnimationDriven` on a cast clip
  binding set (4), and restoring one pre-change cast binding set (2). Exit 1 in all five.
- `.scratch/mco-integration/notes/39-contention-table.md` — **zero contested nodes.** All
  sixteen files are `#shtb$NN` nodes shtb defines itself; no vanilla `#NNNN` base file is
  touched, so nothing meets Nemesis's last-checked-wins resolution. 56 mod codes indexed.
- `python python_scripts/ticket_status.py --lint` — clean.

### The `0_master` bind, decided

There is no bind there to decide about. The triage's "1 in `0_master` binds
`bAnimationDriven`" is a grep hit on `<hkcstring>bAnimationDriven</hkcstring>` at
`0_master/#0106.txt:1304`, which is **vanilla base content outside the shtb MOD_CODE block**
— a variable *declaration* in the graph string data, not a binding. shtb's own block there
(lines 1453–1459) declares four `MCO_*` variables and `SH2_ArtSelector` and nothing else,
and all three shtb `0_master` files are declaration nodes carrying no `hkbVariableBindingSet`
and no `BSIsActiveModifier`. Left alone; the validator pins it.

## Live verification 2026-08-29 (evening)

Deployed the same day: 16 patch files copied to `Dev - Spell Hotbar 2`, Nemesis Update Engine
(128 s) + generation (129 s, 1058 animations) clean, 4 `SH2_CastCommitIsActiveModifier` per
graph byte-verified in the compiled `1hm_behavior.hkx` / `magicbehavior.hkx`.

**Cell 1 (legs animate) — CLOSED, owner eyes.** First live pass, owner verbatim: "legs
animate, there just isn't any forward motion." The suppression hypothesis was right; the
`bAnimationDriven` plant was the leg freeze. No frame needed — the owner watched it live,
the ticket-58 precedent (owner eyes as closing evidence).

**Cell 2 (no glide) — CLOSED AS SUPERSEDED, owner ruling + telemetry.** The same owner
report re-scoped the cell: MSCO's native cast traverses forward slightly (its animmotion),
and that traverse is WANTED, not a glide — a glide is translation without leg animation.
So ticket 40's premise dissolves (its clips-have-no-stepping claim is contradicted by the
owner's own eyes) and its one-line revert was applied: `ClipTranslationDriver` accepts the
four `SH2_Cast*_Clip` names again and `apply()` arms during a driver cast
(`skse_plugin/src/casts/clip_translation_driver.cpp`, commit `88cff9c`). Verified live
after DLL redeploy: `SH2 motion: bound SH2_CastRight_Clip (4 animmotion keys)`, and XY
moved ~5.7 units in a front-loaded decaying curve through the cast — the clip-translation
signature. `bAnimationDriven` polled False three times mid-cast (the plant is gone at
runtime, not just in the source).

**Channel regression guard — GREEN.** Held channel via `setSlotKeyHeld(1,true)` +
`castSlot(1)`: entered, held 3158 ms, exited clean; `bAnimationDriven` True during the
hold (its plant is intact by design); XY frozen to the last digit; no motion binding
logged (the filter correctly excludes `SH2_Channel_Clip`).

**Art regression guard — GREEN, owner eyes + log.** Owner: "disengage works." Log shows
`SH2_Art_Clip` binding 11- and 50-key animmotion on two art casts.

**Cell 3 (movement input does not steer) — OPEN, owner hands.** Injected movement keys
cannot move the player (playbook, measured 2026-08-28), so no agent can prove steering is
blocked. Owner check: hold W mid-cast, confirm the character does not steer.

Fixture notes: owner's live bars backed up and restored via `saveBarsToFile`/
`loadBarsFromFile` (probe added Ice Spike at slot 0); no save was written; all seven
`skse_plugin` test executables pass after the DLL change.

## Comments

Owner 2026-08-25: reported during the tickets 25/06/14 acceptance pass, as the answer to ticket
38's owner-eyes cell. Ticket 38's cell passes on its own terms; this is the follow-on.
