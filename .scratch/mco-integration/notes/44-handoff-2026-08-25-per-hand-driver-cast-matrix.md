# Handoff — restore the per-hand Driver Cast animation matrix, 2026-08-25

Branch `main`, baseline `bf442b2`. Tree was clean before this note. This session was read-only
apart from writing the handoff: no C++, Nemesis, OAR, HKX, MO2, or runtime state changed.

## Owner correction and direction

The Dragon Age staff-animation discussion exposed a larger omission in the fork's Driver Cast
design. The owner expected the new MSCO integration to preserve upstream Spell Hotbar 2's
distinction among left-hand, right-hand, and dual-cast presentation. It does not: the slot and
spell execution still distinguish those modes, but every fire-and-forget Driver Cast enters the
same four borrowed `MSCO_left1.hkx` through `MSCO_left4.hkx` clips.

Owner direction:

> We need to revisit our MSCO implementation and have the same pattern of distinguishing right
> casts, left casts, and dual cast for fire and forget spells. Technically, all of them, as well as
> staff variations.

Treat this as a matrix-design correction, not merely a Dragon Age asset import. Initial focus is
fire-and-forget Driver Casts; the design audit must cover every cast shape so concentration,
ritual, self, ward, staff, and dual variants remain coherent rather than accumulating another
one-off path.

## Confirmed current behavior

### Hand selection and spell execution are already distinct

- `hand_mode` stores `auto_hand`, `left_hand`, `right_hand`, `dual_hand`, or `voice`
  (`skse_plugin/src/bar/hotbar.h:33-40`).
- `set_weapon_dependent_casting_source()` resolves Auto, writes
  `SpellHotbar_CastingSource` (`SpellHotbar.esp` form `0x835`), and returns the resolved hand
  (`skse_plugin/src/game_data/game_data.cpp:1876-1915`). Left maps to the engine's left casting
  source, right to right, and dual uses the left source as its representative while delivery is
  separately told to dual-cast.
- `SpellHotbar_SpellAnimationType` (`0x815`) remains a second OAR-visible selector. Upstream data
  uses it for aimed/self/ritual/concentration families and dual/variant ids; casting source splits
  left from right within single-hand families.
- `cast_spell(..., m_used_hand == hand_mode::dual_hand, ...)` still distinguishes actual dual
  delivery (`skse_plugin/src/casts/casting_controller.cpp:584`).

### Driver Cast presentation is collapsed onto one left clip set

- Every fire-and-forget start resolves `used_hand` and passes it to
  `MscoCastDriver::begin()` (`casting_controller.cpp:978-981`).
- `MscoCastDriver::begin()` explicitly discards that value with `(void)hand`
  (`msco_cast_driver.cpp:256-262`).
- Entry depends only on `CastComboIndex`: `SH2_CastRight`, `SH2_Cast2`, `SH2_Cast3`, or
  `SH2_Cast4` (`msco_cast_driver.cpp:181-195`).
- Both `magicbehavior` and `1hm_behavior` bind those four states to
  `Animations\MSCO_left1.hkx` through `Animations\MSCO_left4.hkx`
  (`nemesis/Nemesis_Engine/mod/shtb/{magicbehavior,1hm_behavior}/#shtb$0/$4/$6/$8/$10.txt`).
- `arm_spellfire(used_hand)` also discards the resolved hand and arms only
  `MLh_SpellFire_Event` (`casting_controller.cpp:139-148`).
- The animation hook's vanilla-caster isolation is left-only
  (`events/animationeventhook.cpp:91-109`).

Therefore left-assigned, right-assigned, dual, and Auto-resolved fire-and-forget spells differ in
payload semantics but currently share one presentation matrix and one left-hand SpellFire
contract.

### Upstream already exposes the intended selection vocabulary

The installed upstream OAR tree contains separate definitions including:

- `cast_1h_left`
- `cast_1h_right`
- `cast_dual`
- `cast_1h_left_staff`
- `cast_1h_right_staff`
- corresponding concentration, start, self, ward, ritual, and dual folders

The single-hand configs combine `SpellHotbar_SpellAnimationType`,
`SpellHotbar_CastingSource == 0/1`, and player conditions. Staff variants add
`IsEquippedType` type `8` against the appropriate physical hand. This is evidence for the
selection dimensions, not proof that those legacy submods replace the fork's new `MSCO_leftN`
Driver Cast paths.

## Dragon Age donor pack

Source supplied by owner:

`C:\Nolvus\Projects\_animations\dragon-age-staff-animations\`

Inventory:

- `mco_attack1.hkx` through `mco_attack5.hkx`
- `mco_powerattack1.hkx` through `mco_powerattack7.hkx`

All twelve are valid reproducible Skyrim HKX files under `hkxc verify`, have 97 transform tracks,
and are unique by SHA-256. Durations range from 1.667 to 4.5 seconds. They are authored as MCO
attacks, carrying combinations of `weaponSwing`, `HitFrame`, `MCO_WinOpen/Close`,
`MCO_PowerWinOpen/Close`, `MCO_Recovery`, SCAR instructions, `MCO_nextattack` /
`MCO_nextpowerattack`, camera shake, and placeholder `PIE.$` / `SoundPlay.` events. They carry no
`MLh_SpellFire_Event` or `MRh_SpellFire_Event`.

The owner has not selected which clips go where. Some will become special attacks; some will
become staff-specific members of the hotbar cast chain. Preserve that choice boundary.

## Required design before implementation

Define one explicit selection matrix. Do not start by copying four clips into a right-staff OAR
folder; that would prove an asset path while leaving the newly discovered product omission in
place.

At minimum decide these independent axes:

1. **Resolved cast hand:** left, right, dual. Auto is a resolver, not a fourth presentation.
2. **Cast family:** fire-and-forget aimed/self, ritual, concentration aimed/self/ward, ritual
   concentration, plus their start/loop distinction where applicable.
3. **Physical equipment context:** ordinary, staff in left hand, staff in right hand. Decide
   whether a staff variation follows the physical staff hand, the resolved casting hand, or a
   deliberate cross-hand combination. Example: a spell assigned left while a staff is physically
   equipped right must not accidentally miss the right-staff set merely because casting source is
   left.
4. **Combo position:** fire-and-forget keeps the existing four-step walk. Decide whether each hand
   owns four independent clip paths or whether OAR selects hand-specific replacements of one
   neutral four-path graph.

Two viable implementation shapes must be weighed:

### A. One neutral graph matrix, OAR selects the hand/equipment variant

Keep the four registered paths and use `SpellHotbar_CastingSource`, animation type, dual-family
ids, and physical `IsEquippedType` conditions to replace each path. This is the smallest graph
change and can support the Dragon Age staff set without another Nemesis state. Its cost is that
the graph and SpellFire contract remain deceptively left-named, every pack must author the full
condition matrix correctly, and dual selection remains encoded partly through animation ids.

### B. First-class left/right/dual graph matrices

Register distinct clip paths and select them from the resolved hand (for example, four each for
left/right/dual), leaving OAR to select spell family and staff/equipment variants within the
correct hand family. This makes the model explicit and gives right-hand clips a natural
`MRh_SpellFire_Event`, but it expands the Nemesis patch, entry routing, tests, and runtime event
correlation.

Do not choose between A and B from aesthetics. Spike the smallest version that proves:

- OAR can reliably see the current casting-source/animation-type values while the new shtb state
  is active in both hosting graphs; and
- a right-hand SpellFire clip can commit exactly once without vanilla also firing an equipped
  right-hand spell.

If the first statement is reliable and the second forces substantial symmetric hook machinery,
shape A may be the deeper module despite its legacy names. If OAR cannot reliably distinguish the
state or the release event must be hand-correct for animation/effect integrity, shape B wins.

## Code seams a per-hand implementation must close

- Make `MscoCastDriver::begin(..., hand_mode, ...)` use the resolved hand or deliberately document
  why selection is delegated entirely to OAR.
- Replace `arm_spellfire()`'s hard-coded left mask. A dual cast must accept its authored event but
  deliver once, never once per hand.
- Generalize equipped-caster isolation. The current hook suppresses vanilla only for
  `MLh_SpellFire_Event` and interrupts only the left `MagicCaster`; a right-hand clip can otherwise
  complete an equipped right-hand spell alongside SH2's immediate payload.
- Preserve commitment, missing-annotation fallback, GCD retirement, four-step cast-index advance,
  SpellFire-to-WinClose window, combo sampling/restoration, Cast Plant, and watchdog behavior.
- Preserve per-graph event registration. Both `magicbehavior` and `1hm_behavior` host the shtb
  states, and a clip annotation resolves against its hosting graph's event table.
- Keep concentration separate from fire-and-forget. `SH2_Channel_State` is a held looping state on
  `1HM_Shout_Inhale.HKX`; single-play Dragon Age attacks are not channel-loop assets without an
  authored start/loop/end treatment.

## Donor adaptation contracts

For a Dragon Age clip used as a **Driver Cast**:

- choose the visual release frame and author the correct committing SpellFire event for the
  settled matrix;
- replace attack combo windows with the Driver Cast window contract;
- remove physical-hit, SCAR, camera-shake, and MCO next-attack instructions unless that particular
  cast is deliberately designed to retain them;
- author recovery and any intended `animmotion` explicitly;
- verify the binary after annotation edits.

For a Dragon Age clip used as a **Custom Ability / special attack**:

- retain useful `HitFrame`, `MCO_WinOpen/Close`, recovery, and intended motion;
- replace empty PIE/sound placeholders with the chosen effect payload;
- remove SCAR and combo-variable writes that conflict with SH2's Ability combo-continuity model;
- package through the existing `SH2_ArtSelector` Custom Ability path.

## Acceptance matrix

Static/build completion is not runtime acceptance. The first completed slice needs:

- left-assigned fire-and-forget visually selects the left set and delivers once;
- right-assigned fire-and-forget visually selects the right set and delivers once;
- dual fire-and-forget visually selects the dual set and delivers once with dual mechanics;
- Auto resolves to the same result as the corresponding explicit assignment;
- four consecutive casts visibly and in logs walk 1 → 2 → 3 → 4 for each supported hand set;
- every selected clip commits at its authored release and avoids the no-SpellFire fallback warning;
- staff-left and staff-right conditions activate only for their intended physical equipment
  contexts, including the cross-hand case (left-assigned spell while staff is held right);
- non-staff casts remain on their intended hand sets;
- drawn-weapon (`1hm_behavior`) and sheathed/magic (`magicbehavior`) hosts both work;
- equipped left/right spells do not double-fire when a hotbar Driver Cast raises SpellFire;
- concentration, ritual, Ability, MCO attack-chain, combo restoration, and Cast Plant regressions
  remain closed;
- visible animation identity is proven by OAR animation log plus a frame/video, not inferred from
  a successful cast or graph event alone.

## Next action

Create the implementation ticket (next free number is currently 44) from this handoff. Its first
deliverable is the decision-complete matrix and A-vs-B spike result. Clip selection from the Dragon
Age donor pack follows owner review; do not assign donor clips to cast or Ability roles on the
owner's behalf.
