# 10 — Runtime support beyond SE 1.5.97

**Type:** task
**Status:** deferred — post-release

The first release claims **Skyrim SE 1.5.97 only**, because that is the only runtime that has ever
been tested. Owner ruling, 2026-08-31: *"The only Skyrim version I can say is the one that I
tested, 1597... I truly cannot say anything about AE 1.6X or VR, and at this point I am not
concerned, especially not concerned with VR."*

The build itself is not restricted to 1.5.97 — `add_commonlibsse_plugin` in
`skse_plugin/CMakeLists.txt` carries no runtime argument, so it is a CommonLibSSE-NG default
multi-runtime build and probably loads on AE 1.6.x. **Probably is not a claim worth publishing.**
The page says what was tested, and nothing else.

Open this when users start asking, which they will.

## What it would take

- **AE 1.6.x.** Install a 1.6.x runtime, load the DLL, and drive the acceptance matrix that
  `docs/agents/headless-testing-playbook.md` already defines. The likely friction is the behavior
  side rather than the DLL: Nemesis patches and the MSCO/Ashes of War chain each have their own
  runtime story.
- **VR.** A separate question entirely, and explicitly not a concern. VR needs its own Address
  Library, its own CommonLibSSE-NG target, and a UI that never assumed a flat screen. Treat as its
  own effort if it ever happens, not as a checkbox on this one.

## Not a blocker for the first release

`deploy/release/release.json` carries `supported_runtimes` as the tested string, and the page
prints that. Claiming less than the binary may support is the safe direction to be wrong in.
