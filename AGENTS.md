## Agent skills

### Issue tracker

Issues and specs live as local markdown under `.scratch/`. See `docs/agents/issue-tracker.md`.

### Triage labels

Canonical triage roles use the default label strings. See `docs/agents/triage-labels.md`.

### Headless testing

What an agent can and cannot validate through DevBench, the oracle ladder for animation
identity, and the relaunch-batching economics. See `docs/agents/headless-testing-playbook.md`
before planning any live-game validation.

### Domain docs

Single-context: root `CONTEXT.md` + `docs/adr/`. See `docs/agents/domain.md`.

### Weapon Art icon prompts

For Skyrim-grounded image-generation prompts or revisions under
`python_scripts/weapon_art_icons/`, use
`.agents/skills/skyrim-weapon-art-icon-prompter/SKILL.md`. It requires animation evidence,
archetype-specific visual language, causal action geometry, and a 32 px acceptance check before
generation.
