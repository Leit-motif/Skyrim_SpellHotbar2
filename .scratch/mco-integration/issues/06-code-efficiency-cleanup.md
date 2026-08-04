# 06 — Clean up inefficient code

**Type:** task (Core Fork)

**What to build:** Targeted efficiency fixes in this fork's own code, argued from measurement
rather than from reading.

**Blocked by:** None, but explicitly **secondary**. It is not a prerequisite for any
integration ticket and must not be bundled into one.

**Status:** ready-for-agent

The owner named this as a secondary goal alongside MCO integration. Keeping it in its own
ticket keeps integration diffs reviewable — a chain-out fix and a refactor in the same commit
are indistinguishable to a reviewer, and one of them is load-bearing.

- [ ] Identify the actual hot paths rather than the ugly ones. The plugin hooks the main game
      loop and renders ImGui every frame; a per-frame cost is worth more attention than a
      per-cast one, however untidy the latter reads.
- [ ] Measure before changing. Cost is latency, not line count, and the heavy patterns hide
      in code that looks simple.
- [ ] Cover the Papyrus side (`papyrus/Scripts/Source/`) as well as the C++ — different rules
      apply, and Papyrus cost shows up as script lag and stack dumps rather than frame time.
- [ ] Keep each change independently revertible. An efficiency change that cannot be backed
      out separately from an integration change is a liability.
- [ ] Do not refactor for taste. Anything that does not show a measured improvement, or fix a
      genuine defect found on the way, belongs in a note rather than a diff.

## Found 2026-08-03 — `saveBarsToFile` CTDs on a filename with no directory

This is a crash fix, not a tidy-up, and it is the one item here worth doing regardless of
whether the rest of this ticket ever runs.

`SpellHotbar::Bars::save_bars_to_json` (`skse_plugin/src/bar/hotbars.cpp:291`) opens with:

```cpp
std::filesystem::path file_path(path);
std::filesystem::create_directories(file_path.parent_path());
```

Given a bare filename, `parent_path()` is empty and `create_directories("")` **throws**. It is
the throwing overload — no `error_code`, no try/catch — so `std::filesystem_error` unwinds out
of the DLL and the game dies.

Reproduced by accident: `SpellHotbar.saveBarsToFile("devbench_bars_dump.json")` over Papyrus
took Skyrim down instantly. Crash log
`Documents\My Games\Skyrim Special Edition\SKSE\crash-2026-08-04-01-59-34.log` shows five
consecutive `SpellHotbar2.dll` frames under `VCRUNTIME140.dll` and `KERNELBASE.dll` — the
signature of an unhandled C++ throw — reached from the Papyrus VM.

**Not reachable through the MCM.** `SpellHotbarMCM.psc:662` builds
`get_user_dir_bars_path() + "/" + name + ".json"`, which always carries a parent path. So this
is a latent footgun in a public Papyrus function rather than a bug players hit in normal play
— but any Papyrus caller, in this mod or another, that passes a bare filename crashes the
game with no diagnostic.

- [ ] Guard it: use the `error_code` overload, or skip `create_directories` when
      `parent_path()` is empty. One line either way.
- [ ] Check `load_bars_from_json`, `load_preset` and `save_preset` for the same unguarded
      filesystem pattern rather than fixing only the instance that happened to bite.
- [ ] Prefer reading the bars JSON shape from `Bars::to_json` in source over calling
      `saveBarsToFile` to discover it.
