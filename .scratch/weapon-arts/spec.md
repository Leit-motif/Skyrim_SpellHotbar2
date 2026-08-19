# Weapon Arts for Spell Hotbar 2

Status: in progress — 01–03 and 05 resolved; 04 agent-done (owner cells 1–2 open); 07 agent-done (owner cells 1–3 open); 06, 08–09 remain.

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
30. As a player, I want a bound art to sit on the bar even when my weapon class does not match, and
    to gray out and refuse until it does, so a dead press is explained rather than playing a different
    clip.
31. As a player, I want empty numbered art folders I can drop a clip (and name/icon) into, so adding
    an art does not require editing the fork.
32. As a player who does not customize, I want the Ashes of War catalogue when that mod is installed,
    so I have a baseline set without copying anyone's `.hkx`.

## Implementation Decisions

### Vocabulary (belongs in `CONTEXT.md`; proposed here)

- **Weapon Art** — a bindable attack animation played from a hotbar slot without equipping
  anything. _Avoid_: power attack, additional attack, ash of war.
- **Art Selector** — the global the fork sets to name which Weapon Art plays; OAR conditions read
  it. Zero means no fork art is selected. _Avoid_: art keyword, worn art.
- **Art Pack** — a set of OAR submods keyed to the Art Selector. _Avoid_: animation mod, moveset.
- **Art Class** — 1H / 2H / Dual / Generic; gray-out and refuse. _Avoid_: weapon type, ash keyword.
- **Custom Art Folder** — SH2-owned `Weapon_Art_N` drop-in; a catalogue row, not a slot index.
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
- `SH2_Art_State` reuses **Cast Plant**: WASD capture follows `ArtDriver::is_active()`, and the
  clip generator is wrapped with the same `bAnimationDrivenIsActiveModifier` as the Driver Cast
  states. That wrap plants against WASD. Clip `animmotion` is consumed by SH2 (ticket 05): the
  bound file’s keys are applied as XY while the shtb driver is live. The fork does not enter
  another mod’s moving state to get that motion.

### Selection

- The Art Selector is a global in the fork's plugin, written natively immediately before the entry
  event is raised, and cleared when the state exits. Zero is the resting value and means *no fork
  art*, so the author's worn-item conditions win unchanged.
- Art Packs are **this fork’s** OAR submods. Conditions compare the Art Selector (plus player);
  worn keywords and fine OAR weapon-type gates stay on the author’s configs. SH2 does **not**
  copy those ORs onto its `config.json` (that would fall through to Sword Neutral / an inert
  placeholder and still spend stamina). Priority is a reserved SH2 band that wins over stance
  defaults, including Sword Neutral (`1001002544`). Adding an art is a folder and a condition
  file; it requires no change to the fork. Ticket 04’s emit is SH2-owned `config.json` under
  `OpenAnimationReplacer/SpellHotbar2Arts/<Art>/`.
- **Art Class** is data on the catalogue row, not an OAR condition. Values: **1H** (one-handed
  melee, including empty off-hand / shield / spell), **2H** (two-handed melee), **Dual** (two
  melee weapons — own tag; Dual clips are not correct on a single 1H or 2H), **Generic** (any
  of those, including fists / punch-kick while a weapon is held). Bow, staff, and magic gray
  all four until a later staff-arts effort (SH2.mco / Dragon Age-style staff). Mixed Ashes
  (e.g. Elegant Slash = sword or greatsword) collapse to Generic.
- Wrong class: the art **binds anyway**. The slot is gray; a press is refused (error highlight,
  MagFail, no selector write) the same way unaffordable is. This is the WoW dead-ability
  pattern. Rejected: OAR fall-through (plays the wrong clip); play-anyway (jank with no signal).
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
  icon, Art Selector value, Art Class, stamina cost, cooldown, and global cooldown. Balancing is a
  data edit. Distinct per-art icons are ticket 06 (extra atlas, original glyphs, sample first).
- The press routes through the existing input-mode dispatch, so the existing Papyrus slot-activation
  function drives it unchanged. `try_start_art` refuses on cooldown, unaffordable stamina, or Art
  Class mismatch **before** writing the selector.
- Cost and cooldown are the fork's, using its existing casting-instance machinery, so the hotbar can
  render the cooldown. `Additional Attack by Loop`'s spells and perks are not consulted. This fork
  does not require that mod's hotkey or Stances perks for bound arts.
- A refused press — unaffordable, on cooldown, wrong Art Class — highlights the slot in the error
  state the other slot kinds already use, and grays the icon while the mismatch holds.

### Mid-swing

- A press during a live MCO attack is refused by the graph, exactly as a cast press is, and the
  intent is handed to ShoutMCO's cast-intent API under ADR-0005, revalidated and executed once on
  release. This is the same seam ticket 04 builds; the payload differs, the policy does not.

### Ownership (ADR-0001)

- **Core Fork**: the slot kind, the art data loader, Art Class gray-out, the Art Selector write,
  the graph patch, the normalise policy, the deferral adapter, and the empty `Weapon_Art_1`…`N`
  Custom Art Folders (`N` = `max_bar_size` = 12; extra numbered folders still scan).
- **Compatibility Package**: the `Ashes of War` **pointer** pack (catalogue + SH2 `config.json`
  with `overrideAnimationsFolder`, no copied `.hkx`), and any Nolvus-specific gating. Installer
  gates that group on the items plugin.
- The graph patch living in the core fork repeats ADR-0006's recorded deviation and is accepted on
  the same terms.

### The `Ashes of War` integration

- **Pointer pack**, not a redistributed overlay. Ships as Spell Hotbar 2 OAR submods —
  `config.json` under this fork’s Art Pack tree, no animation files copied. Each submod points
  at the author’s existing clip (`overrideAnimationsFolder`). Authoring-time scan of their
  folders is data, not a runtime call.
- Generated by a script that reads an installed `Ashes of War`; both the output and the script
  ship. The installer gates the group on the items plugin. A FOMOD cannot run the conversion.
- A renamed source folder misses; the script exists to make that recoverable.
- Ticket 04 landed the SH2-owned Art Pack: `config.json` only, `overrideAnimationsFolder`
  aimed at the author’s folder, reserved priority `2000000000 + selector`. Worn-item configs
  are no longer shadowed, so story 18 is reachable again at selector 0.
- The generator also stamps **Art Class** by collapsing the author’s OAR type ORs into
  1H / 2H / Dual / Generic (ticket 07). Those ORs stay off SH2 configs.
- Custom arts do **not** live in `Animations - Rapier M` or the stance-default `Ashes of War *`
  folders. Those remain selector-0 / AA. Players drop clips into SH2 Custom Art Folders
  (ticket 08).

### Custom Art Folders

- Core ships `Weapon_Art_1` … `Weapon_Art_12` under `SpellHotbar2Arts`, each a catalogue row
  the bind menu lists. Folder index is not a hotbar slot index.
- The player drops `AABL_Attack_A.hkx` (and optional display-name / icon files) into a folder.
  Extra numbered folders (`Weapon_Art_13`, …) still scan in. Default Art Class is Generic.
- In-game rename / icon pick waits with the Weapon Arts editor (ticket 09). V1 is folder files.
- A dummy template clip may carry a PIE / `SH2_ArtEffect` placeholder if that is cheap; pointed
  AoW `.hkx` files are not annotated. The editor later assigns a spell/MGEF onto that marker.

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
- Redistributing `Ashes of War`'s clips, or any other Art Pack's. The public optional pack is a
  pointer; a user who wants copies drops files into Custom Art Folders themselves.
- Runtime notifies, DLL calls, or ownership variables of MSCO, Additional Attack, or Ashes of
  War. The fork plays clips from its own shtb states and its own OAR directories.
- Reimplementing `Additional Attack by Loop`'s stamina spells, sweep perks or damage perks. The
  fork's own cost and cooldown replace them for arts it drives; that mod's own hotkey is untouched.
- Weapon arts for NPCs. Player only.
- Arts outside melee **in this effort**. Sheathed, bow, and staff have no shtb art state yet; those
  slots gray. A later SH2.mco staff path (Dragon Age-style MCO staff, integrated here) can un-gray
  staff arts; it is not this spec.
- Fine OAR `IsEquippedType` gates on SH2 `config.json`. Art Class is catalogue data.
- Slot-locked folders (`Weapon_Art_1` is not key 1).
- In-game Weapon Arts editor (pick clip, assign PIE spell/MGEF, rename, pick icon). Folder files
  are v1; ticket 09 is the enhancement.
- Editing the sibling MCO shout behavior engine from this repository. Consuming its published
  cast-intent API is in scope.
- Per-art tuning of release timing beyond the clip's own annotations.
- Publishing modified binaries.
- Nordic UI second icon tint (later).

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
  target modlist and the clips depend on them. Disengage’s playing file already carries ~3 m of
  Y `animmotion`; AMR parses it; SH2 consumes it on shtb clips (ticket 05). Dumps:
  `.scratch/weapon-arts/disengage-aow.txt` and `disengage-delia.txt`.
- `Ashes of War` art identity is currently a slot-55 clothing item's keyword across 57 items; that
  entire mechanism becomes redundant for arts the fork binds, and is left alone rather than
  removed.
- Nolvus AoW is two layers: stance-default AA (High/Mid/Low/Neutral × weapon class, Stances perks)
  and 57 named ashes (worn KYWD + often fine type ORs). Wrong weapon on the AA hotkey **falls
  through** to the stance default. SH2 bound arts do not: they gray/refuse (ticket 07). Generator
  skips `Ashes of War *` stance-default folders (no items-plugin keyword).
- Grill 2026-08-18: pointer pack + Custom Art Folders; Art Class 1H/2H/Dual/Generic; WoW gray-out;
  editor later; icons original/hybrid/extra-atlas/sample-first. Dual is its own tag.

Two decisions in this spec are architectural and hard to reverse — the animation path as the
compatibility contract, and selector-keyed rather than path-keyed selection. Both are ADR-0007
and ADR-0008.

## Tickets

| # | Status | What |
|---|---|---|
| [01](issues/01-play-a-bound-weapon-art-from-drawn-idle.md) | resolved | Play a bound art from drawn idle |
| [02](issues/02-bind-a-weapon-art-from-the-bind-menu.md) | resolved | Bind an art from the Binding Menu |
| [03](issues/03-map-an-art-pack-folder-to-a-catalogue-row.md) | resolved | Folder → catalogue row + selector |
| [04](issues/04-own-the-art-pack-in-sh2-oar-directories.md) | claimed — agent 3–6 passed | Art Pack lives in SH2 OAR directories |
| [05](issues/05-consume-clip-translation-on-shtb-states.md) | resolved | shtb states consume clip translation |
| [06](issues/06-procure-weapon-art-icons.md) | ready-for-agent | Distinct icon per named ash |
| [07](issues/07-gray-out-arts-on-wrong-art-class.md) | claimed — agent 4–7 passed | Art Class gray-out / refuse |
| [08](issues/08-ship-custom-art-folder-templates.md) | ready-for-agent | `Weapon_Art_1`…`12` drop-in folders |
| [09](issues/09-weapon-arts-editor.md) | needs-triage | In-game editor (PIE / rename / icon) |
