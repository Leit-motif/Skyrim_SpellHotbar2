# Weapon Arts for Spell Hotbar 2

Status: ready-for-agent

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

The fork owns the trigger, the state and the rules. It does not own the animations, and does not
want to: an art is an attack clip that somebody else authored, and the fork's job is to make one
bindable. Two design commitments follow from that, and everything else in this spec is downstream
of them.

**The compatibility contract is an animation path, not a mod.** `Ashes of War` is 96 OAR
replacements of `animations\AABL_Attack_A.hkx`, and no `Ashes of War` submod references
`Additional Attack By Loop.esp` — only the items plugin that supplies its keywords. Whatever plays
that path gets the art. The fork's own state therefore plays that path, so the existing pack keeps
working with no patch, no repack, and no dependency on the mod whose name the file happens to
carry. This is not a new trick here: the `shtb` cast state already borrows `MSCO_left1.hkx` the
same way.

**Which art plays is data the fork sets, read by conditions somebody else can author.** A slot
holds an art id; pressing it sets an **Art Selector** global; OAR conditions on that global choose
the clip. This is the mechanism the fork already runs for cast animations — 56 submods selected by
`CompareValues` against globals — pointed at attack clips instead. Selector zero means *no fork
art*, at which point `Ashes of War`'s own worn-item conditions win on priority and today's
behaviour is exactly preserved.

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
14. As a player, I want an art pressed in the middle of a swing to land after that swing rather
    than cancelling it, so that a mistimed press costs me nothing.
15. As a player, I want an art to hit and damage what it visibly strikes, so that the animation and
    the outcome agree.
16. As a player who already runs `Ashes of War`, I want my existing arts available on the bar, so
    that adopting this costs me nothing I already had.
17. As a player who already runs `Ashes of War`, I want the worn-item behaviour to keep working
    untouched when no fork art is selected, so that nothing I rely on regresses.
18. As a player who does not run `Ashes of War`, I want the feature to install and behave sanely,
    so that the fork does not require a mod I do not have.
19. As a player, I want the installer to detect whether `Ashes of War` is present and configure
    accordingly, so that I am not asked a question the installer can answer itself.
20. As a player running MCBO, I want nothing here to disturb it, so that both continue to work.
21. As an animation author, I want to add a new art by dropping a clip and a condition file, so
    that extending the set does not require touching the fork.
22. As an animation author, I want to use any MCO attack clip as an art, so that the set is not
    limited to one pack.
23. As a modlist maintainer, I want the `Ashes of War` integration to ship as metadata rather than
    copied animations, so that no redistribution question arises.
24. As a modlist maintainer, I want to regenerate that integration against a different
    `Ashes of War` version, so that a future update does not silently stop working.
25. As the fork maintainer, I want a Weapon Art press to be drivable from a script, so that runtime
    verification does not depend on OS input reaching a focused window.
26. As the fork maintainer, I want art definitions to live in data rather than code, so that
    balancing costs and cooldowns is not a rebuild.
27. As the fork maintainer, I want a bar containing arts to survive a save/load round trip, so that
    bindings are durable.
28. As the fork maintainer, I want a missing or broken art to degrade loudly, so that a silent dead
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
- The state's clip generator names `animations\AABL_Attack_A.hkx`. This is the compatibility
  contract with every existing Art Pack and is the one path in this spec that is deliberately
  spelled out: renaming it is a breaking change, not a refactor.
- Entry is appended to the weapon ready state's own transitions, as ticket 08 did. It is **not** a
  local wildcard with its condition disabled. `Additional Attack by Loop` uses a wildcard, which is
  why its hotkey cuts a live swing; the fork refuses mid-swing instead and defers, so that both
  kinds of hotbar press behave the same way under the same conditions. Rejected alternative:
  wildcard entry, because it would make a Weapon Art press cancel a swing while a cast press does
  not, and it contradicts ADR-0005.
- Exit blending is the fork's own transition effect. The perceived abruptness of a Terminal Art is
  tuned here, and is independent of whether the clip opens a chain window.

### Selection

- The Art Selector is a global in the fork's plugin, written natively immediately before the entry
  event is raised, and cleared when the state exits. Zero is the resting value and means *no fork
  art*, so an installed Art Pack's own conditions win unchanged.
- Art Packs are OAR submods whose conditions compare the Art Selector, at a priority above the
  packs they coexist with. Adding an art is a folder and a condition file; it requires no change to
  the fork.
- Rejected alternative: one distinct animation path per art. It removes the condition file, but
  fixes the number of arts at Nemesis-patch time, requires shipping an inert placeholder clip per
  slot, and still needs condition files the moment an art wants to be weapon-specific.

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

### The `Ashes of War` integration

- Ships as OAR user-override files only — no animation files are copied. An override is a
  full-document shadow of a submod's config, matched by folder path, and can live in a separate mod
  overlaid by the mod manager. Roughly one per existing submod.
- The overrides are generated at authoring time by a script that reads an installed `Ashes of War`,
  and both the generated output and the script ship. The installer gates the group on the presence
  of the `Ashes of War` items plugin.
- The installer cannot perform the conversion itself: a FOMOD is declarative and runs no code. It
  detects and installs; it does not transform. Users on a different `Ashes of War` version
  regenerate with the shipped script.
- Overrides match by folder path, so a renamed submod folder misses silently. The script exists
  precisely to make that recoverable.

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

Two decisions in this spec are architectural and hard to reverse — the animation path as the
compatibility contract, and selector-keyed rather than path-keyed selection. Both should be
recorded as ADRs alongside the implementation tickets.
