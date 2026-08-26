# 42 — The GCD is the clock; SpellFire is the floor

Ticket 41 gave every slot type one gate: `current_cast == nullptr`, red flash on refusal. The
owner then played it and named the cost precisely: it feels stiff. The stiffness is not the gate
— it is what feeds the gate. For fire-and-forget spells the lockout is the *animation clip*, not
a chosen number, and the clip is long: measured live 2026-08-25, Firebolt's press-to-free time
was **2.05 s**, of which the payload left at 0.59 s. 71% of the lockout is follow-through with
a dead button.

**Status:** done — built, merged (`4e0094d`), live-accepted 2026-08-25. One cosmetic
follow-through: the retirement log's `spellfire-floor` label never fires (see floor cell);
fix alongside ticket 43.

**Blocked by:** nothing hard, but read the owner-work note at the bottom — clips 3/4 currently
missing their SpellFire annotation make one acceptance cell unmeasurable until fixed.

## Owner decision 2026-08-25

The model is WoW/FF14: **an action costs one number; instants cost a smaller number; the
animation is presentation.** Quotes:

> Basically, an action should be one number. And then instant type things should be another
> number.

> What I'm trying to understand is: why can't we just use that number, and after the GCD is up,
> we can transition to the next spell, regardless of what the animation annotation says? Or if
> we have to, the animation annotations can be a second layer afterwards.

Agreed numbers, per instance type ("action" = 1.5 s, "instant" = short):

| instance | today | target |
|---|---|---|
| spell (FNF) | clip length (`m_gcd`=0.0, ignored anyway) | **1.5 s from press** |
| ritual | casttime + 1.5 | 1.5 s from press |
| concentration | 0.25 tail after release | keep (instant-class) |
| power | 0.5 | keep (instant-class) |
| shout | 1.5, re-armed to 0.5 at fire | keep |
| potion | 1.0 / `potion_gcd` | keep (owner: "treated more as an ability"; MCM-tunable already) |
| weapon art | per-art `GlobalCooldown`, default 1.0 | keep mechanism; **default to 1.5** in the catalogue defaults so arts sit in the action class (`art_definition.cpp:199`, `art_pack_gen.cpp:273`) |

## Why "set m_gcd = 1.5" alone does nothing (the trap)

Two independent reasons, both verified against the code:

1. **`m_gcd` measures from cast completion, not from the press.** `m_cast_timer` starts at the
   authored cast time and counts down past zero; expiry is `m_cast_timer <= -m_gcd`
   (`casting_controller.cpp:372`). Occupancy is `casttime + gcd` — the HUD even draws that sum
   (`get_current_gcd_duration()`, :404). Firebolt at 0.5 s authored casttime + 1.5 = 2.0 s,
   i.e. within 50 ms of the current feel. No change.
2. **The FNF branch never consults `m_gcd` at all.** Once `casted()`, a cuttable instance is
   held until `MscoCastDriver::is_active()` goes false — `SH2_CastExit`, the clip's end
   (`update_cast`, :866; ticket 18 built this deliberately to kill the *post*-clip tail).

So the change is two moves, not one number.

## The change

**lockout = max(GCD measured from the press, time to SpellFire)** — the owner's "second layer"
shape. The GCD is the primary clock; the annotation is a floor, not the clock.

1. **Measure the GCD from the press.** Give `BaseCastingInstance` a press-anchored clock (e.g.
   an `m_lockout` timer set at construction, decremented in `advance_time`) rather than deriving
   expiry from `m_cast_timer + m_gcd`. Keep `get_current_gcd_progress/duration` reporting the
   same shape so the HUD sweep still renders; it now sweeps the real lockout.
2. **Flip the FNF branch** (`update_cast` :862-880): a cuttable, casted instance retires when
   **both** are true — the press-anchored GCD has expired **and** the cast is committed
   (`is_cast_committed()`, i.e. SpellFire has fired). Not on `MscoCastDriver::is_active()`.
   - If the GCD expires first (the normal case: 1.5 > 0.59), retire the instance and leave the
     clip playing as follow-through. The next press starts a new cast; the driver's entry path
     already cuts a still-playing clip (`yield_shtb_for_non_chain_start` exists for exactly
     this).
   - If SpellFire hasn't fired when the GCD expires (slow charge, long clip), hold until it
     does. This is the floor that prevents the WoW-model failure: unlike WoW, our payload IS
     the animation event — releasing early drops the cast.
   - A clip with no SpellFire at all degrades to today's behavior via the existing :529
     fallback (deliver at clip end). Never worse than current, never eats a cast.
3. **Set the numbers** per the table. `CastingInstanceSpell` `m_gcd` 0.0 → 1.5
   (:628); ritual stays 1.5 but now means 1.5-from-press (:633); art catalogue default 1.0 →
   1.5 in both default sites. Instants unchanged.
4. **Do not touch** ticket 41's gate, the ShoutMCO deferral, the CastIntent caps (36/37), or
   the Nemesis patch. No behavior-file change, no Update Engine run. `#shtb$5.txt`'s
   `SH2_CastExit` trigger stays — the graph still needs its exit; the *controller* just stops
   waiting for it.

Out of scope: making the 1.5 an MCM setting. Worth a follow-up ticket if the number needs
tuning by feel; wire it like `potion_gcd` (`game_data.cpp:140`).

## Risks to check while building

- **`reset_cast` while the clip still plays**: confirm animation vars
  (`reset_animation_vars`) and the isolated left-hand caster restore correctly when retirement
  now precedes `SH2_CastExit`. The exit event will arrive for an instance that no longer
  exists; the driver's exit handling must tolerate that (it already tolerates load-time
  teardown, :255).
- **Combo index**: `CastComboIndex` advances at SpellFire and resets on a dropped press. A
  next-press that cuts the follow-through must read as a fresh start, not a drop — verify the
  1→2→3→4 walk still holds at the new cadence.
- **Charge-curve interaction**: attackspeed scaling stretches time-to-SpellFire (0.483 authored
  → 0.59 real at 0.815×). The floor must use the real event, never the authored time.
- **HUD**: the sweep should show 1.5 s now, not casttime+gcd. Check both bar and oblivion bar
  (`hotbar.cpp:561`, `oblivion_bar.cpp:142`).

## Acceptance

Live-runtime on the deployed DLL, `castSlot` driven, log-evidenced like ticket 41's run.
Run 2026-08-25 22:19–22:23, evidence `evidence/t42/acceptance-2026-08-25.log`:

- [x] Firebolt: press-to-free ≈ 1.5 s (was 2.05 s). Press 22:19:26.045 → `lockout over at
      1.50s` 22:19:27.537 (1.49 s); next press 22:19:27.721 accepted into clip 3.
- [x] Press at 1.6 s starts the next cast and cuts the follow-through cleanly. The 27.721
      press entered `SH2_Cast3` from inside the live state while clip 2 still played; its
      SpellFire fired, its own exit processed cleanly, and the immediately following art
      swing (`IsAttacking=1`) proved the weapon usable.
- [x] Spam during the 1.5 s refuses with the red-flash call every time: four presses at
      +0.33/+0.65/+0.97/+1.30 all logged `refused by the press gate (cast live=true)`.
- [x] Floor cell, no manufacturing needed: clip 4's SpellFire lands at ~1.78 s (owner's new
      annotation) — instance held past the 1.5 s GCD and retired at 1.80 s, 27 ms after the
      event. Seen twice (22:21:09.005 scripted, 22:22:11.870 owner hands-on). Known cosmetic
      defect: the log line says `released by gcd-expired` where it should say
      `spellfire-floor` — the sticky flag is never set because the retirement branch only
      runs after delivery. Behavior correct; label fix queued.
- [x] Combo walk 1→2→3→4 at the new cadence: 22:21:10.340 clip 1 → 12.724 clip 2 → 14.264
      clip 3 → (earlier run) 07.196 clip 4; every entry accepted from inside the live state,
      no dropped-press combo resets. Unblocked by the owner's clip 3/4 annotation fix.
- [x] Shout regression: slot 8 fired (`queued input event=true`), both spam presses refused,
      deferred selectedPower write-back restored after `IsShouting` fell (22:22:14–17).
      Potion and concentration were not slotted on the test bar and remain untested; both
      construct with casttime 0 / their own overrides, where the old and new clocks are
      arithmetically identical (static check only).
- [x] Mid-swing art deferral unchanged: 22:23:06.454 press with `IsAttacking=1` → `not
      consumed (mid-swing)` → `deferred to ShoutMCO (handle 1)` → `RELEASE (ready)` at .933 →
      `fired slot 11 -> true`.
- [x] Owner hands-on 2026-08-25: "feels better. It properly blocks cast. It feels a bit
      slow." Diagnosis from the log: clips 1–3 free at exactly 1.50 (GCD's number), clip 4 at
      ~1.80 (animation floor). Verdict: make the number tunable rather than guess again —
      ticket 43.

## Owner work, parallel

Clips 3 and 4 (`MSCO_left3.hkx`, `MSCO_left4.hkx`) are missing `MLh_SpellFire_Event` — clip 3
observed live raising `SH2_CastExit` with no SpellFire, resetting the combo and delivering
late. Under this ticket that degrades safely, but the floor and the combo cells can't be
measured on those clips until the annotations land. Reference layout (clip 2):
`0.483 MLh_SpellFire_Event`, `0.680 MSCO_WinOpen`, `1.633 MSCO_WinClose`.
