# 43 — The GCD is the whole clock; delivery detaches from the lockout

Ticket 42 shipped lockout = max(press-anchored GCD, SpellFire floor). The owner's verdict after
hands-on play (2026-08-25): the GCD alone is the model — "I don't want to deal with all of these
interdependencies that could crop up… spells are the only ones that are the odd one out." Every
other class (shout, power, potion, weapon art) is one number; spells keeping an
annotation-dependent floor makes cadence vary per clip (measured: clips 1–3 free at 1.50, clip 4
at 1.80) and couples the feel to whatever animation set happens to be installed.

**Status:** ready-for-agent

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

- [ ] MCM shows the spell GCD slider; value persists through save/load and config export.
- [ ] Live at 1.0: Firebolt press-to-free ≈ 1.0 s; at 1.5: ≈ 1.5 s. No rebuild between.
- [ ] Clip 4 at GCD 1.0: next press at ~1.1 s cuts it and the payload still lands (delivery
      log line at the cut, projectile/mana effect observed) — no eaten cast, ever.
- [ ] Clip 4 unpressed: payload delivers at its own SpellFire (~1.78 s) as today.
- [ ] Spam refusal and mid-swing art deferral regression cells still pass (tickets 41/42).
- [ ] Combo walk 1→2→3→4 at 1.0 s cadence — cuts every clip, combo still advances.
- [ ] Owner hands-on: dial by feel; record the number they land on.
