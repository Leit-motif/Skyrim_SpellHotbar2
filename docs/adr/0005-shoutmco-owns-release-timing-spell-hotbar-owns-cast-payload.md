# ShoutMCO owns release timing; Spell Hotbar owns the cast payload

Date: 2026-08-08

Status: accepted

## Context

A Direct Cast hotbar activation never reaches ShoutMCO's vanilla shout-input hook. Building an
independent MCO event sink and `HitFrame` buffer in this fork would duplicate the engine's timing
policy, create races between two state models, and make future compatibility depend on both copies
remaining identical.

ShoutMCO cannot own the alternative either: a hotbar payload includes slot assignment, spell,
resources, state, and execution semantics that belong to this fork and may change independently.

## Decision

Spell Hotbar 2 is a cast driver. It retains its payload and calls ShoutMCO's optional versioned C
ABI synchronously. ShoutMCO returns `pass through` or `deferred`, then provides exactly one
main-thread `release` or `abandon` callback for a deferred intent.

On release, Spell Hotbar 2 revalidates the current slot assignment, cooldown, resources, player
state, and normal Direct Cast preconditions, then attempts once. On abandonment or replacement it
clears the payload. It does not poll ShoutMCO or independently observe MCO events.

If the API is absent or its major version is incompatible, Direct Cast fails open and the UI/log
reports `unavailable` or `incompatible` once without a popup.

This boundary schedules when a cast may begin. It does not replace ADR-0004: once a cast has begun,
the graph's `Voice_SpellFire_Event` remains its commitment point.

## Consequences

- ShoutMCO remains independent of Spell Hotbar 2 and never receives a spell or slot payload.
- Spell Hotbar 2 has no independent MCO timing state, polling loop, or worker thread.
- The animation T-pose stays a separate Spell Hotbar compatibility defect even though it blocks
  end-to-end publication acceptance.

## Scope amendment — 2026-08-12

The original decision is unchanged. ShoutMCO still owns release or abandonment of a deferred Cast
Intent. Spell Hotbar 2 still owns the payload. It still does not poll ShoutMCO, still does not
observe MCO events to decide when a cast may begin, and still does not keep an independent MCO
timing cache for that purpose.

"It does not independently observe MCO events" is a **release-timing** rule. It forbids a second
HitFrame / window / "may this cast start now" cache. It does not describe combo-position continuity.
The Consequences bullet "no independent MCO timing state" is the same rule: no second authority for
when a press may start. A sample used only to restore combo position is not that state.

**Named exception — combo position across a Driver Cast.** For a Driver Cast this mod itself
started, Spell Hotbar 2 may sample attack and ready tags and write `MCO_nextattack` /
`MCO_nextpowerattack` so the swing that follows continues the combo. Preserve the sampled value;
never derive a next index. Gate the write to a cast this mod started. Do not call ShoutMCO for
this: its API answers whether a press may start during someone else's animation, not where a cast
sits in the combo (ownership answer 2026-08-12).

**What the exception is not.** Those tags are not a HitFrame buffer, a chain window, or any other
"when may this press start" policy. Combo restore and release timing do not share state, callbacks,
or a cache. Ticket 13 implements the exception against this contract.
