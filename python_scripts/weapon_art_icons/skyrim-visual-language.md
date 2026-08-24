# Skyrim visual language for Weapon Art icons

This is the canonical visual vocabulary for translating Skyrim-facing art direction into original
Weapon Art hotbar icons. It is not permission to copy Bethesda artwork, meshes, textures, symbols,
or third-party assets. Named game material is research context; shipped glyphs remain original.

## The Skyrim filter

Skyrim identity comes from **construction, material, wear, and culture**, not from adding a dragon
emblem or generic Nordic ornament. Prefer:

- practical layers that look built from iron, steel, leather, hide, fur, wood, bone, chitin,
  bonemold, cloth, or carved stone;
- weight, weathering, soot, scratches, uneven edges, and restrained decoration;
- silhouettes that reveal a culture or combat role before surface detail;
- one concentrated supernatural color against grounded materials;
- equipment proportions that remain plausible for a physical fighter.

Avoid unrequested glossy high-fantasy plate, pristine esports gradients, superhero anatomy, anime
weapons, ornamental halos, faction logos, and costume-detail showcases. An icon may be vivid, but
its figure should still feel built from Skyrim's world.

## Resolve race into archetype

A race name is not a costume. Resolve **race/anatomy**, **culture or faction**, **archetype**,
**equipment family**, **action frame**, and **effect** independently before prompting. When the
owner names an archetype, its clothing and body language are hard constraints. The locally shipped
archives contain dedicated Blades, Stormcloak, Imperial, Orcish, Thalmor, bonemold, chitin, and
Morag Tong families, but no equivalent vanilla `armor\breton`, `armor\bosmer`, `armor\khajiit`, or
`armor\argonian` cultural family. Shared race-shaped helmet variants establish anatomical fit, not
a racial uniform.

| Identity | Positive visual vocabulary | Common drift to reject |
| --- | --- | --- |
| **Nord barbarian** | airborne or driving motion; powerful build; rough layered animal fur and hide; coarse leather straps and wraps; exposed or lightly covered arms; compact iron or steel hand axes; cold-weather wear without polished plate | full plate champion; Stormcloak uniform; symmetrical knight armor; static horned-helmet emblem; Viking mascot; automatic frost magic |
| **Nord warrior or housecarl** | mail, scale, iron or steel plates over cloth and fur; round shield or practical heavy weapon; planted direct posture | barbarian nudity when disciplined armor is requested; generic medieval crusader |
| **Imperial soldier or veteran** | disciplined posture; ordered plate or segmented construction; crimson cloth; blackened steel, bronze, ivory, and restrained gold; sword, shield, spear, or bow | victory wreaths and Roman costume pageantry; pristine gold hero armor |
| **Breton knight — owner project convention** | French-medieval coding; closed bascinet or restrained great helm; mail at joints and neck; simple plate; blue and burgundy surcoat; sword or mace; courtly or spellsword accents | dedicated vanilla Skyrim Breton armor claim; hooded rogue; generic wizard robe; ornate tournament heraldry |
| **Breton spellsword** | practical one-handed blade plus disciplined conjuration or warding; royal blue, burgundy, charcoal, silver, muted gold; compact mail or plate rather than robes | pure mage; thief silhouette; uncontrolled spectral clutter |
| **Redguard duelist** | athletic sword work; mobile layered cloth or leather; deep red, sand, warm brown, restrained gold or turquoise only when chosen; curved or Yokudan sword when culturally requested; sash used to reinforce motion | automatic fire magic; generic Arabian costume; anonymous ninja; pirate caricature; jewel-heavy showcase |
| **Dunmer spellsword** | lean mobile silhouette; choose an explicit bonemold soldier, chitin scout, Morag Tong assassin, ashlander, cultist, or Telvanni mage identity when culture matters; straight sword, dagger, bow, or evidence-backed destruction magic | automatic fire from racial association; generic dark elf in black plate; oversized pointed ears as the whole identity |
| **Bosmer hunter or skirmisher — owner project shorthand** | small lean mer silhouette; practical shared leather or hide; compact bow or daggers; restrained wood or bone accents; subdued forest-earth palette; evasive and acrobatic motion | dedicated vanilla Bosmer armor claim; generic Elven-tier armor; leafy druid costume; antlers; automatic green nature magic |
| **Orsimer shock trooper** | broad forceful build; heavy angular iron or steel layers; hard cheek and shoulder geometry; dark green, black, rust, blood red; warhammer, battleaxe, or greatsword | smooth black fantasy plate; demon horns; slow static armor showcase |
| **Altmer battlemage** | tall economical silhouette; controlled magical gesture; elegant gold, ivory, pale yellow, deep green or emerald; staff, bound weapon, or destruction magic | baroque gold ornament; generic angelic paladin |
| **Khajiit martial artist or rogue** | feline head, ears, muzzle, and tail only when readable; explicit caravan scout, thief, mage, martial artist, or armored-warrior archetype; shared Skyrim materials adapted to anatomy | invented racial uniform; desert jewelry overload; cat ninja; detailed fur portrait; mascot expression |
| **Argonian guerrilla** | reptilian head profile, scales, and tail only when readable; explicit scout, dockworker, mage, assassin, or armored-warrior archetype; shared Skyrim materials adapted to the snout and crest | invented racial uniform; generic lizard tribal regalia; feather headdress; swamp magic without evidence |
| **Blades warrior** | Skyrim's specific Akaviri-influenced equipment family; compact lamellar-like plated masses; guarded shoulders; flared protected helmet silhouette; one katana-like Blades sword unless the animation requires two; subdued weathered metal and cloth verified from reference | anime samurai; ninja cloth; ornate shogun; kimono portrait; western knight slash; copied Blades emblem or exact armor asset |

Use only the cues needed at hotbar scale. If the ability does not support a specific identity, use a
neutral faceless Skyrim fighter rather than forcing a race or faction.

## Weapon silhouette vocabulary

Name the weapon by construction and motion, not only by category.

- **Iron or steel war axe:** compact one-handed wooden haft, practical bearded iron or steel head,
  broad cutting edge; visibly swingable, not a double-headed fantasy badge. Use the distinct
  `nordicwaraxe` family only when Nordic Carved equipment is explicitly selected.
- **Battleaxe:** long two-handed haft and one dominant heavy head; the body must counterbalance its
  weight.
- **Warhammer:** long haft, compact crushing face, strong vertical or driving line; avoid a giant
  decorative block that obscures the action.
- **Greatsword:** two-handed straight blade with readable weight and reach; avoid anime scale.
- **Imperial sword:** practical straight one-handed blade with disciplined guard and compact
  proportions.
- **Breton sword:** practical medieval arming-sword or longsword language; straight crossguard,
  restrained decoration.
- **Redguard scimitar:** single curved cutting blade with warm-metal or cloth accents; preserve the
  hand path that creates its arc.
- **Blades sword:** the specific `weapons\akaviri\bladessword.nif` family; prompt as a long,
  narrow, gently curved single-edged blade with compact guard and wrapped grip only after a render or
  game capture verifies the required detail. Use only with Blades or explicit Akaviri direction.
- **Dagger:** short close-range blade whose hand remains near the hit; do not enlarge it into a
  sword for readability.
- **Spear or pike:** one continuous long thrust axis with the point leading; the body stays behind
  the line rather than presenting the shaft as a prop.

For dual wielding, specify which hand owns each path. At least one visible weapon endpoint must make
the effect causality legible at 32 px.

## Action and VFX causality

Freeze one instant in which the action can be reconstructed from the silhouette:

- **leap:** both feet off the ground, knees and trailing cloth or fur support the travel direction;
- **rush:** torso and rear leg drive behind the weapon, with wake behind rather than around;
- **cross-cut:** two weapon paths cross once because the arms and shoulders carry them through that
  point; an X floating behind a guard pose is an emblem, not an attack;
- **spin:** hips, shoulders, trailing cloth, and weapon tangent share one rotation plane;
- **slam:** weapon mass, torso compression, impact point, and debris align on one force line;
- **retreat:** body and effect move away from the threat; do not turn the trail into an attack.

Every major trail has a physical source. Put the weapon head, blade, fist, foot, or projectile at
the leading or terminal point of its trail. Repeated animation hits may be distilled into fewer
visible effects when literal counting would damage 32 px readability, but the prompt must record
that decision.

## Supernatural palette vocabulary

These are starting associations, not automatic race palettes. The animation payload or spell record
selects an element; race, complementary color theory, and ability-name mood do not:

- fire: white-yellow core, red-orange body, ember-red wake;
- frost: white core, ice-blue or steel-blue edge, crystalline fragments used sparingly;
- shock: white-violet core, blue-violet branching energy;
- restoration or divine force: ivory-white core, restrained gold, pale blue only when it serves a
  separate enchantment;
- blood or physical ferocity: crimson, claret, dark red, small white-red impact; avoid realistic
  gore unless explicitly required;
- shadow or darkness: aubergine, charcoal, violet, narrow magenta focal light;
- poison or corrosive force: yellow-green or sickly green against dark neutral material;
- nonmagical force: steel-white edge, dust, debris, sparks, compression ring, or motion blur rather
  than unexplained elemental glow.

The animation and owner direction choose the effect. Race does not automatically choose magic.
Orange/teal grading is never a default and must not be added merely for cinematic contrast.

## Reference protocol

Use original approved icons for **icon grammar** only unless another role is explicitly assigned.
For Skyrim research images, assign narrow roles such as `fur construction`, `Blades helmet`,
`scimitar silhouette`, or `weathered material finish`. A reference's pose, camera, character,
palette, and composition are excluded unless listed as roles.

Do not use a rejected generation as a reference after the owner changes subject, action, clothing,
weapon, or composition. Start from the canonical brief. Use a rejected image only for a precise
edit when the owner explicitly identifies what must remain.

## Prompt drift checks

Reject the prompt before generation if any answer is unclear:

- What exact instant is frozen?
- What is the figure's center of gravity and travel direction?
- Which Skyrim archetype is visible through construction and silhouette?
- What does each hand hold, and where does each weapon finish?
- What physically creates every dominant trail?
- What is the one primary read at 32 px?
- Which recent icon axis, palette, or silhouette is intentionally not being repeated?

## Evidence basis

Bethesda describes Skyrim concept art as the means of capturing the game's overall look and feel
before production, with gameplay and storytelling built into images; it also distinguishes the
flavor of a concept from the literal implementation. This project follows that same separation:
Skyrim sources establish vocabulary and sensibility, while shipped icons remain new compositions.
See [Bethesda, “From Concept to Character: Creating Skyrim's Artwork”](https://elderscrolls.bethesda.net/en-AU/news/4w9Z3Td0tdhLjuSPSZN2ry/concept-to-character-creating-skyrims-artwork).

Detailed item, armor, faction, and race evidence used to maintain this vocabulary is recorded in
`.scratch/weapon-arts/research/skyrim-visual-language-research.md`. Its read-only archive evidence
comes from `Skyrim - Meshes0.bsa` and `Skyrim - Meshes1.bsa` in the named Nolvus Awakening stock
game. Archive names establish shipped families, but exact hues, finishes, and silhouette details
still require a local render or in-game capture. Active Nolvus loose-file winners may also differ
from vanilla assets. Owner corrections and approved icon prompts remain the authority for this
project's specific race and MMO-icon shorthand.
