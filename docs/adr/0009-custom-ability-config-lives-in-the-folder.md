# Custom Ability configuration lives in the folder, not in icon_edits or the save

A Custom Ability is a drop-in folder (`Custom_Ability_N`) whose clip already lives there. Name, icon, cooldown, Ability Class, and Ability Costs persist as one sidecar in that folder.

Custom Ability Spell keys may still exist in that sidecar. They are unused until ticket 12: the live fire is the dropped clip’s author payloads, not an emitted Payload Interpreter `$custom_ability_N` instruction. Ticket 12 would emit PI config from the sidecar; that config is not the source of truth.

Rejected: `name.txt` / `icon.txt` (v1 stand-in, three files, no spell/cost). Rejected: `icon_edits` JSON keyed by art id (player icon presets; copying the folder would not copy the definition). Rejected: SKSE co-save (per character; Custom Ability is load-order data).
