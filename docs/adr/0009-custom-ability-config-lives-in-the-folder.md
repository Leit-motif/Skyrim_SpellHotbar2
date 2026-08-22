# Custom Ability configuration lives in the folder, not in icon_edits or the save

A Custom Ability is a drop-in folder (`Custom_Ability_N`) whose clip already lives there. Name, icon, Custom Ability Spell, cooldown, and Ability Costs therefore persist as one sidecar in that folder. Payload Interpreter config is emitted from that sidecar; it is not the source of truth.

Rejected: `name.txt` / `icon.txt` (v1 stand-in, three files, no spell/cost). Rejected: `icon_edits` JSON keyed by art id (player icon presets; copying the folder would not copy the definition). Rejected: SKSE co-save (per character; Custom Ability is load-order data).
