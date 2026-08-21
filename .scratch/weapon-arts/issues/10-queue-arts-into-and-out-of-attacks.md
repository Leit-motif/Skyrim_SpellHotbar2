# 10 — One hotbar press queue (Abilities, spells, shouts)

A bound Ability currently needs a full stop to idle before it will play, and it does not chain
into the next attack. Owner playtest 2026-08-19 (ticket 07): “ashes of war don't combo into and
out of attacks… ideally we give it a bit of queuing.”

Owner 2026-08-21: spells, Abilities, and hotbar shouts are the **same press**. One stored
activation, last tap wins, fires at the first legal frame. Fold in mco-integration tickets 23
and 09.

**Blocked by:** 01 (resolved)

**Status:** resolved — owner 1, 2, 6, 7 passed; agent 3–5 passed

**Type:** task (Core Fork)

## Player rule

I press a hotbar button. I want that animation. If I am already in an animation, keep the press
and play it at the first legal frame. A newer press replaces the stored one. Do not mash-through
(do not skip the current hit).

Spell, Ability, and **hotbar shout** are payloads of that one queue. The vanilla shout **key**
(Z / mapped Shout) stays thuum — this ticket does not re-time that key.

## You test this

Profile `Nolvus Awakening`. Weapon drawn.

1. Ability during an MCO swing → Ability starts after the hit, swing not cancelled.
2. After that Ability, left-click in recovery → next combo step, not hit 1, no full idle.
3. During a live Ability, tap a second Ability early → one start at the Ability latch.
4. During a live Ability, tap a **spell** → one Driver Cast at that latch.
5. During a live Driver Cast (before SpellFire), tap the spell again → one chain at SpellFire,
   not mash-through, last tap wins (old ticket 23).
6. During a live Ability or Driver Cast, tap a **hotbar shout** → shout starts at that same
   latch, one event, not dropped.
7. During an MCO swing, tap a hotbar shout → shout after HitFrame (same clock as a spell).

If any of those still need a full return to idle, it fails.

## Agent tests the rest

- One retained payload (slot + type + art id or FormID). Newest tap replaces it. Revalidate once
  on fire; invalid discards.
- **Behind an MCO attack or a real (non-hotbar) shout:** `CastIntent::offer` (ADR-0005). Do not
  inject a `"Shout"` `ButtonEvent` until release — injecting immediately is a second hold in
  ShoutMCO’s shout hook (mco-integration 09).
- **Behind a Driver Cast or Ability this mod started:** SH2 latch, not a second HitFrame cache.
  Driver Cast latch is ticket 22’s window (SpellFire → WinClose). Ability latch: WinOpen if the
  bound hkx has it, else HitFrame, else `SH2_ArtExit`. Classify from annotations at start.
- `try_start_art` graph refuse → same offer/queue as a spell, not a dead press.
- Drop `!is_shouting` on the spell/shout start paths so a press can reach the queue (09).
- Input mode today refuses shouts/arts while a cast instance is live (`modes.cpp`). That refuse
  becomes “store in the queue,” not “dead press.”
- Combo: do **not** write `MCO_nextattack=1` on Ability entry. Reuse `RollingMcoCombo`. Attack 2
  → Ability → light 3.
- Attack press after the Ability latch: `SH2_ArtExit`, press uncaptured (mco-integration 10).
- Costs on successful start, not on the stored tap. Immediate refuse: sheathed, wrong class,
  unaffordable, empty custom folder.
- ShoutMCO absent: mid-swing still fail-open (dead press). SH2-owned latches still buffer.
- Restore fixtures; close Skyrim after agent-only telemetry.

## What to build

One module: a single last-wins **Cast Intent** (glossary) that can be a spell, an Ability, or a
hotbar shout. Two clocks, not three products:

| Busy with | Who says the frame is legal |
|---|---|
| Someone else’s MCO swing, or a real shout | ShoutMCO (existing API) |
| Our Driver Cast or Ability | This mod’s latch |

On fire: run the existing start (`try_start_cast` / `try_start_art` / shout `ButtonEvent` +
`try_cast_power`). Do not duplicate those pipelines.

Ability combo restore as already grilled (ADR-0005 exception widened).

## What this is not

Not mash-through (14). Not replacing thuum’s vanilla shout key. Not Art Class / icons / editor
(07/06/09/11). Not potions (09 left those as “work during shout, no queue”). Not concentration.

Supersedes [mco-integration 23](../../mco-integration/issues/23-one-slot-buffer-for-consecutive-driver-casts.md)
and [mco-integration 09](../../mco-integration/issues/09-defer-a-hotbar-press-made-during-a-shout.md).

## Comments

Filed from ticket 07 close-out.

**2026-08-21 grill** then owner unify: one press queue for Ability, spell, and hotbar shout.
Ticket 23 and 09 fold in. Vanilla shout key stays thuum.

**2026-08-21 agent:** One Cast Intent, two clocks. Local latch for Driver Cast (SpellFire–WinClose)
and Ability (WinOpen else HitFrame else `SH2_ArtExit`). ShoutMCO for someone else's swing / real
shout. Hotbar shout ButtonEvent waits until fire. Ability entry no longer writes `MCO_nextattack=1`;
combo restore reuses `RollingMcoCombo`. `combo_cache_test` + plugin Release build green. Owner cells
1–7 still open (Nolvus Awakening, weapon drawn).

**2026-08-21 playtest script** (owner cells). Profile `Nolvus Awakening`. 1H drawn. Bar: an Ability,
a fire-and-forget spell (Firebolt), a hotbar shout (not the Z key).

1. Mid-swing Ability — light attack, tap Ability during the swing. Pass: swing finishes its hit,
   Ability starts after, no cancel to idle.
2. Combo out — from attack **2**, Ability, left-click in recovery (after the latch, not before the
   hit). Pass: next light is **3**, not 1, no full idle.
3. Ability → Ability — during Ability 1, tap Ability 2 early. Pass: one start at the latch, last
   tap wins, Ability 1’s hit still happens.
4. Ability → spell — during Ability, tap Firebolt. Pass: one Driver Cast at the Ability latch.
5. Spell → spell — during Driver Cast **before SpellFire**, tap Firebolt again (or another FNF).
   Pass: current hit still fires; one chain at SpellFire; last tap wins. Fail: second clip starts
   in the windup.
6. Hotbar shout behind ours — during Ability or Driver Cast, tap the shout slot. Pass: one shout
   at that latch, not dropped, no second overlapping thuum hold.
7. Hotbar shout behind MCO — mid-swing, tap the shout slot. Pass: shout after HitFrame, swing not
   cancelled. Fail: dead press or shout on top of the swing.

Fail any cell that still needs a full return to idle. Do not use vanilla Z; that key is thuum.

**2026-08-21 owner playtest** (Save10, rapier drawn):
1. Pass — Ability after recovery window opens, swing not cancelled.
2. Fail — no combo continuation (recovery light is not 3).
6. Shout works; too-early press overlapped Ability clip + shout clip. Owner: legal shout cancel must stop the previous clip.
7. Pass — shout after HitFrame, swing cancelled when legal.

**2026-08-21 agent drive** (Save10 last-save, CS-Test, Noble Rapier drawn, Nolvus Awakening).
Slots: 1 and 3–7 Ability, 2 Ice Spike, no shout on the bar. Cold boot ~6 min to Main Menu.

- Cell 3 log pass: Ability 1 start, slot 3 retained on local latch, one fire at latch.
- Cell 4 log pass on a WinOpen art (slot 3). Slot 1 art (latch = `SH2_ArtExit` only) retained the spell then `fired … type 3 -> false` at ArtExit — owner: use an art that has WinOpen/HitFrame.
- Cell 5: first run discarded on `allowed_to_cast` (live MagicCaster during our Driver Cast). Skip that check when releasing behind our Driver Cast/Ability. Retest: retained before SpellFire, `chaining the next clip`, `fired slot 2 type 3 -> true`. Uncommitted `cast_intent.cpp` change is in the live DLL.
- Cell 1 log half: `attackStart` then Ability → `deferred to ShoutMCO` while `IsAttacking=1`, then fire. Visual “after the hit” still owner (real LMB).
- Cells 2, 6, 7 not closed: 2 needs a real recovery click; 6–7 need a hotbar shout bind (not Z).
Skyrim left running for owner playtest.

**2026-08-21 follow-up:** Owner closed 1 and 7; 3–5 were agent-log. Cell 2 failed (no combo out). Cell 6 shout works but overlapped the Ability clip if pressed early. Fix in flight: do not record MCO combo on our shtb HitFrame; write the sampled index on ArtExit/cancel before the recovery press; `shoutStart` leaves `SH2_Art_State` / Driver Cast so a legal shout stops the previous 1hm clip. Graph change needs a Nemesis rebuild.

**2026-08-21 owner retest** (Save11, post-deploy + Nemesis): combo continued (cell 2); shouts chained with the previous clip stopped (cell 6). Cells 1–7 closed.
