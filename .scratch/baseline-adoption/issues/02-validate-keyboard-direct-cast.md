# 02 — Validate Direct Cast with physical keyboard

**What to build:** Evidence that Direct Cast behaves according to normal upstream expectations when activated from a physical keyboard in the controlled Nolvus Awakening fixture.

**Blocked by:** 01 — Establish the reproducible validation fixture.

**Status:** claimed

- [ ] Exercise representative fire-and-forget, concentration, self-targeted, and aimed/projectile spells through physical keyboard bindings.
- [ ] Cover first-person and third-person casting, left-hand and right-hand choices, and dual casting where the Installed Configuration and character state enable it.
- [ ] Observe initiation, targeting, sustain/release or cancellation, animation, movement restrictions, resource cost, cooldown, and promised restoration of equipment or selected-power state.
- [ ] Check that physical keyboard activation does not unintentionally trigger another Skyrim or installed-mod action.
- [ ] Record every exercised matrix cell as passed, failed, or open with evidence tied to the controlled save, exact environment, commit, and binary.
- [ ] Inspect the SpellHotbar2 log for the coherent test run and attach screenshots for visible acceptance claims.
- [ ] Restore the controlled fixture and close Skyrim unless an immediate authorized follow-up requires it to remain open.

## Comments

### 2026-08-03 — First live session

Environment: fixture re-verified before launch (profile fingerprints, tested binary
`9846FB9B…` built from `a50bda1`, and both halves of the controlled save all matched the
ticket 01 record). Launched through MO2's SKSE shortcut on profile `Nolvus Awakening`;
DevBench online; controlled disposable save
`Save20_EBCD0A92_…_QASmoke_000547_20260723170328_17_1` loaded. Character Xaelle, level 17
Nord, 339 magicka, greatsword equipped right and Incinerate left.

**Setup obstacles resolved before any cell could run.** The installed configuration's
auto-profile is the controller preset, so no keyboard keys were bound; and its Bind Menu
key is Numpad 5, which the owner's TKL keyboard does not have. Both were fixed live through
the mod's own Papyrus surface rather than by editing files: `SpellHotbar.loadConfig`
loaded `all_bars.json` (verified — `GetKeyBind(0)` returned scancode 2, the "1" key), and
`SetKeyBind(22, 210)` moved the Bind Menu to Insert. `isDisableMenuBinding` read false
afterwards, so ordinary menu binding worked; an earlier warning that the preset might leave
it disabled was wrong.

**Cells recorded** (see `../acceptance-matrix.csv`):

- `KB-FAF-3P-1` **passed** — Blight Curse on slot 1, single tap, third person. Cast fired
  once; magicka 339.0 → 317.56; equipment unchanged after the cast.
- `KB-CONC-3P-1` **passed** — Sparks on slot 4, 3.0 s hold, third person. Sustained while
  held, stopped cleanly on release; magicka 339.0 → 313.38 = 25.62 total (~8.5/s);
  equipment unchanged.

The first reading was taken with magicka regen still running, so its cost is only a lower
bound. Regen was then frozen with `player.forceav MagickaRate 0` (baseline `MagickaRate`
3.0, **must be restored**) and every later cost figure is exact. One intervening cast — Ice
Spike, slot 2 — produced no usable cost data because regen had already refilled the pool;
it is not recorded as a cell. It was also driven by a wrong instruction: it was asked for as
a concentration test when Ice Spike is fire-and-forget.

`KB-FAF-1` and `KB-CONC-1` specify **first person** and stay `open`. The owner declined
first-person testing entirely. The third-person observations are recorded as separate cells
rather than folded into the first-person ones, because a cell may not pass on evidence from
conditions it does not name. User story 10 still asks for first person, so ticket 07 cannot
report a complete baseline while these are open.

### Material Interaction reproduced: SYHO shadows every casting animation

`SEAM-OAR-1` is recorded **failed**. Every hotbar cast played the shout animation from
`SYHO - Shout Your Heart Out` instead of this mod's own clip.

Attribution is unambiguous and needs no further reproduction: SYHO's 8 OAR submods declare
priorities `99999990`–`99999996`; this mod's 56 submods top out at `99901002`. SYHO's
*lowest* priority outranks this mod's *highest*, so wherever both sets of conditions pass
SYHO wins every time. This is not intermittent and not a tuning accident.

This confirms `CONTEXT.md` finding 11, which raised exactly this hazard and explicitly left
it unconfirmed because SYHO's conditions had not been read. They still have not been read —
the priority comparison alone is sufficient here, since it holds regardless of conditions
wherever both match.

One consequence goes beyond cosmetics and should shape any fix: **SYHO's clip does not
loop.** A fire-and-forget borrowing the wrong animation looks wrong; a concentration spell
channelling for an arbitrary duration against a one-shot clip has no animation that can
represent it at all. So the remedy cannot be "renumber and hope" — this mod's own
concentration clips have to win, or sustained casts stay visibly broken.

Not yet evidenced by a captured frame; the claim currently rests on the owner's direct
observation. `SEAM-OAR-1` belongs to ticket 05, and remediation belongs to a separate
diagnosis ticket, not to this one.

### Open findings carried out of this session

- **Battle Mage perk tree renders blank.** Not explained. The CSF2 choice is correct —
  `CustomSkills.dll` 2.0.2 contains the string `Data/NetScriptFramework/Plugins`, which is
  where this install's config sits — and the tree reportedly works on the `Dev - Skeleton`
  profile, which points at a Nolvus-side interaction rather than a wrong installer answer.
  Nolvus enables six other Custom Skills trees plus a Custom Skill Menu overhaul, so a menu
  or skydome collision is the first thing to check. Owner deferred it. `FEAT-FOMOD-1` stays
  `open`.
- **The mod ships no `.seq` file**, and all three of its quests (`SpellHotbarInitQuest`,
  `SpellHotbarMCMQuest`, `SpellHotbarBattleMageInitQuest`) carry `Flags = 273`, which
  includes Start Game Enabled. Start-game-enabled quests in a plugin added to an existing
  save do not start without a `.seq` listing them, which would explain the missing MCM. All
  three were started by hand with `startquest` this session — a one-session workaround that
  no fresh install inherits. **This did not fix the Battle Mage tree**, so it is not the
  whole story, and whether it fixed the MCM was not confirmed before the session moved on.
  Treat the `.seq` gap as a real and generally applicable defect — it is not Nolvus-specific,
  so it belongs in the Core Fork — but not as a proven explanation of either symptom.

### Fixture state — NOT yet restored

`MagickaRate` is currently forced to `0` on the player and **must be restored to `3.0`**.
Skyrim was still running at the end of this entry. The controlled save was loaded but never
saved over.
