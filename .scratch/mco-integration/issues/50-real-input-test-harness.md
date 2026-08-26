# 50 — Test harness that sends REAL inputs through the DLL's input hook

**Type:** infrastructure. Owner-directed 2026-08-26 after ticket 49's regression shipped on a
green headless matrix: "We will have to work on a testing harness that allows you to send real
inputs, whether that means you will need to take foreground mode or what."

**Status:** ready-for-agent (design first). Blocked-by: nothing, but ticket 49's diagnosis
should inform the design (it defines the class of bug the harness must be able to catch).

## The gap this closes

DevBench's `input` tool injects `ButtonEvent`s into `BSInputDeviceManager` sinks — DOWNSTREAM
of SpellHotbar2's `PollInputDevices` hook (verified 2026-08-12; the hook comment at
`input.cpp` ~:498 states injected input never reaches it). Consequence: any change to the
mod's input hook is untestable headlessly, and a green matrix on injected input proves nothing
about the physical-input path. Ticket 49 is the cost, paid live by the owner.

## Preferred design: inject upstream, stay headless

Extend the `devbench-input` SKSE plugin (`C:\Nolvus\Projects\devbench-input`) to inject at the
device-poll stage — the same seam the hardware feeds — so an injected event traverses
`PollInputDevices` and every DLL hook exactly as a physical press does. Candidate seams, to be
established against the running engine rather than assumed:

- Hook/feed the per-device input buffers `BSPCKeyboardDevice` / `BSPCMouseDevice` read during
  the poll, so events surface inside `PollInputDevices` itself.
- Or a trampoline just before the SH2 hook's site that splices synthetic events into the same
  event list the poll returns.

Requirements either way:

- A capture-visibility oracle: the harness must be able to OBSERVE that a DLL hook captured or
  passed an event (e.g. a devbench-side tap after the hook chain), so "the press was eaten"
  becomes a readable assertion, not an inference from silence.
- Existing downstream injection stays available (it is the right tool for engine-side control
  driving and is proven); the new path is additive, selected per test.
- Works unfocused, same as the rest of the DevBench setup (`bAlwaysActive=1` profile).

## Fallback: foreground real input

If the upstream seam proves impractical: `SendInput` with the game window foregrounded, run as
a scheduled exclusive window (the machine is the harness's briefly, owner informed, never
during owner play). This is strictly worse (focus stealing, scheduling friction) — take it
only if the SKSE seam fails.

## Standing rule (applies now, before any harness exists)

No change touching `src/input/input.cpp` or any input-hook site ships on injected-input
evidence. Until this harness lands, such changes carry an explicit owner-hands acceptance
cell, scheduled with the owner.

## Acceptance

- [ ] An injected press demonstrably traverses the SH2 `PollInputDevices` hook (log-line or
      oracle proof) while the game is unfocused.
- [ ] A regression test reproduces ticket 49's class of bug: a press that a DLL capture eats
      is detected by the harness as eaten.
- [ ] The ticket-46 acceptance matrix re-run on the new harness catches — or exonerates —
      the ticket-49 mechanism.
- [ ] Owner sign-off that the harness's evidence is acceptable for input-path changes.
