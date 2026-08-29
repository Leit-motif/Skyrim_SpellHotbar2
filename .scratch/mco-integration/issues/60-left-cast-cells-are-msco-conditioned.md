# 60 — The left cast column is decided by MSCO, not by the fork

**Status:** built, awaiting owner acceptance

Ticket 46 shipped the right and dual cells and ruled that "left keeps the bound clips (no
submod needed)". That premise is false. The left path is not bound to one clip set — MSCO's
own submods compete on it, and two of them are conditional on what the player has EQUIPPED:

| MSCO submod | priority | condition |
| --- | --- | --- |
| `Base - default` | 6700 | none |
| `Base - Left Staff` | 6800 | `IsEquippedType 8`, left hand |
| `Base - Self Left` | 6901 | `CurrentDeliveryType` source 0 (left), delivery 0 (Self) |

So with a self-delivery spell equipped in the left hand, every fire-and-forget cast the fork
resolves to the left hand — **aimed casts included** — plays MSCO's self-cast art. The right
and dual columns are immune only because ticket 46 owns them at 2000001101-04.

## Evidence

Owner reported it in play (2026-08-29) with a self-cast concentration spell in the left hand.
The owner's own `OpenAnimationReplacer.log` names both winners on the same clips minutes apart:

```
13:09:42  MSCO_left4.hkx (Clip: SH2_Cast4_Clip)  - MSCO Animations / MSCO - Default Animations (Inquisitor)
13:09:53  MSCO_left1.hkx (Clip: SH2_CastRight_Clip) - MSCO Animations / Self Left
13:09:55  MSCO_left2.hkx (Clip: SH2_Cast2_Clip)  - MSCO Animations / Self Left
```

The `SH2_Cast*_Clip` names are the fork's own Driver Cast states, so this is not MSCO's own
casting — it is an SH2 hotbar cast wearing MSCO's self art.

`set_weapon_dependent_casting_source` resolves Auto to `kLeftHand` for fists, one-hand + spell,
dual-wield, two-hand, crossbow and spell/spell (`game_data.cpp:1817-1838`), so the left column
is the common case, not an edge.

## Fix (built)

Own the left column the way ADR-0018 owns the others — two submods in `SpellHotbar2Casts`:

| submod | priority | conditions | clips |
| --- | --- | --- | --- |
| `cast_left` | 2000001105 | `0x815 == 1` AND `0x835 == 0` | `Base - default` `MSCO_left1..4` |
| `cast_left_self` | 2000001106 | `0x815 == 2` AND `0x835 == 0` | `Base - Self Left` `MSCO_left1..4` |

Both carry `MLh_SpellFire_Event` at MSCO's own frames (0.483333 / 0.300000 / 0.350000 /
0.916667 aimed; 0.483333 / 0.634583 / 0.483333 / 0.483333 self), so the commitment contract is
unchanged. `cast_left`'s clip 4 inherits the fork's zeroed root motion — the three `animmotion`
keys read `0 0 0`. Stamps per ADR-0017: one `SH2_PackStamp_cast_left` for the aimed cell (its
four clips are already distinct), per-file `SH2C_cast_left_self_N` for the self cell, because
MSCO's self-left set repeats one clip across slots 1, 3 and 4. All eight shipped files hash
distinct, and `hkxc verify` reproduces each.

## Staff cells (owner ruling 2026-08-29)

The first cut of this fix let `cast_left` override MSCO's `Base - Left Staff`, deferring staff
art to ticket 47. Owner overruled: a staff cast must play MSCO's staff animations. Two more
cells, so no cell in the matrix is decided by fall-through:

| submod | priority | conditions | clips |
| --- | --- | --- | --- |
| `cast_left_staff` | 2000001111 | `0x815 == 1` AND `0x835 == 0` AND `IsEquippedType 8` left | `Base - Left Staff` `MSCO_left1..4` |
| `cast_right_staff` | 2000001112 | `0x815 == 1` AND `0x835 == 1` AND `IsEquippedType 8` right | `Base - Right Staff` `MSCO_right1..4` |

The hand boolean is the PHYSICAL hand, MSCO's own vocabulary at its 6800/6801 submods. Left
staff clips carry `MLh_SpellFire_Event`, right staff clips `MRh_SpellFire_Event`, at
0.400000 / 0.333333 / 0.400000 / 0.366667 in both sets; neither set has root motion.

`cast_right_staff` fixes a gap ticket 46 shipped rather than one this ticket introduced:
MSCO's `Right Staff` sits on the `MSCO_right*` paths, which the Driver Cast states never ask
for, so a right-staff hotbar cast has played the plain right set since ticket 46 landed.

**Self plus staff stays self art.** MSCO's own precedence puts `Self Right`/`Self Left`
(6900/6901) above `Right Staff`/`Left Staff` (6800/6801), and MSCO ships no self-staff set, so
the staff cells gate on the aimed family only. Overrule this by adding `0x815 == 2` twins.

## Consequences
- `cast_left_self`'s clips carry MSCO's `MLh_Equipped_Event` at 0.5, which the right-self set
  does not. Watch for a left-hand magic pop during the acceptance pass.
- Ticket 46's "left keeps the bound clips" line is superseded; the pack's own `config.json`
  now records why the column is owned.

## Acceptance

- With a self-delivery concentration spell equipped in the LEFT hand, an aimed hotbar cast
  plays the aimed art. Animation Log names `SH2 Cast - Left (aimed)`.
- A self hotbar cast on the left hand names `SH2 Cast - Left (self)`.
- Right and dual rows stay green; the four ticket-46 submods still win their cells.
- A staff in the casting hand plays MSCO's staff art, both hands, aimed family. Animation Log
  names `SH2 Cast - Left Staff (aimed)` / `SH2 Cast - Right Staff (aimed)`.
