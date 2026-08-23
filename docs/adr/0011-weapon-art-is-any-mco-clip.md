# A Weapon Art is any MCO attack clip; SH2 and PIE own the machinery

Date: 2026-08-17

Status: accepted

Supersedes ADR-0007 (hardcoded `AABL_Attack_A.hkx` as the compatibility contract).
Amends ADR-0008: the Art Selector remains SH2 data, not Ashes of War worn-item identity.

## Context

Ticket 01 proved a hotbar slot can play a special attack from drawn idle. The first clip on that
state was `Animations\AABL_Attack_A.hkx` because Ashes of War already OAR-replaces that path.
That was a convenient placeholder, not the product.

Ashes of War is the *concept* to steal: a named special attack you choose, not another LMB combo
step. It is not machinery to import. We do not need its slot-55 clothing keywords, its AABL
hotkey, Additional Attack by Loop's Papyrus, or a requirement that the file be named
`AABL_Attack_A.hkx`.

A Weapon Art is a special MCO attack animation that plays when the assigned hotbar button is
pressed. Hit frames, combo windows, recovery, and clip motion already live on that HKX as
annotations and Payload Interpreter payloads. SH2 already owns the slot, the notify, the art
state, Cast Plant, stamina, and cooldown.

## Decision

- **Clip identity is data.** An art catalogue row names the HKX to play (any MCO-annotated attack
  clip the load order can see). The graph state is SH2's; the file is not required to be AABL.
- **Machinery is SH2 and/or PIE.** SH2: bind, press, `SH2_ArtStart` / `SH2_Art_State` / plant,
  cost, cooldown, Art Selector as a SH2 global. PIE (and AMR / Precision already on the clip):
  what the animation *does* while it plays.
- **Ashes of War folders are a content source**, not a runtime dependency. A generator may turn
  those folders into catalogue rows that point at the existing HKX files in the VFS. It must not
  copy clips, and must not drive arts through worn keywords or the AABL hotkey.

How a named file actually reaches the one `hkbClipGenerator` (OAR replacement of a SH2-owned
placeholder vs additional registered clips) is an implementation ticket. That choice must not
re-introduce AABL-only or item-keyword machinery.

## Consequences

- Ticket 01's `AABL_Attack_A` generator is an inert bootstrap clip, not a contract. Renaming or
  retargeting it is allowed.
- ADR-0008's selector is still the way SH2 names *which* art is live for OAR/PIE conditions. It
  is not how Ashes of War used to pick an ash (worn keyword).
- Extending the set is a catalogue row plus a clip already in the load order, not a Nemesis
  rebuild per art and not a Spell Editor / FormID registration.
