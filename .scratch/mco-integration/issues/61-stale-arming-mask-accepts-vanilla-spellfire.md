# 61 — The arming mask outlives its cast and accepts vanilla SpellFire events

**Type:** defect (DLL), low severity. Promoted out of ticket 51's secondary finding when that
ticket closed (2026-08-29).

**Status:** built, unit-green, live confirmation open. Small and self-contained; no live session
needed to fix, one to confirm the log goes quiet.

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

- [x] The unit-level arming tests still pass — `combo_cache_test` green, plus two new cases:
      `a_vanilla_spellfire_after_the_clip_commits_nothing` and
      `acceptance_and_isolation_answer_the_same_question` (the second cross-checks isolation and
      acceptance over every hand/mask/driver combination, so the drift cannot return silently).
      All seven test binaries pass; `SpellHotbar2.dll` links clean.
- [ ] A vanilla staff or spell release AFTER a hotbar cast logs no `graph raised a … SpellFire
      event` line and no isolation line. **Needs a live session** — not deployed or run.
- [ ] A hotbar cast still commits exactly once at its clip's own SpellFire frame, both hands and
      dual (ticket 46's rows stay green). **Needs a live session.**
