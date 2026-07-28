# Spell Hotbar 2 Integration

This context defines the language used while adapting Spell Hotbar 2 for the user's Nolvus mod list.

## Language

**Nolvus Integration Fork**:
A maintained fork that preserves Spell Hotbar 2's core product while adding the configuration, compatibility, fixes, and selective improvements required by the user's Nolvus mod list.
_Avoid_: Rewrite, successor, total redesign

**Baseline Adoption**:
The first integration milestone, in which the unmodified base mod must load cleanly, expose its interface, bind an item, and successfully cast or use it in the active Nolvus profile before customization begins.
_Avoid_: Feature-complete integration, customization milestone

**Direct Cast**:
The fork's primary casting mode, where activating a hotbar slot casts its bound spell directly instead of first equipping it. This behavior is the central reason for adopting Spell Hotbar 2.
_Avoid_: Equip-first casting, secondary test mode
