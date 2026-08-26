# 44 — Per-hand Driver Cast matrix: left, right, dual, and staff variants

Created from `notes/44-handoff-2026-08-25-per-hand-driver-cast-matrix.md`, which is the
authoritative brief — read it in full before any work. Short version: the fork's MSCO
integration collapsed every fire-and-forget Driver Cast onto the four borrowed
`MSCO_leftN.hkx` clips. Payload semantics still distinguish left/right/dual, but presentation
and the SpellFire contract are left-only (`MscoCastDriver::begin` discards `hand_mode`,
`arm_spellfire` arms only `MLh_SpellFire_Event`, caster isolation is left-only). The owner
wants upstream's distinction restored: left, right, dual — and staff variations — across,
eventually, every cast family.

**Status:** closed 2026-08-26. All four deliverables shipped: the selection matrix
(`notes/44-selection-matrix.md`), the spike run with live evidence in both hosting stances
(`notes/44-spike-results-2026-08-26.md`, artifacts in `notes/44-spike-artifacts.md`, frame
`evidence/44-spike/right-cast-magic-2.png`), ADR-0018 (shape A), and implementation tickets
46/47/48. The spike also landed the per-hand SpellFire contract in C++ (arming, isolation,
any-hand commitment point, MRh registration in both graphs) and the `setSlotHand` test seam.

**Blocked by:** nothing for the design/spike. Donor clip assignment (Dragon Age pack) is
owner-gated: do not assign donor clips to cast or Ability roles on the owner's behalf.

## First deliverable (this ticket's scope)

Not an implementation. Per the handoff's "Required design before implementation":

1. **The decision-complete selection matrix** over the four axes: resolved hand (L/R/dual),
   cast family, physical equipment context (ordinary / staff-left / staff-right, including the
   cross-hand case), combo position.
2. **The A-vs-B spike result**, smallest version that proves:
   - OAR reliably sees `SpellHotbar_CastingSource` / `SpellHotbar_SpellAnimationType` while
     the shtb state is active in BOTH hosting graphs (`magicbehavior`, `1hm_behavior`);
   - a right-hand SpellFire clip can commit exactly once without vanilla also firing an
     equipped right-hand spell.
   Shape A = one neutral graph matrix, OAR selects the variant. Shape B = first-class
   left/right/dual graph matrices. Choose from the spike evidence, not aesthetics.
3. The chosen shape written up as an ADR (this decides the SpellFire contract and the Nemesis
   surface for everything after), plus the implementation ticket(s) it implies.

## Interaction with ticket 43 (worth exploiting in the design)

Ticket 43 makes the GCD the entire lockout and detaches delivery (SpellFire, cut, or clip
end — whichever first, one-delivery latch). Consequences for this matrix:

- The SpellFire contract no longer gates the button, only the payload's normal exit — so a
  hand-variant clip with a missing or mistimed event degrades to deliver-on-cut/clip-end
  instead of breaking cast cadence. The matrix is a presentation-and-delivery problem, not a
  feel problem.
- `arm_spellfire`'s replacement must fold into ticket 43's delivery latch: a dual cast
  accepts its authored event but delivers once, never once per hand.

Sequence accordingly: land 43 first, design 44's contract on top of its delivery model.

## Acceptance

The handoff's acceptance matrix carries over verbatim for the eventual implementation. For
THIS ticket: the matrix document, the spike transcript/logs (both hosting graphs), the ADR,
and the follow-on tickets — enough that the next agent can build without re-deriving any of
it. Visual identity claims in the spike need the OAR animation log plus a frame, not a graph
event.
