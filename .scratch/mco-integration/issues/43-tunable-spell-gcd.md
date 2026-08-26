# 43 — The spell GCD is a setting, not a constant

Ticket 42 made the lockout one press-anchored number (1.5 s) with SpellFire as the floor. The
owner's hands-on verdict (2026-08-25): "feels better. It properly blocks cast. It feels a bit
slow… I almost feel like having a tunable GCD makes sense." The log settles what "slow" is made
of — clips 1–3 free at exactly 1.50 s (the GCD's number), clip 4 at ~1.80 s (its SpellFire
annotation is the floor) — so the number is the right knob for three of four clips and the
fourth is an annotation decision, not code.

**Status:** ready-for-agent

**Blocked by:** nothing.

## The change

1. **`spell_gcd` as a GameData setting**, exactly the `potion_gcd` shape (`game_data.cpp:140`,
   `set_potion_gcd`/`get_potion_gcd` at :357-364 with clamp 0.1–10.0, Papyrus pair
   `setPotionGCD`/`getPotionGCD` in `papyrus_functions.cpp`, MCM slider + config
   save/load like the potion one). Default **1.5**.
2. **`CastingInstanceSpell` and `CastingInstanceRitual` read it at construction** instead of
   the literal 1.5 (`casting_controller.cpp:677`, `:685`). Instants, shout, potion, and the
   weapon-art per-art `GlobalCooldown` are untouched — arts already have their own number in
   the catalogue.
3. **Fix the retirement label** (carried from ticket 42's acceptance): `released by
   spellfire-floor` never prints because the FNF retirement branch only runs after delivery,
   so `note_lockout_waiting_for_spellfire` never sees the committed=false window. Set the flag
   where the SpellFire event lands instead (`casting_controller.cpp:154` handler): if the
   instance's lockout is already expired when the event arrives, note it there. Keep
   `classify_fnf_retirement` as is; extend the combo_cache test if the flag's seam moves.

## Out of scope

- A per-clip or per-spell GCD. One number for the action class, per the ticket 42 model.
- Clip 4's annotation (owner's call: leave the finisher heavy at ~1.8, or move
  `MLh_SpellFire_Event` earlier in `MSCO_left4.hkx` to bring it under the GCD).

## Acceptance

- [ ] MCM shows the spell GCD slider; value persists through save/load and config export.
- [ ] Live: set 1.0 → Firebolt press-to-free ≈ 1.0 s in the log; set back to 1.5 → ≈ 1.5 s.
      No rebuild between the two measurements.
- [ ] Floor unaffected: clip 4 still held to its SpellFire at either setting, and the
      retirement line now reads `released by spellfire-floor` there.
- [ ] Spam refusal and mid-swing art deferral regression cells still pass (ticket 41/42).
- [ ] Owner hands-on: dial by feel; record the number they land on.
