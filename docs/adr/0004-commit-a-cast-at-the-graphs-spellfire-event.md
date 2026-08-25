# Commit a cast at the graph's spellfire event

A hotbar cast becomes committed at the vanilla `Voice_SpellFire_Event` raised by the exhale clip. Before that instant an interruption cancels the cast as it does today; from that instant the spell is delivered whatever happens to `IsShouting`. The fork builds no cross-mod commitment handshake with the MCO shout behaviour engine, because that event already is one.

The problem this settles is the one collision that loses player data. The engine chains by cutting the shout with `shoutStop`, which clears `IsShouting`; `CastingInstance::update` gates every frame on a raw `IsShouting` read and destroys the cast instance the moment it goes false. A cast cut before its own timer fires never reaches `cast_spell` and the spell is gone. A real shout survives the same cut because vanilla puts the magic out a tenth of a second into the exhale; this mod deliberately does not use that event, which is precisely why the cut is safe there and destructive here.

The engine side proposed two shapes and favoured the second: **A**, delay the cut until the cast's own timer has fired, which needs a delay nobody can derive and is a guess dressed as a design; and **B**, a deliberate handshake in which this mod tells the engine "my spell is out, you may chain now". Both are rejected, and B is rejected for a better reason than A. Its premise — that only the driver knows when its own spell has fired — is true of this mod as written, and it stops being true the moment the mod stops ignoring the event the graph already raises. A handshake would encode this fork's deviation from vanilla into a permanent contract between two mods, where a version of either can drift out of step with it silently. Reading the vanilla event instead removes the deviation, which is the thing that made a contract seem necessary.

The two rules then coincide by construction rather than by arrangement. The engine measures its chain window from `Voice_SpellFire_Event` and refuses to open before it, on the stated grounds that spellfire is the earliest point at which cutting still delivers the shout. This mod adopting the same instant as its commitment point means the engine's floor and the fork's guarantee are the same instant, for the same reason, with nothing passed between them. Neither side can move it without moving the vanilla event, and if a third consumer ever enters the shout graph the same contract already covers it.

Three consequences worth stating, because they are behaviour changes and not merely internal.

A cast is now robust to every cause of lost shout state after spellfire, not only to an MCO chain. A stagger, a killmove, a menu, or another mod clearing `IsShouting` late in the exhale used to lose the spell and refund nothing — the magicka is deducted only after `cast_spell` succeeds. That was never intended; it was the same defect with a different trigger, and the fix is indifferent to the cause because the fork cannot observe the cause without the API it just declined.

The release leads stay exactly as authored. A ritual cast notifies the exhale 250 ms before it fires, and a ritual concentration cast 1.0 or 1.5 s before, per animation. Those leads are why the collision is not confined to a rounding error: at the engine's default window the chain lands inside the longest of them. Salvaging the spell rather than shortening the lead keeps every uninterrupted cast byte-for-byte as it is, and changes behaviour only in the case that today is a bug.

A chain ends a concentration channel, and the channel must let it. The loop re-notifies `ShoutStart` every half second and checks liveness only afterwards, so a cut channel would re-enter the shout graph and tear down the MCO attack the player just chained into — the two systems fighting twice a second, indistinguishable from a transition bug. Checking liveness before re-notifying makes the trade honest: the first application has already landed, and the player has exchanged the rest of the channel for an attack.

What this does not settle is that nothing here makes a chain happen. The engine never arms on a hotbar cast — it arms on `BeginCastVoice`, which a hotbar cast does not raise — so it must still be changed to arm on the exhale, and that half is its ticket 38. This decision is the fork's answer to that ticket: the contract it asked for already exists, and what it needs from this side is the guarantee recorded here rather than a message. Casts must also reach the `CombatReady_*` branch first, which is ticket 02. And whether the spell fires at all on a hotbar cast has never been objectively confirmed.

## Scope amendment — 2026-08-08

The rejection of a cross-mod API is specifically a rejection of a **commitment handshake** for an
already-started cast. It remains correct: `Voice_SpellFire_Event` is still the commitment point.

ADR-0005 adds a different API at a different seam: before a Direct Cast starts during an active
MCO attack, Spell Hotbar retains the payload while ShoutMCO decides when to release or abandon the
input intent. That API neither reports spellfire nor changes this decision.

## Scope amendment — 2026-08-23

The commitment event followed the cast into the weapon graphs. Ticket 08 moved every hotbar cast
out of the shout graph and into the shtb Driver Cast states, whose clips carry their own spellfire
annotation; the committing event is therefore `MLh_SpellFire_Event` on the playing clip (+0.46s on
`MSCO_left1`, runtime-verified 2026-08-11), not `Voice_SpellFire_Event`. The principle is
unchanged: a cast commits at the spellfire event the playing graph raises, and no cross-mod
handshake exists. The `IsShouting` defect this decision fixed is now gone at the root — no cast
path reads `IsShouting` at all — but the commitment gate remains load-bearing for the narrower
hazard: a chain-out cut taken before the clip's spellfire still cancels, and ticket 10's chain-out
gates on commitment for exactly that reason. Ticket 07 closed against this ADR on 2026-08-23.
