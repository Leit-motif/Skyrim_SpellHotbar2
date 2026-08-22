# 24 — Native MSCO should chain with the same feel as the rest of the package

**Type:** feature (Cast Intent / native MSCO)

Native left/right MSCO (a charge this mod did not start as a Driver Cast) should use the same
chaining policy as every other interactable: **no mash-through**, last-wins Cast Intent, chain
when the clip’s **WinOpen** window is open, close that window at **WinClose**.

**Blocked by:** weapon-arts ticket 10 (resolved). Do not start while 09/11 are in flight unless
the owner re-prioritizes.

**Status:** needs-triage

## You would test this

Profile `Nolvus Awakening`. 1H drawn. Ice Spike (or any FNF) in the left hand. Ability on the
bar. Driver Cast spell on the bar.

1. Native LH charge — tap Ability **during windup** (before WinOpen). The Ice Spike still
   throws. Ability starts at the chain window, not by cutting the throw. Left caster does not
   stick `IsCasting` until sheathe.
2. Native LH charge — tap Ability **during WinOpen**. Ability starts there.
3. After WinClose of that native clip, behaviour is whatever triage picks (queue until idle vs
   cut remaining recovery). Record the choice in this file before implementation.
4. Regression: hotbar Ability from idle still starts. Disengage (and other bound ashes) still
   play. Ticket 10 cells 1–7 still hold. Driver Cast follow-up still uses ticket 22
   (SpellFire → WinClose), unless triage deliberately unifies that clock too.

If Ability or Disengage goes dead with a spell in the left hand, it fails.

## What this is

Same *feel* for every interactable. Native MSCO is in the package, not a side path that Ability
can mash through. Mash-through means starting the next action so early that the current clip’s
hit / SpellFire never happens.

## What this is not

Not ShoutMCO’s clock for someone else’s MCO swing or a real shout (ADR-0005). Not ticket 11
fire-time. Not re-opening weapon-arts 09. Not a second Driver Cast buffer (that already lives in
weapon-arts 10).

## Known good (do not regress)

- Weapon-arts ticket 10: one Cast Intent. Local latch for **our** Driver Cast (ticket 22:
  SpellFire → WinClose) and Ability (WinOpen else HitFrame else `SH2_ArtExit`). ShoutMCO when
  busy with someone else’s swing / real shout.
- `0bc14d5`: `interrupt_left_caster_if_spell` on Ability begin, Driver Cast begin, and yielding
  our shtb clip. That fix stays even if this ticket later gates *when* Ability may start.

Today native MSCO is **not** on that local latch. Ability can `SH2_ArtStart` during a native
charge; interrupt then kills the caster. That is mash-through. It is the gap this ticket names.

## Failed approach (do not retry without a new design)

2026-08-21: treat `IsCasting` + a spell in a hand as local-busy, open the Driver Cast combo
window on `MSCO_WinOpen` (including native clips), and skip `allowed_to_cast`’s `IsCasting`
refuse when that busy bit was set.

Owner playtest: **Abilities stopped firing. Disengage did not work.** Reverted the same evening.
The likely mechanism: Ice Spike stays equipped, so `IsCasting` (or a sticky caster) made every
Ability press a retained Cast Intent whose latch never opened like an Ability latch.

Do not reuse `IsCasting && spell_in_a_hand` as “we own this clip.” Do not alias Ability WinOpen
detection onto the Driver Cast combo-window predicate. Do not move Driver Cast chain-open from
SpellFire to WinOpen as a drive-by in the same change unless triage says the whole package
shares one clock.

## Triage must decide

1. **One clock or two.** Native MSCO WinOpen–WinClose vs keeping Driver Cast on SpellFire–
   WinClose (ticket 22, MSCO cadence research in ticket 18 notes). Owner asked for WinOpen/
   WinClose for chaining; ticket 22 shipped SpellFire because clip 3 felt slow waiting for
   WinOpen.
2. **Busy detection** that cannot swallow idle Ability / Disengage.
3. **After WinClose** while the native clip is still playing: queue until idle, or allow a cut.
4. **Priority** vs Ability Editor 09/11.

## Notes

Owner 2026-08-21 after ticket 10: every interactable should have the same policy; look for
WinOpen / WinClose to allow chaining; native MSCO must not be mash-through. Regression fear is
explicit; last known good is ticket 10 + the interrupt commit.

## Comments

Filed from the 2026-08-21 native-MSCO unification attempt and revert.
