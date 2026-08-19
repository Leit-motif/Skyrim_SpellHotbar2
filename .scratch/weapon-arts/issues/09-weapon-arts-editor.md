# 09 — Weapon Arts editor (enhancement)

An in-game UI that treats an art as a **motion plus a timed effect**: pick a clip from the
catalogue, assign a spell/MGEF onto a PIE placeholder in that clip, optionally rename and pick
an icon. Parked. Stories implied by 22 / 31; not required to ship 06–08.

**Blocked by:** 08

**Status:** needs-triage

## You test this

(Unwritten until this is un-parked.) Binding Menu or a sibling of the Spell Editor: pick
Disengage as a base, assign a spell to the placeholder, bind the result, the effect releases at
the annotation time.

## Agent tests the rest

(Unwritten.) Persistence must be art data (catalogue / PI INI / `PIE.$customName`), not rewriting
`.hkx` from ImGui. Pointed AoW clips stay read-only.

## What this is

Authoring UI over Custom Art Folders and the placeholder convention ticket 08 may plant.

## What this is not

Not v1. Not slot = folder. Not MagFail gray-out (07). Folder `name` / `icon` files are the v1
stand-in.

## Notes

Spell Editor already edits CSV metadata for spells, not Havok files. Payload Interpreter already
supports `PIE.$customName` without re-annotating. `try_start_art` / Art Class stay as they are.

## Comments

Grill 2026-08-18: Q7 = later enhancement. Rename/icon in-game is the first slice when this
opens; PIE assignment is the rest.
