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

- [x] `slotArt(slot, artId)` then `castSlot(slot)` from drawn 1h idle: notify `SH2_ArtStart`
      returns true; Art Selector equals the bound art's selector while the state is live; a
      captured frame shows the art clip (or the inert `AABL_Attack_A` placeholder if no Art Pack
      won). — Save65 2026-08-16, log `SH2_ArtStart -> true`, selector 1 while live. Frames
      `.scratch/weapon-arts/shots/art-idle-1.png`–`6.png` (owner: confirm the clip, not idle).
- [x] Combo counters `MCO_nextattack` and `MCO_nextpowerattack` are 1 after entry.
- [x] Art Selector is 0 after the state exits. — `SH2_ArtExit` at clip end, then selector 0.
- [ ] A press that cannot enter (sheathed, unknown art id, on cooldown, unaffordable) highlights
      the slot red and logs the reason; nothing is equipped or unequipped. — unknown id logged
      at bind (`Unknown weapon art 999`). CD / sheathed / unaffordable reason-logs not captured:
      a second press while `current_cast` is live never reaches `try_start_art`.
- [ ] A bar with a bound art survives save/load (serialization version 6).
- [ ] Existing spell/shout/potion slots are unchanged.

## Comments

Paused 2026-08-12 before in-game test (owner was mid SH2 casting work). Skeleton is
`01d68f1`. **2026-08-16:** `main` (Driver Cast through ticket 22) was merged into
`weapon-arts`. Art graph objects moved to `#shtb$23`–`$26` so they do not collide with
ticket 21's wrap on `$10`–`$22`. Checkout `weapon-arts` in a new session; do not start
from `01d68f1`.

### 2026-08-16 — live proof (Save65, Nolvus Awakening)

Deployed from `weapon-arts`: DLL SHA-256 `9C0AE942…` (then rebuilt after the occupancy
fix), `SpellHotbar.pex` with `slotArt`/`getArtSelector`, `artdata/arts.csv`, `shtb`
Nemesis patch, BDI JSON. Nemesis Update Engine 66s + Build 131s, 1045 animations.
Merged `1hm_behavior.hkx` contains `SH2_Art_State` / `SH2_ArtStart` / `SH2_ArtExit` /
`AABL_Attack_A` and zero leftover `#shtb$` tokens.

**Occupancy fix:** `get_skill_in_bar_with_inheritance` used `formID != 0`, so a bound
art looked empty and `castSlot` saw `skill type=0`. Gate is now `!skill.isEmpty()`.

Happy path (drawn 1h, Iron Rapier + Incinerate, `slotArt(0,1)` then `castSlot(0)`):
`SH2_ArtStart -> true`, selector 1 while live, both MCO next-attack vars 1, stamina
115→90, hands unchanged, `SH2_ArtExit` then selector 0. Frames in
`.scratch/weapon-arts/shots/art-idle-*.png`.

Still open: CD / sheathed / unaffordable reason logs; save/load v6; owner eyes on the
clip. Game left running on Save65 for that. Do not merge to `main` until those close.
