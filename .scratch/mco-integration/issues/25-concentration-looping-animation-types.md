# 25 — Concentration spells need their own looping animation types

A concentration channel must play a looping clip for as long as the player holds it. Fire-and-forget
Driver Cast types (`MSCO_left1`–`left4`, `MODE_SINGLE_PLAY`, exit on clip end) cannot represent that.

**Blocked by:** 08 (resolved). Ticket 08 parked this: looping conc state + release-opened chain
window. The 2026-08-11 spec direction is the tier plan.

**Status:** needs-triage

## Owner ask (2026-08-21)

> concentration spells need their own looping animation types

## You test this

(Unwritten until triage.) Hold Flames (or another known concentration) from a hotbar slot with a
weapon drawn. The clip loops for the hold. Release ends the channel. A fire-and-forget on the same
bar still uses its existing throw type, not the loop.

## What this is

Dedicated **animation types** for concentration so the Spell Editor / `SpellHotbar_SpellAnimationType`
picker and OAR can choose looping clips, instead of mapping conc onto the F&F throw set.

Today `chose_default_anim_for_spell` already assigns different ids for conc (aimed 1001, self 1002,
ritual 11001, ward 1003). Those ids pick OAR submods. The shtb Driver Cast clip generators are still
single-play and the state exits when the clip ends, so a concentration default still dies with the
first loop of a throw anim. Ticket 11’s `replay()` re-notifies `SH2_CastRight` and explicitly does
not walk the combo clip set; that is a restart, not a loop.

Related: ticket 05 (SYHO) — SYHO’s clip does not loop; conc in this load order stays broken until
this mod’s looping clips win.

## What this is not

Not Ability / Custom Ability Spell (weapon-arts 12). Concentration is excluded from that picker.
Not ticket 22’s F&F SpellFire→WinClose window. Not native left-hand MSCO (ticket 24).

## Triage

Answer these before `ready-for-agent`:

1. **Graph vs type.** Is this (a) new looping animation type ids + OAR clips on the existing shtb
   states, (b) a dedicated looping conc state in the shtb patch (spec tier 1), or (c) both?
2. **Clip source.** Author new loops, reuse vanilla / MSCO concentration loops, or retarget an
   existing pack? Do not ship someone else’s HKX without an answer.
3. **Release.** Spec tier 2 is a chain-out window opened at button release. Same ticket, or a
   follow-up after the loop is visible?
4. **Combo.** Ticket 11 left channels out of the cast-index walk. Keep that, or does a looping
   type also need combo-position rules?
5. **First person / ritual / ward.** Same looping type family, or separate ids as today
   (1001 / 1002 / 11001 / 1003)?

## Notes

Spec 2026-08-11: (1) looping conc state, exit on release; (2) chain window at release; (3)
combo-position continuity. This ticket is the owner’s animation-type slice of that path.

`msco_cast_driver::replay` comment: “looping channel does not walk the clip set; ticket 11 leaves
channels out.”

## Comments

Owner 2026-08-21: filed from the post-09 punch list. Not specified enough to implement.
