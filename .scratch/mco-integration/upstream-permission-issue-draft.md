# Draft: GitHub issue to pWn3d1337 requesting Nexus distribution permission

**Status:** draft for the owner to post. Not sent. Post at
<https://github.com/pWn3d1337/Skyrim_SpellHotbar2/issues>.

**Why it is shaped this way**

- **It asks the three permission chains separately** — his code, the icons, the SkyUI assets — so he
  can say yes to the part that is actually his without having to think about the rest. A single
  blanket "can I publish this?" is the version that gets no reply, because answering it correctly
  requires him to audit his own asset provenance first.
- **It offers him an easy no** that still leaves you shipping. If he declines redistribution you
  fall back to an add-on requiring his GitHub release, which needs no permission at all
  (ADR-0002). Naming that yourself removes the pressure and makes a yes cheaper to give.
- **It does not claim succession.** Your own `CONTEXT.md` avoids "successor" and "rewrite" for the
  fork, and proposing a name under his rather than over it is the difference between a compliment
  and a takeover.
- **It leads with what you built**, because the credible reason you are asking is that you did real
  work on top of his, not that you want his download count.

**Before posting, check these are still true:** the MCO integration is owner-accepted and working,
and you are content to be held to whatever you promise here about credit and linking.

---

## Title

Permission to distribute an MCO integration fork on Nexus?

## Body

Hi — thanks for Spell Hotbar 2, it is the best implementation of hotbar casting I have found, and
Direct Cast in particular is the reason I built on it rather than anything else.

I have been maintaining a private integration fork that adapts it to work with **MCO/ADXP** melee
combat. The substantive part is that SH2's cast states are distributed into the weapon behaviours,
so a cast can start from a drawn weapon stance and commits on the animation's SpellFire event
instead of refusing. I am also wiring it into a separate SKSE plugin of mine so a cast pressed
mid-swing is held and released at the hit frame rather than cancelling the attack. It is working in
my own game.

I would like to publish it on Nexus. My honest reason is reach: SH2 has no Nexus page, and most
people I talk to have never heard of it. A page that credits and links back to your repo would
probably send more people to your project than it takes away.

Three things I want to check before I do anything, because I do not want to assume:

1. **Your code.** `skse_plugin/LICENSE.txt` is MIT, which I read as allowing a derivative build if
   I ship your notice. But there is no repository-level licence, so I would rather have you say it
   than infer it. Would you be OK with a Nexus release of a fork of the plugin, crediting you and
   linking here? Adding a root `LICENSE` would also settle it permanently for anyone else who asks.
2. **The icons.** Your README credits ArchAngelAries. I saw you agreed to icon bundling in #84, but
   I would rather confirm whether those are yours to license or whether I should be asking them.
3. **SkyUI assets.** I am assuming these should not be redistributed and I will exclude or replace
   them unless you know otherwise.

If you would rather I did not redistribute your files at all, that is completely fine — I can ship
only my own plugin as an add-on that requires your GitHub release, and I will do that instead. I
would just rather ask than guess.

On naming, I was not planning anything that implies it replaces SH2 — something like "Spell Hotbar
2 - MCO Integration" rather than a version number of your project. Happy to use whatever you prefer,
or to drop the Spell Hotbar name entirely if you would rather.

Thanks either way.
