# Ticket 05 progress — 2026-08-11, the MSCO-drive approach (handoff option 1)

The owner asked for an entirely new approach after the voice-pipeline driver kept T-posing.
This session implements the owner-blessed plan B: stop borrowing shout states and drive
**MCBO's own casting states**, the ones its Nemesis patch builds and its clips are authored
for. The voice driver is retired in place (still compiled, nothing calls it) because option
3 (ESAS-style) would reuse it wholesale if this fails.

## The real MSCO contract (read from source, corrects the handoff)

The handoff's gate names `bMSCO_LeftCasting` / `NotCasting` **do not exist** — not in
MSCO.dll's source (`C:\Nolvus\Projects\magic-casting-behavioral-overhaul\ref\src`), not in
the Nemesis patch (`Magic Casting Behavior Overhaul\Nemesis_engine\mod\msco`). What actually
exists, verified in both places:

- **Entry recipe (what MSCO.dll itself does on `BeginCastLeft`):**
  `SetGraphVariableBool("IsCastingLeft", true)` then `NotifyAnimationGraph("MSCO_start_left")`
  (`AnimEventFramework.cpp:318-334`). Right/dual analogues: `IsCastingRight`/`MSCO_start_right`,
  `IsCastingDual`/`MSCO_start_dual`.
- **The transitions need no gate variables at all.** `magicbehavior/#msco$2` is the wildcard
  transition array installed on `MagicCastingRootBehavior` (`#1010`, in `magicbehavior.hkx`):
  `MSCO_start_left→51`, `MSCO_start_right→50`, `MSCO_start_dual→52`, `MSCO_start_lr→53`, every
  one `condition null`, `FLAG_IS_LOCAL_WILDCARD`.
- **`bIsMSCO` is an output, not a gate:** bound to `bIsActive0` of an is-active modifier
  (`#msco$30`), so the graph writes it true while an MSCO casting state is active. MSCO.dll
  reads it exactly that way. It is our "the entry was accepted" signal.
- **Exit:** `MCO_EndAnimation` is the only transition-triggering exit event; the clip's
  `MCO_Recovery` annotation arms it. On state exit the graph raises `CastingStateExit` and
  MSCO.dll resets all three `IsCasting*` vars **and calls `InterruptCast` on both hands**.
- **Commitment:** the original clips carry their SpellFire annotations (`MSCO_left1.hkx`:
  `MLh_SpellFire_Event` at 0.483s, duration 1.667s, plus `MCO_Recovery`, MSCO combo windows,
  and PIE combo-index payloads — dump in `_animations\msco\Base - default\_inspect`).

## What changed in the plugin (branch `claude/spellhotbar2-mco-animation-a6629d`)

- New `src/casts/msco_cast_driver.{h,cpp}`: `begin(pc, hand)` = set the hand's `IsCasting*`
  var + notify `MSCO_start_*` (logs the notify return); `is_active` = read `bIsMSCO`;
  `cancel` = `MCO_EndAnimation` + drop vars; `replay` re-enters for concentration loops;
  `finish` clears leftovers only when the state is already gone (never disturbs a recovery).
- `casting_controller.cpp`: the four `start_*` entries call `MscoCastDriver::begin` instead
  of the voice begin/press pair; `is_anim_ok` reads `bIsMSCO` instead of `IsShouting`; the
  timed voice release blocks are gone (the clip's annotation owns payload timing, ADR 0004
  unchanged); commit/cancel/reset paths swap to the new driver. Entry grace stays 1.5s.
- `animationeventhook.cpp`: the commitment tags are now `MLh_SpellFire_Event` /
  `MRh_SpellFire_Event` (was `Voice_SpellFire_Event`).
- `plugin.cpp`: no more `VoiceCastDriver::initialize` — no runtime dummy shout is created.
- Build: clean, deployed to `Dev - Spell Hotbar 2` (post-build copy verified 09:59).

The probe mod `Spell Hotbar 2 - MCBO Cast Animations` is now inert under this path (nothing
plays shout states any more) — disable or leave; it no longer participates.

## What this session did NOT verify (the in-game questions, in order)

1. **Does `MSCO_start_left` transition when no spell is equipped?** The wildcard lives in
   `magicbehavior.hkx`'s `MagicCastingRootBehavior`. MSCO's own use always has a spell
   equipped in the hand; whether that state machine is in the active graph branch with a
   weapon (or nothing) in both hands is unknown and is the first thing the log answers:
   `MSCO cast: notified MSCO_start_left -> {sent}` then `casting state active became true`.
2. **Does the winning clip keep its annotations?** OAR may override the `MSCO_left*` clip
   paths in this load order; an override without the SpellFire annotation plays but never
   commits — visible as anim-plays-no-spell in the log.
3. **Concentration is expected rough:** the clips are fire-and-forget shaped, and
   `CastingStateExit` interrupts both hands when the state exits, which will cut a channel
   that outlives the clip. FNF (Incinerate, slot 0) is the acceptance test; conc is polish.
4. **Vanilla-cast interference:** starting a hotbar cast while a real equipped-spell MSCO
   cast is mid-swing restarts the state under it. Accepted for the probe.

Acceptance stays what the handoff set: **the owner's eyes or continuous video.** Sparse frame
captures do not count.

## Codex review (pre-test gate), 2026-08-11

Codex session `019ff159-6555-74d0-97e2-7fe30f30b75c` reviewed commit `3263243`; six findings,
three fixed in the follow-up commit, three dispositioned:

- **Fixed — spellfire/hand correlation:** a SpellFire event from a hand the cast does not
  throw with no longer commits it (`spellfire_mask`, armed per cast right before the MSCO
  entry). Residual, accepted: a simultaneous vanilla cast from the SAME hand could still
  commit a pending hotbar cast early.
- **Fixed — entry grace now closes on confirmed entry:** once `bIsMSCO` has been seen true,
  losing the state pre-commit cancels immediately instead of consuming up to 1.5s more grace.
- **Fixed — first-frame concentration cancel** now ends the accepted animation
  (`MscoCastDriver::cancel`) and resets the animation globals instead of orphaning both.
- **Accepted by design — cast time does not pace the release:** ADR 0004 puts the payload on
  the clip's throw frame (0.483s for MSCO_left1), so a slow spell commits earlier than its
  authored cast time. Pacing the clip (or picking variants) by cast time is the polish step
  once anything plays at all. Same for the concentration duration window still being measured
  from the old release time.
- **Accepted, known-rough — concentration beyond the first clip exit:** `CastingStateExit`
  interrupts the channel and `replay` restarts only the animation. FNF is the acceptance
  test; conc needs its own pass.
- **Accepted — `MCO_EndAnimation` only sent while the state is active:** cancelling inside
  the ~2-frame entry window can leave one ghost clip playing out. The alternative
  (unconditional send) risks cutting an unrelated MCO attack, which is worse.
- **Deliberate — the `IsShouting` gate in `try_start_cast` stays:** no hotbar cast may start
  during a real shout.

## Test recipe (unchanged from the voice session)

Save `Codex_T05_Smooth_Riverwood`; `SpellHotbar.loadBarsFromFile(<worktree>\.scratch\
evidence\bars-fixture-probe.json, same)`; cast via `SpellHotbar.castSlot(0)` (DevBench
papyrus, window focused) or the owner pressing "1"; watch `SpellHotbar2.log` debug lines and
the OAR animation log.

## 2026-08-11 in-game smoke test — entry REJECTED; root cause is the load order, not the code

Run: save loaded headless via DevBench, bars fixture loaded, `castSlot(0)` at 10:36:16 with
**Incinerate genuinely equipped in the left hand** (best case for the graph), third person:

```
[10:36:16] MSCO cast: notified MSCO_start_left -> false
```

The owner then pressed "1" live at 10:48:51-54 — six more presses, six identical
`MSCO_start_left -> false` rejections through the real input path. The driver's fail-safe
behaved as designed each time: variables dropped, instance reset, no stuck controls, no
ghost cast. The code path is fine; the event name does not resolve in the runtime graph.

**Root cause — CORRECTED after two retractions (the section below supersedes what this
note first claimed and what was said in-session):**

Two claims made during diagnosis were WRONG and are retracted with evidence:

- ~~"The msco Nemesis patch was never built"~~ — **false.** The last Nemesis run
  (07-08-2026 19:48, `Nemesis Output\Nemesis_Engine\cache\mod settings`, 29 patches) had
  `msco` ticked ("Mod Checked 17: msco", PatchLog) and compiled `magicbehavior` clean. MO2
  wrote each output file **in-place into that path's winning provider**, which is why
  `Nemesis Output` looks incomplete: the merged `magicbehavior.hkx` lives in **TK Dodge
  RE's** folder (mtime 19:48:56, matching the PatchLog to the second) and DOES contain
  `MSCO_start_left` (binary grep = 1; the session's earlier `strings`-based grep returned a
  false 0 and was trusted — do not use `strings` for hkx greps, use `grep -ac`). The merged
  root `0_master.hkx` lives in **Jump Behavior Overhaul's** folder; the character project
  `defaultfemale.hkx` (also rebuilt 19:48:53) registers the MSCO clips.
- ~~"BDI is broken for every mod" / "replace 78146 with 78159"~~ — **false and retracted.**
  The game is 1.5.97 (exe version verified), so 78146 v0.13 is the correctly paired build;
  78159 is the 1.6+ fork. BDI's successes log at DEBUG level (source:
  `github.com/max-su-2019/BehaviorDataInjector`, `DataHandler.cpp` `InjectEvents`) and the
  log level only shows warnings — 33 of 60 stored events (CPR, Smooth Moveset, Dynamic
  Sprint, TK Dodge parries…) never log a failure. BDI is not globally dead.

**What is actually broken — the root event table.** The msco Nemesis patch adds its events
to `magicbehavior.hkx`'s own string data, NOT to the root `0_master.hkx` — binary grep of
the freshly built root: **zero MSCO strings**. Per the thuum project's resolution rule
(`NotifyAnimationGraph` resolves names against the ROOT graph's event table; sub-behavior
tables don't fire externally), an external `NotifyAnimationGraph("MSCO_start_left")` cannot
resolve. MCBO knows this — its `MSCO_BDI.json` exists precisely to inject the names at
runtime before `CreateSymbolIdMap` (the function BDI hooks). For these 15 events on the
live player graph that injection demonstrably does not produce a resolvable name (seven
`notify -> false`, scripted + owner presses; BDI logs `Fail to Inject` into
`MagicBehavior.hkb` where the names now already exist in the built file). Why BDI's
root-graph injection doesn't stick here is UNRESOLVED (its `AddEvent` lives in a patched
CommonLib fork; failure semantics not fetched).

**Implication for MCBO itself:** MSCO.dll enters its states through the same external
`NotifyAnimationGraph` calls, so its animation layer very likely never fired in this load
order — consistent with the owner wanting MCBO's animations and not having them. A real
equipped-cast by the owner (vanilla pose vs MSCO combo swing) confirms or refutes this in
ten seconds; console `player.cast` does not exercise that pipeline and proves nothing.

## 2026-08-11 owner correction — MCBO WORKS; the analysis above is re-scoped

**Owner statement (authoritative): "Magic Casting Behavioral Overhaul has been working great
the entire time"** (sole known issue was a Valhalla timed-block interaction, since resolved).
That falsifies the "MCBO's animation layer never fired here" implication above and PARKS the
0_master surgery recommendation — if MSCO.dll's `NotifyAnimationGraph("MSCO_start_left")`
demonstrably works in this load order, the name IS deliverable from its call site, and a
root-table gap cannot be the whole story.

What survives, per the thuum project's tested semantics (CONTEXT.md ~382, ~2227): the return
value means *received*, not *consumed*; mod-declared names returning `false` where vanilla
names return `true` means non-delivery. So our seven `false` results are real non-delivery —
**the same event name delivers from MSCO.dll's call site and does not deliver from ours.**
Same API (`actor->NotifyAnimationGraph`), same actor, third person both.

Candidate discriminators, now the open question:

- **Call context.** MSCO.dll sends from inside the `BSAnimationGraphEvent` ProcessEvent
  vtable hook (mid graph-event dispatch). Ours send from the Papyrus VM thread (`castSlot`)
  and the input path (owner's "1"). If context is the difference, relaying our send through
  our own animation-event hook context is a small code change.
- **Session state.** BDI injects at `CreateSymbolIdMap`, guarded by `!eventIDMap` — a graph
  instance that misses its injection window may stay unresolvable for the whole session.
  The owner's "works great" is from normal play; nobody has confirmed MCBO working in THIS
  session's DevBench-loaded state.

**The 5-second discriminating test (owner-driven, zero code, one session):** with Incinerate
equipped, (a) cast it normally with the cast button — does the MSCO combo swing play? then
(b) press "1" — read the `MSCO cast: notified ... -> ?` log line.
- (a) plays + (b) false → call-site difference → relay our notify through the graph-event
  hook context and retest.
- (a) vanilla-pose + (b) false → session-state / injection-window problem → chase BDI's
  injection timing for this load path; owner's normal sessions differ somehow.
- (b) true → whatever changed, proceed straight to observing the animation.

## 2026-08-11 call-site fix — the graph-event relay (commits 412c166 + 5a5e29b)

The owner's 10:48:39 equipped cast settled the discriminator with telemetry that already
existed in `MSCO.log`: `BeginCastLeft` → `LeftMSCOStart` (state entered) → `replaceNode` →
`consumeResource` — MSCO.dll's `MSCO_start_left` delivered and transitioned from inside
graph-event dispatch, twelve seconds before our seven calling-thread sends returned false
on the same graph. Session state and BDI are exonerated; the call site is the variable.

Fix (copying MSCO.dll's own working pattern): a send that fails on the calling thread parks
in an atomic and `Animation_event_hook` delivers it from dispatch context on the next
player graph event (`MSCO cast: relay carried by graph event '<tag>'` in the log). Codex
review (recovered from the job log after the companion runtime restart dropped the report):
the hook now chains the original vtable implementation BEFORE relaying so earlier handlers
finish with the carrier (CastingStateExit cleanup vs re-entry inversion), and `finish()`
preserves a parked `MCO_EndAnimation` while the state still plays. Accepted residual: an
already-claimed in-flight start can land just after teardown — rare, cosmetic (ghost clip),
self-healing via MSCO.dll's CastingStateExit cleanup. Open empirical question for the next
run: graph-event arrival rate while standing idle (the relay needs one carrier event within
the 1.5s entry grace; the log's carrier line answers it either way).

## Load-order hygiene (owner question, separate from this ticket)

MO2 has **no output redirection configured for the Nemesis executable** (no
`customOverwrites` in `ModOrganizer.ini`), so Nemesis writes in-place into each output
path's current VFS winner — the scatter that misled this session (built `magicbehavior.hkx`
inside TK Dodge RE, built `0_master.hkx` + `defaultfemale.hkx` inside Jump Behavior
Overhaul). Once-and-for-all fix: in MO2, edit the Nemesis executable and set **"Create
files in mod" → `Nemesis Output`**, then after the next run restore TK Dodge RE's and JBO's
original files from their archives (paths in each mod's `meta.ini`) so foreign mods stop
carrying Nemesis outputs. Until that cleanup, TK Dodge RE's and JBO's copies ARE the live
merged behaviors — do not "restore" them without a redirected rebuild in the same step, or
the game loses the merged graph.

**Session hygiene / environment changes this session:**

- Skyrim closed (qqq), save untouched, fixture bars were runtime-only and discarded.
- New MO2 mod **`Dev - Display Tweaks Quarter Window`** created and enabled at top priority
  (modlist backup `modlist.txt.bak-devdisplay-*` beside it): 1720x720 borderless +
  `LockCursor=false`, dev-only for headless testing; **disable before owner
  visual-acceptance sessions**. Takes effect next launch; if the window comes up full-size,
  refresh MO2 (F5) — the modlist was edited while MO2 was open.
- Headless finding: this setup **pauses the game main thread when the window is unfocused**
  (no `bAlwaysActive`). DevBench queues commands but they only flush while focused. Prior
  sessions' "background" runs worked because the game window happened to keep focus.
  Options: grab focus per test burst (owner authorized), or set `bAlwaysActive=1` in the
  profile Skyrim.ini — owner's call, it changes normal play (world runs while alt-tabbed).
- Cosmetic: `castSlot` log line still says "queued voice press" — stale wording from the
  voice driver, fold into the next cleanup.

## 2026-08-11 retest verdict — option 1 FALSIFIED (three independent sources)

Owner retest ~12:57 (relay build, commits 412c166+5a5e29b): pressing "1" with Firebolt
slotted did nothing; an equipped MCBO reference cast and a shout both worked.

**Telemetry (SpellHotbar2.log):** the relay worked as designed and still failed —
`notified MSCO_start_left from graph-event context -> false` five times. The event does
not deliver even from inside BSAnimationGraphEvent dispatch. The call-site hypothesis is
dead: delivery context was never the difference.

**MCBO's real mechanism (MSCO.log, the 12:57:39 reference cast):**

```
BeginCastLeft: 'Firebolt', CastingType = kFireAndForget   <- vanilla anim event, hooked by its AnimEventFramework
LeftMSCOStart | chargeTime=0.500                          <- its own bookkeeping event
replaceNode: output_node='NPC L MagicNode [LMag]'          <- DLL swaps the animation node
consumeResource: Spell 'Firebolt' cost=33.064
```

MSCO.dll rides the **vanilla animated casting pipeline** (hooks
`AttackBlockHandler::ProcessButton`, `Magic::RequestCastImpl`, and the anim-event stream)
and swaps the animation at the node level. It never enters its graph states from outside.

**Graph dissection (merged magicbehavior.hkx, hkxc → XML, VFS winner confirmed =
TK Dodge RE's copy via housecarl_asset_status):**

- `MSCO_start_left/right/dual/lr` (event ids 239/238/240/241) appear ONLY in transition
  arrays flagged `FLAG_USE_TRIGGER_INTERVAL|FLAG_USE_INITIATE_INTERVAL`: initiate window
  `MSCO_WinOpen`(231) → `MSCO_WinClose`(232), trigger window `MRh_SpellFire_Event`(26) →
  `MSCO_WinClose`. They are **combo-chain events** — valid only while an MSCO clip is
  already playing and its window annotation is open. They were never the entry door.
- **Zero transitions** listen for `LeftMSCOStart`/`RightMSCOStart`/`MSCOExit`/
  `CastingStateExit` (event ids 233/234/235/237). Entry into MSCO clips is not
  event-driven at all; it is the DLL's replaceNode on top of a real cast.
- BDI's `Fail to Inject ... to graph "MagicBehavior.hkb"` for every MSCO event is the
  benign duplicate case (the names already exist in the merged graph's 264-event table).

**Conclusion:** no event sent from a mod can enter MCBO's casting animations from idle.
The only entry is the vanilla animated casting pipeline itself. Our spell delivery via
`CastSpellImmediate` (instant caster, `kOther`/`kInstant` source) bypasses everything MCBO
hooks.

**Candidate plan C (not yet owner-blessed):** make the hotbar cast a *real* hand cast —
prime the hand's `ActorMagicCaster` (currentSpell) and enter the vanilla charge states via
vanilla event names (`MLh/MRh_SpellAimedStart` chain — vanilla names deliver from any call
site per thuum semantics), so the graph raises `BeginCastLeft` and MCBO skins the cast
exactly as it does an equipped one. Encouraging detail: in the reference cast MSCO logged
`GetEquippedSpell: No spells` and `GetCastingSpell: No magicCaster->spell` yet still
resolved 'Firebolt' and chargeTime — it is robust to sparse spell context. The spellfire
mask, entry grace, and anim-event hook all carry over. Alternative remains handoff option
2 (dedicated Nemesis state, weeks). Handoff option 3 (ESAS engine cast) does NOT deliver
MCBO anims — an engine voice/instant cast is not a hand cast and MCBO ignores it.

## 2026-08-11 route confirmed + Nemesis hygiene EXECUTED

Owner picked **plan B (dedicated Nemesis state), minimal slice first** after the A-vs-B
brief (`~/.agents/briefs/2026-08-11-plan-a-vs-b.html`).

Hygiene applied (game effect: none — byte-identical files, new provider):

- Pre-seeded `Nemesis Output` (priority 607, above both carriers) with the 7 merged files
  Nemesis had written in place: TK Dodge RE's `behaviors/1hm_behavior.hkx` +
  `magicbehavior.hkx` (3rd + 1st person) and JBO's `0_master.hkx`, `defaultmale.hkx`,
  `defaultfemale.hkx`.
- Restored both mods' shipped originals from their archives (F:\Nolvus\ARCHIVE\...; both
  archives ship all 7 files — they were legitimate mod files Nemesis overwrote, not
  droppings).
- Verified via housecarl_asset_status: Nemesis Output now WINS all 5 checked behavior
  paths. Future Nemesis in-place writes land in Nemesis Output permanently.
- Backup of the merged copies: scratchpad `hygiene-backup/merged-droppings.tar`.

## 2026-08-11 plan B authoring spec (verified facts, patch-FID space)

Patch: slug `sh2c`, project folder `magicbehavior`, canonical source
`nemesis/Nemesis_Engine/mod/sh2c/` in this repo, deployed into `Dev - Spell Hotbar 2`.
Template mods verified on disk: Hot Key Skill (`hotkey` — the exact shape: hotkey clip
states appended to magicbehavior), msco, thuum's `shmco` (+ ticket 45 playbook at
`C:\Nolvus\Projects\thuum-fully-animated-shouts-mco\.scratch\shout-mco-engine\issues\`).

- Patch FIDs = Nemesis base numbering, shared by all published patches: `#0077`
  stringdata (12 mods append), `#0078` valueset, `#0079` graphData, `#1346` = ROOT SM
  `MagicBehavior` (vanilla states `#1345 MagicRoot id=4` = startState, `#0084 MRh_Shout
  id=3`), `#1344` = MagicRoot's transition array (2 vanilla entries → toStateId 3), blend
  effect `#0082` (vanilla, reused by hotkey). `temp_behaviors\xml\` in Nemesis Output =
  post-merge reference (different numbering, do not hard-code from it).
- **Do NOT touch `#1346` `wildcardTransitions` — tkds already replaces it (null→#tkds$8).**
  Entry instead appends ONE transition to `#1344`: eventId `$eventID[SH2_CastRight]$`,
  toStateId 746002, transition `#0082`, condition null, flags `FLAG_DISABLE_CONDITION`.
- `#0077.txt`: append `SH2_CastRight`, `SH2_CastExit` to eventNames (MOD_CODE append, no
  ORIGINAL). `#0079.txt`: append 2 eventInfos entries (flags 0) — events need NO valueset
  entry (only variables do; the 3-list ordinal alignment trap is variables-only, thuum
  A45.2b). Vanilla node text base: strip other mods' MOD_CODE blocks (keep ORIGINAL half)
  from msco's/hotkey's copies of the same files.
- `#1346.txt`: append `#sh2c$2` to `states` (ordinary multi-mod append — hotkey/tkds/tkuc
  all do this).
- New: `#sh2c$0` hkbClipGenerator (`Animations\MSCO_left1.hkx`, MODE_SINGLE_PLAY, triggers
  null) — **MSCO_left1 fires MRh (RIGHT-hand) SpellFire at 0.283s** (anno dump; left/right
  clip naming is swapped vs events), duration 1.667, also `MCO_winopen` 0.8 / `MCO_winclose`
  1.2 / `MCO_recovery` 1.2 (lowercase — case vs registered names unverified, do not rely).
  `#sh2c$1` transition array (state-local): `$eventID[SH2_CastExit]$`→toStateId 4 and
  `$eventID[IdleStop]$`→4 (safety, mirrors hotkey), both via `#0082`. `#sh2c$2`
  hkbStateMachineStateInfo: generator $0, transitions $1, name SH2_CastRight_State,
  stateId 746002 (TK uses 5695600 — big unique ints are the convention).
- DLL side (task #11): driver sends `SH2_CastRight` (any hand → right for the slice),
  `SH2_CastExit` on finish/cancel; spellfire commit arms RIGHT (MRh) for MSCO_left1.
  Delivery expectation: event lands only while magicbehavior active (MAGIC STANCE DRAWN —
  acceptance test must draw a spell first); from other stances notify returns false (the
  full matrix later patches 1hm_behavior etc., the TK Dodge distribution pattern).
- Verify loop (task #10): tick patch in Nemesis (name shown = info.ini `name`), run, then
  grep -ac the new event names in Nemesis Output's magicbehavior.hkx + check temp XML.
  Nemesis writes in-place into Nemesis Output now (hygiene done). thuum
  `tools/run-nemesis.ps1` automates tick→launch→wait; `verify-nemesis-patch.mjs` is the
  patch linter pattern.
