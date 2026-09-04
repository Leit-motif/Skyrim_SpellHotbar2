## Agent skills

### Issue tracker

Issues and specs live as local markdown under `.scratch/`. See `docs/agents/issue-tracker.md`.

### Triage labels

Canonical triage roles use the default label strings. See `docs/agents/triage-labels.md`.

### Headless testing

What an agent can and cannot validate through DevBench, the oracle ladder for animation
identity, and the relaunch-batching economics. See `docs/agents/headless-testing-playbook.md`
before planning any live-game validation.

### Release packaging

The distributable archive is built by `python_scripts/build_mod_release.py`, which enforces the
overwrite ruling against the installed base mod rather than trusting a file list. See
`docs/agents/release-packaging.md` before changing what ships.

### Domain docs

Single-context: root `CONTEXT.md` + `docs/adr/`. See `docs/agents/domain.md`.

### SMF addon line

`ng/smf-next`, SKSE Menu Framework guest, Mod Control Panel, or MCM retirement: read
`docs/agents/smf-addon-line.md` before editing the UI host or merging that branch.

### Weapon Art icon prompts

For Skyrim-grounded image-generation prompts or revisions under
`python_scripts/weapon_art_icons/`, use
`.agents/skills/skyrim-weapon-art-icon-prompter/SKILL.md`. It requires animation evidence,
archetype-specific visual language, causal action geometry, and a 32 px acceptance check before
generation.
