# 01 — Spike: does a queued ButtonEvent fire OCPA and dodge?

**Type:** spike (live)

**What to build:** One injected down/up on the shout queue, aimed at OCPA's configured
scancode, then the same for dodge. The owner compares each to the physical key. The answer
writes which Action kinds v1 may ship. No bind-menu tab.

**Blocked by:** Nothing.

**Status:** claimed

Line: `ng/smf-next`. Overlay: `Dev - Spell Hotbar 2 SMF Next`. Spec: `../spec.md`.
Do not write `Dev - Spell Hotbar 2` or `Dev - Spell Hotbar 2 SMF`.

## You test this

Weapon drawn, standing still, OCPA bound as it is today (`B` / DX 48 last measured).

1. Press physical `B`. A power attack plays. That is the control.
2. Trigger the spike inject (agent will say which native / console / `castSlot` path).
   A power attack must play, same as (1). If idle, a twitch, or nothing, it fails.
3. Repeat (1)–(2) for dodge with that mod's bound key.

A log that a `ButtonEvent` was queued is not a pass. The move has to play.

## Agent builds the inject

Throwaway is allowed. Prefer a hidden native or a one-shot `castSlot` rider over a new
product surface.

- Read the power-attack scancode from `get_ocpa_keys()` in
  `skse_plugin/src/input/input.cpp`. Do not hardcode 48.
- Queue `RE::ButtonEvent::Create` onto `RE::BSInputEventQueue`, keyboard device, that
  scancode. Down then up in the VoiceCastDriver shape
  (`skse_plugin/src/casts/voice_cast_driver.cpp`). Down-only is illegal (phantom hold).
- Do not use DevBench `input`, OS `SendInput`, or Papyrus `Input.TapKey`. Ticket 50:
  sink injection never reaches this hook; this spike is the *queue* seam, which is
  different, and still needs the owner's eyes.
- Log device, scancode, down/up, and whether the queue accepted the event.

## What this ticket decides

Write the answer under `## Answer` and set Status to `resolved`.

- Both OCPA and dodge play: physical-scancode kind is v1. Tickets 03–04 ship that kind.
  Engine-control Power Attack stays unused.
- OCPA fails: physical-scancode is not a combat product. Ticket 04's Power Attack arm is
  engine-control (`userEvent` for the attack chord). Dodge waits or drops from v1.
- Dodge fails independently: record it; do not block Power Attack on that cell.

## Notes

OCPA is documented in mco-integration ticket 10 as raw input on its own hook. This spike
exists because that may mean the queue never reaches it. Do not "fix" a miss by adding
`SendEvent` or SendInput on the same press.

## Comments

Agent 2026-09-04: throwaway inject is `SpellHotbar.castSlot(-1)` for OCPA's configured
power scancode (`get_ocpa_keys().power`) and `castSlot(-2)` for TK Dodge RE
`DodgeHotkey`. Both queue a keyboard `ButtonEvent` down then up on `BSInputEventQueue`.
VFS at build time: OCPA power **79** (Numpad 1, not B/48), dodge **81** (Numpad 3).
Overlay: `Dev - Spell Hotbar 2 SMF Next`. Owner-hands still open.
