# 03 — Map an Art Pack folder to a Weapon Art catalogue row

Players will want each Ashes of War (or any OAR folder that replaces `AABL_Attack_A.hkx`) to show
up as its own named art in Spell Hotbar 2. Ticket 01 has one hardcoded Test Art in `arts.csv`.
This ticket grows the catalogue from folders, without copying animation files.

**Status:** ready-for-agent

**Blocked by:** 01

## You test this

With Ashes of War present in the load order (Nolvus), after the generated pack is installed:

1. The bind menu Arts list (ticket 02) — or `arts.csv` if 02 is not landed yet — names more than
   Test Art. At least one real art name from the pack is visible (e.g. a named Ash).
2. Bind that art, press the slot with a weapon drawn. The clip that plays is that art, not the
   inert `AABL_Attack_A` placeholder and not a different Ash.
3. Bind a second art to another slot. Pressing each slot plays a different clip.
4. Unequip any slot-55 art clothing. The bound art still plays (selector, not worn keyword).

If every slot plays the same placeholder, or if the worn item still picks the clip, it fails.

## Agent tests the rest

5. Art Selector is 0 when no art is live. Worn-item Ashes of War behaviour still works on the
   AABL hotkey path (spec story 18).
6. Regenerating the pack against the installed Ashes of War does not copy `.hkx` files into this
   repo or the compatibility package.
7. A missing folder or renamed OAR submod logs loudly (spec story 29); the other arts still bind.

## Notes

ADR-0007: the graph always plays `Animations\AABL_Attack_A.hkx`. ADR-0008: which replacement wins
is the Art Selector global, via OAR conditions at higher priority than worn-item conditions.

Do **not** add one Nemesis path per art. Do **not** redistribute Ashes of War clips.

Authoring-time script (spec “The Ashes of War integration”): read installed OAR submods, emit
(1) `arts.csv` rows (id, name, icon, selector, stamina, cooldown) and (2) OAR user-override
configs that `CompareValues` the Art Selector. The installer gates the group on the items plugin;
it does not run the script.

Custom spell registration is FormID-keyed overrides of existing spells. An art is not a spell
form. Mapping a folder → catalogue row + selector + OAR override is the registration path.
An in-game “add custom art” dialog can wait until this generator has proved the data shape.
