# 15 — Chain MSCO hand casts and hotbar casts in both directions

**Type:** feature (driver + input), parent ticket 11

**What to build:** A committed hotbar cast and an MSCO hand cast chain into each other in both
directions, and one press fires one spell. The 2026-08-11 matrix had MSCO → hotbar working and
hotbar → MSCO not. Owner playtest on 2026-08-12 found a broader failure: left-hand spell + hotbar
1 fires both.

**Blocked by:** None — can start immediately. Ticket 10 already ships the attack-press cut this
mirrors.

**Status:** ready-for-human

## What this is not

Not combo-position restore (ticket 13). Not consecutive hotbar clips (ticket 14). Not
concentration: left-hand Flames resetting the combo is expected and out of scope.

## Behaviour

A hand-cast press during a committed hotbar cast ends the hotbar state the same way an attack
press now does. An MSCO hand cast must still be able to chain into a hotbar cast. A hotbar press
while a spell is in the left hand must not also fire that left-hand spell.

- [ ] MSCO hand cast → hotbar cast chains, with no dual fire.
- [ ] Hotbar cast → MSCO hand cast chains, with no dual fire.
- [x] Left-hand spell + hotbar slot does not cast both.
- [x] Concentration hand casts remain out of scope and must not be treated as a counterexample.
- [x] Restore fixtures and close Skyrim after runtime work.

## Comments

**2026-08-13 — dual-fire closed on the public path; chain cells need owner presses.**

The 2026-08-12 playtest's broader failure is a second delivery of the equipped left-hand
spell: the borrowed `MSCO_left*` clip raises `MLh_SpellFire_Event`, and an uncaptured
hotbar key is vanilla Hotkey1. A committed left-hand cut already existed from ticket 11
and is unchanged in shape (ticket 10's attack cut, mirrored). This ticket adds:

- Capture a handled spell-slot press when the left hand holds a spell/scroll, so vanilla
  does not also see it.
- Isolate the left-hand MagicCaster at Driver Cast `begin()` (`InterruptCast`) so the
  clip's SpellFire cannot complete the equipped spell.
- Policy for both lives on `combo_cache.h` with `combo_cache_test`.

A mid-MSCO-charge interrupt was not added: that would cancel an uncommitted hand cast.
MSCO → hotbar stays the 2026-08-11 post-delivery path (`allowed_to_cast` still refuses
while `IsCasting`). Concentration is still a refusal, not a cell-4 counterexample.

**Observed on Save65** (Xaelle, Iron Rapier, Firebolt left, profile `Nolvus Awakening`).
Runtime DLL `SpellHotbar2.dll` SHA-256
`F5212733D6F323399F4AB9104295BD533E604564997EEC100B2473765646DC6F` (isolate path;
later `D760FEF35434C6F463CA8CE5557CEAE63AD9BA57CED39314F6F5955AD5B15550` drops the
uncommitted-MSCO overlay). Log:
`Documents\My Games\Skyrim Special Edition\SKSE\SpellHotbar2.log`.

`castSlot(0)` with magicka regen stopped (`forceav magickarate 0`, pool 200):
`isolated left-hand caster to prevent dual fire`, `SH2_CastRight (clip 1) -> true`,
left SpellFire, `SH2_CastExit -> true`. Magicka **200 → 168.4** (one cost, not two).
A follow-up `castSlot(0)` walked clip 2 the same way (ticket 14 unregressed).
`combo_cache_test` green.

The two chain cells ride `DispatchInputEvent`. Injected input never reaches that hook
(ticket 10). Owner: Firebolt left, press 1 (one projectile), then committed hotbar →
left attack, and MSCO FNF → hotbar during the window.

Fixtures: spawned Bandit `FF0012F3` disabled/deleted; `setessential 3DE8A 0`; `tcai`
toggled back; Save65 reloaded (magicka 1000, health 500, Firebolt left, Iron Rapier);
`qqq`; DevBench ping offline. MO2 left running.
