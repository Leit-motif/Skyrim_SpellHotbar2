# 53 — Adopt Enemy Magelock's casting commitment (the state-machinery approach)

**Type:** research + trial-install, then integration build (Nemesis patch + FOMOD)

**Status:** ready-for-agent — spec approved shape, prototype-first, owner-supplied reference.

**Blocked by:** None. Supersedes ticket 33's mechanism (33 stays open only for its record of
what failed and its acceptance list, which this ticket inherits).

## The ruling that shaped this ticket

After three failed mechanisms in one morning (2026-08-29: the `bAnimationDriven` plant on the
layered vanilla concentration states — fails for player AND NPC; the DLL movement-event capture
— movement is poll-side; the conditioned SpeedMult −100 record — a crawl, not a root, and the
slow latched past the cast), the owner ruled:

> "this can only happen through rooting the character in the manner that it's done for msco
> animations such as aimed, etc. no other strategy will work."

and then supplied the reference implementation:

> "https://www.nexusmods.com/skyrimspecialedition/mods/49378 — i suppose i could just install
> this, but i want you to use the approach here. it's already been done apparently (by our
> favorite botuser999)"

**Enemy Magelock — NPC Magic Casting Commitment** (Nexus 49378, BOTuser999, 2021, 2.8k
endorsements): "Commits the NPC in place when they cast magic animations... they are now fully
commited to there cast." Archive: `C:\Users\Rando\Downloads\Enemy Magelock-49378-1-0-0-1619990342.rar`,
extracted for study at the session scratchpad `enemy-magelock/` (re-extract from Downloads if gone).

## What the reference actually is (first-pass read, 2026-08-29)

- A Nemesis patch, mod code `altmag`, entirely in `magicbehavior`: 34 vanilla-node patches and
  **199 new nodes — 27 `hkbStateMachine`s, 64 state infos, 14 clip generators, 19 blending
  transition effects, 12 modifier generators, 9 transition arrays**. This is owned state
  machinery wrapping the cast flow — the MSCO shape — not a modifier bolted onto vanilla states.
- The clips are intro/release wrappers: `MLh_PreAimedCon`, `MLh_PreCharge`, `MLh_PreReady`,
  `MLh_PreTelekinesis`, `MLh_Release`, `MLh_SelfRelease`, plus staff variants
  (`StfMagic_Pre*`/`Release`) and four `1HM_Walk*AttackIntro` names. Files ship via DAR
  `_CustomConditions\94010` with condition `Random(1)` (always active) — the DAR-as-file-carrier
  trick, presumably to reuse registered animation names; it ALSO patches
  `animationdatasinglefile` (sections 87/89/92/94, both genders).
- Its one `BSIsActiveModifier` binds **`bAllowRotation` only** (`#altmag$51/$52`) — no
  `bAnimationDriven` anywhere. The rooting mechanism is the state structure + clips, which is
  precisely the owner's ruling and consistent with every failure this week: layered states
  cannot be rooted by flags; owned full-body flow roots by construction.
- Contention warning: it patches `#0281` (MRh ready transitions — the SAME array MSCO gates
  with `iMSCO_ON==0`), `#0088` (LeftHandMagicCast_MSG), and ~30 more vanilla nodes in the exact
  territory `msco`, `pscd`, `sbeef`, and `shtb` already contest. Nemesis resolves single-value
  conflicts last-checked-wins (measured on ticket 33) — the integration risk is real and is most
  of this ticket's work.

## Phases — evidence before authoring, kill criteria at each gate

**Phase 0 — calibration captures (minutes, needs owner hands, game up).** Owner performs two
known-good moving-entry roots — an MSCO fire-and-forget equipped cast and a hotbar channel —
while telemetry records the signature of CORRECT rooting (bAnimationDriven / position / heading
at ≤150 ms cadence, cliplog filtered to the player). This signature is the headless oracle every
later cell is compared against BEFORE costing an owner feel test. (Lesson: ticket 33's NPC
false-positive — AI stillness masquerading as a working root. Displacement proves nothing unless
the actor demonstrably wanted to move.)

**Phase 1 — trial-install, unmodified (fastest possible evidence).** Install Enemy Magelock
exactly as shipped into this load order (mod folder + tick `altmag` + Update Engine + Launch —
new file set), and let the owner feel an enemy mage. Questions it answers: does the approach
root in THIS stack at all; what does it do to the player (the mod claims NPC scope — establish
how it distinguishes, if it does); what breaks against msco/pscd/sbeef/shtb (read the merged
output for every contended node before launching, list the displacements). KILL CRITERIA: if
the unmodified mod cannot root an NPC channel here, the approach needs rework, not adoption —
stop and re-spec. Restore point: untick `altmag`, regen.

**Phase 2 — full dissection (headless, parallel with Phase 1's owner half).** Produce the
mechanism map: per cast type (aimed FF, aimed concentration, self, ward, telekinesis, staff),
which altmag state hosts it, entered how, exited how, and WHAT stops translation (state
structure vs clip content vs transition wiring). Deliverable: a document a builder can implement
from without opening the reference again. Also: exactly how player vs NPC scope is achieved,
and the full contention table against our stack's patches.

**Phase 3 — integration spec + hostile review.** Decide adopt-vs-adapt: (a) ship Enemy Magelock
as a FOMOD-bundled dependency with a compatibility patch for the msco/shtb contentions, or
(b) author our own `shcc2` patch implementing the dissected mechanism scoped to concentration
only (the owner's actual want — Magelock commits ALL NPC casting, which may be wanted or may
over-reach; MSCO already owns FF). The spec carries kill criteria and the six-state scope
explicitly, gets a second-model review (Cursor/Codex, not self-review), and the owner rules on
adopt-vs-adapt before any authoring.

**Phase 4 — build to the spec, one state first.** Single-state prototype (right-hand aimed
concentration), validated headlessly against the Phase-0 signature, THEN one owner feel test.
Only after that cell is owner-green does the remaining matrix get built. No FOMOD work until
the mechanism is certified.

## Process guards (the anti-rush rules, owner-demanded)

1. No mechanism is assumed transferable — every extrapolation gets a one-state live probe first.
2. Telemetry oracles are calibrated against owner-certified-correct behavior before use.
3. Every phase has kill criteria; a miss stops the line and re-specs rather than stacking a new
   mechanism on an undiagnosed one.
4. The integration spec is reviewed by a second model before authoring begins.
5. Never save a test game mid-trial; quarantine any save that crossed a plugin change.

## Cleanup carried from ticket 33 (do during Phase 1's restart cycle)

- Disable + delete the trial `houseCARL - SpellHotbar_RootedConcentration` mod (records are
  inert but dead weight; the SpeedMult approach is rejected).
- Decide the DLL movement capture's removal (inert since `shcc` unticked; it is dead code
  keyed to a failed mechanism — default: revert `98350c9`'s input.cpp/combo_cache.h half in the
  next DLL change, keep the ADR amendment corrected).
- ADR-0015 needs its fourth amendment: the third amendment's premise (capture fixes layered
  states) is disproven; the ruling is now "commitment requires owned full-body state machinery"
  with Enemy Magelock as prior art.

## Acceptance

Inherits ticket 33's acceptance list unchanged (NPC displacement on a real mage, rotation
tracking, end paths, player equipped-hand parity, FOMOD option, evidence naming), plus:

- [ ] Owner feel sign-off on an enemy mage channel ("no perceivable stutter, needs to feel
      natural") — the cell that failed every prior mechanism.
- [ ] Owner feel sign-off on player moving-entry equipped-hand channel.
- [ ] The msco/pscd/sbeef/shtb contention table exists and every displacement is deliberate.
- [ ] Phase-0 calibration signatures recorded and cited by path.
