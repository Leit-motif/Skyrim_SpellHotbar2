# Ticket 28 handoff (b) — combo-state design review, 2026-08-24

Branch `weapon-arts`, tree committed. This note follows `28-handoff-2026-08-24.md` and answers its
step 1 request: "revisit the combo approach as a whole". No behavior changed this session — the
owner's "do not build further" stands. What changed: comment corrections in `combo_cache.h`,
`msco_cast_driver.cpp`, and `combo_cache_test.cpp` (facts, not logic; test rebuilt and passing),
plus new static evidence committed under `evidence/t28/annotations/`.

## Verdict

The sample-and-restore design is the right shape and stays. The sampling half is now **proven
correct statically**. The entire remaining failure is the restore half: writing `MCO_nextattack`
back did not select the clip, and the next session's one job is to measure why, per the
experiment below. If no write moment works, the fallback is a seam move (bypass the reset state),
not another sampling fix.

## New evidence, all static, no game session

The stance framework submod folders hold only `config.json`; the clips come from other MO2 mods
overlaying the same VFS path. Winners resolved against
`MODS/profiles/Nolvus Awakening/modlist.txt` (first line = highest priority):

- **Sword Neutral** → `Elder Creed - Blade` (only enabled supplier). Ships `mco_attack1..5`.
- **Greatsword Neutral** → `Animations - Mercenary Greatsword` (beats `Berserker Greatsword
  Moveset`). Ships attacks 1–4, 9, 10.

Annotation dumps (`hkxc-anno-cli dump`, files in `../evidence/t28/annotations/`):

| Clip | `@SGVI\|MCO_nextattack\|` | at | HitFrame | MCO_WinOpen | MCO_WinClose |
|---|---|---|---|---|---|
| sword a1 | 2 | 0.633 | 0.433 | 0.633 | 1.167 |
| sword a2 | 3 | 0.633 | 0.467 | 0.633 | 1.467 |
| sword a3 | 4 | 0.767 | — | — | — |
| greatsword a1 | 2 | 0.823 | 0.493 | 0.823 | 1.453 |
| greatsword a2 | 3 | 0.853 | 0.493 | 0.853 | 1.503 |
| greatsword a3 | 4 | 1.100 | — | — | — |
| MSCO_left2 (cast clip) | — (writes `msco_nextright\|2` at 0.0) | | | 0.800 | **1.200** |

Four consequences:

1. **The WinClose sampling change was right, and the sampled value (3) was in range on both
   weapons** — both movesets have an attack3 with a conventional annotation. The "maybe this
   stance is only two hits long" escape route in handoff (a) is dead. The failures
   (`a2` repeat on 1H, `a1` reset on greatsword) are genuinely the restore write not taking
   effect on the clip that follows.
2. **Annotation timing is pack data, not an MCO guarantee — this is the design's hidden
   dependency and it explains the whole history.** Thuum's reference clips annotate at t=0.06,
   before HitFrame, so attack-time sampling read the post-advance value and ticket 13 verified
   clean. These packs annotate AT `MCO_WinOpen` (same timestamp — WinOpen races the write),
   after HitFrame, so the same edges silently read the pre-advance value: the owner's
   `a1 → a2 → hold → a2`. Nothing in SH2 changed; the clip data under it did. `MCO_WinClose` is
   the one conventional edge always after the advance.
3. **The measured post-cast "`MCO_WinClose` carrying 1" is our own borrowed cast clip's event.**
   The ready reset fires `@SGVI` payloads, never a WinClose; `MSCO_left2.hkx` carries
   `MCO_winclose` at 1.2s (plus `MCO_winopen` 0.8, `MCO_recovery` 1.2). The value 1 was the
   reset's stomp read at that moment. The stomp-to-undo predicate happens to handle it while a
   restore is pending. Unchecked gap: if `consume()` fires at the ready tags before 1.2s,
   the clip's own winclose arrives with nothing pending, and once `is_active()` has dropped it
   records garbage into the rolling cache. Every Driver Cast fires MCO window events of its own.
4. **The two failures differ in a telling way.** Greatsword `a1` = the reset value won. 1H `a2`
   = attack2 replayed, and **nothing writes 2** — no annotation, no reset, no restore. Most
   natural reading: `AttackNodes_StateMachine` did not read the variable at that moment and
   resumed its last active state. That points at latch time: an `hkbVariableBindingSet` on
   `startStateId` is plausibly applied at state-machine ACTIVATION, not per attack. Testable
   below.

## The experiment (do this first, before any code)

Handoff (a) step 1, sharpened: only write-effectiveness is in question. For each probe, judge by
the **OAR Animation Log clip name**, never a graph read (handoff (a) documents why the variable
proves nothing).

Fixture: 1H sword, Sword Neutral (5-hit, annotations verified above). Chain a1 → a2, cast slot 3
(`papyrus action=call script=SpellHotbar function=castSlot args=[3]`), swing. Expected if
working: a3.

Probes, one per run:

1. Write `MCO_nextattack=3` at the ready tags (current behavior) — baseline, expected fail.
2. Write on the attack press itself, immediately before the dispatch travels.
3. Write during the cast clip, after its 1.2s `MCO_winclose`.
4. Log which graph each write lands in — the shtb state lives in both `1hm_behavior` and
   `magicbehavior`, and per-graph resolution is a known trap in this repo (see memory:
   SpellFire annotations resolve per-graph). Write to both explicitly in one probe if the API
   allows.
5. Control: no cast, confirm a1 → a2 → a3 uninterrupted on the same stance, so the moveset and
   log pipeline are validated in the same session.

Outcomes:

- **Some write moment works** → keep the design; the fix is the restore edge. Small change.
- **No write moment works** → the binding latches at activation and the data seam is dead from
  outside. Move the seam from data to control: author the `shtb` exit transitions to bypass
  `1HM_Ready_State` (`#shtb$1.txt` is authored `toStateId 0`; its enter payload `#0006` IS the
  reset — ticket 10's trace). If the exit never passes the resetting state, the combo is never
  stomped and `RollingMcoCombo`, the hold credit, the stomp classification, and the age cap all
  delete. That is the refactor the owner asked to be weighed; do not start it until the
  measurement forces it.

## Either way, write the ADR

Name the compat surface: sampling may rely only on the MCO clip convention "`@SGVI` write at or
before WinOpen; WinClose after". Annotation timing is pack data and the load order swaps packs
per stance and per weapon — this is the fact whose absence cost ticket 13's verification its
meaning, and it will bite again on the next animation-pack change.

## Also carried forward from handoff (a), unchanged

- Channel combo hand-off needs a real held key (owner) once the write question is settled.
- Dual self concentration `11004`; t0/t+3s frames.
- File the dual-cast fire-and-forget ticket (MSCO_dual1..10 clips).
- Make the OAR probe disable (`config.json.disabled`) repo-owned rather than a fixture edit.

## Repro for the dumps

```
C:\Tools\SkyrimHKX\hkxc-anno-cli.exe dump -i <clip> -o <out.txt>
```

Clip paths under `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\`:
`Elder Creed - Blade\...\Nolvus OAR Stance Combat Framework\Sword Neutral\mco_attackN.HKX`,
`Animations - Mercenary Greatsword\...\Greatsword Neutral\mco_attackN.hkx`,
`Magic Casting Behavior Overhaul\meshes\actors\character\animations\MSCO_left2.hkx`.
