# Weapon Arts for Spell Hotbar 2

Status: in progress — ticket 01 resolved on `weapon-arts`; bind menu and Art Packs are next

Created 2026-08-12. Depends on `../mco-integration/` — specifically its ticket 08 (the `shtb`
state distributed into `1hm_behavior`, owner-verified) and ticket 04 / ShoutMCO ticket 50 (the
cast-intent deferral API). This spec does not re-derive their mechanics; it reuses them.

## Problem Statement

The player's combat is MCO. Their hotbar casts spells without occupying a hand, which is why this
fork exists. But every *attack* they can perform is still bound to the same two mouse buttons —
so the only way to reach a special attack is to change what is equipped, or to remember a separate
mod's hotkey and its separate rules.

`Ashes of War` is the concrete case. It ships 96 special attack animations, and today reaching one
means wearing the right item: art identity is a slot-55 clothing item's keyword, one worn at a
time, chosen in the inventory rather than in combat. The trigger is a hotkey owned by
`Additional Attack by Loop`, gated by that mod's own Papyrus, with its own cost and cooldown that
the hotbar cannot see.

From the player's side the complaint is simple: **a special attack is a hotbar action, and it is
not on the hotbar.** Binding one to slot 4 the way a spell binds to slot 3 is impossible — the
hotbar can only hold Spells, Scrolls, Shouts and potions, and a special attack is none of those.

## Solution

A **Weapon Art** becomes a fourth thing a hotbar slot can hold. Binding one to a slot and pressing
that slot plays the art, from a drawn weapon stance, without equipping anything and without the
player touching their inventory.

The fork owns the trigger, the state and the rules. It does not own the animations: an art is an
MCO attack clip somebody else authored, and the fork's job is to make one bindable. **Ashes of
War is the concept** (named special attacks on a button). **It is not the machinery.** We do not
take its slot-55 items, AABL hotkey, or a requirement that the file be named `AABL_Attack_A.hkx`.
A Weapon Art is a special MCO animation that plays when the assigned hotbar button is pressed.
SH2 owns bind, notify, `SH2_Art_State`, Cast Plant, stamina, and cooldown. PIE and the clip's own
annotations own hits, windows, and motion (ADR-0009).

**Clip identity is data**, not a hardcoded path. A catalogue row names whichever HKX to play. The
Art Selector is SH2's name for which art is live (OAR/PIE can read it). It is not Ashes of War's
worn keyword. Ticket 01's `AABL_Attack_A` generator is an inert bootstrap, not a contract.

## User Stories

1. As a player, I want to bind a Weapon Art to a hotbar slot, so that a special attack is reachable
   the same way a spell is.
2. As a player, I want pressing that slot to play the art with my weapon drawn, so that I never
   leave combat to reach it.
3. As a player, I want the art to play without equipping or unequipping anything, so that my hands
   and my current loadout are untouched.
4. As a player, I want each slot to hold a *different* art, so that a bar is a set of options
   rather than one repeated attack.
5. As a player, I want the art I bound to be the art that plays, so that my binding means something
   regardless of what I am wearing.
6. As a player, I want the bound art to show an icon and a name on the bar, so that I can tell my
   slots apart at a glance.
7. As a player, I want an art to cost stamina, so that it is a considered choice and not a free
   spam button.
8. As a player, I want an art to go on cooldown, so that it reads as a special attack rather than a
   basic one.
9. As a player, I want the hotbar to show that cooldown on the slot, so that I can see when the art
   is available without guessing.
10. As a player, I want an art I cannot currently afford to refuse visibly, so that a dead press is
    explained rather than silent.
11. As a player, I want an art that chains into my next swing to keep chaining, so that combat flow
    is unbroken where the animation was authored for it.
12. As a player, I want an art that does not chain to end cleanly and leave me ready, so that a
    terminal art is not a stutter.
13. As a player, I want my next ordinary swing after any art to start at the beginning of the
    combo, so that the combo counter never carries over invisibly.
14. As a player, I want WASD ignored during an art the same way it is during an MCO attack, so
    that I cannot walk out of the clip, while the clip's own motion still carries me.
15. As a player, I want an art pressed in the middle of a swing to land after that swing rather
    than cancelling it, so that a mistimed press costs me nothing.
16. As a player, I want an art to hit and damage what it visibly strikes, so that the animation and
    the outcome agree.
17. As a player who already runs `Ashes of War`, I want my existing arts available on the bar, so
    that adopting this costs me nothing I already had.
18. As a player who already runs `Ashes of War`, I want the worn-item behaviour to keep working
    untouched when no fork art is selected, so that nothing I rely on regresses.
19. As a player who does not run `Ashes of War`, I want the feature to install and behave sanely,
    so that the fork does not require a mod I do not have.
20. As a player, I want the installer to detect whether `Ashes of War` is present and configure
    accordingly, so that I am not asked a question the installer can answer itself.
21. As a player running MCBO, I want nothing here to disturb it, so that both continue to work.
22. As an animation author, I want to add a new art by dropping a clip and a condition file, so
    that extending the set does not require touching the fork.
23. As an animation author, I want to use any MCO attack clip as an art, so that the set is not
    limited to one pack.
24. As a modlist maintainer, I want the `Ashes of War` integration to ship as metadata rather than
    copied animations, so that no redistribution question arises.
25. As a modlist maintainer, I want to regenerate that integration against a different
    `Ashes of War` version, so that a future update does not silently stop working.
26. As the fork maintainer, I want a Weapon Art press to be drivable from a script, so that runtime
    verification does not depend on OS input reaching a focused window.
27. As the fork maintainer, I want art definitions to live in data rather than code, so that
    balancing costs and cooldowns is not a rebuild.
28. As the fork maintainer, I want a bar containing arts to survive a save/load round trip, so that
    bindings are durable.
29. As the fork maintainer, I want a missing or broken art to degrade loudly, so that a silent dead
    button is never the failure mode.

## Implementation Decisions

### Vocabulary (belongs in `CONTEXT.md`; proposed here)

- **Weapon Art** — a bindable attack animation played from a hotbar slot without equipping
  anything. _Avoid_: power attack, additional attack, ash of war.
- **Art Selector** — the global the fork sets to name which Weapon Art plays; OAR conditions read
  it. Zero means no fork art is selected. _Avoid_: art keyword, worn art.
- **Art Pack** — a set of OAR submods keyed to the Art Selector. _Avoid_: animation mod, moveset.
- **Terminal Art / Chaining Art** — an art whose clip does not, or does, carry MCO window
  annotations. A property of the clip, never of the binding.

### The graph

- The fork ships its own state in the weapon behaviour, entered by its own event and exited by its
  own end-of-clip trigger, replicating the shape ticket 08 already proved. It does not reuse
  `Additional Attack by Loop`'s event, state or Papyrus, and does not require that mod.
- The state's clip generator currently names `animations\AABL_Attack_A.hkx` as a ticket-01
  placeholder. ADR-0009: the product clip is whatever the art row names; retargeting that
  generator is allowed.
- Entry is appended to the weapon ready state's own transitions, as ticket 08 did. It is **not** a
  local wildcard with its condition disabled. `Additional Attack by Loop` uses a wildcard, which is
  why its hotkey cuts a live swing; the fork refuses mid-swing instead and defers, so that both
  kinds of hotbar press behave the same way under the same conditions. Rejected alternative:
  wildcard entry, because it would make a Weapon Art press cancel a swing while a cast press does
  not, and it contradicts ADR-0005.
- Exit blending is the fork's own transition effect. The perceived abruptness of a Terminal Art is
  tuned here, and is independent of whether the clip opens a chain window.
- `SH2_Art_State` reuses **Cast Plant**: WASD capture follows `ArtDriver::is_active()`, and the
  clip generator is wrapped with the same `bAnimationDrivenIsActiveModifier` as the Driver Cast
  states. This is input lock, not a freeze — AMR / clip translation still apply.

### Selection

- The Art Selector is a SH2 global, written natively immediately before entry and cleared on exit.
  Zero means no SH2 art is live. OAR or PIE may read it. It is not a worn-item keyword.
- A catalogue row names the clip (any MCO-annotated HKX). Adding an art is a row plus a file
  already in the load order. How that file is attached to `SH2_Art_State` is ticket 03.
- Rejected: importing Ashes of War's AABL path, hotkey, or slot-55 identity as SH2 machinery
  (ADR-0009). One Nemesis generator per art is also rejected unless ticket 03 proves it is the
  only way to attach arbitrary HKX.

### Chaining

- On state entry the fork normalises MCO's combo counter variables to their first-attack value,
  then leaves the clip's own annotations to decide everything else.
- This is deliberately uniform and requires no per-art classification. A Chaining Art writes the
  same value a fraction of a second later, so the normalise is idempotent; a Terminal Art writes
  nothing, so the normalise is the only thing that prevents a stale combo step surviving into the
  next ordinary swing.
- Rejected alternative: recording terminal-vs-chaining per art and guarding only the terminal ones.
  It duplicates a fact the clip already states, and a wrong flag fails silently.

### Slot and binding

- A new slot kind holds an **art id**, not a FormID. Every FormID-keyed path in the hotbar —
  assignment, serialisation, icon resolution, the bind menu — needs an identity for it. Serialised
  bar data gains a version.
- Art definitions are data, following the existing spell-data loader precedent: id, display name,
  icon, Art Selector value, stamina cost, cooldown, and global cooldown. Balancing is a data edit.
- The press routes through the existing input-mode dispatch, so the existing Papyrus slot-activation
  function drives it unchanged.
- Cost and cooldown are the fork's, using its existing casting-instance machinery, so the hotbar can
  render the cooldown. `Additional Attack by Loop`'s spells and perks are not consulted.
- A refused press — unaffordable, on cooldown, wrong stance — highlights the slot in the error state
  the other slot kinds already use.

### Mid-swing

- A press during a live MCO attack is refused by the graph, exactly as a cast press is, and the
  intent is handed to ShoutMCO's cast-intent API under ADR-0005, revalidated and executed once on
  release. This is the same seam ticket 04 builds; the payload differs, the policy does not.

### Ownership (ADR-0001)

- **Core Fork**: the slot kind, the art data loader, the Art Selector write, the graph patch, the
  normalise policy, the deferral adapter.
- **Compatibility Package**: the `Ashes of War` integration, and any Nolvus-specific gating such as
  the per-weapon-class perk checks that modlist's own script adds.
- The graph patch living in the core fork repeats ADR-0006's recorded deviation and is accepted on
  the same terms.

### Using Ashes of War as a clip pile

- Concept only: named special attacks. Not its items, hotkey, or AABL-only path.
- A generator may read an installed Ashes of War folder tree and emit catalogue rows that *point
  at* those HKX files in the VFS. No copies. Missing/renamed folders fail loudly.
- People who still use Ashes of War the old way keep that path; SH2 arts do not go through it.

## Testing Decisions

A good test here exercises externally visible behaviour through the seam a real press uses, and
never asserts on the fork's internals. The project's standing evidence rules apply and are not
relaxed: a behavioural claim about the graph or input timing needs a live observation, a claim
about which animation plays needs a **captured frame**, and a build plus static inspection
diagnoses but never establishes that an integration works.

**Seams — one existing, none new.** The existing Papyrus slot-activation function drives a slot
through the same input-mode dispatch a real key does, including any native input event the mode
queues. A new slot kind handled in that dispatch is therefore covered by the existing seam with no
new test surface. Everything else needed is already readable without adding anything:

- Art Selector value — read the global.
- MCO combo counter after an art — read the animation variable.
- Which OAR submod won — OAR's own in-game menu and log.
- Entry accepted or refused — the graph notify return, already logged by the driver.
- What actually played — a captured frame, per the project rule that record identity proves
  attachment and never appearance.

Prior art: the mco-integration effort's ticket 08 established this exact loop — drive the slot from
Papyrus, read the notify return, confirm with the owner's eyes on a frame — and its fixture rules
carry over unchanged (the owner's latest save used read-only, the fingerprinted profile, fixtures
restored and the game closed afterwards).

Proportion the evidence. One discriminating control beats many absolute measurements: a single
paired press — one Terminal Art, one Chaining Art, combo counter read after each — settles the
normalise policy, and no amount of per-art measurement adds to it.

## Out of Scope

- Authoring or editing animations. The fork plays clips; it does not make them.
- Redistributing `Ashes of War`'s clips, or any other Art Pack's.
- Reimplementing `Additional Attack by Loop`'s stamina spells, sweep perks or damage perks. The
  fork's own cost and cooldown replace them for arts it drives; that mod's own hotkey is untouched.
- Weapon arts for NPCs. Player only.
- Arts outside melee. The weapon behaviour is the only graph in scope; sheathed, bow and staff
  stances have no state to enter and are not given one.
- Editing the sibling MCO shout behaviour engine from this repository. Consuming its published
  cast-intent API is in scope.
- Per-art tuning of release timing beyond the clip's own annotations.
- Publishing modified binaries.

## Further Notes

Measured during exploration on 2026-08-12, by static inspection and annotation dumps of the
installed modlist. Recorded so the next agent does not re-derive them:

- The base `AABL_Attack_A.hkx` shipped by `Additional Attack by Loop` carries **zero annotations**
  and a 1.83s duration. It is inert. Any placeholder the fork needs at that path is genuinely
  empty of behaviour.
- A named art (`Blood Flurry`, 9.0s, 1120 annotations) carries **no MCO window annotations** — it
  is Terminal. A stance-framework default (`Ashes of War Sword Mid`, 2.17s) carries the full
  chaining set plus a payload that sets the combo counter to its first value at 0.06s. **Both
  kinds ship in the same pack**, which is why the normalise policy is uniform rather than
  per-art.
- Clip motion comes from Animation Motion Revolution annotations, not from the behaviour project's
  registered motion data, so arts sharing one registered path is a small risk rather than a large
  one. Animation Motion Revolution, Precision and Payload Interpreter are all present in the
  target modlist and the clips depend on them.
- `Ashes of War` art identity is currently a slot-55 clothing item's keyword across 57 items; that
  entire mechanism becomes redundant for arts the fork binds, and is left alone rather than
  removed.

Architectural decisions: ADR-0009 (any MCO clip; SH2/PIE machinery; Ashes of War is concept not
system), ADR-0008 (Art Selector is SH2's live-art name). ADR-0007's AABL path contract is
superseded.
