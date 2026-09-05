# Spell Hotbar 2 Integration

This context defines the language used while adapting Spell Hotbar 2 for the user's Nolvus mod list.

## Language

**Nolvus Integration Fork**:
A maintained fork that preserves Spell Hotbar 2's core product while adding the configuration, compatibility, fixes, and selective improvements required by the user's Nolvus mod list.
_Avoid_: Rewrite, successor, total redesign

**Baseline Adoption**:
The first integration milestone: a compatibility check that confirms the base mod behaves normally in the active Nolvus load order and identifies unintended interactions with that environment before customization begins.
_Avoid_: Experimental proof of concept, reduced feature trial, customization milestone

**Direct Cast**:
The fork's primary casting mode, where activating a hotbar slot casts its bound spell directly instead of first equipping it. This behavior is the central reason for adopting Spell Hotbar 2.
_Avoid_: Equip-first casting, secondary test mode

**Installed Configuration**:
The exact set of components and options selected in the user's current FOMOD installation. Compatibility acceptance covers this configuration, not unselected installer alternatives.
_Avoid_: Every installer permutation, theoretical support matrix

**Core Fork**:
The maintained Spell Hotbar 2 source that owns generally applicable native behavior, fixes, and improvements.
_Avoid_: Nolvus patch, load-order bundle

**Compatibility Package**:
A separate integration layer that owns records, presets, configuration, and other adaptations specific to the user's Nolvus load order.
_Avoid_: Core fork, upstream source

**Personal Integration**:
The private runtime result maintained for the user's own Nolvus installation, without a public release or compatibility claim for other users.
_Avoid_: Community release, supported mod-list distribution

**Accepted Baseline**:
The independently validated Installed Configuration before any behavior customization, retained as the known-good reference for attributing later regressions.
_Avoid_: Build success, provisional smoke test

**Dual-Input Compatibility**:
Compatibility of the Installed Configuration with both native keyboard input and native gamepad input, including the user's reWASD mappings between them. That bar includes configuring binds and using the bind menu, not only Direct Cast.
_Avoid_: Controller-only support, keyboard-only support

**Mod Control Panel**:
SKSE Menu Framework's in-game menu that hosts this fork's configuration pages and native editor windows after ADR-0012.
_Avoid_: MCM, SkyUI config, Prisma menu

**ImGui Host**:
The single plugin allowed to own the Dear ImGui context and the presentation and UI-input hooks. This fork is a guest of SKSE Menu Framework, not a second host.
_Avoid_: Shared imgui, dual overlay, SH2 renderer as host

**Material Interaction**:
A repeatable Nolvus-specific conflict, regression, stability problem, or meaningful behavioral deviation from normal upstream expectations that prevents acceptance.
_Avoid_: Harmless warning, unselected feature behavior, theoretical conflict

**Compatibility Evidence**:
The traceable runtime record supporting acceptance, tied to the tested source and binary, MO2 environment, Installed Configuration, saves, input paths, logs, and visible results. For reWASD, record the mappings exercised rather than archiving the full profile unless a defect depends on it.
_Avoid_: Build success, undocumented smoke test

**Driver Cast**:
The mod's casting mechanism: a hotbar cast enters one of `SH2_CastRight_State` / `SH2_Cast2_State` / `SH2_Cast3_State` / `SH2_Cast4_State`, states this mod's own `shtb` Nemesis patch appends to the root state machine of `magicbehavior` and `1hm_behavior`. Entry is the matching `SH2_CastRight` / `SH2_Cast2` / `SH2_Cast3` / `SH2_Cast4` notify's own true return; the clip set is `MSCO_left1` through `left4`, walked by SH2's own cast index. The state ends on `SH2_CastExit`, from its end-of-clip trigger or from the mod. Combo-position restore for a Driver Cast this mod started is this mod's work (ADR-0005 named exception), not ShoutMCO's release-timing API. Supersedes **Shout-Graph Cast**, which described the retired voice path (ADR-0006, tickets 07 and 08).
_Avoid_: Shout-Graph Cast, shout, spell casting animation, magic behavior

**Cast Channel**:
A concentration cast held from a hotbar slot. It enters `SH2_Channel_State`, its own `shtb` state
entered by `SH2_CastChannel`, whose `MODE_LOOPING` generator carries no end-of-clip trigger and so
is held for the whole hold, ending on `SH2_CastExit` at release. The generator's path is
`Animations\1HM_Shout_Inhale.HKX` — the one clip every non-idle concentration submod replaces — so
OAR still picks the per-family clip from `SpellHotbar_SpellAnimationType` (ADR-0013, ticket 28).
A channel does not walk the cast-combo index and does not open the follow-up-press window — both
belong to a clip that is still playing — and it commits on the authored cast-time floor rather
than a clip's SpellFire, because these clips carry none (ADR-0006). It remains a member of the MCO
combo chain: its chain-out window is the hold itself, an attack inside it ends the channel and
becomes the swing, and the hold is credited back against the combo sample's staleness cap so a
hold of any length still hands its position on.
_Avoid_: looping cast state, concentration Driver Cast, held cast clip

**Cast Plant**:
Input lock for a live shtb state: WASD cannot steer or walk the actor out of the clip. The animation may still translate the body through its own `animmotion` keys or overlays — that motion is separate, not cancelled by the plant. Ticket 19 is the plant: a `BSIsActiveModifier` on the state writing `bAnimationDriven`. **It no longer covers fire-and-forget Driver Casts.** Those commit by routing instead — `SH2_Cast{Right,2,3,4}_State` are exclusive full-body states, so entering one deselects the locomotion-blending subtree outright — with `bAllowRotation` for target tracking and no `bAnimationDriven` write at all (mco-integration ticket 39, ADR-0015's 2026-08-29 amendment: the root is authored in the graph, and where a state routes around locomotion it is the routing, not a flag). The Cast Channel and the Ability/Art states still carry the ticket-19 plant unchanged. Abilities ticket 05 applies clip translation from the playing file's `animmotion` (supersedes the deferred mco-integration ticket 21).
_Avoid_: root motion (ambiguous — in Havok/Skyrim tooling that usually means animation-driven translation, not input lock), freeze the actor in place, "the plant" as a name for how a Driver Cast commits

**Chain Window**:
The interval late in a shout exhale during which attack input is honored, letting the animation hand off to an MCO attack. Owned by the separate MCO shout behavior engine, not by this mod — and it governs **shouts only**. A driver cast has no window: an attack press past its commitment point ends the cast state directly, on this side (ticket 10).
_Avoid_: Attack cancel, animation blend, driver-cast chain-out

**ShoutMCO**:
The **internal** identifier for the owner's shout mod — it fixes `ShoutMCO.dll`, `ShoutMCO.ini`, the SKSE log filename, and the header this repo compiles against (`skse_plugin/src/casts/cast_intent.cpp` includes `ShoutMCO_CastIntent.h` and calls `ShoutMCO_CastIntentApi`). Source is the sibling repo `thuum-fully-animated-shouts-mco`.
**It is ONE mod, and it carries several names. None of them is a second mod.** Public name is `Thu'um Reborn` (see below); `Shouts for MCO` is the retired public name, still on `Shouts-for-MCO-1.0.3.zip` and the MO2 folder because renaming those touches the live playtest profile. Reading two names as two mods happened on 2026-08-29 and put a phantom second requirement on the release page.
_Avoid_: Shouts for MCO (as a current name), MCO Shout Behavior Engine (older still), a separate animation pack, "an engine that ships no animations"

**Thu'um Reborn**:
The public name of the ShoutMCO mod, and a hard requirement of this one: every hotbar shout routes through its cast-intent API. **One page, one file** — behavior engine, Nemesis patch and all 216 clips together (sibling-repo owner ruling 2026-08-28: *"no the animations are not a separate file. they will be shipped with the mod."*). It is an upgrade to BOTuser999's `Thu'um - Fully Animated Shouts` (Nexus 50559), whose animation assets it ships with credit. The public name is **settled: `Thu'um Reborn`** (owner ruling 2026-08-29, recorded in the release effort's ticket 01). `Thu'um - Fully Animated Shouts Reborn` was the other candidate and was not taken.
_Avoid_: the animation half of Shouts for MCO, a shout animation pack, an optional requirement

**Cast Driver**:
A mod that owns a cast payload and asks ShoutMCO whether the request should pass through now or be deferred. That call is release timing (ADR-0005). Spell Hotbar 2 is the first driver; it keeps ownership of the slot, spell, resources, and execution. Combo-position continuity across a Driver Cast this mod started is a named exception in the same ADR, owned here, not a second ShoutMCO call.
_Avoid_: ShoutMCO spell integration, engine-owned hotbar slot

**Cast Intent**:
One pending hotbar activation (spell, Ability, or hotbar shout). Last tap wins. Spell Hotbar 2
retains and revalidates it. ShoutMCO owns release only when the player is in someone else’s MCO
swing or a real shout; this mod owns release when the player is in a Driver Cast or Ability.
_Avoid_: Copied spell payload, queued spell object, separate queues per slot kind

**Ability**:
A bindable animation launched from a hotbar slot without equipping anything. The slot holds an ability id, not a FormID. SH2 owns bind, notify, state, Cast Plant, stamina, and cooldown. PIE / clip annotations own hits, windows, and motion. The clip may be any MCO-annotated HKX the load order can see — not only `AABL_Attack_A.hkx` (ADR-0011). Ashes of War is the concept (named special attacks), not the machinery (slot-55 items, AABL hotkey). Pointer-pack ashes and Custom Abilities are both Abilities. An Ability is a member of the MCO combo chain: the swing after it continues the sampled index; it does not reset to hit 1. _Avoid_: weapon art, art (as the product name), power attack, additional attack, ash of war (as a system).

**Ability Selector**:
The TESGlobal SH2 writes to name which Ability is live so OAR or PIE conditions can read it.
Zero means no fork ability is selected. The ESP form is still `SpellHotbar_ArtSelector` (form
`D63`); that identifier is load-order wiring, not the UI name. _Avoid_: art keyword, worn art,
Ashes of War item identity.

**Ability Pack**:
A folder of clips plus catalogue rows that name those files, keyed to the Ability Selector. May be
generated from an installed Ashes of War tree; it is not that mod's OAR-on-AABL machinery. The
on-disk pack folder remains `SpellHotbar2Arts`. _Avoid_: animation mod, moveset.

**Weapon Art Icon Brief**:
The semantic contract written before an Ability icon prompt: frozen animation moment, body
mechanics, Skyrim archetype, clothing construction, weapon paths, causal VFX, camera, palette, and
32 px hierarchy. The owner approves its meaning; the agent owns conversion into image-model prose.
_Avoid_: raw prompt, mood board, generated candidate

**Skyrim Visual Language**:
The project vocabulary that grounds original Ability icons in Skyrim through archetype, material,
construction, weapon silhouette, wear, and physical action. It guides new art without copying game
assets or treating a race name as a costume. Canonical source:
`python_scripts/weapon_art_icons/skyrim-visual-language.md`.
_Avoid_: generic fantasy style, Bethesda asset library, race palette alone

**Ability Class**:
The coarse weapon-class tag on an Ability used for gray-out and refuse: **1H**, **2H**, **Dual**, or **Generic**. Live when the player's current `EquippedType` matches that tag; otherwise the slot is gray and the press is refused. Catalogue files still use an `ArtClass` column. Not OAR `IsEquippedType` and not a worn keyword. _Avoid_: weapon type, ash keyword, stance.

**Custom Ability**:
An Ability the player fills. Core ships drop-in folders `Custom_Ability_1` … `N` (plus extras). Default catalogue name is **Custom Ability N** until the Ability Editor sets another. A catalogue row, not a hotbar slot index. _Avoid_: slot folder, moveset slot.

**Custom Ability Folder**:
The on-disk OAR submod for a Custom Ability (`Custom_Ability_N`). The folder name is the scan key. Name, icon, cooldown, GCD, Ability Class, and Ability Costs live in that folder, not in the save, not in Payload Interpreter config, and not as `name.txt` / `icon.txt`. Custom Ability Spell keys may round-trip in the sidecar unused (ticket 12).
_Avoid_: icon_edits JSON as the definition, co-save.

**Ability Editor**:
The in-game ImGui that configures one Ability: name, atlas icon, Ability Class, cooldown, GCD, and Ability Costs. Opened from the Abilities tab for Custom Abilities and pointer-pack ashes. It does not author clips and does not assign a Custom Ability Spell (ticket 12).
_Avoid_: Weapon Arts editor, ash editor, Spell Editor (that surface edits spell FormIDs).

**Custom Ability Spell**:
Parked (ticket 12). The fire-and-forget spell that would be assigned to a Custom Ability Folder's Payload Interpreter instruction. Until 12 ships, dropped clips fire their author `@CAST` / `@CASTSPELL` / `SoundPlay`. Vanilla Firebolt is the unused sidecar default. Includes ritual fire-and-forget, lesser/greater powers, and voice spells. Not concentration, potions, scrolls, or a TESShout form.
_Avoid_: art effect, Ashes of War payload as SH2 assignment.

**Ability Cost**:
Stamina, magicka, and health spent by SH2 when an Ability starts. Defaults: 25 stamina, 0 magicka, 0 health, 8s cooldown, 1s GCD. The editor may change those numbers. Assigning a Custom Ability Spell (ticket 12, parked) does not seed or overwrite them. The press is refused if any required meter is short. Payload Interpreter costs stay at zero.
_Avoid_: PIE magicka line, shout recovery, Additional Attack stamina spell, cost seeding from an assigned spell.

**Action**:
A bindable hotbar payload that mirrors its slot key onto one authored input rather than starting
a spell, shout, potion, or Ability. An Action has its own stable catalogue id, name, icon, target
source, and optional resource cost, cooldown, and GCD. Power Attack and Dodge resolve their
external scancodes when pressed; a Custom Action may hold a captured physical scancode. The
player's edits to those fields live in `action_overlays.json` under the icon-edits user dir and
are shared across characters, the same as `art_icons.json`; only the slot assignment is
per-character co-save state. An Action does not own an animation, `SH2_Art_State`, Ability
Selector value, or fallback chain. _Avoid_: Ability, hotkey slot, virtual key (that is a separate
integration), click-to-cast.

**Mirror**:
What an Action does with its slot key. The source key owns its target from the accepted down edge
until that same source's up; held and repeat events are mirrored per frame carrying the source's
own held duration. The target, its device, and its user-event name are frozen at the accepted
down, so a rebind, modifier, menu, or mode change cannot redirect the up. A game load defers the
release to the first loaded frame; a mode change releases immediately. _Avoid_: tap, re-running
admission on held events, redirecting the up after a rebind.

**Action Editor**:
The SMF bind-menu dialog that edits one Action's name, icon, target source, captured input, and
costs. Capture arms on the next button down of any device, and Escape cancels. Save writes the
global overlay sidecar; Reset reloads the catalogue row. _Avoid_: MCM, per-character overlay.

**Action Kind**:
The single input seam an Action uses when it is pressed. The current product slice supports only
physical scancodes; engine-control and VirtualKey kinds remain future extensions. The target is
resolved once for that press, and a failed or recursive target refuses visibly. _Avoid_: trying
multiple seams for one activation, storing a live external keycode in the catalogue.

**Action Cost**:
Optional stamina, magicka, and health spent before an Action's input is injected, with optional
Action cooldown and GCD. Zero is free. It reuses the Ability affordability rule and refusal
feedback; it is not a second spell-cost system. Cost is also what decides the live-cast rule: any
costless Action pressed in the committed cuttable span cuts the cast, as the physical key would,
while a costed Action is refused for as long as a cast is live. _Avoid_: charging after a failed
injection, flashing a zero-cost meter, or treating a costless Action's cast cut as a new SH2 cast.

**Terminal Ability / Chaining Ability**:
An ability whose clip does not, or does, carry `MCO_WinOpen`. A property of the clip, never of the binding. Chain-out still happens: WinOpen when present, otherwise HitFrame.

**Committed Action**:
An action the actor cannot steer out of once it starts — an MCO attack, a shout, a Driver Cast, a Weapon Art. Commitment is authored on the behavior state that owns the action and nowhere else (ADR-0015), so every actor entering that state is committed, player and NPC alike. The fork's bar, stated by the owner 2026-08-24: a seamless modernized MCO-feel combat experience with Spell Hotbar 2 as the interface, which means every action a hotbar slot can start commits the same way.
This behavior term is distinct from the catalogue **Action** input payload, which does not itself
enter a committed behavior state. _Avoid_: Movement lock, input block, rooting as a DLL feature,
player-only commitment

## Findings

Verified 2026-07-31 by static inspection of this repository unless noted. The vanilla
`shout_behavior` facts are cross-referenced from the sibling MCO shout behavior engine project,
where they were dumped and verified against the graph itself.

### 1. Casting rides the shout behavior graph, through exactly three events

The mod has no casting graph. Every hotbar cast is three notifications into the vanilla shout
graph: `ShoutStart` to begin, `MT_BreathExhaleShort` to release, `ShoutStop` to cancel. An
exhaustive search of the plugin finds these three literals and no others; nothing overrides the
virtuals that return them.

This is why Direct Cast can cast without equipping, and therefore without occupying a hand —
the property the whole MCO integration exists to preserve.

### 2. The release event is stance-blind, so drawn-weapon casts play the sheathed clip

The vanilla graph's six exhale events encode word count **and stance** together.
`MT_BreathExhaleShort` selects the sheathed short exhale; the `CombatReady_*` events select the
weapon-drawn branch. The mod fires the sheathed event unconditionally.

Two consequences. A cast with a weapon drawn plays the sheathed shout clip, which is a plain
animation defect on its own terms. And every cast lands outside the `CombatReady_*` branch that
MCO chaining is designed around.

### 3. Only the short exhale is ever used

Because the release event never varies, the mod reaches one exhale clip per stance rather than
the twelve-name shout animation API a normal shout pack must cover. The animation surface to
author is correspondingly small.

### 4. Release is timer-owned, not graph-owned, and the cast dies with the shout state

The mod advances its own cast timer and fires the spell itself. It does not use the graph's own
`Voice_SpellFire_Event`, which vanilla places 0.1s into every exhale and which is what makes
vanilla shouts robust to varying clip durations.

It also treats the `IsShouting` graph variable as a liveness check: the moment `IsShouting` goes
false, the cast instance is torn down. Anything that ends the shout state early — including an
MCO chain-out during a Chain Window — destroys the cast and the spell never fires.

**Decided 2026-08-05: the cast gains a commitment point.**
[ADR 0004](docs/adr/0004-commit-a-cast-at-the-graphs-spellfire-event.md) makes the graph's own
`Voice_SpellFire_Event` — the event this finding notes the mod does not use — the instant from
which the spell is delivered regardless of `IsShouting`. Before it, teardown is unchanged. This is
the fork's answer to the engine's ticket 38, and it deliberately builds no cross-mod commitment
handshake: the
engine already refuses to open its window before that same event, so the two rules become one
instant rather than two parties. Ticket 07 implements it.

**Scope correction 2026-08-08:** “no cross-mod API” applies to the *spellfire commitment point*.
It does not apply to scheduling a new Direct Cast behind an active MCO attack. That separate input
problem now uses ShoutMCO's generic cast-intent API under ADR-0005; no commitment handshake was
added.

Note the exposure this closes is not confined to chaining, and not small. The magicka is deducted
only *after* `cast_spell` succeeds, so an interruption in the gap costs the player the spell and
refunds nothing. A ritual cast notifies the exhale 250 ms before it fires, and a ritual
concentration cast 1.0 or 1.5 s before, per animation — a stagger anywhere in that lead loses the
spell today, with no MCO engine involved at all.

### 5. Which animation plays is data, not code

A global set per cast (`SpellHotbar_SpellAnimationType`, `SpellHotbar.esp`) is read by OAR
conditions to pick the clip. Animation id `0` is named "Skyrim Shout" — the vanilla fallback.

A cast that looks like an unmodified vanilla shout may therefore be an unmapped spell rather
than a design limit. Check the in-game spell editor's animation assignment before treating it
as a defect.

Concentration still uses those animation ids (aimed 1001, self 1002, ritual 11001, ward 1003)
but the Driver Cast shtb clips are fire-and-forget single-play. Looping concentration types
are mco-integration ticket 25.

### 6. ~~The MCO shout behavior engine needs no changes to serve this mod~~ — WITHDRAWN 2026-08-05

~~That engine's DLL is a pure animation-event observer, and its ADR-0002 forbids it from reading
shout cooldown or availability state. It cannot distinguish a Shout-Graph Cast from a real
shout. All integration work is therefore on this side of the line.~~

**Both halves are wrong, and the conclusion they support is the load-bearing one.**

**(a) ADR-0002 was mis-cited.** Its decision governs **cooldown** state only — the voice recovery
timer, "is a shout currently available", and anything derived from either. It never mentions
equipment state. Source: `docs/adr/0002-never-inspect-shout-cooldown-state.md` in the engine repo,
which now carries a scope note recording exactly this. The engine side has struck its own half of
the mis-citation (its finding 18).

**(b) "It cannot distinguish" is false.** The engine has read `selectedPower` since its ticket 06
(`ReadWindowKey`) and reads `high->currentShout` in `ReadShoutVariation` — both in
`src/ShoutChainEngine.cpp`, the latter added by a cold review whose comment argues this exact
point: *"widens the read away from the cooldown fields, not toward them."* Reading equipment state
to identify a driver's cast was already established, reviewed and shipping on that side while this
finding claimed it was forbidden.

**So the conclusion "all integration work is therefore on this side of the line" has no stated
justification left.** Finding 12 goes further and inverts it: the engine is not merely *able* to
participate, it is currently the party that is structurally absent, and the connection cannot be
built here alone.

The one part of this finding that survives: its documentation frames consumers as animation
*packs*, and this mod is a second consumer class — a *driver* that enters the shout graph
programmatically with no shout equipped. That note has since been made on that side (its finding
18), and unlike the rest of this finding it did imply engine changes.

### 7. Battlemage separates cleanly except for the part that pays out

The perks, configuration globals and proc magic effect live in the main plugin; only the Custom
Skill Framework menu power lives in the optional battlemage plugin. The proc triggers are
ordinary hit and game-loop event sinks, portable to a standalone plugin.

The proc's benefit is not portable. Near-instant cast and reduced cost exist only inside this
mod's own cast pipeline, where cast time is a synthetic timer it owns and magicka is deducted
manually. Vanilla casting offers neither lever: cost is recoverable through a perk entry point
conditioned on the proc effect, but cast time has no vanilla mechanism and would need its own
hook.

### 8. The MCO engine's chain-out would destroy a cast, so finding 6 needs a caveat

Recorded 2026-08-02 from the engine side, after a session on the sibling project.

**Two amendments, 2026-08-05, and they pull in opposite directions — read both.** The mechanism
below is sound and is still the crux of the whole integration. But it describes a path that is
**not reachable today** (finding 12: the engine never arms on a hotbar cast, so it never cuts one),
and its closing claim about ADR-0002 is withdrawn with finding 6's.

Finding 6 says that engine needs no changes to serve this mod. That is true of its *code* and
misleading about its *behaviour*, and the gap is finding 4's.

The engine's chain works by **cutting the shout with `shoutStop`** and then firing `attackStart`
from the ready state — two existing vanilla transitions fired in order. `shoutStop` clears
`IsShouting`. Finding 4 establishes that the cast instance is torn down the moment `IsShouting`
goes false, and that the mod fires the spell itself on its own timer.

So a chain-out taken during a cast **destroys the cast before its timer fires, and the spell never
goes off.** For a real shout the cut is harmless — vanilla puts `Voice_SpellFire_Event` 0.1 s into
the exhale, so the magic is already out (that engine's finding 1, gate A11). This mod deliberately
does not use that event, which is exactly why the cut is safe there and destructive here.

Two consequences:

- ~~**Chaining out of a Shout-Graph Cast is not a feature to enable; it is a data-loss bug to
  prevent.** Whatever integration is built must either fire the spell before the cut or suppress
  chaining while a cast is live.~~ **Resolved 2026-08-12 by taking the first horn.** The spell now
  fires before the cut: a cast is committed at the graph's own SpellFire annotation (ticket 07,
  ADR-0004), and ticket 10's chain-out will not cut anything that has not reached it. The narrow
  hazard survives — a cut inside the clip's first 0.483 s still cancels the cast — and the
  commitment gate exists precisely to keep the chain out of that window.
- ~~The engine cannot help. ADR-0002 forbids it from reading shout state to tell a cast from a
  shout, and finding 6 is right that the distinction has to be made on this side.~~ **WITHDRAWN
  2026-08-05** with finding 6(a): ADR-0002 governs cooldown state only, and the engine already
  reads equipment state. The engine can help, and finding 12 says it is the only party that can
  start.

### 9. Casts land on the sheathed branch, which is not where MCO chaining lives

A restatement of finding 2 with the engine's own numbers, because the two projects describe the
same transition array from opposite sides.

The engine's design note is explicit that **MCO chaining only ever concerns the `CombatReady_*`
branch** — nested states 1, 12 and 13 out of the inhale's transition array `#0244`. The `MT_*`
branch is sheathed, where there is no MCO attack to chain into.

This mod fires `MT_BreathExhaleShort` unconditionally. **Every cast therefore lands in the one
branch the chain engine does not serve**, whatever the player has drawn. Fixing finding 2's
stance-blindness is not only an animation-quality fix; it is the precondition for any MCO
integration at all.

### 10. The engine's new mid-swing protection does not cover casts

The sibling project shipped a fix on 2026-08-02 for a shout pressed partway through an MCO attack:
the press is held until `HitFrame` and handed to the game after the swing lands, so the attack is
no longer cancelled mid-swing. Its ticket 15.

**It will not fire for a hotbar cast.** The hook is on `ShoutHandler::ProcessButton`, matched on
the `Shout` user event. A cast triggered from a hotbar key never reaches that handler, so a cast
pressed mid-swing still tears the attack down exactly as before.

~~The *pattern* transfers — swallow the input, wait for the swing, replay it — but the hook point
does not. Anything built here needs its own hold on this mod's own input path.~~

**Superseded 2026-08-08:** this mod owns the payload but not an independent hold policy. It asks
ShoutMCO's generic API to pass through or defer, then revalidates and executes once on release.
That keeps one authority for MCO **release timing** and avoids duplicating `HitFrame`/ready
tracking here.

**Scope note 2026-08-12:** that sentence is the release-timing rule. Sampling attack/ready tags to
restore `MCO_nextattack` / `MCO_nextpowerattack` across a Driver Cast this mod started is
ADR-0005's named exception, not a second hold policy. See ticket 12.

**Mechanism 2026-08-24 (ADR-0014):** that restore lands in the **root** graph, because Havok links
a nested graph's variable names to the root's ids and `MCO_Attack.hkb` reads the root's storage
when it reactivates. The `shtb` patch's `0_master` declaration of `MCO_nextattack` is what makes
that storage exist; without it every weapon replays `attack1`. The value itself is never derived —
a successor table learns `playing → advance` pairs from the clips' own `@SGVI` annotations, which
arrive as event `Pie` carrying a **payload**, not as a tag.

One correction to carry across with it, because it cost a build to learn: **`MCO_WinOpen` is not
proof a swing happened.** On the measured power attack the window opens ~180 ms *before*
`HitFrame`, and gating on it produced clean shouts with zero swing events. That inverts the
ordering in the engine project's own finding 6. `HitFrame` is the event that means the hit landed.

### 11. Fixture hazard: a higher-priority shout pack can shadow this mod's animations

This mod's OAR submods declare `"priority": 99000010` (56 submods under
`meshes/actors/character/OpenAnimationReplacer/SpellHotbar2/`, 1040 `.hkx`, and **no Nemesis
patch** — selection is entirely OAR conditions on its own globals plus `IsPlayer`).

`SYHO - Shout Your Heart Out` declares **99999990** and is enabled in the sibling project's
`Dev - Skeleton` profile. OAR picks the highest-priority submod whose conditions pass, so if
SYHO's conditions pass during a cast it wins and this mod's clip never plays.

Not confirmed as an actual conflict — SYHO's own conditions have not been read — but it is the
first thing to check before treating a wrong cast animation as a defect here, and it is a second
reason (with finding 5) that a cast looking like a vanilla shout may not be a code problem.

A `Dev - Spell Hotbar 2` mod folder already exists in that MO2 instance alongside `Spell Hotbar 2`.

### 12. The engine never arms on a hotbar cast, so the two mods are safe and disconnected

Recorded 2026-08-05 from the engine side, which drove **the first Spell Hotbar 2 hotbar cast ever
traced in either project**. It inverts the premise both repos had been working from, and it is why
findings 6 and 8 carry corrections.

**A hotbar cast does not raise `BeginCastVoice`.** That is the engine's arming event. Without it
its `BeginShoutLocked` never runs, `shoutActive` is never set, no window is scheduled, and an
attack press during a cast is handed straight to the game:

```
>>> INPUT forwarded to the game (down edge, shout inactive, engine enabled)
```

As far as the engine's hook sees, a hotbar cast raises only `Voice_SpellFire_Event` → `shoutStop`
→ `inRdy`. **No press taken, no cut sent.**

**Control, run in the same session so the absence means something:** an ordinary shout key does
raise `BeginCastVoice` and arms with `>>> SHOUT begin (shout 00013E07)`. The instrument works; the
absence belongs to the hotbar cast, not to the trace.

Two consequences, and they are opposite:

- **Safe today.** The engine is structurally absent from a hotbar cast, so it cannot destroy one.
  Finding 8's data loss is unreachable rather than merely unlikely. The engine's release docs no
  longer call the two mods incompatible.
- **Disconnected today, and the obvious next move is the dangerous one.** Nothing chains, which is
  the feature this integration exists to get. Arming the engine on this mod's own events
  (`ShoutStart` or the exhale) *without first solving finding 8's cut* is **strictly worse than
  today**: today there is no chain and no harm; that change gives a chain that silently eats the
  spell. See ticket 03.

Evidence, in the engine repo (read-only from this side):
`.scratch/shout-mco-engine/T37-hotbar-cast-then-attack-2026-08-05.log`,
`.scratch/shout-mco-engine/T37-session-RAW-ShoutMCO-2026-08-05.log`, and the write-up
`.scratch/shout-mco-engine/issues/37-do-not-cut-a-driver-cast.md`. The original graph-entry
integration is that repo's ticket 38. **Current 2026-08-08 input-entry work is ShoutMCO ticket 50
plus this repo's ticket 04; ticket 38's already-built driver-cast chain-out remains separate.**

Fixture: binary `969C59D6` (v1.0.3.0), profile `Nolvus Awakening`, the owner's own recent save
rather than `RD-A67`, slot 0's keybind read live as DIK 2 via `SpellHotbar.getKeyBind(0)`.

**Not established by that session, and nobody should repeat it as settled:** whether the spell
actually fires on a hotbar cast was never objectively confirmed. Magicka read 339/339 after the
trials, but it also reads full at rest and regenerates in well under a minute, so it discriminates
nothing. Every claim above is engine-side — no arm, no press taken, no cut. **Direct Cast mode was
not driven at all**, nor anything beyond one spell in one slot in one configuration.

### Open question

> **Superseded 2026-08-12.** Both questions below interrogate the retired voice-path entry
> (`ShoutStart` notify). The shipped mechanism is the mod's own `shtb` states (see **Hotbar
> Cast** above): entry from drawn idles is owner-verified, mid-swing entry is refused by the
> graph by design, and a cast no longer stakes its life on `IsShouting` — it commits at
> spellfire (ADR-0004, ticket 07). Kept for the record of how the question narrowed; ticket 01
> is closed as superseded.

Whether `ShoutStart` is honored while the player is inside an MCO attack state. If the graph
refuses the transition, the liveness check in finding 4 fails on the first update and the cast is
swallowed silently. Not answerable by static inspection.

**Narrowed 2026-08-02, not closed.** A real shout pressed mid-MCO-attack *is* honored: the sibling
project drove it three times on a live fixture and the graph entered the shout every time, tearing
the outgoing attack down through `MCO_AttackExitNotify` → `attackStop` → `inRdy` about 3 ms later.
So the graph does not refuse entry from inside an attack state, and the attack is what loses.

That narrows the question rather than answering it. Those traces entered through the shout
**control**; this mod enters by notifying `ShoutStart` directly with no shout equipped. Same
destination, different entry.

What still needs a live test, and it is now two questions rather than one:

- Is the **notify** path honored from an attack state, as the control path is?
- Does finding 4's liveness check survive the teardown pass that entry provokes? That pass runs
  through `inRdy` about 3 ms in. If `IsShouting` reads false at any point across it, the cast dies
  on its first update — for reasons that have nothing to do with chaining, and that would look
  exactly like the graph refusing the transition.

The second is the more dangerous, because it would make casting-from-combat fail intermittently
and be misdiagnosed as a transition problem.
