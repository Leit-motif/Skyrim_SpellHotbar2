# Ticket 46/48 range — Codex review (gpt-5.6-sol, high) and dispositions

Reviewed range `1b40411..54cf9ee` on origin/master, 2026-08-26. Full report in the job log
(`~/.agent-config/codex-jobs/spell-hotbar-2/20260826224313067-xu75qt.ndjson`). 3 high / 3
medium / 4 low. Dispositions, applied the same day in the commit citing this note:

| # | Finding | Disposition |
|---|---------|-------------|
| 1 (H) | Generation identifies re-arm timing, not the emitting cast: a stale event processed after a re-arm adopts the new generation | DOCUMENTED, no code. Event dispatch is synchronous — the residual window is a mid-hook re-arm from the game loop, and fixes 2's freshness checks shrink it to instructions. Event-carried generations would need clip-side data that does not exist. |
| 2 (H) | Commitment + isolation ran on the entry snapshot alone, before the generation-aware notify, no rollback | FIXED: the hook re-reads the arming word immediately before the commitment call and before the interrupt; a moved generation zeroes the effective mask / skips isolation. |
| 3 (H) | `CastComboIndex::index_` plain int = data race (pre-existing, load-bearing for this range) | FIXED: `std::atomic<int>`, CAS advance. |
| 4 (M) | Armed mask never cleared at teardown; vanilla SpellFires accepted post-cast (ticket 51's log stream) | FIXED: `clear_spellfire()` drops mask + latch (generation preserved); every call site is a teardown and `arm_spellfire` rebuilds at start. |
| 5 (M) | Fast-ritual ids alias the dual ids, so fast rituals select dual art | ACCEPTED as upstream's own id convention (recorded in ADR-0018 consequences and the build note). Revisit only if the owner objects to a fast ritual's look. |
| 6 (M) | Ignoring `Animation2` for dual removes a documented per-spell override | FIXED: `is_dual_family_id()` — the column is honored for dual when it names one of the four structural dual ids, ignored otherwise (Firebolt's 10001 stays out). |
| 7 (L) | 32-bit generation wrap not strictly harmless | ACCEPTED — wrap requires ~centuries of casts; comment already says equality-only. |
| 8 (L) | No adversarial interleaving tests | NOTED as test debt; the schedules need a threaded harness the suite doesn't have. Carried here, not ticketed. |
| 9 (L) | `clear_spellfire` doc contradicted implementation | FIXED with 4 (comment + header doc now describe the disarm). |
| 10 (L) | Build note stale after the self-art rebuild | ACCEPTED — the closed ticket and this note are the accurate record; the build note stays as the historical state at its date. |

Also confirmed clean by the review: the packed word doesn't tear; decode centralization; the
`allowed_to_cast` rewrite is semantically equivalent; pack conditions/priorities match
ADR-0018 exactly; and the SYHO duplicate adds ~2 MILLION priority (not +2e9 as the ticket-44
inventory said) topping out ~101,901,002 — the pack outranks it with room.

Post-fix verification: full rebuild clean, all 7 unit suites pass (new cases:
`is_dual_family_id` gate), DLL deployed 18:31 with the game closed. The freshness checks and
disarm are runtime-consequential only under adversarial timing; live behavior of the accepted
matrix is unchanged by design.
