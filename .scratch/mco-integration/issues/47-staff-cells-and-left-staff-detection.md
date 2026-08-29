# 47 — Staff cells: physical-hand staff variants and the left-staff blind spot

Second slice of ADR-0018. The matrix's D2: staff variation follows the PHYSICAL hand,
layered over the resolved casting hand, with a defined fallback chain (exact → casting-hand
staff state → plain hand set). Upstream welds the staff condition to the casting source, so
the cross-hand case (spell left, staff right) falls through to plain-left; ours makes it a
real cell.

**Status:** ready-for-agent (after ticket 46) — scope 1's aimed cells landed with ticket 60

Ticket 60 shipped `cast_left_staff` (2000001111) and `cast_right_staff` (2000001112) on MSCO's
own `Base - Left Staff` / `Base - Right Staff` art, conditioned on `IsEquippedType 8` with the
physical-hand boolean. What remains here: the cross-hand cells (spell in one hand, staff in the
other), hazard 2's live check, and the CROSS-HAND cells (a spell in one hand, a
staff in the other), which still ride the plain hand set.

Two of this ticket's questions are now answered and are NOT open work:

- **Self plus staff** is settled: plain self art, matching MSCO, which has no staff-self art at
  all (its `Self` submods outrank its `Staff` submods). Ticket 60 built self-staff cells on
  MSCO's staff clip 5, the owner saw the mismatch in play, and they were deleted the same day.
- **Scope 2, left-staff detection**, shipped with ticket 60: `EquippedType::STAFF_LEFT`
  classifies a left-held staff wherever the right hand establishes no stance of its own, maps
  to the magic bar, and resolves Auto to `kLeftHand` explicitly.

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
