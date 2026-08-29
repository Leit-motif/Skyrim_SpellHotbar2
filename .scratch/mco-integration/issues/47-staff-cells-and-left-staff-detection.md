# 47 — Staff cells: physical-hand staff variants and the left-staff blind spot

Second slice of ADR-0018. The matrix's D2: staff variation follows the PHYSICAL hand,
layered over the resolved casting hand, with a defined fallback chain (exact → casting-hand
staff state → plain hand set). Upstream welds the staff condition to the casting source, so
the cross-hand case (spell left, staff right) falls through to plain-left; ours makes it a
real cell.

**Status:** closed 2026-08-29 — scope 1 landed with ticket 60; scope 2 landed with ticket 60;
the two leftovers are answered by ADR-0018's amendment, not open work

Ticket 60 shipped `cast_left_staff` (2000001111) and `cast_right_staff` (2000001112) on MSCO's
own `Base - Left Staff` / `Base - Right Staff` art, conditioned on `IsEquippedType 8` with the
physical-hand boolean. Everything this ticket asked for is now answered:

- **Self plus staff** is settled: plain self art, matching MSCO, which has no staff-self art at
  all (its `Self` submods outrank its `Staff` submods). Ticket 60 built self-staff cells on
  MSCO's staff clip 5, the owner saw the mismatch in play, and they were deleted the same day.
- **Scope 2, left-staff detection**, shipped with ticket 60: `EquippedType::STAFF_LEFT`
  classifies a left-held staff wherever the right hand establishes no stance of its own, maps
  to the magic bar, and resolves Auto to `kLeftHand` explicitly.
- **The cross-hand cells** (spell in one hand, staff in the other) need no cell. MSCO plays
  plain art there — its `Left Staff` keys on a staff in the LEFT hand and replaces the left
  clip set, so casting from the hand that holds the spell gets the plain set — and SH2 does
  the same thing today. ADR-0018's amendment ("agree with MSCO, do not out-specify it") is
  exactly the rule that settles this, and it settles it in favor of the current behavior.
  Building the cell would recreate the divergence the self-staff cells were deleted for.
- **Hazard 2** (does MSCO's 6800/6801 swap our clips behind our back?) is answered: every SH2
  cell outranks them at 2000001101+, and ticket 60's live pass included staff casts in both
  hands with the owner confirming the art.

This ticket's framing premise — "ours makes the cross-hand case a real cell" — is the thing
that got dropped. Reopen only if a live session shows the plain set reading wrong with a staff
in the off hand.

**Blocked by:** ticket 46 (the pack this extends).

## Scope

1. **Staff submods in `SpellHotbar2Casts`:** per D2, condition on `IsEquippedType 8` with the
   explicit hand boolean, additive to the hand/family conditions, priority above the plain
   cells. Owner ruling: MSCO's `Base - Left Staff` / `Base - Right Staff` clip sets ARE the
   Dragon Age staff animations — reuse those bytes (`MSCO_left1..6` / `MSCO_right1..6` from
   the staff submods). Cross-hand cells (e.g. left cast, staff right) exist in conditions and
   may ride the fallback chain until art is assigned; the matrix's hazard 2 check (does MSCO's
   6800/6801 already swap our clips behind our back?) must be settled here with one
   staff-equipped cast and the Animation Log.
2. **Left-staff detection in the DLL:** `getPlayerEquipmentType()` inspects only the right
   hand for `kStaff` (`game_data.cpp:814-819`); a left-held staff classifies as `FIST`. Fix
   the classification (matters for bar selection and Auto), and re-examine
   `get_cast_hand_from_equip()`: a right-hand staff is today the one context that flips Auto
   to the right hand — decide and document what a LEFT staff does to Auto.
3. **SpellFire annotations** for any staff clip that lacks one, per ticket 46's audit flow.

## Acceptance

- staff-left and staff-right conditions activate only for their physical contexts, including
  the cross-hand case, in both hosting stances (Animation Log named winners + one frame per
  staff cell).
- A left-held staff is classified as a staff by the DLL (unit-test the classification seam if
  one exists, else log-verify), and Auto's resolution with each staff hand is documented and
  matches the matrix.
- Non-staff casts stay on their plain hand sets; ticket 46's rows stay green.
