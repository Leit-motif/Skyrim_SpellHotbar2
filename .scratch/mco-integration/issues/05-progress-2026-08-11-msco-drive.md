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

**Fix routes (decision pending):**

1. **Root-table override probe (recommended, no Nemesis re-run, reversible):** hkxc
   round-trip the built `0_master.hkx` to XML, append the 15 MSCO event names to the root
   `hkbBehaviorGraphStringData.eventNames` + matching `eventInfos` (flags 0), repack, ship
   as a new top-priority dev mod providing only `0_master.hkx`. If notify then resolves and
   the state plays, the thuum root-table rule is confirmed, our driver works, AND MCBO's own
   casting starts working for the owner. Disable the mod to revert. (Tooling:
   `C:\Tools\SkyrimHKX` hkxc; the thuum project round-trips behavior files routinely.)
2. **BDI forensics:** work out why `AddEvent` fails/doesn't stick on the root graph
   (requires its CommonLib fork's source; heavier, upstream-shaped).
3. **Nemesis-patch route (clean long-term):** author a small patch adding the event names
   to 0_master's string data and re-run Nemesis with the exact 29-patch set from `mod
   settings` — same effect as (1) but permanent and rebuild-gated.

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
