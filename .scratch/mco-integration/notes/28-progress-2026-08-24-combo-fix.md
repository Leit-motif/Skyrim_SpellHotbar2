# Ticket 28 progress — the combo restore diagnosed and fixed, 2026-08-24

Branch `weapon-arts`, commits `6e1eef6` (per-graph probe), `16e0b82` (SGVI-edge sampling +
begin instrumentation), `fc0c3a1` (begin-time live sample), `0f27b83` (consume at attack
initiate). Tree committed, DLL deployed to `Dev - Spell Hotbar 2` (12:29:34). This answers
handoff (b)'s experiment and closes most of its verdict tree.

## The diagnosis, one paragraph

Handoff (b) asked "does writing `MCO_nextattack` at ANY moment change the clip?" The answer is
that it always did. Live probe read-backs show every write landing in the only graph that carries
the variable (`DefaultFemale`; `FirstPerson` has no such variable — the per-graph trap is closed)
and surviving to the next attack press, and the engine playing exactly the attack the variable
names. The failure was the SAMPLED VALUE: `MCO_WinClose` sampling is post-advance, but a cast
that interrupts a swing lands after the advance (`@SGVI` at WinOpen-time) and before WinClose, so
the interrupted swing never taught its advance and the restore faithfully restored the previous
swing's stale teaching — the owner's `a1 → a2 → cast → a2` and `a1 → cast → a1`, both weapons,
one cause. The latch hypothesis is dead; no seam move / state-machine bypass is needed.

## The fix, three parts

1. **Live sample at `begin()`, gated on graph bool `IsAttacking`** (`fc0c3a1`). Mid-swing the
   graph still holds the interrupted swing's advanced value (measured n=3 during attack2); the
   ShoutMCO deferred re-begin ~270ms later reads the post-stomp 1 with `IsAttacking=0` and
   records nothing. The gate is the whole discriminator.
2. **`@SGVI|MCO_nextattack|N` payload parsing** (`16e0b82`) — built as the primary edge, then
   measured: those tags NEVER reach the animation-event sink under this load order. Kept as a
   dead fallback for packs that do emit them; do not build on payload visibility.
3. **Consume a pending restore at `MCO_AttackInitiate` / `MCO_PowerAttackInitiate`**
   (`0f27b83`). The ready edges (`SBF_ReadyStart`/`MSCO_MagicReady`) never fire on greatsword,
   so a pending restore stuck forever and the WinClose stomp-undo branch rewrote the stale index
   over every later swing's teaching — the combo would pin at the restored attack. The initiate
   tags fire on the fresh swing on every weapon, after the engine has read the variable, and a
   Driver Cast never raises them.

## Evidence (all in `SpellHotbar2.log`, session of 2026-08-24 12:25–12:38)

**1H sword, injected fixture a1 → a2 → castSlot(3) → swing — PASS, clip-identified:**

```
12:25:40.936  begin live sample next=3 power=3 attacking=1 recorded=true
12:25:41.209  begin live sample next=1 power=1 attacking=0 recorded=false   (deferred re-begin)
12:25:43.199  combo restore armed next=3 ... after write_mco [DefaultFemale n=3 p=3]
12:25:44.829  sampled MCO next=4 power=2 at MCO_WinClose                    (a3's own teaching)
```

The post-cast swing's WinClose taught 4 — attack3's unique annotation (a1→2, a2→3, a3→4), so the
teaching identifies the clip: the chain went a1 → a2 → firebolt → **a3**. (The OAR text log wrote
nothing all session — see "OAR logging" below — so clip identity rests on the teaching values,
which the clips themselves write.)

**Greatsword, same fixture — mechanisms PASS, clip identity OPEN:**

```
12:37:21.038  begin live sample next=3 power=3 attacking=1 recorded=true
12:37:21.397  begin live sample next=1 power=1 attacking=0 recorded=false
12:37:23.386  combo restore armed next=3 ... [DefaultFemale n=3 p=3]
12:37:24.942  sampled MCO next=2 power=2 at MCO_WinClose      <- RECORD path: pending was
                                                                 consumed at the swing's
                                                                 MCO_AttackInitiate; no pinning
```

Before `0f27b83`, the same run showed four consecutive WinCloses hitting the stomp-undo branch
(12:27:34–38), i.e. the pending restore never consumed and the stale 3 rewritten over every
teaching. After it, the first post-cast swing records normally.

**Why greatsword clip identity is still open:** this save's active greatsword stance teaches
values that do not match handoff (b)'s Greatsword Neutral dump (a fresh chain taught 1, 2 while
the graph advanced to 3), so the active stance is a different pack than the one dumped and its
teachings cannot name clips. The owner's OAR Animation Log eyeball closes this cell.

## OAR logging: what was learned

- `AnimationLogEntry` lines appear in `OpenAnimationReplacer.log` ONLY in sessions where the
  in-game Animation Log UI is reachable/open (the morning session had it via Risa's menu; three
  agent sessions with identical ini settings wrote zero entries). `uAnimationActivateLogMode=2`
  did not unlock UI-less logging, and injected F13 does not open the OAR UI.
- OAR rewrites `OpenAnimationReplacer.ini` (the copy in `Open Animation Replacer - Nolvus
  Settings`) from its stored state at startup: a `uToggleUIKey` edit (100 → 199/Home) was
  reverted by OAR itself; the `uAnimationActivateLogMode` 1 → 2 edit survived. The toggle-key
  rebind therefore did NOT stick — the OAR UI still wants F13, so Risa's menu remains the way in.
- **Risa's All In One Menu is re-enabled** (modlist edit with MO2 closed, priority preserved;
  no Risa leftovers in Overwrite). Owner asked for this; the OAR UI is reachable through it.

## Injection limits confirmed (also in memory)

- Injected presses reach `PlayerControls` (swings play) but never SH2's own `input.cpp` hook —
  the press probe and mode-2 press-write fire only on physical presses.
- `castSlot(N)` runs the full Driver Cast path including the ShoutMCO deferral, so the fixture is
  faithful for fire-and-forget. A channel still needs a real held key (owner).

## Remaining cells, owner's hands

1. **Greatsword eyeball**: chain two swings, cast, swing — expect the third attack, not a repeat
   or reset. OAR Animation Log via Risa's menu names the clip.
2. **Sword eyeball**: same, expect a3 (agent evidence already says a3; this is the visual
   confirmation).
3. **Channel combo hand-off with a real held key** (Flames): a1 → a2 → hold → release → swing
   should continue at a3. The begin-time sample records on the press that starts the channel
   (mid-swing), and `end_channel` arms the same restore path, so the fix should carry over — but
   a channel enters through `SH2_CastChannel`, whose begin() also runs the live sample; verify.
4. Then the untouched carry-overs from handoff (a): dual self conc `11004`, t0/t+3s frames,
   dual-cast F&F ticket, repo-owned probe disable.

## Probe teardown, later

`combo_probe.h/.cpp`, the `SH2 probe:` log lines, and the begin()/press instrumentation are
throwaway (marked as such in comments). Remove once the owner confirms both weapons and the
channel cell, keeping: the begin-time live sample, the SGVI parser + partial-update cache, and
the initiate-tag consume edges — those are the fix. The ADR handoff (b) asked for should name the
real compat surface, which shrank to: "the clip advances `MCO_nextattack` before a chain-out
press is legal (at or before WinOpen), and the graph holds that advance while the swing is up."
Write it after the owner's confirmation.
