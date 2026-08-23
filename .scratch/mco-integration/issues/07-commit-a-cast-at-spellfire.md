# 07 — Commit a cast at the graph's spellfire event

**Type:** task (Core Fork)

**What to build:** The two corrections to this mod's own cast lifecycle that make a cast survive
losing `IsShouting` after the magic is committed, per
[ADR 0004](../../../docs/adr/0004-commit-a-cast-at-the-graphs-spellfire-event.md).

**Blocked by:** None. Split out of ticket 03 on 2026-08-05 because neither change depends on
ticket 01's live answer or on ticket 02's branch change — both are corrections to this mod's own
lifecycle and are correct with no MCO engine present at all.

**Status:** claimed — **but the work appears to be IN THE SOURCE already; this line is stale.**
Checked 2026-08-23 while answering the owner on what SH2 work v1 still needs:
`skse_plugin/src/casts/casting_controller.cpp` carries `arm_spellfire()`, `spellfire_seen`,
`spellfire_mask`, and the `MLh_SpellFire_Event` commit path, with runtime-verified comments dated
2026-08-11. **Close it against ADR-0004's criteria rather than rebuilding it** — what is missing is
a verdict, not code.

## Why this is not chaining work

Today a cast is destroyed the moment `IsShouting` goes false, at any point before its own timer
fires (`skse_plugin/src/casts/casting_controller.cpp:247`, `is_anim_ok`). The magicka is deducted
only *after* `cast_spell` succeeds, so an interruption in that window costs the player the spell
and refunds nothing because nothing was taken. An MCO chain-out is one cause of that. A stagger, a
killmove, a menu, or any other mod clearing the graph variable is the same defect with a different
trigger, and it is reachable today with no engine involved.

The exposure is not a rounding error. A ritual cast notifies the exhale **250 ms** before it fires
(`casting_controller.cpp:322`), and a ritual concentration cast **1.0 or 1.5 s** before, per
animation (`skse_plugin/src/game_data/spell_cast_data.cpp:146-158`).

## The two changes

**1. Commitment point.** Re-enable the dormant player animation-event hook
(`skse_plugin/src/events/animationeventhook.cpp`; its installer is commented out at
`skse_plugin/src/plugin.cpp:32`) and watch for the vanilla `Voice_SpellFire_Event`. From that
event until the spell fires, a live cast ignores the `is_anim_ok` liveness check and delivers its
spell rather than being torn down. Before it, behaviour is unchanged.

**2. Channel ordering.** `casting_controller.cpp:440-447` re-notifies `ShoutStart` every 0.5 s and
checks `is_anim_ok` only afterwards. A channel cut mid-loop therefore re-enters the shout graph
and tears down whatever the player is now doing — and this repo's own narrowed open question
records that the graph *does* honour shout entry from an attack state. Check liveness first and
end the channel instead.

## Built 2026-08-05, unverified at runtime

Both changes are in and `build/release` links clean (12/12, no new warnings). **Nothing below is
ticked**: a build establishes that the code compiles, never that the behaviour is right, and every
cell here is behavioural. The live pass is owed.

Changed:

- `skse_plugin/src/casts/casting_controller.cpp` — file-scope atomic `spellfire_seen` with
  `notify_spellfire` / `clear_spellfire` / `is_cast_committed`; cleared on cast start
  (`try_start_cast`, `try_cast_power`) and in `reset_cast`; honoured in `CastingInstance::update`
  and in the concentration charge loop; channel re-notify reordered behind its liveness check.
- `skse_plugin/src/events/animationeventhook.cpp` — watches the player's
  `Voice_SpellFire_Event`; sets the atomic and touches nothing else, since it runs on the
  animation thread.
- `skse_plugin/src/plugin.cpp` — `events::install()` re-enabled.

## Acceptance

- [x] The commitment point is the graph's own `Voice_SpellFire_Event`, not a timer offset and not
      a constant. If the event never arrives, behaviour falls back to today's exactly.
      **Live 2026-08-12**, five casts, plus the traced cast in ticket 08's correction: the event is
      now `MLh_SpellFire_Event` — ADR-0006 moved the commitment point onto the clip's own
      annotation and named the wrong one; it is amended with the measured event and time — and
      the log line
      `SH2 cast: graph raised a left SpellFire event` lands at +0.46–0.49 s on every cast, and the
      timer-floor warning never fires. The fallback is the ADR-0006 floor, still unexercised
      because no clip without the annotation has been found.
- [ ] A ritual concentration cast interrupted during its 1.0–1.5 s lead delivers its spell and
      deducts its magicka, rather than losing both.
- [ ] An interruption **before** spellfire still cancels the cast, unchanged, and still costs the
      player nothing.
- [ ] An uninterrupted cast of every kind — plain, ritual, dual, concentration, ritual
      concentration — behaves exactly as before. This is the regression that matters most: the
      leads are authored timings and must not move.
- [ ] A channel whose shout state is cleared ends, and does not re-notify `ShoutStart` afterwards.
- [ ] Live verification, on the fixture in `../baseline-adoption/fixture.md`. **A magicka reading
      does not discriminate** — it reads full at rest and regenerates in under a minute (finding
      12). The spell landing needs a damaged target or a visible effect.
- [ ] Restore fixtures and close Skyrim after runtime work.

## Runtime evidence so far (2026-08-12)

Only the first cell above is closed. What the session could reach was the *arrival* of the
commitment point, not the *survival* it buys: nothing in a normal cast tears the state away, so
"committed casts survive a teardown" stays unproven until ticket 10's cut is exercised by hand.
Ticket 10 leans on it, and says so.

The uninterrupted regression cell is partly covered — plain casts from a drawn weapon behaved
exactly as before across the session, and a control cast landed on a 5000 HP fixture (5000 →
4992.5) — but ritual, dual, and both concentration kinds are untouched, so the cell stays open.

## Known gap

A cast whose exhale clip carries no `Voice_SpellFire_Event` annotation has no commitment point and
falls back to today's behaviour. SYHO's clip does raise it (the sibling project's T37 trace), but
that is one clip. Worth a note rather than a mechanism until a clip is found that does not.
