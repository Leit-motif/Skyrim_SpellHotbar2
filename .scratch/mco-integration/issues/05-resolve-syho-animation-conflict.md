# 05 — Resolve the SYHO animation conflict

**Type:** task (likely Compatibility Package)

**What to build:** Casting animations that actually play in this load order, instead of being
shadowed by another mod's shout clips.

**Blocked by:** None. Independent of the graph work and can proceed in parallel.

**Status:** ready-for-agent

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

Remaining:

- [ ] Re-enable the overlay in MO2 above `Spell Hotbar 2` and relaunch; the VFS only changes
      on restart.
- [ ] **Confirm in game.** Nothing above proves what plays; it only proves what OAR should
      select. Untested.
- [ ] Read SYHO's own OAR conditions if the overlay does not resolve it. They have still
      never been read — the priority comparison settled attribution without them, and the
      overlay may settle the fix without them too.
- [ ] **Capture frames** for a fire-and-forget and a sustained concentration cast. This claim
      is entirely visual; an OAR priority table does not prove what played.
- [ ] Confirm real shouts still play SYHO's animations. This ticket must not fix casts by
      breaking the shout overhaul the player installed deliberately.
