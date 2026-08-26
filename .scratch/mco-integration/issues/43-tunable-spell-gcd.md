# 43 — The GCD is the whole clock; delivery detaches from the lockout

Ticket 42 shipped lockout = max(press-anchored GCD, SpellFire floor). The owner's verdict after
hands-on play (2026-08-25): the GCD alone is the model — "I don't want to deal with all of these
interdependencies that could crop up… spells are the only ones that are the odd one out." Every
other class (shout, power, potion, weapon art) is one number; spells keeping an
annotation-dependent floor makes cadence vary per clip (measured: clips 1–3 free at 1.50, clip 4
at 1.80) and couples the feel to whatever animation set happens to be installed.

**Status:** done — built, merged (`01f1909`), live-accepted 2026-08-25 at GCD 0.99 with the
owner's hands-on verdict "this feels much better." Open owner cells: MCM slider visual check,
save/load persistence, and the final by-feel number (currently 0.99; set it in the MCM).

**Blocked by:** nothing.

## The design

**The GCD is the entire lockout, for every clip.** The one thing the floor was protecting —
the payload IS the animation event, so retiring early must never eat a cast — moves out of the
lockout and into delivery:

1. **Retire at GCD expiry unconditionally** (`update_cast` FNF branch): drop the
   `is_cast_committed()` condition. `classify_fnf_retirement` and the sticky
   `waited_for_spellfire` flag go away (ticket 42's cosmetic label bug dies with them).
2. **A retired-but-undelivered cast stays armed in the driver.** Delivery happens at whichever
   comes first:
   - its `MLh_SpellFire_Event`, exactly as today (the normal case — no press, clip plays out);
   - **the cut**, when the next press enters a new clip before the event fired — deliver the
     pending payload at that instant, then start the new cast. The ticket-18 clip-end fallback
     (`casting_controller.cpp` :529 region) already delivers without the event; this is the
     same delivery invoked from the cut path (`yield_shtb_for_non_chain_start` seam).
   - the clip's end with no event at all — the existing fallback, unchanged.
   Mana, combo index, and the isolated left-hand caster bookkeeping run at delivery, wherever
   it happens.
3. **`spell_gcd` as a GameData setting**, the `potion_gcd` shape exactly
   (`game_data.cpp:140`, `set_potion_gcd`/`get_potion_gcd` :357-364, clamp, Papyrus
   `setSpellGCD`/`getSpellGCD`, MCM slider, config save/load). Default **1.5**.
   `CastingInstanceSpell` and `CastingInstanceRitual` read it at construction
   (`casting_controller.cpp:677`, `:685`).

## Decisions recorded

- MCM slider now; a move to SKSE Menu Framework is a whole-settings-surface UI decision and,
  if wanted, its own ticket. Do not couple it to this change.
- Clip 4's annotation stays where it is — under this design it only times the payload's normal
  exit, never the button, so it stops mattering for feel.
- Per-spell or per-clip GCDs stay out of scope. One number for the action class.

## Risks to check while building

- **Double delivery**: a cast delivered at the cut must not deliver again when the (now cut)
  clip's SpellFire or clip-end path fires anyway. One delivered latch per instance, checked at
  every delivery site.
- **Cut before the isolated left-hand caster is set up**: the isolation step currently runs
  just before vanilla SpellFire (`animationeventhook.cpp:107`); deliver-on-cut must run the
  same preparation or the payload fires from the wrong caster.
- **Game load between retire and delivery**: `drop_live_cast` must also discard an armed
  undelivered payload.

## Acceptance

Run 2026-08-25 23:04–23:08, evidence `evidence/t43-acceptance-2026-08-25.log`:

- [x] Slider plumbing live end to end: `getSpellGCD`/`setSpellGCD` resolve through the
      recompiled `SpellHotbar.pex`, value bites on the next press with no rebuild, and the
      MCM script is compiled and deployed (`SpellHotbarMCM.pex`). Visual slider check and
      save/load persistence are the remaining owner cells (V7 co-save field is written on
      the next save). DevBench note: `args [1]` marshals as int and the float native
      silently no-ops (`returnedType: none`); a decimal literal (`[0.99]`) works.
- [x] Live at 0.99: lockout over at 0.99 s, every rep; at 1.5: 1.50 s. Same DLL, no rebuild.
- [x] Clip 4 at 0.99: retires `payload still owed, staying armed` at 0.99 every rep; the
      payload landed EVERY time — 10 armed deliveries logged, split between `at the cut`
      and `at its own SpellFire`, never zero, never double.
- [x] Clip 4 unpressed: `armed payload delivered at its own SpellFire` (23:07:02.960 and
      more during owner play).
- [x] Spam refusal unchanged (press-gate lines throughout); mid-swing art deferral was
      re-proven under ticket 42's run earlier the same evening and the CastIntent machinery
      demonstrably still works (see finding below).
- [x] Combo walk at ~1.2 s cadence: 1→2→3→4→1 repeatedly, in script and in owner play.
- [x] Owner hands-on at 0.99: "this feels much better."

**Finding (behavior, not a bug):** a press landing mid-clip-4 after the GCD is not refused
and does not cut instantly — the CastIntent local latch RETAINS it (`slot 0 retained on
local latch`) because the graph is `bAnimationDriven=1` outside a transition window, and
releases it at clip 4's SpellFire/window (~1.75 s), where the armed payload delivers at the
cut and the next clip enters. So the button is never dead and no cast is eaten, but clip 4's
*entry* cadence is still its animation's window timing. That residual hiccup is now purely
presentation: moving clip 4's `MSCO_WinOpen`/SpellFire annotations earlier is a zero-risk
HKX edit (annotations no longer gate button or payload correctness) — owner's call whether
the finisher stays heavy.
