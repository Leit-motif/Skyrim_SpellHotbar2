# Release packaging

How the distributable archive for this fork gets built, and the rules the build enforces so it
cannot quietly ship the wrong thing. Ticket 59 in `.scratch/mco-integration/issues/` owns the
scope; this file is the operating manual.

```bash
python python_scripts/build_mod_release.py
```

Output lands in `build/` (gitignored): the `.zip` and a `.manifest.json` listing every file with
its SHA-256 and its classification against the base mod.

## What is being built

Our mod is an **overwrite** over base Spell Hotbar 2, not a standalone mod and not a pure
addition. Base SH2 is a hard requirement; ours installs after it and wins the conflict. Our DLL
is compiled from upstream's own source at release `0.0.14`, with our changes on top, so it
replaces the base DLL rather than sitting beside it.

The archive holds 158 files: 155 additions and exactly three overwrites.

| Overwrite | Why |
|---|---|
| `SKSE/Plugins/SpellHotbar2.dll` | Our build of upstream's source. 25 of upstream's own `.cpp` files are modified. |
| `Scripts/SpellHotbar.pex` | `SpellHotbar.psc` is modified. |
| `Scripts/SpellHotbarMCM.pex` | `SpellHotbarMCM.psc` is modified. |

Everything else is new: the `shtb` and `shcr` Nemesis patches, the `SpellHotbar2Casts` and
`SpellHotbar2Arts` OAR submods, the Ability catalogues, the Weapon Art icon atlas, the
Behavior Data Injector config, and our two `.psc` sources.

## The five rules the build enforces

Each of these is a failure the script produces, not advice it prints.

**Only git-tracked files ship.** Trees are enumerated with `git ls-files`, so a playtest
animation drop, a scratch file, or anything `.gitignore` covers cannot reach a release. The DLL
and the two `.pex` are the named exceptions, because they are build outputs.

**Every member is classified against the installed base mod, byte for byte.** Absent from base
is an addition. Present and different is an overwrite. Present and *identical* is redundant, and
fails the build — a byte-identical file is by definition an upstream-untouched asset, and it
belongs to the user's base install. This is what makes "upstream assets provably absent"
checkable rather than asserted. It needs the base mod on disk; `base_mod.install_path` in
`deploy/release/release.json` points at it.

**The overwrite set is declared.** `EXPECTED_OVERWRITES` in the script lists the three files
above. If classification disagrees in either direction the build fails, so a new overwrite has
to be a deliberate edit to that list.

**Nemesis patch files ship CRLF.** The repo stores them LF and relies on `core.autocrlf=true`
to restore CRLF on checkout. A machine with `autocrlf=false` would hand the build LF files and
ship them, and nothing downstream would say so, so the build checks and fails instead.

**No real name reaches a packaged file.** The guard reads committer names out of `git log` and
scans every packaged text file for them. Nemesis prints `author=` from `info.ini` in its own
mod list, and that is exactly where a real name leaked in the sibling repo's release. Both
`info.ini` files read `author=Leitmotives`.

## Compiled Papyrus scripts

There is no Papyrus compiler on this machine — Nolvus ships no Creation Kit — so the build
imports the two `.pex` instead of compiling them:

```bash
python python_scripts/build_mod_release.py --refresh-pex
```

That copies them from `compiled_scripts_dir` (the deployed MO2 dev mod) into
`papyrus/Scripts/`, and records in `papyrus/Scripts/compiled.json` the SHA-256 of the `.psc`
each one was compiled from. Every later build re-checks that hash and fails if a `.psc` has
changed since, so a stale `.pex` cannot ship silently. After editing a `.psc`: recompile, then
`--refresh-pex`.

## Identity is not frozen yet

`deploy/release/release.json` carries `public_name`, `version`, and `identity_frozen`. Release
ticket 01 owns the first two. While `identity_frozen` is `false` the archive filename gains a
`-provisional` suffix, mechanically, so a verification build cannot be mistaken for an upload
candidate. Set the flag when ticket 01 records the name and version scheme.

Ticket 02 of the release effort also wants our version in the DLL's own version resource.
`skse_plugin/CMakeLists.txt` still reads `project(SpellHotbar2 VERSION 2.0.10)`, which is
upstream's number. Changing it requires a DLL rebuild and belongs with ticket 01's version call.

## Publication gate

`publication_blocked` in `release.json` is `true`. Redistributing a modified build of pWn3d's
DLL is a permissions question the owner settles; the draft ask is at
`.scratch/mco-integration/upstream-permission-issue-draft.md`. The build prints the block and
records it in the manifest. Build freely; do not upload while the flag is set.

## The version coupling, which bites silently

Our fork point is exactly upstream tag `0.0.14` (`git describe upstream/master` confirms it), and
the installed base is `0.0.14`, so nothing upstream fixed is currently being reverted. That
changes the moment upstream releases again: on a newer base, our older DLL overwrites theirs
while the newer assets stay, and nothing warns the user. Upstream has been idle since June 2025.
Re-check `base_mod.supported_version` on any upstream release.

## One exclusion, and why

`data/SKSE/Plugins/SpellHotbar/localization/translation.txt` stays out of the archive. It is
base's English translation with one typo fixed (`Globald Cooldown`), and the base FOMOD installs
whichever language the user picked under that same filename — so shipping ours would force
English on every non-English user to fix one word. The DLL's compiled default already carries
the corrected string, and it carries the ten keys this file lacks. The script prints the
exclusion and its reason on every run.
