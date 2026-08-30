# 17 — The bound-slot strip lies about liveness and shows no tint tiers

Filed from ticket 14's owner acceptance pass (2026-08-25). Ticket 14 tinted the Abilities *list*
and deliberately left the slot strip unchanged. The owner's pass showed that the strip needs the
same treatment, plus one defect of its own.

**Blocked by:** nothing. Ticket 14 closed DONE, owner-accepted 2026-08-25 — this ticket has
been unblocked since then and the line above said otherwise until the 2026-08-29 sweep.

**Status:** DONE — owner-accepted 2026-08-29. See `## Diagnosis and build` at the bottom: the
staleness was never staleness, and the first fix reintroduced it in a new coat of paint.

## What the owner saw (Two-Handed bar, Binding Menu)

1. **Stale gray.** Slots bound before this session showed the wrong gray state until each was
   rebound; rebinding refreshed them. Suspicion: the gray/liveness state is baked into the slot at
   bind time rather than computed at render against the currently selected bar. Diagnose before
   fixing — if it recomputes per frame, find what actually cached.
2. **No tint tiers.** The strip shows only icon-gray for a dead art. The owner expects the same
   yellow / white / gray tiers the Abilities list got in ticket 14, driven by the same Ability
   Class against the same selected bar.
3. **Partial gray.** Where an art is dead (e.g. Blood Seeker, 1H, on the Two-Handed bar), only its
   icon grays. The keybind label ("NP7") and the name should gray with it — the whole row reads as
   dead, not just the picture.

## You test this

Profile `Nolvus Awakening`, Binding Menu, Two-Handed bar, without rebinding anything:

1. Open the menu fresh with arts already bound. Every bound slot's tint is correct immediately —
   no rebind needed.
2. Switch bars in the dropdown. The strip's tints follow the selection at once.
3. A dead art's icon, keybind label, and name are all gray. A direct match (Blood Flurry on
   Two-Handed) shows yellow on the same elements the Abilities list yellows. Generic stays white.

## What this is not

Not the Abilities list (ticket 14 owns it and passed on the left pane). Not HUD tinting — the HUD
knows the live weapon and has no any-match to disambiguate. Not a bind-behavior change:
`apply_bind_drop` and slot contents stay untouched; only presentation changes.

## Comments

Owner 2026-08-25: reported during ticket 14's acceptance pass with two screenshots (Two-Handed
bar, before and after rebinding the stale slots). The label/name gray is called a nit, but wanted.

Owner 2026-08-25, second observation: switching the bar to **Dual Wield** turned the entire strip
gray — every slot, including ones that should be live on that bar. Rebinding is expected to fix
each slot again. This is the same staleness as (1) but its full-strip form, and it is the clearest
repro: **change the bar dropdown and watch the strip without touching a binding.** The owner asks
whether it is merely cached inheritance from the previously selected bar, which is the first thing
to check.

Scale note for triage: the strip is wrong on every bar switch, not occasionally. The owner's
judgment is that the left pane carries the real information, so this is presentation polish rather
than a blocker — but it is wrong on screen every time a bar changes.

## Diagnosis and build, 2026-08-29

### Nothing was cached. Gray meant two things.

The ticket asked to find what cached before fixing it, and the answer is that nothing did. The
strip's liveness overlay was already recomputed every frame against `Bars::menu_bar_id`
(advanced_bind_menu.cpp, ticket 13's last hunk), and ImGui redraws the whole menu each frame, so
it could not lag a bar switch.

What the owner was reading as a stale liveness gray was a *different* gray on a different element:

```cpp
//draw text in grey if it was inherited from parent bar
int grey_val = inherited ? 127 : 255;
```

The slot *name* was grayed to mark a slot the bar inherited from its parent, and that predates the
weapon-arts work entirely. Both reported symptoms fall out of it exactly:

- **The stale slots.** Arts bound on Main and viewed on Two-Handed are inherited, so their names
  drew gray. Rebinding on the Two-Handed bar writes the slot into that bar's own sub-bar, the slot
  stops being inherited, and the name goes white. That is the "rebinding refreshed it" the owner
  saw — a bind-ownership change, not a recomputation.
- **The whole Dual Wield strip.** On Dual Wield every slot inherits from Main, so every name drew
  gray at once. Liveness was never involved: only 2H arts are dead on that bar, and 1H, Dual and
  Generic are all live there (now pinned in `the_owners_two_handed_and_dual_wield_cases`).

So the owner's answer to their own question — "merely cached inheritance from the previously
selected bar" — was half right. It was inheritance. It was never cached.

### The fix: hue is liveness, alpha is inheritance

Two signals had been sharing one channel, which is why they overwrote each other. They are now on
separate ones, and both survive:

- **Hue** is the tier, from the new `art_bar_tint` in `art_definition.h`: gray dead, yellow direct,
  white generic. The Abilities list and the strip both read it, so ticket 14's left pane and this
  ticket's strip cannot drift apart — that was the point of putting the rule in a shared header
  rather than repeating the two predicates at the second call site.
- **Alpha** is inheritance: an inherited slot draws at half alpha in whatever tier colour it has.
  Half alpha lands close to where the old 127/255 grey sat against this menu's dark background, so
  a bound spell slot looks about as it did.

The tier drives the icon overlay, the keybind label (text and, in keybind-icon mode, the button
icon's tint) and the name, so the whole row reads dead together rather than the picture alone.

Non-art slots have no tier and stay white, which means an inherited *spell* slot's keybind label
now dims with its name where it used to stay full white. That is a small widening of what
inheritance marks, and it is consistent: the row is the unit.

`apply_bind_drop` and slot contents are untouched, as the ticket required.

### Acceptance

- [x] Tier logic is pure and tested: `art_bar_tint` composes the two existing predicates, and
      three new cases in `art_data_test.cpp` pin the tier across every bar (parents, sneak
      variants, Magic/Ranged), prove gray always outranks yellow, and pin the owner's own
      Two-Handed and Dual Wield rows. All seven suites pass.
- [x] **Owner cell**, accepted 2026-08-29. Open the menu fresh with arts already bound: every
      bound slot's tint is correct with no rebind.
- [x] **Owner cell**, accepted 2026-08-29. Switch bars in the dropdown; the strip's tints follow
      at once, and Dual Wield shows only 2H arts gray rather than the whole strip.
- [x] **Owner cell**, accepted 2026-08-29. A dead art's icon, keybind label and name gray
      together; a direct match is yellow on label and name; everything else stays white.

Visual cells are owner cells by the standing no-agent-screenshot ruling in the headless testing
playbook.

### The fix took two passes, and the first one was the same bug

Worth recording, because the first pass looked right and shipped anyway. The diagnosis above is
correct: the collision was inheritance-grey sitting on the liveness channel. The first fix moved
inheritance from a grey *hue* to a half *alpha* and declared the two signals separated. They were
not. **Brightness is one channel**, and hue-vs-alpha is a distinction the eye does not make on a
dim slot label — so the strip still dimmed nearly every row on a child bar, and the owner reported
it again four days later, this time on a Dual Wield strip of nothing but spells and shouts:

> they're all spells and shouts, so they shouldn't be grayed out. it's like there's some sort of
> stale reference or something

Which is the original complaint, verbatim, against a build that was supposed to have fixed it.

So the second pass spends brightness on liveness and nothing else. `inherited` still drives the
hover affordance and the drag source; it no longer touches colour. A slot with no art class —
every spell, shout, and power — is white, always.

**The cost, stated rather than buried:** the strip no longer shows which slots are inherited from
the parent bar versus bound on this one. That is a real signal, deliberately dropped. The argument
for dropping it is that on a child bar nearly every slot is inherited, so it approached "always
on" — low information, at the price of the one channel the tint tiers need. If it is wanted back,
it needs a channel that is not brightness; a corner marker on the icon is the cheap option. Owner
accepted the trade on 2026-08-29 without asking for it back.

Owner 2026-08-29, acceptance: "weaponart17 working. we can close it." — verified against the
21:51 build on profile `Nolvus Awakening`, after a relaunch, with no rebinding.
