# 04 — Hold hotbar input during an MCO swing

**Type:** task (Core Fork)

**What to build:** This mod's own equivalent of the sibling engine's mid-swing protection, on
this mod's own input path — so a hotbar cast pressed partway through an MCO attack no longer
tears the swing down.

**Blocked by:** 01 — Answer the open question.

**Status:** ready-for-agent

The sibling engine already solved this shape (its ticket 15): swallow the press, wait for the
swing, replay it. But it hooks `ShoutHandler::ProcessButton` and matches the `Shout` user
event, and **a cast triggered from a hotbar key never reaches that handler**. The pattern
transfers; the hook point does not. A cast pressed mid-swing still tears the attack down
exactly as before.

- [ ] Hold the press on this mod's own input path and hand it to the game after the swing
      lands, rather than reusing the engine's hook.
- [ ] **Gate on `HitFrame`, not `MCO_WinOpen`.** This correction cost the sibling project a
      build: on the measured power attack the window opens ~180 ms *before* `HitFrame`, and
      gating on it produced clean shouts with zero swing events. `HitFrame` is the event that
      means the hit landed. This inverts the ordering in that project's own finding 6.
- [ ] Decide and record what happens to a held press that is never followed by a `HitFrame` —
      swing cancelled, staggered, interrupted. A press that vanishes is its own data loss.
- [ ] Verify live: press a hotbar key mid-swing; the attack completes and the cast still
      happens afterwards.
- [ ] Confirm no added latency on a cast pressed outside an attack.
