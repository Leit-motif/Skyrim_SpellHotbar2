# 03 — Prove the capture path carries the hotbar

**Type:** prototype
**Status:** closed 2026-09-05 — owner: "we already know that desktop capture pulls in the hotbar--close this." The desktop-duplication rig (`record-demo.ps1`, ffmpeg via Desktop Duplication) composites the presented frame including SH2's ImGui bar; only the engine's native `capture` path omits it. Tickets 04 and 07 are unblocked on this point.

**Status (superseded — see the top):** ready-for-agent

Three seconds of footage decides whether tickets 04 and 07 are possible as planned. Do this before
writing a shot list.

## The problem

The hotbar is the mod, and the engine's own screenshot path does not draw it. Measured 2026-08-25:
`capture kind=native` returned a 3440x1440 frame containing the world and no hotbar at all,
despite reporting `uiExcluded: false`, because SH2 draws its bar through its own ImGui present
hook. `capture kind=providers` returns `[]` on this instance. Memory
`sh2-imgui-hotbar-is-uncapturable` holds the measurement.

Desktop-level capture composites the frame the game actually presented, so it *should* carry the
overlay. That is a reasonable expectation, not a measurement. Prove it.

## The rig already exists

The sibling repo built it and it transfers:

- `C:\Nolvus\Projects\thuum-fully-animated-shouts-mco\.scratch\shout-mco-engine\record-demo.ps1`
  — ffmpeg through the Desktop Duplication API, started hidden so it never takes the foreground,
  given a hard `-t` so it finalises itself rather than leaving a headless mp4. Smoke tested at
  3440x1440, 217 frames in 4 s.
- `...\drive-input.ps1` — `SendInput` with hardware scancodes, spin-waited timing, 150 ms presses
  (45–60 ms drops one press in six), and an `Assert-SafeToSend` interlock.

**Do not invoke ffmpeg through `WinGet\Links\ffmpeg.exe`.** That is a zero-byte execution alias
that fails outside an interactive desktop session; call the real executable under
`WinGet\Packages\`.

## Constraints that are not negotiable

- **Unattended, not headless.** Skyrim must own the foreground on an unlocked, physically present
  desktop — `drive-input.ps1` refuses to send input unless SkyrimSE is frontmost. `bAlwaysActive=1`
  keeps the game ticking unfocused, but OS input still only reaches the foreground window. No RDP,
  no locked screen, no using the machine mid-take.
- **Never minimize the Skyrim window and never park it fully offscreen.** Minimizing crashed the
  NVIDIA D3D11 driver outright; fully offscreen loses the DWM redirection surface and capture
  returns blank frames. Covered is fine — visibility is not focus.
- **One game instance.** Check `ListAgents` before launching; a peer session's `qqq` has killed a
  live test here before.

## Acceptance

- [ ] A three-second desktop-duplication take, recorded with the hotbar visible, is decoded to a
      still, and the still **contains the bar with legible slot icons**. Committed under
      `.scratch/release/evidence/` and cited by path.
- [ ] The same take is checked for the cooldown sweep and the slot-strip tint tiers. Those are the
      subtlest thing the video has to sell; if desktop capture flattens them, the shot list needs
      another way to show them.
- [ ] If the bar does **not** appear, stop and report. Do not plan a shot list around a capture
      path that cannot see the product, and do not fall back to the native path, which is already
      known to omit it.
