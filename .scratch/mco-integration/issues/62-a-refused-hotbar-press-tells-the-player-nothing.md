# 62 — A refused hotbar press tells the player nothing

**Type:** defect (UX), owner-reported. **Handed over from the Shouts for MCO side 2026-08-30** after
that engine was investigated and cleared.

**Status:** DONE — owner-accepted 2026-08-31. **No code was written for this ticket.** The red
feedback already existed; the ticket was filed from a stale read of the source. See the correction
below. One follow-up observation is recorded and deliberately not fixed.

## What the owner hit

Mid-fight against bandits, hotbar cast of Whirlwind Sprint: *"it seemed like one of my casts of
Whirlwind Sprint was eaten and did not fire."* Then, on being shown where it went:

> "I was unable to cast without visual feedback, and that's still undesired behavior."

**The cast really was refused. The complaint is not that it was refused — it is that nothing said
so.** The press disappeared and the player was left to guess whether the mod was broken.

## CORRECTION 2026-08-31: it already drew it

**The premise below is wrong and stays only as a record of how the ticket was filed.** Every
refusal path already calls `RenderManager::highlight_skill_slot(slot, 0.5, true)`, which sets
`highlight_isred` and paints the slot red:

- `modes.cpp:74` — a spell refused by `allowed_to_cast` (casting / sprinting / swimming / jumping).
- `modes.cpp:119` — the outer press gate, the `cast live=true` refusal.
- `modes.cpp:298` — the same two, in the second input mode.

`allowed_to_cast` at `input.cpp:927` is a pure predicate: it logs and returns false, and its
callers own the drawing. Reading only that function is what produced the wrong premise.

**Owner verification, 2026-08-31**, jumping and pressing a hotbar slot: red fired on a spell and
red fired on a shout. *"I'd say for where we are right now, that is suitable."*

## The original (incorrect) premise, kept as a record

From `SpellHotbar2.log` during that fight — the only two refusals in the session:

```
20:14:46.992  SH2: slot 8 refused by the press gate (type=6, cast live=true)     modes.cpp:117
20:14:55.652  SH2 cast: refused, casting=false sprinting=false swimming=false jumping=true
                                                                                 input.cpp:927
```

So the refusal is **already decided, already attributed, and already written to disk**. The two
gates are `input.cpp:927` (casting / sprinting / swimming / jumping) and `modes.cpp:117` (a cast is
already live). Everything needed for feedback is in hand at the moment of refusal.

## The owner's ruling on what feedback means

Recorded in the Shouts for MCO repo's `DECISIONS.md`, 2026-08-30:

> **A shout pressed during a stagger is BLOCKED WITH FEEDBACK, never buffered.** *"just block the
> input. It should show red, as in this spell/ability is not castable/usable right now... If you
> were playing World of Warcraft and you were stunned and you tried to use an ability, you would get
> an error."*

Clarified the same day: **the design language is the red border on the Spell Hotbar 2 icon.**
Whether there is an error sound is **undecided** — ask before adding one.

The ruling names a stagger because that is what the investigation first suspected. **The stagger was
a wrong lead** (see below); the ruling's substance is the general one — a refused press must show
red rather than vanish. Apply it to the refusal reasons that actually exist.

## Why this is not the Shouts for MCO engine's to fix

Checked in that repo's trace and cleared, so nobody re-runs the investigation:

- **ShoutMCO is never consulted for these.** Both gates refuse inside SH2, before `try_start_cast`
  reaches `CastIntent::offer` and therefore before `ShoutMCO_CastIntentApi::Request` is called. A
  `SHOUTMCO_CAST_REJECTED` return could not have fired for either press.
- **The engine's own ledger balanced** for the session: 15 intents deferred, 13 released, 2
  cancelled, nothing leaked.
- The first analysis blamed a stagger on the vanilla shout key. **That was wrong** — it fixated on a
  `SHOUT KEY down` during a stagger in ShoutMCO's log without first cross-checking SH2's refusals,
  and the owner correctly pushed back that he was on the hotbar.

## Acceptance

- [x] A62.1 — already drawn at `modes.cpp:74`. Owner confirmed live on a spell and a shout.
- [x] A62.2 — already drawn at `modes.cpp:119`.
- [x] A62.3 — 0.5 s, and `update_highlight` clears `highlight_slot` and `highlight_isred` when the
      timer expires, so it cannot bleed into the next cast. Owner read it fine in combat.
- [x] A62.4 — **no error sound. Owner ruled 2026-08-31: _"agree not necessary at this time."_**
- [x] A62.5 — owner drove it by jumping and pressing a slot. Nine refusals in the log between
      15:25:23 and 15:25:42, all `jumping=true`, session DLL `e27041a9`.

## Follow-up, observed and NOT fixed

The owner's **first** attempt after a fresh load showed no red, though the press was refused:
*"I first tried it with a spell upon a fresh load, and I didn't see any red feedback, but nothing
fired."* Every later attempt in the same session drew it.

One press, one session, no reproduction attempted, and the owner accepted the behavior as-is. A
plausible mechanism — `highlight_skill_slot` sets the flash whether or not the bar is currently
drawn, so a flash during the bar's post-load fade would be invisible — is a **guess from reading
the fade path, not a diagnosis.** Recorded so it is not rediscovered from scratch; open a fresh
ticket if it recurs.

## One loose end handed over with it

There is a `>>> SHOUT KEY down` in ShoutMCO's log at **20:14:56.526**, mid-stagger, with **no SH2
activity anywhere near it** and no shout resulting. If the owner was purely on the hotbar, its
source is unexplained. It is not the press he felt, and it may be a stray physical press — but
unexplained input events have been worth chasing in this pair of mods before. Worth a glance if SH2
turns out to synthesise shout-key input on any path.
