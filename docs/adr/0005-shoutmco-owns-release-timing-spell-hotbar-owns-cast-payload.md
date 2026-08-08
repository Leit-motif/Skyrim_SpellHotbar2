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
