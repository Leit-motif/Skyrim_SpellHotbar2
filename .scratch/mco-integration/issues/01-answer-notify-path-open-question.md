# 01 — Answer the open question: is the notify path honoured from an MCO attack state?

**Type:** research (live)

**What to build:** A recorded answer to the two-part open question in `CONTEXT.md`, good
enough that tickets 03 and 04 can be designed against it rather than around it.

**Blocked by:** None.

**Status:** claimed

Baseline `795fb5f`. Session A also carries two riders, agreed 2026-08-03, because the game
has to be loaded for this ticket anyway and a Nolvus launch costs ~4 minutes:

1. **Input-injection test (2 min, run first).** Establish whether computer-use keystrokes
   reach Skyrim. DevBench has no input injection — `console`, `papyrus`, `inspect`, `game`,
   `menu` and `scenario` cannot press a key — and Skyrim uses DirectInput, which may ignore
   synthetic events. Bind a slot programmatically, inject the key, watch magicka. If it
   works the owner hands over the machine and leaves; if not, the owner presses keys and
   nothing else. This decides the shape of every remaining live session, so it runs before
   anything expensive.
2. **Confirm the SYHO overlay** (`../issues/05-...`). Purely visual, needs one cast, free
   while the game is up.

Spell binding and frame capture are **not** owner tasks and must not be asked for.
`SpellHotbar.saveBarsToFile` / `loadBarsFromFile` set bar contents programmatically, and
`ancient-magic-frost-rework/tools/capture_ingame.ps1` captures the game window in the
background via `PrintWindow` without taking focus. Both were done by hand in the previous
session; that was waste.

This is first because it can invalidate the design of the chain-out and mid-swing work. Do
not start those until this is answered.

- [ ] Drive a hotbar cast from inside a live MCO attack state on the controlled fixture, and
      record whether the graph honours the `ShoutStart` **notify** (this mod's entry) as it
      honours the shout **control** entry.
- [ ] Determine whether the cast's `IsShouting` liveness check survives the teardown pass
      that entry provokes — the pass running `MCO_AttackExitNotify` → `attackStop` → `inRdy`
      about 3 ms in. A single false reading anywhere across it kills the cast on its first
      update.
- [ ] Distinguish the two failure shapes explicitly: graph refuses the transition, versus
      graph accepts and the liveness check kills the cast. They look identical from the
      player's seat and imply completely different fixes.
- [ ] Repeat enough times to tell a deterministic result from an intermittent one. If it is
      intermittent, say so and characterise when — intermittent is the dangerous answer,
      because it gets misdiagnosed as a transition problem.
- [ ] Record the answer in `CONTEXT.md`, replacing the open question with what was found.
- [ ] Restore fixtures and close Skyrim.
