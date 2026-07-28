# Spell Hotbar 2 Integration

This context defines the language used while adapting Spell Hotbar 2 for the user's Nolvus mod list.

## Language

**Nolvus Integration Fork**:
A maintained fork that preserves Spell Hotbar 2's core product while adding the configuration, compatibility, fixes, and selective improvements required by the user's Nolvus mod list.
_Avoid_: Rewrite, successor, total redesign

**Baseline Adoption**:
The first integration milestone: a compatibility check that confirms the base mod behaves normally in the active Nolvus load order and identifies unintended interactions with that environment before customization begins.
_Avoid_: Experimental proof of concept, reduced feature trial, customization milestone

**Direct Cast**:
The fork's primary casting mode, where activating a hotbar slot casts its bound spell directly instead of first equipping it. This behavior is the central reason for adopting Spell Hotbar 2.
_Avoid_: Equip-first casting, secondary test mode

**Installed Configuration**:
The exact set of components and options selected in the user's current FOMOD installation. Compatibility acceptance covers this configuration, not unselected installer alternatives.
_Avoid_: Every installer permutation, theoretical support matrix

**Core Fork**:
The maintained Spell Hotbar 2 source that owns generally applicable native behavior, fixes, and improvements.
_Avoid_: Nolvus patch, load-order bundle

**Compatibility Package**:
A separate integration layer that owns records, presets, configuration, and other adaptations specific to the user's Nolvus load order.
_Avoid_: Core fork, upstream source
