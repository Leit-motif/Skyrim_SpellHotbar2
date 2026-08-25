# Elder Scrolls figure guidance

Owner-provided reference for choosing a race-coded silhouette. This is thematic shorthand, not a
requirement to show a specific race in every icon. Prefer a neutral, generalized figure when the
art's weapon compatibility or animation does not support a clear choice.

Current atlas balance direction (2026-08-24): prioritize Nord, Dunmer, Breton, and Redguard. The
run already has sufficient Khajiit and Argonian representation, so do not add another Argonian.
Deliberately vary female and male presentation instead of defaulting every action figure to male.
When Khajiit recur, vary visible fur color and markings rather than repeating one tawny pattern.

Race is not a costume. Resolve culture or faction, combat archetype, equipment family, physical
action, and effect separately through `skyrim-visual-language.md`. Several races have no dedicated
vanilla Skyrim cultural armor family; do not invent one from generic fantasy associations.

| Race | Typical fighting style | Favored weapons or tools |
| --- | --- | --- |
| Nord | Aggressive frontline warrior; hardy, direct, often two-handed | Greatswords, battleaxes, warhammers, swords and shields |
| Imperial | Disciplined soldier; flexible sword-and-board combat | Swords, shields, bows, heavy armor |
| Redguard | Fast, highly skilled melee duelist; stamina-heavy offense | Swords and scimitars, dual wielding, sword and shield |
| Breton | Battlemage or spellsword; defensive magic mixed with melee | Swords, maces, conjured weapons and magic |
| High Elf (Altmer) | Pure mage or magically enhanced combatant | Destruction magic, staffs, bound weapons |
| Dark Elf (Dunmer) | Versatile spellsword; mobile offensive magic and blades | Swords, daggers, destruction magic, bows |
| Wood Elf (Bosmer) | Hunter, scout, skirmisher; ranged and evasive | Bows, daggers, light weapons |
| Orc (Orsimer) | Brutal shock troop; closes distance and overwhelms | Warhammers, battleaxes, greatswords, heavy armor |
| Khajiit | Agile rogue, ambusher, thief, and martial artist | Claws, daggers, bows, dual wielding, martial arts, stealth |
| Argonian | Mobile guerrilla or survival fighter; stealth and attrition | Daggers, swords, bows, light armor |

Race is outline, not costume. The icon communicates an ability at hotbar scale. A figure is a
solid near-black mass that explains the verb; it is not a character, portrait, or armor plate.

## Rendering contract

This is the style master: `icons/aow_08_crane_style.png` and its prompt in
`prompts/aow_08_crane_style.md`. Aimed Blow, Akatosh Charge, Blood Flurry, Blood Seeker, Blood
Spiller, Champion's End, and Dragon Strike obey the same grammar. Later icons that still said
"faceless" while listing cuirass construction, filigree, fur, and visible grips are the failure
mode this contract exists to stop.

### Mass

The fighter is one solid near-black silhouette occupying 25–45% of the square. Crop to the action:
torso, weapon, and path. Extra anatomy (feet, trailing cape, idle off-hand) is discarded unless it
is the verb. The face is a void: hood, helm, veil, or shadow with no eyes, nose, mouth, hair, or
fur painting.

### Race cues

Encode race with two or three outline cues only.

| Race | Outline cues |
| --- | --- |
| Nord | Horned or nasal helm mass, broad shoulders |
| Imperial | Closed officer/legion helm, rectangular shield or short cape wedge |
| Redguard | Wrapped head/veil, scimitar curve |
| Breton | Closed bascinet or knight helm, compact mail mass |
| Altmer | Pointed-ear hood, tall narrow proportion |
| Dunmer | Close mask/hood, short-blade or chitin wedge |
| Bosmer | Small hooded hunter mass, light weapon |
| Orsimer | Heavy angular helm, brute shoulder mass |
| Khajiit | Ear points, short muzzle, tail completing the motion arc |
| Argonian | Snout, crest or tail, never a creature portrait |

A sash or wrap may exist as one color wedge. It is not embroidery, rivets, mail rings, tabard
heraldry, or a materials inventory.

### Hierarchy

The ability effect and weapon path own the square. The figure is a small anonymous verb behind
them. If a 32 px reduction names the race, sash, or hair before it names the kick, slash, slam,
thrust, or beam, the figure is too rendered.

### Physics without costume

Prove grip and force with pose: connected hand masses on a connected haft, a supported center of
gravity, and a trail that starts on the moving edge. Do not prove physics by painting gauntlets,
finger anatomy, or armor joints.

### Pasteable figure block

Include this block in every generation prompt. Fill the brackets. Do not replace it with a clothing
construction paragraph.

```text
FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, weapon, and
path. Face is a void. Race as outline only: [two or three cues]. No fur, hair, eyes, cloth folds,
rivets, filigree, or armor inventory. The figure is the small anonymous verb; the effect and
weapon path are the subject.
```

See `color-guidance.md` for the companion race palette reference. Race color belongs in the field,
the wake, and at most one large wedge, not in painted costume.
