# 46 — build note: the SpellHotbar2Casts pack and the arming rework

Built 2026-08-26 in an isolated worktree of `main`. **Nothing was deployed** — no MO2 mod, no
game, no Nemesis run, and `nemesis/` was not touched (ticket 44 already landed the `MRh`
registration in both of `1hm_behavior`'s parallel arrays). The coordinator deploys and runs the
live acceptance matrix.

Raw annotation dumps for every source and every shipped clip: `46-annotation-dumps.txt` beside
this file.

---

## 1. What shipped

```
data/meshes/actors/character/animations/OpenAnimationReplacer/SpellHotbar2Casts/
  config.json                 mod level: name, author, the IsPlayer conditionPreset
  cast_right/                 prio 2000001101   0x815 == 1      AND 0x835 == 1
  cast_right_self/            prio 2000001102   0x815 == 2      AND 0x835 == 1
  cast_dual/                  prio 2000001103   0x815 == 10016  (no source condition)
  cast_dual_self/             prio 2000001104   0x815 == 10017  (no source condition)
```

Each submod holds `MSCO_left1.hkx` … `MSCO_left4.hkx`. **The file names are the binding.** OAR
matches a replacement to the base animation by path, and the graph asks for
`Animations\MSCO_leftN.hkx`; a file named `MSCO_rightN.hkx` would match nothing and silently do
nothing. The submod FOLDER is the identity, and it is the folder name the Animation Log prints.

Grammar provenance is the spike's, unchanged: the `CompareValues` / global-form shape and the
`IsPlayer` preset are copied from upstream Spell Hotbar 2's own installed configs (spike note
§6). Lowercase `conditions` at submod top level, capital `Conditions` inside the preset's `OR`.
The values are read from the fork's code rather than assumed — `cast_anim_ids.h:30-31` gives
`kAimed{1, 10016}` and `kSelf{2, 10017}`, and `set_weapon_dependent_casting_source` writes
`RE::MagicSystem::CastingSource` so right is `1`.

### Priority choice

`2000001101`–`2000001104`, i.e. the Art Pack's own band (`2000001001`) plus 100, one per submod.

| competitor | priority |
| --- | --- |
| every MSCO submod on these paths | 6700 – 6901 |
| upstream Spell Hotbar 2 cast submods | 99000 0xx |
| `Spell Hotbar 2 - OAR Priority Over SYHO` | 101000 0xx |
| fork `SpellHotbar2Arts` Custom Ability | 2 000 001 001 |
| **this pack** | **2 000 001 101 – 104** |
| retired ticket-44 probe band (unused) | 2 000 002 xxx |

Distinct from the Art Pack, above everything that competes for these clip paths, below the
retired probe band, and the four are unique among themselves (OAR has no tiebreak for equal
priorities). No upstream submod was touched, and the SYHO duplicate mirrors upstream's tree
only — a fork-owned pack is not duplicated there.

---

## 2. Clip sourcing

**Correction to the spike note.** The spike recorded `Base - default` inside *MSCO Magic Casting
Behavior Overhaul - **custom animations*** as the runtime winner. That mod is **disabled** in the
active profile — `modlist.txt` line 456 reads `-MSCO Magic Casting Behavior Overhaul - custom
animations` — so nothing it ships reaches the VFS at all. The live source is the base mod, whose
`Base - default\MSCO_left1.hkx` happens to be byte-identical in size to the copy the spike took.
All eight sources below come from the enabled base mod.

Among MSCO's own submods on these paths, the highest-priority **unconditional** ones are:

- `MSCO_right1..4` → `Base - default` (6700). `MSCO Brand New Spell Cast Base` (6710) and
  `Base - default Variation 2` / `Variation 3` (6702/6703) are all higher but all carry
  `"disabled": true`. Everything above 6700 that is enabled is conditional (`Self Right` 6900 on
  `CurrentDeliveryType`, `Right Staff` 6801 on `IsEquippedType 8`, the NPC pair on `IsForm`).
- `MSCO_dual1..4` → `Dual Cast Base` (6730). `Dual Cast Base - Kynetcist` is also 6730 and also
  unconditional — a genuine tie OAR does not break. **Chosen: `Dual Cast Base` ("Base Dual
  Casting (Inquisitor)")**, because the right set is the Inquisitor `Base - default` and one
  author's set across the pack is the coherent read. Swapping to Kynetcist is a one-line change
  to the source folder if the owner prefers that art.

| shipped as | source | sha256 | bytes |
| --- | --- | --- | --- |
| `cast_right{,_self}/MSCO_left1.hkx` | `Base - default\MSCO_right1.hkx` | `089F2A4237A400B9316285A0E4A0ECB8E94D0365D2F33801DB5BBC578F621C22` | 17376 |
| `cast_right{,_self}/MSCO_left2.hkx` | `Base - default\MSCO_right2.hkx` | `D53CA7A282513866934A17EA42CDE9925E8DE11DFC3AC85BBEB42E4C8968ADBD` | 17264 |
| `cast_right{,_self}/MSCO_left3.hkx` | `Base - default\MSCO_right3.hkx` | `F42F7DB6AEA7512DABD6920A20F4EC55EBD9D8DF7CE176DF6C0505594EC1A592` | 17360 |
| `cast_right{,_self}/MSCO_left4.hkx` | `Base - default\MSCO_right4.hkx` | `B8360D9D58EE9097679E8A420F10A0F6ECF67509C5E616960942E9287D46B67B` | 17744 |
| `cast_dual{,_self}/MSCO_left1.hkx` | `Dual Cast Base\MSCO_dual1.hkx` | `A350102CC260E6E0FE47906D4E613F2A94779DC7A7E8DF1567993EF862C52FD4` | 13392 |
| `cast_dual{,_self}/MSCO_left2.hkx` | `Dual Cast Base\MSCO_dual2.hkx` | `6F9EFAC1E22451A1BE80675774424BD49B69E4CE6553C767268097267993DE9C` | 13536 |
| `cast_dual{,_self}/MSCO_left3.hkx` | `Dual Cast Base\MSCO_dual3.hkx` | `A23033978BB6183B2B507FC4FCC26D11EFA347D8DCF9DAE02691C943BEC8633C` | 13312 |
| `cast_dual{,_self}/MSCO_left4.hkx` | `Dual Cast Base\MSCO_dual4.hkx` | `861E9E3157F78E6A0B1D91855CED6978B5EE2981E6D60462F29C941AE28B9115` | 13168 |

Root of both source folders:
`C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\MSCO Magic Casting Behavior Overhaul\meshes\actors\character\animations\OpenAnimationReplacer\MSCO Animations\`.

---

## 3. Annotation audit — the SpellFire contract needed no stamping

The ticket allowed for stamping a missing `MRh_SpellFire_Event`. It was not needed: **every MSCO
right clip already carries `MRh_SpellFire_Event`, and every dual clip carries
`MLh_SpellFire_Event`.** Nothing was renamed and no commitment event was invented.

| clip | SpellFire event | at | for reference, its `MSCO_leftN` sibling |
| --- | --- | --- | --- |
| `MSCO_right1` | `MRh_SpellFire_Event` | 0.516667 | left1 `MLh` @ 0.483333 |
| `MSCO_right2` | `MRh_SpellFire_Event` | 0.516667 | left2 `MLh` @ 0.300000 |
| `MSCO_right3` | `MRh_SpellFire_Event` | 0.333333 | left3 `MLh` @ 0.350000 |
| `MSCO_right4` | `MRh_SpellFire_Event` | 0.600000 | left4 `MLh` @ 0.916667 |
| `MSCO_dual1` | `MLh_SpellFire_Event` | 0.700000 | — |
| `MSCO_dual2` | `MLh_SpellFire_Event` | 0.800000 | — |
| `MSCO_dual3` | `MLh_SpellFire_Event` | 0.633333 | — |
| `MSCO_dual4` | `MLh_SpellFire_Event` | 0.983333 | — |

Dual clips carry only the left event; a dual cast arms both hands, so that one event commits.
They also carry `MRh_WinStart`/`MLh_WinStart`/`MRh_WinEnd`/`MLh_WinEnd`, which the right and left
sets do not — preserved untouched.

Everything else is preserved exactly: the `PIE.@SGVI|MSCO_next*` payloads, `MSCO_WinOpen` /
`MSCO_WinClose`, `MCO_Recovery`, every `animmotion` key (except the deliberate right4 edit
below), duration, and the 97 transform tracks. Note the dual clips run **1.833333s** where the
left/right sets run 1.666667s.

One cosmetic consequence worth knowing: the right clips' payloads write `MSCO_nextright`, and the
dual clips' write `MSCO_nextdual`, where the base path's own clip would have written
`MSCO_nextleft`. That is MSCO's internal next-clip hint; SH2's walk is driven by its own
`CastComboIndex` (`msco_cast_driver.cpp:54-60`), so nothing in this integration reads it.

### The `MSCO_left4.hkx.bak_before_root` finding

`Base - default` holds both `MSCO_left4.hkx` (14352 bytes, modified Jul 20) and
`MSCO_left4.hkx.bak_before_root` (12032 bytes). The annotation diff is exactly two keys:

```
 0.283333 animmotion 0 0 0        0.283333 animmotion 0 0 0
 0.433333 animmotion 0 41 0   ->  0.433333 animmotion 0 0 0
 0.666667 animmotion 0 115 0  ->  0.666667 animmotion 0 0 0
```

The fork zeroed clip 4's forward root motion — `animmotion` keys are how AMR-style root motion
travels on these clips, and the `.bak_before_root` name says so. Nothing else in the dump
changed (same 11 annotations, same `MLh` at 0.916667, same duration, same track count).

**`MSCO_right4` carries the identical pair** (`0.433333 animmotion 0 41 0`,
`0.666667 animmotion 0 115 0`) — it is the same choreography mirrored — so the same treatment
was applied to both shipped right4 copies. Verified after the write: right4's three motion keys
now read `0 0 0`, matching the live `MSCO_left4` exactly.

Not treated, deliberately:

- `MSCO_right1..3` carry the same small keys their left siblings carry untouched
  (`0 -12 0` / `0 45 0`, `0 48 0`, `0 61 0`). The fork left those alone on the left set, so
  leaving them alone here is the faithful choice.
- The dual clips carry **no `animmotion` annotations at all**, so there is nothing to zero.

Limit of this finding: only the ANNOTATION layer was compared. The `.bak` is 12032 bytes against
14352, but hkxc-anno-cli round-trips through XML and its re-serialization is not byte-identical to
the original packer, so a size or byte diff proves nothing either way. If the fork's original edit
also touched transform tracks, this note does not see it.

### Byte-uniqueness stamping

`cast_right` and `cast_right_self` ship the same art, as do `cast_dual` and `cast_dual_self`.
Nolvus runs OAR's duplicate filter with hash-only comparison, which collapses byte-identical
clips and leaves one submod with an empty clip generator (ADR-0017, weapon-arts ticket 15). Each
submod's copy therefore carries one benign annotation of its own:

```
1.666667 SH2_PackStamp_cast_right          (and _cast_right_self)
1.833333 SH2_PackStamp_cast_dual           (and _cast_dual_self)
```

Convention taken from the repo's own precedent, `python_scripts/stamp_art_clips.py`: the
`SH2_PackStamp_` prefix plus the submod name, placed at the clip's **duration** — the last
instant it can occupy, so a clip cut short never reaches it, and nothing registers for the name in
either hosting graph. (The ticket suggested `t=0.0`; the existing script's placement is the safer
one and was followed instead.)

### Reproduction

```
hkxc-anno-cli dump   -i <clip>.hkx -o anno.txt
# right4 only: 0.433333/0.666667 animmotion Y -> 0; then for every clip:
#   bump "# annotations: N" and append "<duration> SH2_PackStamp_<submod>"
hkxc-anno-cli update -a anno.txt  -i <clip>.hkx
hkxc verify <clip>.hkx
```

---

## 4. The C++ rework

One coherent pass over the arming and decoding path, all four deferred Codex findings plus the
log line. Every site was re-read before editing; the ticket's line references had drifted and
`allowed_to_cast` lives in `input/input.cpp`, not `casting_controller.cpp`.

### How the armed mask reaches the predicates

The hook is the only place that sees both the event and the arming, so it reads **one** snapshot
per event and hands it down; nothing downstream re-reads the mask at a different instant.

```
ProcessEvent_PC
  event_hand = spellfire_hand_for_tag(tag)                 // one decoder, combo_cache.h
  arming     = CastingController::spellfire_arming()       // { mask, generation }
  isolate_caster_before_vanilla_spellfire(active, event_hand, arming.mask)
  ProcessEvent(..., event_hand, arming)
      MscoCastDriver::observe_graph_event(..., event_hand, arming.mask)
          is_msco_combo_window_open_event(event_hand, armed_mask)
      CastingController::notify_spellfire(event_hand, arming.generation)
```

- **Finding 2 (arm-aware swallowing).** `isolate_caster_before_vanilla_spellfire` gained an
  `armed_mask` parameter and returns `none` for an unarmed hand. Isolation is also what takes the
  skip-vanilla path, so this is the same switch: an unrelated vanilla cast's SpellFire released
  mid-Driver-Cast now reaches vanilla and completes normally. Still pure, still `constexpr`,
  still unit-tested.
- **Finding 3 (graph-side commitment).** `is_msco_combo_window_open_event` no longer takes a tag;
  it takes `(SpellFireHand, armed_mask)` and is exactly `spellfire_hand_is_armed`. Combo window,
  `clip_committed`, and the cast-index advance now fire only on an armed hand's event. The tag
  comparison moved into the shared decoder, so the driver no longer decodes strings itself.
- **Finding 4 (cast generation).** `spellfire_seen` and `spellfire_mask` were two atomics; they
  are now one `std::atomic<uint64_t>` — mask in bits 0-1, latch in bit 2, generation in the high
  32. `arm_spellfire` bumps the generation; `notify_spellfire` commits with a
  `compare_exchange_weak` against the generation the hook read **for that event**. An arming that
  lands between the hook's read and the commit therefore drops the stale event instead of
  committing a cast whose own clip has not reached its throw frame. Packing is what makes that
  atomic — two separate words would let a re-arm slip between the mask read and the latch write.
  Generation is compared for equality only, so wrapping is harmless.
- **Finding 8 (one decoder).** `spellfire_hand_for_tag` in `combo_cache.h` is now the single place
  a SpellFire tag becomes a hand. `notify_spellfire(bool left_hand)` became
  `notify_spellfire(SpellFireHand, uint32_t generation)`, and the hook's second decode is gone.
  `fire_left`/`fire_right` in `casting_controller.cpp` are gone too — the bit encoding lives once,
  in `spellfire_hand_bit`.
- **The log line.** `Input::allowed_to_cast`'s silent refusal now emits
  `SH2 cast: refused, casting={} sprinting={} swimming={} jumping={}` at debug. Bounded by
  presses, not by frames.

Semantics deliberately unchanged: ticket 43's deliver-once latch (a dual clip that raised both
events would still set one flag and deliver once), `clear_spellfire` (clears the latch, leaves the
mask), and `is_cast_committed`.

### Tests

`combo_cache_test.cpp`: `spellfire_of_either_hand_opens_the_combo_window` became
`spellfire_of_either_armed_hand_opens_the_combo_window` and gained the unarmed-hand cases;
`spellfire_tags_decode_to_their_own_hand` and `driver_cast_leaves_an_unarmed_hands_spellfire_to_vanilla`
are new; the two existing isolation suites were ported to the three-argument predicate.

---

## 5. Proof

### Build

Reproduced with the spike note's commands — `vcvars64.bat`, `VCPKG_ROOT=C:\Nolvus\_vcpkg`,
`cmake --preset release` with `-DOUTPUT_FOLDER=<worktree>/.build-out` and the pinned
`14.41.34120` `cl.exe`, then `cmake --build build/release --config Release`. Neither
`configure-release.bat` nor `build-release.bat` was run: both hard-wire the canonical checkout and
the first deploys into the live MO2 mod.

```
[71/72] Building CXX object CMakeFiles\SpellHotbar2.dir\src\rendering\advanced_bind_menu.cpp.obj
[72/72] Linking CXX shared library SpellHotbar2.dll
```

Warnings in the touched files: none new. `animationeventhook.cpp(36): warning C4100:
'a_eventSource'` is pre-existing — that parameter's only use has been commented out since before
ticket 44 (`git show HEAD~1:…/animationeventhook.cpp`); it simply had not surfaced in a log tail
before. `casting_controller.cpp(1395)`/`(1656)` C4100 are likewise untouched pre-existing lines.

### Unit tests — all seven suites

```
--- art_bind_record_test.exe   exit=0   ok
--- art_data_test.exe          exit=0   ok
--- art_pack_gen_test.exe      exit=0   ok
--- bind_drop_test.exe         exit=0   ok
--- cast_anim_ids_test.exe     exit=0   ok
--- clip_translation_test.exe  exit=0   ok
--- combo_cache_test.exe       exit=0   ok
TOTAL FAILING SUITES: 0
```

### HKX

`hkxc verify` on all sixteen shipped clips: `Complete hkx reproduction` for every one, exit 0.

### JSON

All five `config.json` files parse (`ConvertFrom-Json`, no errors); the four submod priorities
read back as 2000001101/02/03/04.

### SHA-256 uniqueness — 24 files, 24 distinct hashes

| # | submod | file | sha256 | bytes |
| --- | --- | --- | --- | --- |
| 1 | cast_dual | MSCO_left1.hkx | `211189E1BE8C6EAFA7668FF54F0D7069D4820D346902314FB498B703F518DDA1` | 15760 |
| 2 | cast_dual | MSCO_left2.hkx | `F838A902870EF57C8CB62A198051970C964A456F021DE322E94DFB1D1D10052A` | 15904 |
| 3 | cast_dual | MSCO_left3.hkx | `E2FCD043B1B286C433CA6738B1C777CED5EE910458312F4BE00A87F1E775E28A` | 15680 |
| 4 | cast_dual | MSCO_left4.hkx | `0DC0DA281C92F2825C2EDC7CA03003369C31CF777593B519F3DD96BD01923444` | 15536 |
| 5 | cast_dual_self | MSCO_left1.hkx | `8DFEDFBD3BA92F3413D89D1AC1FAB5188ABB1056151886063B14323ACF538164` | 15760 |
| 6 | cast_dual_self | MSCO_left2.hkx | `E4315F9B32C04512000DC6A7567AC6BA5C80AF0FD6C8B88330AB751EAD031A8E` | 15904 |
| 7 | cast_dual_self | MSCO_left3.hkx | `326D008CF8A79E674111696355770A3840354218E39E9C44BD3C977A062AC759` | 15680 |
| 8 | cast_dual_self | MSCO_left4.hkx | `82C16E044BA0B98DD0699C5F4A3B8166A422BF30128659399D291FEB19C8E775` | 15536 |
| 9 | cast_right | MSCO_left1.hkx | `40DF448D8638FA95DBC46853486458B9E9054F751B7749490632C007C2064FE9` | 19744 |
| 10 | cast_right | MSCO_left2.hkx | `EB6ABA96EB0930D9D374440492D9AF346935810E67445EB42145E160E2DA7C04` | 19632 |
| 11 | cast_right | MSCO_left3.hkx | `17DCCB7911F1548C965C5D38DDE4AC0F42B2A6DEA36F04EE5F6896606DCD86D7` | 19728 |
| 12 | cast_right | MSCO_left4.hkx | `8B9ABE8152B3FD19934FF2C35A1A3DFB6CF9F2145AAAD7B0B1757CE10F58B3FA` | 20112 |
| 13 | cast_right_self | MSCO_left1.hkx | `45AC9F06CFEEAE297E83D2F55FDE808C3048D1338C75C5C634AE7A2EEB5ACE6C` | 19744 |
| 14 | cast_right_self | MSCO_left2.hkx | `53CECAAD72D6CDBBFF8C4C1C79A7DF780B96CAF7596AAF06850D5CD56AACA73C` | 19632 |
| 15 | cast_right_self | MSCO_left3.hkx | `DF6259982459A71BE4D34C5B5295B76EF5530114F19BE9E3BA9D5867378F35CC` | 19728 |
| 16 | cast_right_self | MSCO_left4.hkx | `C3C6AFBD1D904D027B6DAF241EF865DE0774254C0E0B519D90580EF14AE0E522` | 20112 |

The eight MSCO source hashes are in §2; all sixteen shipped files differ from all eight sources
and from each other. The size growth (17376 → 19744 etc.) is serde-hkx's re-serialization layout,
the same effect the spike recorded at 13152 → 15456, and `hkxc verify` calls each one a complete
reproduction.

---

## 6. What is runtime-unverified — the coordinator's live run

Nothing in this note is runtime evidence. Everything above is source reads, a clean build, seven
passing suites, `hkxc verify`, JSON parses, and hashes on disk.

Open for the live run:

1. **Selection.** That each submod actually wins its cell — the OAR Animation Log naming
   `SH2 Cast - Right (aimed)` / `(self)` / `SH2 Cast - Dual (aimed)` / `(self)` during the
   matching cast, in **both** hosting graphs (weapon sheathed → `magicbehavior`, 1H drawn →
   `1hm_behavior`). Check **Detected Problems** shows nothing for `Spell Hotbar 2 Casts` first;
   an INVALID condition makes every later observation meaningless.
2. **Visual identity.** A right clip must visibly cast with the right hand and a dual clip with
   both. A log line proves selection, never appearance — this needs a frame per cell.
3. **Commitment without fallback.** `SH2 cast: graph raised a right SpellFire event` on a right
   cast in both graphs, and no `SH2 cast: no SpellFire event; delivering the payload anyway`. In
   `1hm_behavior` this is also the first live proof that ticket 44's `MRh` registration resolves.
4. **The four-step walk per hand set.** Chained casts 1→2→3→4 each committing at their own event,
   now that clips 2–4 carry their own hand's annotation rather than the left one.
5. **Arm-aware swallowing (the new risk).** With an ordinary spell equipped in the OFF hand,
   release it mid-Driver-Cast: it must now complete normally instead of being eaten. And the
   old guarantee must hold — the ARMED hand's equipped spell must still not double-fire.
6. **The generation counter.** Its window is narrow by construction and no live repro is known;
   the unit tests cover the predicates, not the race. Watch for the opposite failure — a
   legitimate SpellFire silently dropped, which would show as an unexpected clip-end fallback
   line on a chained cast.
7. **MSCO interplay (hazard 2).** One staff-equipped cast with the Animation Log open settles
   whether `Base - Left/Right Staff` (6800/6801, pure `IsEquippedType`) has been staff-swapping
   the fork's clips behind its back. This pack outprioritizes them for the right and dual cells,
   which is itself a change: a staff-in-hand right cast now plays the plain right set rather than
   MSCO's staff art. Staff cells are ticket 47.
8. **The fast-ritual overlap.** `kRitual.variant` is also `10016` and `kRitualSelf.variant` is
   also `10017` (`cast_anim_ids.h:32-33`), so a ritual fast enough to take the variant slot will
   select `cast_dual` / `cast_dual_self`. That is upstream's own id convention rather than a new
   defect, and it is the same object as ticket 48's dual >1.51s leak; noting it because the pack
   makes it visible for the first time.
9. **Clip end.** The shtb states' `SH2_CastExit` trigger arrays live on the STATE, not the clip,
   so replacements inherit them — asserted from the graph, confirmed only by a live run.
10. **`allowed_to_cast`'s new debug line** has never printed; the stuck-`IsCasting` refusal it
    exists to characterize is spike observation 1 and is still uncharacterized.

Deployment is unchanged from the spike note §7: `data/…` mirrors into
`Dev - Spell Hotbar 2\meshes\…`, and the DLL must be rebuilt and copied by
`configure-release.bat` + `build-release.bat` from the canonical checkout after merging. **No
Nemesis run is needed for this ticket** — no file under `nemesis/` changed, so the engine cache is
not invalidated.
