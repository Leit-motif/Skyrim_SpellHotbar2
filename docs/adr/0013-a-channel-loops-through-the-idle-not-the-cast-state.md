# A concentration channel loops through the idle set, not the cast state

A hotbar concentration cast enters the `shtb` cast state for its start clip only. When that clip
ends, the state ends with it and the actor returns to the drawn idle, where the hold is sustained
for as long as the player keeps the key down. The Driver Cast state is not re-entered, held open,
or looped.

The loop lives in the idle because that is where Spell Hotbar 2 built it. The mod ships 24
concentration OAR submods under
`meshes/actors/character/OpenAnimationReplacer/SpellHotbar2/`, keyed on
`SpellHotbar_SpellAnimationType` (`SpellHotbar.esp` form `815`) and gated on
`SpellHotbar_isCastingConcSpell` (form `834`). None of them adds a looping clip generator. Each
replaces the idle and locomotion set — `1hm_idle`, `dw1hm1hmidle`, `staff_idle`, `h2h_idle`,
`mlh_idle` and the `mt_turn*` / `magcast_turn*` family — for as long as `834` is raised. The
channel loops because the idle state loops. That is why there is no `MODE_LOOPING` generator
anywhere in the mod, and why every clip generator in the `shtb` Nemesis patch is
`MODE_SINGLE_PLAY`.

The rejected alternative was to hold the cast state for the hold. `MscoCastDriver::replay` did
exactly that: every half second, if the state had ended, it re-sent `SH2_CastRight` and the
single-play start clip played again from the top. That is a restart, not a loop, and it also
held the actor out of the one state that owns the real loop, so the channel never reached it.
Holding the state open instead would need a looping generator this mod does not have and, under
the ticket 25 scope decision, may not author.

Three consequences follow, and they are the decision as much as the shape is:

- **The globals are the channel's liveness, not the state.** `815` and `834` are raised when the
  cast commits and cleared by `reset_animation_vars()` when the hold ends. Anything that clears
  them early ends the visible channel even though the cast instance is still alive.
- **A channel does not walk the cast-combo index and does not open the follow-up-press window.**
  Both are properties of a clip that is still playing (ticket 22). A channel's clip is gone.
  `CastShape` in `combo_cache.h` is what tells the driver which of the two it is running.
- **A channel's chain-out window is the hold itself.** It opens at the commitment point, when
  the spell starts streaming, and closes when the player lets go or attacks. An attack inside it
  ends the channel and becomes the swing, and the channel hands its MCO combo position on
  through the ADR-0005 write-back, so the swing continues the chain the channel interrupted.

Where a family has no submod for a combination, the mapping is recorded rather than invented:
ritual concentration keeps `11001` in both slots because it ships one submod, and ward
concentration keeps `1003` in both because there is no dual ward pose in vanilla to map onto.
See `skse_plugin/src/game_data/cast_anim_ids.h`.
