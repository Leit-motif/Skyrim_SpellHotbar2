# 09 — Defer a hotbar press made during a shout instead of dropping it

**Type:** task (Core Fork)

**What to build:** A hotbar press made while the player is shouting is offered to ShoutMCO's
cast-intent API and cast when the shout releases, instead of being silently discarded.

**Blocked by:** Nothing. Independent of the spell→attack chain work — that one is about leaving
the cast state, this one is about entering it.

**Status:** ready-for-agent

## Why

**Owner ruling, 2026-08-12.** Raised from the ShoutMCO side while resolving its ticket 55 and
agreed the same day.

Today a hotbar press during a shout does **nothing at all** — no cast, no deferral, no refusal
sound, no notification. Both entry points read the graph and gate every useful branch on
`!is_shouting`, so the press falls through to `return false`:

| Site | Branch |
|---|---|
| `casting_controller.cpp:770` | `if (!is_shouting && (Spell \|\| Scroll))` |
| `casting_controller.cpp:878` | `else if (!is_shouting && AlchemyItem)` |
| `casting_controller.cpp:905` | `if (!is_shouting && FormType::Shout)` |
| `casting_controller.cpp:916` | `else if (!is_shouting && Spell)` — voice-slot power |

That guard predates the cast-intent API. The API's whole purpose is to hold a press that the graph
cannot accept yet and release it at a confirmed state, and a live shout is exactly such a state —
ShoutMCO already has a hold reason named for it.

**It also makes a real upstream fix load-bearing.** ShoutMCO ticket 55 corrected three sites that
end a `kBehindShout` hold, so they now abandon a driver's intent rather than only the player's own
press. Its drive found that fix is currently **dead code**, and this guard is the sole reason:
`kBehindShout` requires `shoutActive && windowOpen`, and no driver can ask for an intent in that
state while this condition stands. See
`.scratch/shout-mco-engine/issues/55-…` in the thuum repo for the full argument.

## The approach: delete a condition, do not add a branch

**Preferred: drop `!is_shouting` from the spell/scroll branch and let the existing refusal path do
the work.** With the guard gone the press runs its normal validation, reaches `start_cast`, and
`MscoCastDriver::begin` returns false because the graph is in the shout state and nothing there
listens for `SH2_CastRight`. That is `start_result::graph_refused`, which `resolve_start`
(`casting_controller.cpp:744-756`) already turns into `CastIntent::offer(slot, keybind)`.

So the mechanism is built, proven by ticket 04, and needs no new code — the press simply has to be
allowed to reach it.

**Verify the refusal actually happens before relying on it.** If `begin` returns *true* during a
shout — some listener consuming `SH2_CastRight` from the shout state — the cast would start on top
of the shout and this approach is wrong. One trace settles it; do not assume the refusal.

**Rejected: offering the intent directly at the guard.** It would skip the spell-known, magicka,
scroll-count and cooldown checks the normal path runs, and duplicate the refusal decision in a
second place. The payload is revalidated at release anyway (`offer` snapshots the slot and the
callback re-attempts the press), so nothing is gained by short-circuiting.

## Three branches that are NOT obviously in scope — decide, do not assume

Only the spell/scroll branch is clearly right to defer. The other three need a call:

- **`AlchemyItem` (`:878`).** `start_potion_use` never touches the animation graph, so there is no
  refusal to defer and no reason a potion should wait for a shout to end. Most likely this guard
  should simply be dropped so potions work during a shout, with **no** deferral. Cheapest correct
  answer, but it is a behaviour change in its own right.
- **`FormType::Shout` (`:905`).** A shout on a hotbar slot builds a `CastingInstanceShout`, which
  injects a `"Shout"` `ButtonEvent`. That press reaches **ShoutMCO's own input hook**, which
  already holds and replays it natively — so this path probably wants the guard dropped and *no*
  cast-intent involvement at all, letting shout→shout work the way a real shout key does.
  Deferring it through the API instead would put two holds over one press.
- **Voice-slot `Spell` (`:916`).** Same shape as the one above; decide it with it.

Getting these wrong is worse than leaving them alone. If the evidence for one is thin, ship the
spell/scroll branch and file the rest.

## Risks worth naming

- **Cast-intent replacement semantics.** `try_start_cast` calls `CastIntent::cancel()` on every
  press before deciding anything, so a second press during the same shout withdraws the first and
  offers its own. That is the intended one-entry-buffer behaviour (ShoutMCO ADR-0008) and needs no
  change — but it means rapid presses during a long shout must end with exactly one cast, and that
  is a cell worth driving rather than reasoning about.
- **The release point is ShoutMCO's, not this mod's.** Do not add a wait, a retry or a frame delay
  on this side; ADR-0005 and thuum ticket 54 both exist because of that temptation.
- **Multi-word shouts hold much longer than a swing.** A charged three-word shout can run several
  seconds, and ShoutMCO's watchdog abandons at `shoutWaitCapMs`. A press deferred early in a long
  shout may legitimately be abandoned rather than cast; confirm that surfaces as the ordinary
  refusal and not as a silent nothing.

## Acceptance criteria

- [ ] `MscoCastDriver::begin` is confirmed by trace to return false during a live shout, before
      the guard is relied on
- [ ] A hotbar spell pressed during a shout is deferred, and casts once when the shout releases
- [ ] ShoutMCO's own trace shows the deferral with the right reason —
      `>>> CAST INTENT deferred for a driver (handle N, behind a live shout)` — which is the first
      time `kBehindShout` has been exercised from a driver at all
- [ ] The spell actually lands: a damaged target or a visible effect. **A magicka reading does not
      discriminate** (finding 12)
- [ ] Two presses during one shout produce exactly one cast, and it is the second one's payload
- [ ] A press deferred behind a shout long enough to hit ShoutMCO's watchdog refuses visibly rather
      than vanishing
- [ ] No regression: a press with no shout running behaves exactly as it does today, and the
      ticket-04 mid-swing deferral still works
- [ ] The three out-of-scope branches are each either changed with a stated reason or left with one
- [ ] Fixture restored and Skyrim closed after the drive
