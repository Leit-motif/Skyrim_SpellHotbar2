# 01 — Play a bound Weapon Art from drawn-weapon idle

**What to build:** Binding a Weapon Art to a hotbar slot and pressing that slot, from a drawn
weapon idle, plays the art without equipping or unequipping anything. The Art Selector is written
before entry, MCO combo counters are normalised to first-attack, and the state exits at clip end.
A missing art or a refused press (wrong stance, unaffordable, on cooldown) highlights the slot
red and logs why.

**Blocked by:** None — can start immediately. Mid-swing deferral is later (needs mco-integration
ticket 04). The `Ashes of War` OAR overrides are later. Bind-menu UI is later; this ticket's bind
path is the Papyrus `slotArt` function so the existing `castSlot` seam can drive it.

**Status:** claimed

- [ ] `slotArt(slot, artId)` then `castSlot(slot)` from drawn 1h idle: notify `SH2_ArtStart`
      returns true; Art Selector equals the bound art's selector while the state is live; a
      captured frame shows the art clip (or the inert `AABL_Attack_A` placeholder if no Art Pack
      won).
- [ ] Combo counters `MCO_nextattack` and `MCO_nextpowerattack` are 1 after entry.
- [ ] Art Selector is 0 after the state exits.
- [ ] A press that cannot enter (sheathed, unknown art id, on cooldown, unaffordable) highlights
      the slot red and logs the reason; nothing is equipped or unequipped.
- [ ] A bar with a bound art survives save/load (serialization version 6).
- [ ] Existing spell/shout/potion slots are unchanged.

## Comments

Paused 2026-08-12 before in-game test (owner was mid SH2 casting work). Code is on
`weapon-arts`. DLL built to `.scratch/weapon-arts-build` only — Dev overlay, Nemesis
Output, and the live casting DLL were not touched.

### Next steps (resume on `weapon-arts`)

1. Compile `papyrus/Scripts/Source/SpellHotbar.psc` so `slotArt` / `getArtSelector` exist
   in a `.pex`. Land it at `Dev - Spell Hotbar 2\Scripts\SpellHotbar.pex`. The CK compile
   needs vanilla + SKSE script sources on the import path (last attempt failed on missing
   `Spell` / `Form` types).
2. Copy into `Dev - Spell Hotbar 2`: the scratch DLL, `data/SKSE/Plugins/SpellHotbar/artdata/`,
   and `nemesis/Nemesis_Engine/mod/shtb/` (plus BDI JSON).
3. Nemesis: `tools/run-nemesis.ps1 -Tick shtb -UpdateEngine -Apply` from the thuum repo.
   Then hkxc-dissect `1hm_behavior` and confirm `SH2_Art_State` / no leftover `#shtb$` tokens.
4. Live drive on Save65 (or equivalent), 1h drawn: `SpellHotbar.slotArt(0, 1)` then
   `SpellHotbar.castSlot(0)`. Prove notify `SH2_ArtStart -> true`, selector `1` while live
   then `0` after exit, `MCO_nextattack`/`MCO_nextpowerattack` = 1, and a captured frame.
   Sheathed / unknown id / CD / unaffordable → red + log.
5. Code-review against this ticket + `.scratch/weapon-arts/spec.md`, then mark cells done.
