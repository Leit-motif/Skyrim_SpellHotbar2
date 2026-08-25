# Pointer-pack player tuning lives in the user overlay, not the ash folder

The Ability Editor can retune any catalogue row, including pointer-pack ashes. Custom Ability still persists in the folder sidecar (ADR-0009). Ash name, icon, Ability Class, costs, cooldown, and GCD persist in the existing user `art_icons.json` overlay (`art_edits`), keyed by art id. That overlay is player data, not load-order data: regenerating the pointer pack must not wipe it, and SH2 must never write the author's HKX.

Rejected: sidecar in the pointed AoW folder (not ours). Rejected: rewriting `arts_ashes.csv` (catalogue, regenerates). Rejected: SKSE co-save (per character; this is the same player overlay as icon edits).
