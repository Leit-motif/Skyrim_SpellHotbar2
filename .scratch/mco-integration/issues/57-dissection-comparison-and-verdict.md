# 57 — Dissect both references, write the comparison, get the owner's verdict, spec the build

**Type:** research + spec (decision-complete handoff; second-model review; no authoring)

**Status:** ready-for-agent. **Blocked by:** 55, 56. Part of the ticket 53 umbrella.

## Deliverables

1. **Mechanism map: Enemy Magelock.** Per cast type (aimed FF, aimed concentration, self,
   ward, telekinesis, staff): which altmag state hosts it, entered/exited how, and what
   physically stops translation (state structure vs clip content vs transition wiring — its
   one `BSIsActiveModifier` binds only `bAllowRotation`, so the rooting is structural). How
   player-vs-NPC scope is achieved. The DAR-as-file-carrier trick (`_CustomConditions\94010`,
   `Random(1)`) and the `animationdatasinglefile` additions, explained well enough to
   re-implement. Source: the extracted archive (re-extract from Downloads if the scratchpad is
   gone) + ticket 56's live evidence.
2. **Mechanism map: CARIM** — largely done in ticket 53's umbrella (read its .psc sources);
   finish with ticket 55's live evidence and the C++ portability assessment: SH2's DLL already
   sinks graph events (`animationeventhook.cpp`); confirm whether that sink sees NPC events or
   is player-only (MSCO's AnimEventFramework in `magic-casting-behavioral-overhaul/ref/src/`
   is the NPC-capable reference).
3. **The comparison**, against the owner's stated values: MCO-consistency ("rooted is
   preferable", 2026-08-28) vs the slowed-channel vision ("closer to what i had envisioned",
   2026-08-29), performance (no Papyrus ships — owner constraint), compatibility surface
   (Magelock's ~34 contended vanilla nodes vs a DLL port's zero behavior edits), scope
   (Magelock commits ALL casting; the want may be concentration-only), and effort.
4. **The owner's verdict**, asked with both trials fresh in their hands: adopt / adapt / compose
   (e.g. root concentration via state machinery, slow FF via the ported edge-driven AV shape —
   or one mechanism for everything).
5. **The integration spec for ticket 58**, decision-complete: exact scope, mechanism,
   compatibility strategy, FOMOD shape, kill criteria, and the one-state-prototype-first order.
   **Reviewed by a second model (Cursor via `scripts/cursor-delegate.mjs`, or Codex) before it
   is handed to 58** — no self-review.

## Constraints carried in

- No Papyrus in anything that ships (owner, 2026-08-29).
- ADR-0015 needs its fourth amendment written here: third amendment's premise disproven;
  commitment on layered states requires owned full-body machinery OR edge-driven native
  AV application (the preserved-design shape, now with the refresh/arrest tricks named).
- Ticket 33's acceptance list is the inherited bar; ticket 53 lists the leftover cleanup
  (trial ESP, inert DLL capture, playbook pointers).

## Acceptance

- [ ] Both mechanism maps exist, implementable without reopening the references.
- [ ] Comparison written; owner verdict recorded verbatim.
- [ ] Ticket 58 spec written, second-model-reviewed, and decision-complete.
- [ ] ADR-0015 fourth amendment drafted (lands with 58's build).
