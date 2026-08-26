# Ticket 44 — the Driver Cast selection matrix

Design document, first deliverable of ticket 44. Evidence base:
`44-inventory-2026-08-26.md` (every factual claim below cites it or the code directly). The
A-vs-B mechanism decision is the ADR's job; this document defines WHAT must be selectable and
the rules that resolve every combination, so the ADR and the implementation tickets can be
written without re-deriving intent.

## The selection key

Every player cast resolves to one presentation through a five-part key:

```
(resolved hand, cast family, staff-left?, staff-right?, combo index)
```

- **Resolved hand** — `left`, `right`, or `dual`. Auto is a resolver, not a value: it runs
  before the matrix (`set_weapon_dependent_casting_source`, `game_data.cpp:1879-1919`) and
  every downstream consumer sees only the resolved hand. `voice` never reaches the matrix
  (shouts keep the vanilla shout graph, `casting_controller.cpp:1356-1420`).
- **Cast family** — the eight `cast_anim_ids.h` families (aimed, self, ritual, ritual-self,
  aimed-conc, self-conc, ritual-conc, ward-conc), which split into two presentation shapes:
  fire-and-forget walks the four-clip set; concentration holds the channel state with a
  start/loop split on `SpellHotbar_isCastingConcSpell` (ADR-0013).
- **Staff-left / staff-right** — two independent physical booleans (OAR `IsEquippedType 8`
  per hand), NOT a property of the resolved hand. Keeping them independent is what makes the
  cross-hand case expressible at all.
- **Combo index** — fire-and-forget only, the existing 1→4 walk.

## Decisions

### D1. Dual is a first-class hand value, never a source refinement

Upstream already treats it that way: dual is a separate animation-type id (10016/10017/
11003/11004) with no casting-source condition, while left/right split one id by source
(inventory §1.2). Delivery is structurally single (`SetDualCasting(true)` on one caster,
`casting_controller.cpp:1523-1533`), so "dual delivers once" is free; only presentation and
the SpellFire contract need dual awareness.

**Corollary (bug to fix in implementation):** a dual cast slower than 1.51s currently leaks
into the plain single-hand family because `start_ritual_cast` picks the variant slot by cast
time (`casting_controller.cpp:1140-1146`, `spell_cast_data.cpp:96-108`). Under this matrix a
dual cast presents dual at any cast time.

### D2. Staff variation follows the physical hand, layered over the resolved hand

The staff booleans refine the resolved hand's cell; they never change which hand casts.
Rationale: a staff clip exists to make the arm holding the staff look right, and each hand's
arm is posed independently in the clip. So the cell for (left, aimed, staff-right) is "left
hand casts, right arm carries a staff" — a real, distinct presentation.

**Fallback chain, defined per cell:** exact match → casting-hand staff state only (drop the
off-hand boolean) → plain resolved-hand set. Which cells get dedicated art is an owner asset
decision; the chain guarantees every combination presents something coherent today. This
closes the handoff's cross-hand warning without forcing asset authoring: upstream's data
falls through to plain-left for (left cast, staff right) because its staff condition is
welded to the casting source (inventory §6.2); ours degrades the same way by DEFAULT but the
cell exists and can be filled.

### D3. The combo index is shared across hands

One `CastComboIndex`, hand-agnostic, exactly as today (`msco_cast_driver.cpp:54-60`). A
chain that alternates hands continues the 1→4 walk rather than each hand keeping its own
position. Rationale: the walk exists to vary presentation across consecutive casts and to
pace the chain; per-hand indices would make alternating-hand play repeat clip 1 forever,
the opposite of the feature's point. Per-hand clip SETS vary the look; the index stays the
cadence.

### D4. Ritual families are hand-degenerate

Ritual and ritual-conc use both hands by definition; the hand axis collapses to one column.
Staff booleans still apply in principle but no upstream or MSCO ritual-staff asset exists —
the cells exist with the D2 fallback to the plain ritual set.

### D5. Ward-conc has no dual cell

Structural upstream fact (`cast_anim_ids.h:49`, no dual ward family or pose exists). The
matrix marks it N/A rather than fallback — a dual-pressed ward is downgraded before the
matrix (`casting_controller.cpp:1269-1273`).

### D6. Auto must resolve identically to the explicit assignment

Acceptance-level rule from the handoff: Auto is resolved once, upstream of the matrix, and
the resolved value enters the same cell as the explicit hand. Today's resolver maps a
right-hand staff to `kRightHand` and everything else non-dual to `kLeftHand`
(`game_data.cpp:1817-1839`); the resolver's quality is out of this matrix's scope, but the
left-staff blindness (a left-held staff classifies as `FIST`, `game_data.cpp:799-802`,
814-819) is filed as an implementation ticket because it starves both the resolver and any
DLL-side staff logic of the signal.

### D7. Concentration stays off the clip set

The channel is a held state on `1HM_Shout_Inhale.HKX` sustained by OAR idle/locomotion
replacement (ADR-0013); per-hand concentration variation is selected by OAR within the held
state (upstream already does exactly this with 24 conc submods, inventory §1.3). No new
graph states for concentration hands.

### D8. SpellFire contract is per-resolved-hand, delivery latches once

- left → arm `MLh_SpellFire_Event`; right → arm `MRh_SpellFire_Event`; dual → arm both.
- Ticket 43's latch already collapses multiple accepted events to one delivery
  (`m_spell_started` + single `armed_cast`, inventory §4.3). A dual clip may raise either or
  both events; first one commits.
- Vanilla isolation generalizes symmetrically: when the driver is active, an armed hand's
  SpellFire event isolates THAT hand's caster and is swallowed before vanilla
  (today left-only, `animationeventhook.cpp:97-110`); same for the begin()-time equipped
  spell interrupt (`msco_cast_driver.cpp:621-636`). Dual isolates whichever armed event
  arrives, each event its own hand.
- Graph prerequisite: `MRh_SpellFire_Event` is in `magicbehavior`'s base event table but NOT
  in `1hm_behavior`'s — it must be registered in the shtb MOD_CODE block there
  (`1hm_behavior/#0085.txt:464-474`) or right-hand commits silently fall to the timer floor
  in the drawn-weapon host (inventory §3.3).

## The matrix

Hand columns after D1/D4/D5; each non-N/A cell carries the D2 staff refinements
(ordinary / staff-L / staff-R / staff-both where assets exist, else fallback chain).

| Family (id) | left | right | dual | notes |
| --- | --- | --- | --- | --- |
| aimed (1 / dual 10016) | cell | cell | cell | fire-and-forget, 4-step walk |
| self (2 / dual 10017) | cell | cell | cell | fire-and-forget, 4-step walk |
| ritual (10000) | — degenerate — | | | one column (D4); fast ritual borrows dual ids today |
| aimed-conc (1001 / 11003) | cell | cell | cell | channel start/loop via 0x834 |
| self-conc (1002 / 11004) | cell | cell | cell | channel start/loop |
| ward-conc (1003) | cell | cell | N/A (D5) | channel start/loop |
| ritual-conc (11001) | — degenerate — | | | one column (D4) |

Out of matrix: voice/shout (vanilla shout graph), potions (no driver), weapon arts /
Custom Abilities (SH2_Art_State + `SH2_ArtSelector`, ADR-0016).

**Initial implementation slice** (per the handoff): fire-and-forget aimed + self, all three
hands, ordinary equipment, both hosting graphs — with the staff cells expressed in
conditions but allowed to ride the fallback chain until the owner assigns art. MSCO already
ships bindable `MSCO_right1..10` and `MSCO_dual1..10` clips and staff submods keyed on
`IsEquippedType 8` (inventory §6.3), so the right/dual cells are not asset-blocked; which
specific clips (MSCO's, or Dragon Age donors) fill which cells stays owner-gated.

## Known hazards the implementation tickets must carry

1. **The SYHO priority duplicate.** A second copy of all 55 upstream submods at +2e9
   priority (`Spell Hotbar 2 - OAR Priority Over SYHO`); any condition or priority change
   must land in both trees or the bumped copy silently wins with old rules (inventory §1.6).
2. **MSCO's own submods sit on the same clip paths.** `Base - Left/Right Staff` (prio
   6800/6801, pure equipment conditions) very likely already staff-swap the fork's
   `MSCO_left1..4` behind its back, and its `CurrentDeliveryType` submods likely never match
   an SH2 cast (both UNPROVEN — one live OAR-log read settles them). Any SH2-authored
   variant selection must outprioritize or deliberately compose with MSCO's.
3. **Dual >1.51s family leak** (D1 corollary).
4. **Left-staff blindness in the DLL** (D6).
5. **Nemesis Update Engine** is required after any shtb file-set change; a parallel
   right+dual state set is ~80 new `#shtb$N` files plus event/transition/state-list edits
   across both graphs (inventory §3.5) — the scope figure that prices shape B.
