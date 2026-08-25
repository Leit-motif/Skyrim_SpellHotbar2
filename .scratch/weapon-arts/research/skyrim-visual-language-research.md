# Skyrim Visual Language for Weapon Art Icon Prompting

Status: research basis for a project context layer; not a generation prompt and not a license to copy shipped art.

## Scope and source policy

This guide converts Skyrim evidence into prompt-level art direction for original, effect-first MMO hotbar icons. Source priority is: (1) the locally installed vanilla Skyrim Special Edition meshes and textures, (2) official Bethesda/Bethesda Game Studios articles and screenshots, and (3) clearly labeled project-owner conventions. It deliberately does not treat fan wikis, generic fantasy imagery, ESO assets, or an image model's prior associations as Skyrim ground truth.

The official Bethesda retrospective explains that Skyrim concept art established tone, setting, and the visual "flavor" that production art then translated into game assets; it also names architecture, props, costume, iconography, and gameplay/storytelling as parts of the same visual-design process. That is the right model here: consult concrete Skyrim references, but synthesize a new readable icon rather than copying one item or pose. [Bethesda, "From Concept to Character: Creating Skyrim's Artwork"](https://bethesda.net/en-EU/news/concept-to-character-creating-skyrims-artwork)

Local archive evidence below comes from read-only listings of:

- `C:\Nolvus\Instances\Nolvus Awakening\STOCK GAME\Data\Skyrim - Meshes0.bsa`
- `C:\Nolvus\Instances\Nolvus Awakening\STOCK GAME\Data\Skyrim - Meshes1.bsa`

Root-agent verification 2026-08-23: AutoMod's archive status command reported that BSArch was not
installed, so no extraction or archive mutation was attempted. Read-only binary-index searches
independently confirmed the named `armor\blades`, `armor\hide`, `armor\imperial`,
`armor\orcish`, `armor\stormcloaks`, `weapons\akaviri`, `weapons\iron`, `weapons\steel`, and
`weapons\orcish` families, plus `bladesarmor*.nif`, `bladeshelmet*.nif`, `bladessword.nif`,
`cuirassheavychieftain*.nif`, `waraxe.nif`, `steelwaraxe.nif`, and `orcishhandaxe.nif` entries.
The same bounded search returned no dedicated `armor\breton`, `armor\bosmer`, `armor\khajiit`, or
`armor\argonian` family.

These are source references, not files to ship or feed wholesale into a model. If a specific visual feature matters, render or capture only the relevant locally owned asset as a narrowly scoped reference and label its role (for example, `armor construction only; do not copy pose/composition`).

## Skyrim-wide sensibility

Use this as the default visual substrate unless a named culture or magic effect overrides it:

- **Material-first, weathered fantasy:** iron/steel, leather/hide, coarse cloth, fur, wood, bone/chitin, and carved stone should read as physical materials with wear, soot, patina, scratches, uneven edges, and practical joins. Avoid pristine chrome, glossy superhero surfaces, or frictionless sci-fi geometry. The vanilla archive's major armor families are literally organized around material/culture sets such as `armor\hide`, `armor\iron`, `armor\steel`, `armor\orcish`, `armor\elven`, and `armor\blades`; Bethesda's concept retrospective likewise shows design ranging from humble instruments and longhouses to glass weapons and Daedric armor rather than one uniform fantasy finish. [Local `Skyrim - Meshes0.bsa`; local `Skyrim - Meshes1.bsa`; Bethesda concept-art retrospective](https://bethesda.net/en-EU/news/concept-to-character-creating-skyrims-artwork)
- **Heavy, carved silhouettes:** even elegant sets tend to use strongly legible ridges, plates, collars, guards, or blade profiles. For a 32px icon, preserve two or three identifying masses rather than surface filigree.
- **Restrained palette before magic:** charcoal, iron gray, muted brown, desaturated cloth colors, bone, and aged gold/bronze are safer defaults than saturated teal/orange cinematic grading. Saturated color should normally identify the attack effect, school, faction cloth, or one focal accent.
- **Asymmetry and hand-built irregularity:** small differences in fur edges, straps, plates, and sparks help Skyrim read as lived-in. Do not make every attack a perfectly centered heraldic badge unless the animation itself is symmetrical.
- **Function before costume showcase:** the attack/effect is the subject. Cultural armor supplies only enough silhouette and material cues to identify the actor. Bethesda's artists explicitly describe building gameplay and storytelling into images; for these icons, the gameplay event is the frozen action frame. [Bethesda concept-art retrospective](https://bethesda.net/en-EU/news/concept-to-character-creating-skyrims-artwork)

## Mandatory prompt-resolution rule

Never expand a race name directly into a costume. Resolve these fields separately:

1. **Race/anatomy:** human, mer, Khajiit, Argonian; body proportions or nonhuman head/tail only when visible at icon scale.
2. **Culture/faction/archetype:** Nord barbarian, Stormcloak soldier, Breton knight, Blades warrior, Thalmor battlemage, Dunmer ashlander, etc.
3. **Equipment family:** exact armor/material and exact weapon silhouette.
4. **Action frame:** what the body and weapon are doing at the selected annotation moment.
5. **Effect:** physically connected to the correct weapon/body contact.

This is necessary because vanilla Skyrim has dedicated faction/material sets for Blades, Stormcloaks, Imperials, Thalmor, and Orcish equipment, but searches of the shipped mesh archives do not reveal parallel `armor\breton`, `armor\bosmer`, `armor\khajiit`, or `armor\argonian` cultural sets. Khajiit and Argonian often appear as anatomy-specific variants of shared helmets (`helmet...khajiit`, `helmet...argonian`), which is evidence of body accommodation, not a racial costume. [Local `Skyrim - Meshes0.bsa`]

## Cultural and archetype prompt vocabulary

The "use" column is a compact prompt vocabulary, not a claim that every member of a race dresses this way.

| Target | Use when explicitly selected | Avoid / distinction | Evidence |
|---|---|---|---|
| **Nord barbarian / fur warrior** | rough layered animal fur and hide; coarse leather straps and wraps; exposed or lightly covered arms; compact iron/steel hand axes; broad athletic silhouette; wind-tossed hair or shadowed face; cold-climate wear without polished plate | Do not silently turn this into a Stormcloak, horned-helmet mascot, plate-armored hero, Viking cosplay, or icy mage. “Nord” alone does not authorize barbarian costume. | Vanilla hide family includes light, medium, heavy, and `cuirassheavychieftain` meshes under `meshes\armor\hide\...`; vanilla iron/steel families contain `waraxe.nif` / `steelwaraxe.nif`. [Local Meshes0/1 BSAs] |
| **Nord soldier / Stormcloak** | sleeved or sleeveless Stormcloak cuirass, simple boots/gloves, plain helmet, round faction shield where required; practical militia silhouette | Keep separate from fur-clad barbarian, plated housecarl, and ancient Draugr/Nordic hero. Do not use this set merely because the actor is a Nord. | `meshes\armor\stormcloaks\cuirassm_0.nif`, `cuirasssleeved_0.nif`, `helmm_0.nif`, `shield3rd.nif`. [Local Meshes0 BSA] |
| **Breton knight (project convention)** | French-medieval knight coding: mail, simple plate pieces, closed bascinet/great-helm silhouette, blue or burgundy surcoat; straight arming sword, mace, or heater-like shield only when the ability requires it | This is a deliberate project convention, not a dedicated vanilla Skyrim Breton armor set. Do not render a hooded rogue, generic wizard, or Renaissance noble. Do not call ESO imagery vanilla Skyrim. | No dedicated vanilla `armor\breton` set found in local archive search. Bethesda nevertheless calls the ESO Breton Knight "iconic" in an official Skyrim featured-mod article, which makes it a defensible cross-series reference when explicitly labeled. [Bethesda featured mods](https://bethesda.net/en-US/news/skyrim-special-edition-may-featured-mods); [Local Meshes0 BSA] |
| **Redguard warrior** | light, mobile layered cloth/leather; wrapped or draped fabric; warm earth/red accents; curved or distinctive Yokudan sword only when culturally requested; agile sword posture | Avoid generic Arabian costume, oversized turbans, belly-dancer shorthand, or automatic fire magic. Race and Hammerfell/Yokudan equipment must be requested separately. | Official Anniversary Edition material identifies "Redguard Elite Armaments" as light armor with new weapons including the Yokudan sword Boneshaver and provides first-party screenshots. [Bethesda/BGS Anniversary preview](https://elderscrolls.bethesda.net/en-AU/news/5eoP4CA02055Pf9Tunp0AS/skyrim-anniversary-edition-sneak-peek-variety-2) |
| **Dunmer** | ash-dark skin/red-eye anatomy when visible; choose an explicit Solstheim/Morrowind archetype: bonemold soldier, chitin scout, Morag Tong assassin, cultist, or Telvanni mage; ash, ember, insect-shell, bone, and dark cloth cues | Do not default every Dunmer to black assassin leather or purple wizard robes. “Ancestor's Wrath”/fire affinity is not permission to add fire to an unrelated weapon animation. | Dragonborn's installed assets contain culture-specific families under `meshes\dlc02\armor\bonemold`, `...\chitin`, `...\moragtong`, plus cultist/Telvanni assets; Bethesda's Survival Mode article explicitly names the Dunmer Ancestor's Wrath ability alongside Flame Cloak. [Local Meshes BSAs]; [Bethesda Survival Mode](https://elderscrolls.bethesda.net/ko-KR/news/5lz4Q7F4li6kwKmakkgWww/skyrim-survival-mode-coming-soon) |
| **Bosmer** | small/lean mer silhouette; hunter/scout or archer only when requested; practical leather/hide, compact bow, wood/bone accents, subdued forest earth colors | No dedicated vanilla Bosmer armor set was found. Do not equate Bosmer with the generic `armor\elven` material tier, leafy high-fantasy druid armor, antlers, or glowing green nature magic. | No dedicated `armor\bosmer` family in local vanilla archive search; vanilla `meshes\weapons\wooden\woodenbow.nif` and shared hide/leather assets are safer neutral components. [Local Meshes0/1 BSAs] |
| **Orsimer / Orc** | heavy, angular, visibly forged Orcish plate; blunt/asymmetric massing; dark weathered metal with restrained earth/green cast only if reference-confirmed; broad shoulders; Orcish hand axe, sword, mace, or warhammer | Avoid Warcraft-style neon green skin, giant tusks, tribal loincloth shorthand, or pristine black plate. Stronghold warrior and smith are archetypes, not automatic for every Orc. | Dedicated vanilla sets: `meshes\armor\orcish\...`, `meshes\weapons\orcish\orcishhandaxe.nif`, `orcishsword.nif`, `orcishmace.nif`, `orcishwarhammer.nif`. [Local Meshes0/1 BSAs] |
| **Khajiit** | feline head/ears/muzzle and tail only if readable; digitigrade impression only when pose permits; choose explicit caravan scout, thief, mage, or armored warrior; shared Skyrim materials adapted around feline anatomy | Do not default to desert robes, jewelry overload, pirate clothing, bare chest, or “cat ninja.” Do not use a human helmet shape that erases the ears/muzzle. | Vanilla shared armor includes Khajiit-specific helmet variants such as `armor\hide\...\helmetlightkhajiit.nif`, `armor\steel\...\helmet_kha_0.nif`, and `armor\blades\bladeshelmetskhajiit_1.nif`. These prove anatomy adaptation, not one cultural uniform. [Local Meshes0 BSA] |
| **Argonian** | reptilian head profile, scales, tail when visible; choose dockworker, scout, mage, assassin, or armored warrior explicitly; shared materials adapted around snout/head crest | Avoid generic lizardman tribal regalia, feather headdresses, swamp shaman effects, or crocodile bulk unless requested. | Vanilla shared armor contains Argonian helmet variants such as `armor\hide\...\helmetlightargonian.nif`, `armor\steel\...\helmet_arg_0.nif`, and `armor\blades\bladeshelmetsargonian_1.nif`. [Local Meshes0 BSA] |
| **Imperial soldier / officer** | disciplined layered uniform/armor, skirted or segmented classical massing, rectangular/curved shield and straight sword where required; muted red cloth and weathered metal only when reference-confirmed; upright drilled posture | Avoid full Roman reenactment, gold imperial-palace fantasy, Spartan crests, or defaulting every Imperial character to Legion armor. | Dedicated vanilla Imperial armor assets are present under `meshes\armor\imperial\...` in the installed archive; use a local render/screenshot to lock the exact rank/set before prompting. [Local Meshes0 BSA] |
| **Altmer / Thalmor** | tall, narrow mer silhouette; for Thalmor, black/dark formal robes or elven armor with restrained gold, high collar/hood, controlled posture, precise magic | Do not equate all Altmer with Thalmor. Do not use generic white/gold angelic elf styling, huge pauldrons, or radiant holy magic. `armor\elven` is an equipment/material family, not a race costume by itself. | `meshes\clothes\thalmor\thalmorrobes...`, `thalmorhood...`; `meshes\armor\elven\...`; `meshes\weapons\elven\...`. [Local Meshes0/1 BSAs] |
| **Blades / Akaviri** | Skyrim Blades armor construction, one Akaviri single-edged sword/katana silhouette, practical dragon-hunter bearing; compact lamellar-like plated masses, guarded shoulders, flared helmet silhouette, subdued weathered metal and cloth verified from reference | Do not produce a generic anime samurai, ninja, ornate shogun, kimono portrait, dual katanas unless animation requires two, or unrelated Japanese architecture. “Akaviri-coded” means the specific Skyrim Blades equipment family. | `meshes\armor\blades\bladesarmor*.nif`, `bladeshelmet*.nif`, `bladesgauntlets*.nif`; weapon `meshes\weapons\akaviri\bladessword.nif`; shield `meshes\weapons\blades\bladesshield.nif`. Bethesda patch notes independently name “Blades Armor” as a shipped asset. [Local Meshes0/1 BSAs]; [Bethesda patch notes](https://bethesda.net/en-US/news/the-elder-scrolls-v-skyrim-special-edition-creations-update-patch-notes) |

## Canonical weapon silhouette vocabulary

Skyrim's archive vocabulary is useful because it prevents prompts from swapping weapon classes:

- **One-handed sword:** `longsword` / `steelsword`; straight, compact blade and clear crossguard. Do not say “greatsword” for a one-handed clip.
- **Greatsword:** `ironclaymore` / `steelgreatsword`; long two-handed grip and blade. “Claymore” appears as an internal asset term, but use “two-handed greatsword” in model-facing prose for clarity.
- **War axe / hand axe:** `waraxe` / `steelwaraxe` / `orcishhandaxe`; compact one-handed haft and one dominant cutting head. For Dual Flurry, say “two compact one-handed bearded war axes” if that is the chosen art direction; never let the model enlarge them into battleaxes.
- **Battleaxe:** `ironbattleaxe` / `steelbattleaxe`; long two-handed haft and broad head. Do not use for dual wield.
- **Mace:** `ironmace` / `steelmace`; short one-handed haft with a blunt flanged/spiked head.
- **Warhammer:** `warhammer` / `steelwarhammer`; long two-handed haft with heavy blunt head.
- **Dagger:** `irondagger` / `steeldagger`; very short blade, grip-led silhouette.
- **Bow:** `ironbow`, `steelbow`, `woodenbow`; show the curved stave/string relationship rather than a glowing crescent.
- **Akaviri/Blades sword:** `weapons\akaviri\bladessword.nif`; a specific single-edged katana-like silhouette. Use only for Blades/Akaviri direction.

Source: read-only local listings from `Skyrim - Meshes1.bsa`, especially `meshes\weapons\iron`, `...\steel`, `...\orcish`, `...\elven`, `...\akaviri`, and `...\wooden`.

At icon scale, weapon class must be legible from haft length, grip count, head/blade proportion, and hand placement. Material-tier ornament is secondary. Every trail must emerge from the currently moving edge or head; a detached X, ring, or streak is an emblem, not a performed attack.

## Magic and VFX visual language

Use the animation payload/spell evidence first. Do not add an element just to complete a color scheme.

| Effect family | Prompt-language default | Local shipped anchors |
|---|---|---|
| Fire | incandescent white/yellow core, orange-red flame body, ember breakup, smoke-dark edge; directional tongues follow motion | `meshes\magic\fxfirecloakhandeffects.nif`, `fxfirecloak01.nif`, `magfirebreathshout.nif`, `dragonflamewaveproject01.nif` |
| Frost | pale blue-white crystalline mass, ice shards, cold mist, opaque rime; heavier and more particulate than teal energy ribbons | `frostruneprojectile.nif`, `fxfrostballwispyprojectile.nif`, `icicleimpactfast01.nif`, `iceformchunk*.nif`, `voicefrostwaveproject*.nif` |
| Shock | branching blue-white electrical arcs, sharp flicker and sparse violet edge; connect source to target/weapon | `fxshockcloakhandeffects.nif`, `fxshockcloak01.nif` |
| Restoration / ward | warm white-gold or pale protective light, rounded shield/halo plane, soft rays; reserve religious/cross imagery unless the spell is explicitly divine | `maginvwardspellart.nif`, `wardbodyfx.nif`, `wardinhandfx.nif` |
| Illusion | Skyrim ships positive/negative and green/red variants; use translucent mind-distortion, wisps, refraction, or controlled colored haze according to the actual spell—not generic purple psychic fog | `illusionprojectile01.nif`, `illusionposfx*.nif`, `illusionnegfx*.nif`, `illusionmassgreen*`, `illusionmassred*` |
| Alteration | restrained physical-field distortion, pale/neutral protective shell, stone/metal/force cue according to the spell | `maginvalteration.nif`, `alterposimpact01.nif` |
| Conjuration / soul | summoned form, portal/spiral, spectral or soul-bound energy tied to the actual record; do not substitute necromancy skulls by default | `summoncommandhandfx01.nif`, `soultrapcastpointfx01.nif` |

The local file names establish effect families, not exact colors by themselves. Exact hue, blend, and particle shape should be verified from an in-game screenshot or rendered winning asset before being elevated to a hard rule.

## Common generic-fantasy drift to reject before generation

- Race substituted for archetype: Nord becomes plate-armored Viking; Breton becomes hooded rogue; Redguard becomes orientalist costume; Khajiit becomes cat ninja.
- Faction substituted with a neighboring real-world stereotype: Blades becomes anime samurai/ninja instead of Skyrim's specific armor and Akaviri sword.
- Material drift: polished chrome, clean esports gradients, neon trim, superhero armor, oversized ornamental pauldrons, or immaculate leather.
- Automatic complementary grading: orange subject on teal background regardless of effect evidence.
- Static emblem drift: crossed weapons or a detached X instead of a body actively producing two intersecting cuts.
- Key-art drift: detailed full-body portrait, face, costume showcase, scenery, or cinematic poster in place of an effect-first ability glyph.
- Element hallucination: adding fire/ice/lightning because of race, palette, or ability name when annotations/payloads do not support it.
- Scale failure: two-handed battleaxe used for dual wield, greatsword used for one-handed slash, or extra weapons added to "balance" the composition.
- Culture soup: mixing Blades, Thalmor, Dunmer, Nordic, and generic Eastern motifs because all are visually distinctive.

## Prompt-construction checklist

Before calling image generation, the prompter must be able to answer yes to all of these:

1. Does the brief name one exact frozen action frame supported by HKX annotations/root motion/payloads?
2. Are race, archetype/faction, armor family, and weapon class four separate decisions?
3. Is each armor or cultural cue traceable to a local asset, official screenshot, or explicitly labeled owner convention?
4. Does the body visibly perform the attack, with hands on the correct weapons and trails attached to the moving edges?
5. Does the effect follow evidence rather than palette habit?
6. Is the icon effect-first and readable at 32px, with the figure generalized and subordinate?
7. Does the composition deliberately differ from recent icons?
8. Has the candidate been self-rejected if action, costume, weapon count/class, trail geometry, or Skyrim identity is wrong?

## Uncertainty and limits

- Archive paths prove shipped asset families and naming, but a filename alone does not prove exact color, material finish, or silhouette details. Those details require a local render or in-game screenshot of the actual winning asset.
- The active Nolvus setup can override vanilla BSA assets with loose files. This report intentionally describes vanilla Skyrim visual language; it does not claim these vanilla files are the current MO2 visual winners.
- Breton knight-coding is a valid owner-approved project convention and has cross-series official support via Bethesda's description of the "iconic Breton Knight," but vanilla Skyrim does not ship a dedicated Breton armor family. Keep that distinction visible in the knowledge base.
- Redguard Elite Armaments is official Skyrim Anniversary Edition/Creation Club material, but it is additional content rather than the 2011 base-game visual baseline.
- Bosmer, Khajiit, and Argonian lack a single safe vanilla cultural costume shorthand. Future prompts must request an archetype or faction, not invent one from race.
- ESO is useful only when explicitly requested as broader Elder Scrolls context. Its cleaner, more saturated, and more ornamental visual language should not silently overwrite Skyrim's local asset evidence.
- This report does not grant permission to reproduce Bethesda art. It supplies vocabulary and reference targets for creating original icons.

## Recommended next evidence pass

For the highest-value recurring archetypes, capture or render a small, owner-only reference sheet from the local vanilla assets: Blades armor + Akaviri sword; hide heavy/chieftain + iron/steel war axes; Stormcloak set; Imperial set; Orcish set; Thalmor robes + elven weapons; Dragonborn bonemold/chitin/Morag Tong; and one representative effect for fire/frost/shock/ward. Each image should be labeled with a single allowed reference role and an explicit “do not copy pose/composition” instruction.
