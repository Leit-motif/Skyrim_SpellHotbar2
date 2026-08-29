# 61 — The arming mask outlives its cast and accepts vanilla SpellFire events

**Type:** defect (DLL), low severity. Promoted out of ticket 51's secondary finding when that
ticket closed (2026-08-29).

**Status:** CLOSED 2026-08-29. Built, unit-green, and both live acceptance cells confirmed in a
headless session (Nolvus Awakening, CS-Test, save `T67-sprint-retest2`, DLL
`1F642DB2B742ECC56E89A5D81D9982F578A2001D70FA0DA7B94DFE1F96C68CCB`).

## The defect

`notify_spellfire` accepts a SpellFire event whenever the LAST cast's mask is still armed and no
re-arm has bumped the generation. The mask is not cleared when a cast retires, so any vanilla
release after a hotbar cast — a staff's own attack, an equipped spell — is accepted and logged
as though it belonged to a driver cast.

**Delivery is safe.** The seen latch is cleared at cast start, so no payload is double-delivered.
This is an evidence defect, not a gameplay one: it writes `SH2 cast: graph raised a … SpellFire
event` lines that did not come from a driver cast, into the exact log that every commitment
finding is read from. It is also the stale acceptance ticket 46's generation counter exists to
prevent, applied at the wrong boundary.

## Evidence

- 2026-08-26 (ticket 51's filing window): vanilla staff releases at 16:50:33, 16:50:40 and a
  16:51:57–16:52:07 stream logged `graph raised a right SpellFire event` with no driver cast,
  accepted by a mask left armed from earlier dual channels.
- 2026-08-29 14:57:03–14:57:08: a right-hand staff's own releases logged bare
  `MRh_SpellFire_Event` traces with no driver cast active and no isolation line — the same
  boundary, still open after ticket 60's work.

## The fix

Clear the arming mask at cast retire, alongside `clear_spellfire()`. One site, and it makes
"this hand is armed" mean "a live cast armed it" rather than "a cast armed it at some point."

## Acceptance

- A vanilla staff or spell release AFTER a hotbar cast logs no `graph raised a … SpellFire
  event` line and no isolation line.
- A hotbar cast still commits exactly once at its clip's own SpellFire frame, both hands and
  dual — ticket 46's rows stay green.
- The unit-level arming tests still pass (`combo_cache_test`).


## Comments

### 2026-08-29 — built; the fix is not the one the ticket names

The proposed fix was already shipped and is not where the defect lives. `clear_spellfire()` has
cleared the mask as well as the latch since `450a843` (2026-08-26, "disarm at teardown"), and
every teardown site calls it. Applying the ticket as written would have been a no-op.

What the log actually recorded is an asymmetry between the two halves of the commitment point.
Both ask the same question — is this SpellFire event a driver cast's own? — and both read the
arming mask, but only ISOLATION also required a live driver:

- `isolate_caster_before_vanilla_spellfire(driver_cast_active, hand, mask)` — two terms, since
  ticket 46.
- `is_msco_combo_window_open_event(hand, mask) && is_active()` — two terms.
- `notify_spellfire(hand, generation)` — the mask alone.

That is exactly the log signature in the evidence: a `graph raised a … SpellFire event` line
with NO isolation line beside it. Isolation said no and acceptance said yes, on the same event.

The mask legitimately outlives the clip that armed it, so clearing it earlier is not available as
a fix. It stays armed through a delivered cast's whole GCD tail, and deliberately through a
retired cast's armed window (ticket 43), where the still-playing clip's own SpellFire is how that
payload leaves. "This hand is armed" can therefore never mean "a live cast armed it" on the mask
alone — the driver term is what carries that, and acceptance was missing it.

**The fix.** One predicate, `spellfire_event_commits_the_cast(driver_cast_active, event_hand,
armed_mask)` on `combo_cache.h`, and both halves expressed through it so they cannot drift apart
again. `notify_spellfire` takes the driver snapshot as an argument, read once in
`ProcessEvent_PC` beside the arming — the same "one snapshot per event" rule ticket 46 set for
the mask, now covering the driver term too.

### Second evidence bullet: not this defect

The 2026-08-29 14:57:03-08 lines are bare `SH2 graph event: MRh_SpellFire_Event` traces, and
those come from `should_trace_graph_events()`, which keeps a small post-cast trace budget on
purpose. No `graph raised` line, no isolation line — that window was already behaving correctly,
and this change does not touch it. The live 2026-08-26 16:50 stream in the first bullet is the
defect.

### Adjacent, not fixed

`start_cast` and `start_ritual_cast` arm SpellFire before `MscoCastDriver::begin()`, and the
CHAINING failure branch returns `start_result::failed` — which `resolve_start` does not clear,
unlike `graph_refused`. The mask is left armed for a cast that never started. With the driver
term in place this commits nothing and isolates nothing, so it is now inert rather than a defect;
recorded here so the next touch of the arming path can tidy it deliberately.

### Can the new gate eat a legitimate spell?

The one way this fix could hurt: a driver cast's own clip raises its SpellFire while
`MscoCastDriver::is_active()` reads false, and the payload is silently lost. Traced, both by the
implementer and independently in review, and the answer is no.

`state_active` is set synchronously in `send_entry` on the game loop, before the clip plays, and
falls only through `clear_state_flags()` (from `cancel`, `finish`, `end_channel`) or the graph's
own `SH2_CastExit` — which a clip raises at its end, after its SpellFire. The window the ticket-43
armed-cast path opens is the one that matters, and `retire_cuttable_cast` deliberately does NOT
call `cancel`/`finish` there, exactly so the still-playing clip keeps its own exit and SpellFire.
So `is_active()` stays true across the whole window in which a delayed legitimate SpellFire can
arrive, and the clip-end fallback (`ArmedDelivery::on_clip_end`) covers the case where none does.
Every `cancel()` caller is a path that intends to abandon the clip's future events.

## Acceptance status

All three cells closed. Log evidence: `docs/evidence/ticket-61-live-2026-08-29.log`.

- [x] The unit-level arming tests still pass — `combo_cache_test` green, plus two new cases:
      `a_vanilla_spellfire_after_the_clip_commits_nothing` and
      `acceptance_and_isolation_answer_the_same_question` (the second cross-checks isolation and
      acceptance over every hand/mask/driver combination, so the drift cannot return silently).
      All seven test binaries pass; `SpellHotbar2.dll` links clean.
- [x] A hotbar cast still commits exactly once at its clip's own SpellFire frame, both hands and
      dual — ticket 46's rows stay green. Driven at 16:02:06-18 with `setSlotHand` + `castSlot`
      on Firebolt (the ticket-46 bar, `bars/t46.json`):

      | hand | clip | event | isolated | accepted | delivered |
      |---|---|---|---|---|---|
      | left  | `SH2_CastRight` (1) | `MLh` 16:02:07.592 | yes | once | 16:02:07.611, its own SpellFire |
      | right | `SH2_Cast2` (2)     | `MRh` 16:02:12.714 | yes | once | before lockout at 16:02:12.795 |
      | dual  | `SH2_Cast3` (3)     | `MLh` 16:02:18.383 | yes | once | 16:02:18.401, its own SpellFire |

      Dual selected animation 10016, the dual family id — no silent downgrade, because the
      fixture's missing Destruction dual perk was granted in memory for the run
      (`player.addperk 000153CF`, discarded with the process).
- [x] A vanilla staff release AFTER a hotbar cast logs no `graph raised a … SpellFire event` line
      and no isolation line. Three consistent repetitions; see the construction below.

## How the vanilla-release cell was actually driven

The first two attempts proved nothing, and the reason is worth keeping.

The mask is cleared at teardown, so a cast retiring on the owner's 0.5 s GCD takes its mask with
it within a few hundred milliseconds. A staff fired after that is rejected by the CLEARED MASK,
not by the new driver term — a green log that tests nothing. The first run (15:59) had exactly
that shape. The second (16:00) used a concentration channel and an 8 s spell GCD, but a channel
does not retire on the spell GCD: a probe press 2.2 s later started a NEW cast, which proved the
instance was already gone AND re-armed the mask to the other hand.

What the cell needs is a window where all three of the acceptance terms hold except the driver:
mask armed, generation unmoved, driver state gone. That is a FIRE-AND-FORGET cast on a long GCD
— the clip ends while the instance keeps running its lockout, and the mask lives with it. Run at
16:05:19 with `setSpellGCD(8.0)` and slot 0 forced right:

      16:05:19.946  notified SH2_CastRight (clip 1) -> true      cast begins, mask arms RIGHT
      16:05:20.452  isolated right-hand caster / graph raised a right SpellFire event
                                                                 its OWN event: isolated + accepted
      16:05:21.368  state exiting (clip end or cancel)            driver INACTIVE from here
      16:05:24.591  sampled MCO ... at MSCO_WinClose              the STAFF's own swing
      16:05:27.991  lockout over at 8.02s (payload already delivered)
                                                                 instance -- and its mask -- live until here

The staff's release sits at ~16:05:23.6 (its `MSCO_WinClose` at 24.591 minus the 0.95 s
SpellFire-to-WinClose spacing measured for this staff at 15:59:24.969-25.925). That instant has
the driver inactive since 21.368 and the mask armed until 27.991, with no re-arm in between --
the exact stale-mask condition -- and the log carries NO second acceptance and NO second
isolation. Repeated at 16:05:38 with clip 2, same shape, same result.

One honest limit: the staff's own `MRh_SpellFire_Event` is not directly visible in that window,
because `should_trace_graph_events()` refills its budget only on a cut and a clip-end exit leaves
it at zero. The swing itself is proven by the `MSCO_WinClose` sample line, which is ordinary
driver logging rather than a trace, and the same injected input was observed raising
`MRh_SpellFire_Event` directly three times earlier in the session (15:59:24.969, 16:00:03.406,
16:00:58.162). The event arrives; it is no longer accepted.

## The oracle

`tools/telemetry/spellfire_pairing.py` pairs every accepted SpellFire event in a log against its
isolation line and reports the orphans -- an accepted event with no isolation beside it is the
defect's whole signature. Exit status 1 on any orphan, so a future session can gate on it rather
than reading the log by eye.

    accepted SpellFire events : 6
    isolation lines           : 6
    orphans                   : 0

Worth noting for anyone re-reading the filing evidence: the pre-fix log from the session before
this one also reports zero orphans over 80 accepted events. That session never provoked the
defect, which is consistent with the second evidence bullet above being trace-budget noise. The
defect needs the mask armed in the hand the vanilla release uses, and it has to be driven
deliberately.

## Fixture notes

In-memory only, all discarded when the process exited: Firebolt and Ice Spike taught
(`player.addspell`), the Ordinator Destruction dual-casting perk granted, `MAG_DestructionStaffFirebolt`
(`0004C6CA`) added and equipped right-hand, the ticket-46 bar loaded over the owner's. Nothing
was saved. The owner's live bar was written to
`Documents/My Games/Skyrim Special Edition/SpellHotbar/bars/t61_owner_backup.json` before the
swap and reloaded from it afterwards; that file is left in place as a restore point.
