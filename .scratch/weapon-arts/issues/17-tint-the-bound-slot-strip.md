# 17 — The bound-slot strip lies about liveness and shows no tint tiers

Filed from ticket 14's owner acceptance pass (2026-08-25). Ticket 14 tinted the Abilities *list*
and deliberately left the slot strip unchanged. The owner's pass showed that the strip needs the
same treatment, plus one defect of its own.

**Blocked by:** nothing. Ticket 14 closed DONE, owner-accepted 2026-08-25 — this ticket has
been unblocked since then and the line above said otherwise until the 2026-08-29 sweep.

**Status:** ready-for-agent

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
