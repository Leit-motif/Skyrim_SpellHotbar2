# Ticket 05 — session end 2026-08-11 ~00:40, owner verdict: still broken, new approach wanted

The owner ended the session: **"it is still broken in the exact same way it has been for the
last 2 hours. we need an entirely new approach or i need to finally give up on this dream."**
Treat that as the acceptance verdict. Everything below is what the next session inherits.

## Hard results this session produced (log-verified, not in dispute)

The driver chain is real and complete — for both scripted `castSlot(0)` and the owner's own
"1" presses, `SpellHotbar2.log` shows: queued press → per-frame held repeats → `IsShouting`
true ~+50ms → 0.55s charge → timed release → `Voice_SpellFire_Event` ~+20ms → committed
cast, spell delivered, no stuck controls. OAR's animation log (owner screenshot) shows the
probe's MCO clip **selected and activated** on `1HM_Shout_Inhale/Exhale`.

Also fixed en route: instant word-fire (payload must be `kVoicePower` with real chargeTime),
phantom held button locking attack/sheathe (always release in restore), 0.5s→1.5s entry
grace, `castSlot` papyrus test seam, clip annotations stripped (MSCO events fired as graph
events mid-shout), `replaceOnLoop:false` removed from the probe config.

## The unresolved contradiction — read this before doing anything

- My background captures (4–5 fps PrintWindow bursts) of scripted casts show full pose
  sequences: wind-up → arm thrust with the fireball → recovery (test6/test9b strips under
  `.scratch/evidence/` on branch `claude/spellhotbar2-mco-animation-a6629d`).
- The owner, watching the live screen continuously, reports the SAME experience as always:
  **brief T-pose flash, then no animation**, on their presses AND unchanged after every fix.

Both can be true: sparse sampling cannot see a sub-250ms T-pose flash, and cherry-picking
the frames where the pose changed overstates smoothness. The owner's continuous observation
is the acceptance standard. Do NOT re-litigate "but the frames show motion" — get
**continuous** evidence (real video, or the owner's eyes) before claiming anything plays.

Unchecked candidate explanations for the discrepancy: first-person vs third-person camera
(probe covers 3rd person only — a 1st-person player sees nothing), stance/movement blends
(SCAR_1hmReadyDummy re-activation spam stomps the states when a weapon is drawn — owner
screenshot), presses during GCD, exhale duration vs the graph's exit timing.

## Options for the "entirely new approach" the owner asked for

1. **Drive MCBO's own casting states** (owner explicitly blessed this as plan B): send
   `MSCO_start_left` with gates `bIsMSCO`, `bMSCO_LeftCasting` true, `NotCasting` false —
   contract extracted 2026-08-08, experiments started, never finished. MCBO ships in this
   load order and its clips are authored for exactly those states — no clip adaptation at
   all. Start from the msco references in `C:\Nolvus\Projects\_animations\msco` and the
   thuum project's shout_behavior knowledge.
2. **Dedicated Nemesis behavior state** (ShoutMCO playbook — the owner built that engine;
   weeks, not hours, but fully owned).
3. **ESAS-style**: make the proxy shout's word spell BE the real spell and let the engine
   cast it (skip code-driven `cast_spell`); ESAS is installed (disabled) as a reference.

The voice-driver work is NOT wasted under options 1/3 — the entry/equip/input plumbing and
the test seam carry over.

## Runtime state left behind

- `Dev - Spell Hotbar 2`: this branch's DLL + pex (castSlot seam) — active.
- Probe mod `Spell Hotbar 2 - MCBO Cast Animations`: clips are MSCO_left1 copies with
  annotations STRIPPED (originals intact in `Magic Casting Behavior Overhaul`), config
  without `replaceOnLoop:false`, extended to exhale_medium/long paths.
- OAR ini override in MO2 overwrite: REMOVED (restored stock settings).
- Save `Codex_T05_Smooth_Riverwood` untouched. Skyrim closed at session end.
