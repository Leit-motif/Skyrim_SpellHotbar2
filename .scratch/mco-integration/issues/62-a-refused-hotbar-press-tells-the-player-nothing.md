# 62 — A refused hotbar press tells the player nothing

**Type:** defect (UX), owner-reported. **Handed over from the Shouts for MCO side 2026-08-30** after
that engine was investigated and cleared.

**Status:** open, ready-for-agent. Not started.

## What the owner hit

Mid-fight against bandits, hotbar cast of Whirlwind Sprint: *"it seemed like one of my casts of
Whirlwind Sprint was eaten and did not fire."* Then, on being shown where it went:

> "I was unable to cast without visual feedback, and that's still undesired behavior."

**The cast really was refused. The complaint is not that it was refused — it is that nothing said
so.** The press disappeared and the player was left to guess whether the mod was broken.

## SH2 already knows the reason and already logs it. It just does not draw it.

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

- [ ] A62.1 — a hotbar press refused at `input.cpp:927` draws the red border on that slot's icon,
      for each live reason: casting, sprinting, swimming, jumping.
- [ ] A62.2 — a press refused at `modes.cpp:117` (`cast live=true`) draws it too.
- [ ] A62.3 — the border is visible for long enough to read during combat, and does not persist into
      the next successful cast.
- [ ] A62.4 — no error sound unless the owner rules for one; ask first.
- [ ] A62.5 — driven in game by the owner, who reproduces a refusal on purpose (jumping is the
      cheapest: jump and press a hotbar slot) and confirms the feedback reads as "not usable now".

## One loose end handed over with it

There is a `>>> SHOUT KEY down` in ShoutMCO's log at **20:14:56.526**, mid-stagger, with **no SH2
activity anywhere near it** and no shout resulting. If the owner was purely on the hotbar, its
source is unexplained. It is not the press he felt, and it may be a stray physical press — but
unexplained input events have been worth chasing in this pair of mods before. Worth a glance if SH2
turns out to synthesise shout-key input on any path.
