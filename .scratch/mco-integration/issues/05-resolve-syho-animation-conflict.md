# 05 — Resolve the SYHO animation conflict

**Type:** task (likely Compatibility Package)

**What to build:** Casting animations that actually play in this load order, instead of being
shadowed by another mod's shout clips.

**Blocked by:** None. Independent of the graph work and can proceed in parallel.

**Status:** CLOSED 2026-08-23 by owner ruling — *"this isn't relevant any more. no t-pose
anywhere."* The T-pose this ticket existed to chase is gone from the load order as the owner
observes it, so there is nothing left to resolve and it is no longer a publication gate. The
corresponding row in the thuum repo's ticket 18 is struck. **Reopen only on a fresh sighting**,
with the save, profile and a captured frame — this closes on an observation, not on a fix, so
the evidence for it is an absence and absences do not stay true by themselves.

~~ready-for-agent~~

> **Owner report 2026-08-08:** the currently enabled MCBO probe still T-poses. Treat that as a
> reproduced-user report, not closed diagnosis: capture the failing frame and OAR winner in one
> session, then continue from the probe's failure branch below. This remains Spell Hotbar-owned
> animation compatibility work and blocks the combined Shouts for MCO publication path.

Reproduced live 2026-08-03 during Baseline Adoption ticket 02, and attributed without needing
to read anyone's conditions:

| Mod | OAR submod priorities |
| --- | --- |
| `SYHO - Shout Your Heart Out` | `99999990`–`99999996` (8 submods) |
| Spell Hotbar 2 | max `99901002` (56 submods) |

SYHO's *lowest* priority outranks this mod's *highest*, so wherever both sets of conditions
pass SYHO wins **every** contested cast. This is not intermittent and not a tuning accident.
It confirms `CONTEXT.md` finding 11, which had raised the hazard and explicitly left it
unconfirmed.

**The part that constrains the fix: SYHO's clip does not loop.** A fire-and-forget borrowing
the wrong animation looks wrong; a concentration spell channelling for an arbitrary duration
against a one-shot clip has no animation that can represent it. So the remedy cannot be
cosmetic — this mod's own concentration clips have to win, or sustained casts stay visibly
broken.

## Progress 2026-08-03 — overlay built, not yet confirmed in game

The owner asked what was stopping a straight priority bump. Nothing was, and the caution this
ticket originally carried turned out to be unnecessary — but it was worth one check, and the
check is what makes the bump safe to do wholesale.

**Every one of the 55 submods is gated on `SpellHotbar.esp:815 != 0`.** That global is set
only while a hotbar cast is live. So no submod's conditions can pass when no cast is
running, and raising all of them above SYHO **cannot** make this mod hijack real shouts. The
"do not renumber blindly" caution is discharged by that fact, not by testing.

One practical trap: a uniform `+1000000` offset does **not** work. The submods span
`98100160`–`99901002`, so the bottom of the range would still sit under SYHO's `99999990`.
The minimum viable offset is `+1899837`.

Applied as a **Compatibility Package overlay**, not an edit to the installed mod:

- New MO2 mod `Spell Hotbar 2 - OAR Priority Over SYHO`, containing only the 55 submod
  `config.json` files plus the root preset config, at their original relative paths.
- Offset `+2000000` → new range `100100160`–`101901002`, entirely above SYHO's `99999996`,
  with the internal ordering of the 55 preserved.
- The installed `Spell Hotbar 2` is untouched; its 56 originals are still in place. Rollback
  is disabling or deleting the overlay mod.

## 2026-08-03 — the configs-only overlay T-posed the player

Tested live on the owner's new save `Save42_EBCD0A92_…_QASmoke_000551_20260804020940_17_1`:

| Overlay | Result |
| --- | --- |
| enabled (configs only) | **T-pose on cast** |
| disabled | SYHO shout animation, cast works |

So the first overlay caused it. Two candidate causes, needing different fixes:

- **(a)** Spell Hotbar's own animations do not work in this load order, and SYHO was masking
  that by winning every contested cast. Nemesis Unlimited Behavior Engine and several Nemesis
  patches are enabled here, and the submods replace not just `mt_shout_*` / `1hm_shout_inhale`
  but the idles for every weapon type (`mt_idle`, `2hm_idle`, `dw1hm1hmidle`, `bow_idleheld`,
  `staff_idle` …, 23 copies each). If (a) holds, this entire ticket's approach is dead.
- **(b)** The overlay broke resolution. It shipped only `config.json` files, so OAR finding the
  animations depended on MO2's VFS merging the base mod's `.hkx` into the same submod folder.
  That assumption was mine and was never tested.

**Overlay rebuilt as a full copy to discriminate them** — 1117 files including all 1040 `.hkx`,
so nothing depends on a merge. Priorities are now edited *textually* (regex on the
`"priority": N` token) rather than by JSON round-trip, leaving each config byte-identical to
its original apart from that number; the first version went through
`ConvertFrom-Json`/`ConvertTo-Json`, which was an unnecessary risk with a parser as
casing-sensitive as OAR's.

Next run decides it: full-copy overlay plays this mod's own casting animation → cause was (b),
and the priority fix is sound. Still T-poses → cause is (a), and the ticket needs rethinking
from the top.

## 2026-08-03 — full-copy overlay still T-posed; ticket redirected

The full-copy overlay T-posed too, so the cause is **(a)**. The owner's OAR animation log
showed `cast_1h_left`, `mt_shout_exhale` and `1hm_shout_inhale` winning — this mod's own
submods — which proves the overlay worked and the priorities are right. The clips themselves
fail.

Cheap explanations ruled out, so nobody re-checks them:

- **Not a bad or wrong-format file.** `mt_shout_exhale.hkx` is `hk_2010.2.0-r1`, byte-for-byte
  the same Havok version string as SYHO's and MCBO's working clips.
- **Not the priorities, and not the JSON.** OAR selected the intended submods, and the
  full-copy overlay's configs differ from the originals by exactly one number.

What remains is that these clips do not work against this load order's skeleton or
Nemesis-rebuilt behaviour. **That is not being diagnosed**, because the owner does not want
this mod's casting animations at all — they want Magic Casting Behavior Overhaul's. Debugging
animations that are going to be replaced buys nothing.

### Redirected: play MCBO clips from the shout paths

The mechanism stays, the source changes. A hotbar cast fires shout events; OAR decides what
those play; so map the shout paths to MCBO's clips.

MCBO is a Nemesis behaviour mod (`Nemesis_engine/mod/msco`, plus a `BehaviorDataInjector`
SKSE plugin) that injects magic states, with `MSCO_left1-10.hkx` sitting in the plain
animations folder as the clips those states play. They are ordinary player-skeleton clips, so
they can be substituted into other animation paths.

Probe built as MO2 mod **`Spell Hotbar 2 - MCBO Cast Animations`** (not in this repo — it is
runtime state):

- One submod `SpellHotbar2_MCBO/cast_left_probe`, priority `101500000` (above SYHO's
  `99999996`).
- `MSCO_left1.hkx` copied over the six shout paths a cast requests: `mt_shout_exhale`,
  `mt_shout_inhale`, `1hm_shout_inhale`, `2hm_shout_inhale`, `sneak1hm_shout_exhale`,
  `sneak1hm_shout_inhale`.
- Gated on `SpellHotbar.esp:815 > 0` (cast active) plus an inlined player check, so real
  shouts are untouched and keep SYHO.

Deliberately one clip, not the full five-variant mapping — it answers whether an MCBO clip
plays from a shout path at all before more is invested. Inhale and exhale share a clip in the
probe; that is a known crudity, not the intended end state.

Remaining:

- [ ] **Disable `Spell Hotbar 2 - OAR Priority Over SYHO`.** Its bumped range reaches
      `101901002`, which would outrank the probe — and it is what makes the broken clips win.
      With it off, this mod's originals sit below SYHO and the probe wins hotbar casts.
- [ ] Enable the probe mod, relaunch, cast, and record whether an MCBO clip plays.
- [ ] If it plays: map the remaining left variants and decide whether inhale and exhale should
      differ. If it T-poses too: MCBO clips cannot be driven from the shout graph, and the
      animation problem escalates to the graph work in tickets 02/03.
- [ ] **Confirm in game.** Nothing above proves what plays; it only proves what OAR should
      select. Untested.
- [ ] Read SYHO's own OAR conditions if the overlay does not resolve it. They have still
      never been read — the priority comparison settled attribution without them, and the
      overlay may settle the fix without them too.
- [ ] **Capture frames** for a fire-and-forget and a sustained concentration cast. This claim
      is entirely visual; an OAR priority table does not prove what played.
- [ ] Confirm real shouts still play SYHO's animations. This ticket must not fix casts by
      breaking the shout overhaul the player installed deliberately.
