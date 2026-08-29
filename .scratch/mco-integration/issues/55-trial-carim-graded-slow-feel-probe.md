# 55 — Trial-install CARIM: does graded slow feel right? (feel probe only)

**Status: CLOSED wontfix 2026-08-29. Do not run this trial.** The owner rejected CARIM's shape
on performance after the script read, and the objection is structural, not a tuning matter: its
`_DISTR.ini` gives the detection perk to every `ActorTypeNPC`, so every NPC carries a scripted
ActiveMagicEffect that registers eleven animation events. Four of them -- `attackStop`,
`bowDraw`, `arrowRelease`, `weaponSheathe` -- fire for every combat actor whether or not anyone
is casting, and each firing enters `OnAnimationEvent`, which runs two `GetEquippedItemType`
calls and several `HasMagicEffect` checks before it can decide the event was irrelevant. The
concentration path additionally holds a Papyrus stack open for the length of every channel
(`while IsCastingRight / Utility.Wait(0.1)`), per casting actor.

The graded-slow direction dies with it. See `notes/57-mechanism-comparison.md` for what CARIM
actually does and the one part still worth stealing (the `CarryWeight` refresh kick, if a
future mechanism ever needs a `SpeedMult` change to take on a moving actor).

**Type:** trial install + owner feel test (throwaway; the mod does NOT ship)

**Status:** ready-for-agent. **Blocked by:** 54 (oracle first). Part of the ticket 53 umbrella.

## Why

The owner: CARIM "was actually closer to what i had envisioned for concentration spells, slowed
speed." But the owner also rejects Papyrus for shipping ("prefer an skse and behavior
approach"), so this trial answers ONE question only: **is the graded-slow FEEL what the owner
wants**, before any C++ port is specced. Cheap: SPID + scripts + light ESP — no Nemesis regen.

## Protocol

1. Install `C:\Users\Rando\Downloads\Casting Aiming Reloading Impede Movement-63459-1-31-1777153574\`
   (plain folder) as an MO2 mod; enable mod + ESP by closing MO2 gracefully and editing
   modlist/plugins per the standing rule (never ask the owner to tick anything). Requirements
   SPID + PapyrusUtil are already in the load order.
2. Launch. Owner config via MCM if wanted (per-category toggles, percentage — try the default,
   then a high value for concentration).
3. Owner feel cells: moving-entry equipped-hand channel; standstill entry; an enemy mage
   channeling (Fort Snowhawk Master Necromancer `0x000D7790` via MoveTo + StartCombat is the
   established fixture). Instrument alongside with the ticket 54 sampling recipe for a
   slowed-signature record.
4. **Uninstall completely** (untick mod + plugin) in the same cycle as ticket 56's install.
   NEVER save while its ability is applied — the orphaned-AV trap (quarantine any save that
   crossed it).

## Kill criteria / outcomes

- Owner says the feel is wrong → graded slow dies as a direction; 57 compares only against
  hard commit.
- Owner says the feel is right (at some percentage) → 57's spec includes the C++ port of
  CARIM's mechanism (cast-edge events, record-owned debuff, CarryWeight kick, EnableAI blip)
  into SH2's DLL.
- Either way the mod itself comes out after the trial.

## Acceptance

- [ ] Owner verdict recorded verbatim (feel: right/wrong, at what percentage, which cast types).
- [ ] Slowed-signature telemetry capture stored under `.scratch/mco-integration/evidence/t55/`.
- [ ] CARIM fully uninstalled afterward; no save carries its effects.
