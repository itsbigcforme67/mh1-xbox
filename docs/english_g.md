# English for Monster Hunter G: what official text covers, and what is left

Agent B, 20 Oct 2026 (second round: tutorial, dialogs, quest maps, talk checks; third round: hand-checked network
messages and menus, town talk). Input for the owner's decision on G's English (docs/DECISIONS.md: official sources first,
MH1 US now, Monster Hunter Freedom later, then our own translation in Freedom's style for the rest).

All numbers come from `python3 tools/text_map.py --report docs/text_coverage.md --ids docs/survey/mhg_text_untranslated.txt`
run on the owner's discs (disc/mh1, disc/mh1_us, disc/mhg, disc/mhp). No game text is in this file or in the repo;
the English tables it makes go to build/text/ (gitignored). Counts are of strings (one table entry, one quest field,
or one line of a description, which G stores one pointer per line).

## Short version

| | strings | share |
|---|---:|---:|
| G text found (main, overlays, quest files) | 13263 | |
| official English from MH1 US, before this work | 6363 | 47% |
| official English from MH1 US, first round | 6776 | 51% |
| official English from MH1 US, second round | 6839 | 51.6% |
| official English from MH1 US, now | 7064 | **53.3%** |
| left, but the same Japanese is in Monster Hunter Portable (so Freedom should have it) | 4730 | 36% |
| left, not in Portable either | 1469 | 11% |

**Update 23 Oct 2026 (agent A):** official English (MH1 US) 7087 (53.4%), **our own translation 1043 (7.9%)**, left
5133: 4703 wait for Freedom (same Japanese in Portable), 430 are not in Portable: 275 keyboard kana tables, 31 G-only
weapon/armour names left for Freedom's naming, about 124 that are not text (character tables, scan false hits, debug
prints). Every string the plan called "our own translation" (town talk, online-hub menus, memory-card / network
messages, weapon/armour lines) now has English. Section "Our own translation" below.

Of the 1469 "neither", 275 are the software keyboard's romaji-to-kana tables and about 100 the online chat's
bad-word filter (neither is text to translate), so about **1090 strings** would have to be written by us: mostly
town (online hub) NPC talk (~430), online-hub menus (~185), memory-card and system messages (~220) and weapon/armour
lines (~170).

421 of the 13263 are keyboard kana tables. Leaving them out, official English is 7060 of 12842 = 55%.

MH1 JP's own table (the same tool) went from 6272 to 6629 strings with US English (81%) over the three rounds.

**Update 24 Oct 2026 (agent A): Freedom is in.** With the owner's Monster Hunter Freedom (ULUS-10084, disc/mhfr/)
G has **11474 of 13263 strings (86.5%) in official English** (was 7087, 53%), plus 1071 of ours (8%): 12545 of 13263
(94.6%) have English. Per category, official English before -> after: items 560 -> 964 (99%), weapons 934 -> 1693
(94%), armour and skills 1063 -> 3545 (98%), monsters 56 -> 71, quests 1013 -> 1183 (99%), NPC talk and tutorials
1929 -> 2038, menus 1184 -> 1587 (88%), system 344 -> 387, kana tables 4 -> 6. Left: 718 (415 of them keyboard kana
tables, not text). How Freedom is paired and what still fails: section "Monster Hunter Freedom: what to give and what
happens" below.

**Update 28 Oct 2026 (agent A): Freedom first** (owner's decision, docs/DECISIONS.md). G's table now takes Monster
Hunter Freedom's line wherever a G string has a Freedom pair, MH1 US only where Freedom has none, our own lines last.
- **Moved:** 1012 G strings changed from MH1 US to Freedom wording; 3556 more had a Freedom pair that says the same as
  MH1 US. The table changed in 969 of its lines (some strings share a line id). Quest clear lines follow the same rule
  (they are quest strings; Freedom's are taken where its quest has the same Japanese).
- **Coverage:** 11770 of 13592 official (86.6%) + 1115 ours = 12885 (94.8%). It is 31 lower than on 27 Oct only
  because Freedom's placeholder "dummy" (the PSP left out G's network and some menu texts) no longer counts as English:
  those 31 G strings were showing "dummy" before and now show their Japanese (MH1 US has no line for them either).
  MH1 JP's table is byte-identical to before.
- **Kept MH1 US (72 Freedom lines, by rule):** more lines than the US line, or wider than the US line, G's Japanese
  (bytes) and the widest US line of the same table (a description is kept or dropped whole); talk is exempt (the PSP
  talk window has the same 22 columns: all but 2 of about 810 Freedom talk rows are <= 22 bytes; those 2 are
  word-wrapped, not cut).
  Also 10 Freedom lines that drop the US line's colour codes or a page counter ("View Quest Details  1/4").
- **Names:** Freedom's names are used in the lines that still come from MH1 US or from us. Only 2 names really differ
  where G's Japanese is the same as MH1's (Sleep / Poison Duration Halved -> "Sleep Dur Halved", "Poison Dur Halved",
  Freedom's short PSP forms); no other G line mentions them. 13 differences were MH1 US pairings in G tables that were
  wrong (G inserted items and armour, and the US name came by position): Freedom corrects them (Rathalos/Rathian
  Guards+ / Vambraces+ -> Vest+ / Mail+ for G's レジスト / メイル, Personal Hat -> Designer Glasses, Toolsaver ->
  Spirit's Whim, Gatherer -> Gathering, CHAT WINDOW SIZE -> DEFAULT SETTINGS ...). **Conflicts left as they are (4):**
  Worm / Bitterbug and Jewel Ticket / Commendation are swapped between MH1 US and Freedom in G's item table (G's にが虫
  = Bitterbug, 釣りミミズ = Worm, 勇気の証 = Commendation, 宝石券 = Jewel Ticket: Freedom is right there); other lines
  keep their own source's name, which is right for them, so no word is replaced.
- **Follow-up (same day):** our own lines for those 31 (text/mhg_en_own.tsv): the in-quest info lines (fish caught,
  items restocked, the name + " fell down!" / " caught!!" / " made!" endings G appends to a name [inferred from the Japanese, the calling code not read]), the controller
  message, the NET file / network settings menu, the character carry-over prompt, and five debug names in a monster
  table (test3, Gramps, Rock, Komos, Raptor: literal). Each is no wider than G's Japanese in bytes and has its line
  count. G: 11770 official + 1146 ours = 12916 of 13592 (95.0%). Not seen on screen: the tour does not reach these
  screens (fishing, the network settings menu, an unplugged pad).
- **Typical wording changes:** "Item will be exchanged. Is this OK?" -> "Replace Item. OK?"; "This item is for
  gunners." -> "Item for use by Gunners."; "Banquet Camera OFF" -> "Party Camera OFF"; "Sharpness Restoration +50%" ->
  "SharpnessRestore+50%"; "~C02Weapon cannot be removed." -> "~C02Cannot remove weapon."; "Save game data" -> "Save
  Game Data"; armour descriptions re-flowed to Freedom's lines (up to 28 bytes, as wide as MH1 US's widest line there).
  Freedom's PSP short forms ("SharpnessRestore", "Dur Halved", "TtlReward") come with it: the owner may want a list of
  them replaced by the MH1 US long form where the window allows.
- Freedom writes "Dual Swords" in 6 of its G-paired lines and "Dual Blades" in 5; G_TERMS still writes Dual Blades.
- **Checked on screen:** g_eng_tour.py (RT_TEXT_JP=1, hidden) before / after, all 16 scenes; item shop, elder talk,
  quest board, reward and save shown side by side: no overflow, the same windows; RT_TEXT_JP lists 3 more "Japanese"
  strings, which are Freedom's full-width spaces and ・ in English lines (they draw as blanks and a dot).

**Update 27 Oct 2026 (agent A): quest clear lines, last visible gaps. Now 12916 of 13592 strings (95.0%) have
English: 11801 official (86.8%) + 1115 ours (8.2%); leaving out the 421 keyboard kana tables (not text), 12910 of
13171 = 98.0%.** Measured the same way as before (`python3 tools/text_map.py --report docs/text_coverage.md --ids
docs/survey/mhg_text_untranslated.txt`, "total" row of the G table: official English + *ours*), but the count now
includes the quest clear lines, so the total grew from 13263 to 13592 (+329 strings) and the old 94.6% is not
directly comparable (on the old 13263 strings the same work gives 12587 = 94.9%).
- **Quest clear lines** (the banner when the quest goal is met, "All requested monsters slain." / "All requested
  items delivered.", and the "Monsters to defeat: N" banners during slay quests): Japanese on all G quests before.
  They sit at the end of the quest file; header word 0x30 points at a table of their offsets (same layout in MH1
  JP, MH1 US and G: `quest_clear` in tools/text_map.py). MH1 JP now takes them from the matched US quest file by
  position (MH1 quests 612 -> 868 strings, 507 -> 712 with English); G takes them by text from those MH1 lines
  (`quest_clear_pairs_g`: G dropped the final full stop and wrote "requested" for "target", so exact or near text with the same
  numbers): 326 of 329 G clear lines official MH1 US English, 3 ours (two "The hunt is over!" for the survival hunts,
  one "Monsters to defeat: 1"). PC side: `rt_text_quest_g` (src/pc/rt/rt_text.c) now also re-points the entries of
  that table; MH1's `rt_text_quest` already scanned it. Seen on screen (hidden run, quest 4, tick 8240,
  build/A/eng6/reward): "All requested monsters slain." in the banner, inside the window; the same frame with the old
  table showed the Japanese (build/A/eng7). tools/g_eng_tour.py's reward scene now takes that frame.
- **45 more own lines** for strings the game shows: skill names (Hunger ×1.5, Heat / Cold Inc ×1.5, Torso Up,
  Guard Up, ThunderRes, Health), Flaming S / Thunder S / Tranq S in the ammo lists and the bowgun load header,
  Combos 2-5 item help (these four were missing from the count: the scan splits them at the circled digits), a
  crystal and two armour descriptions, three Training School / elder lines about tranquilizers, the character-import
  yes/no rows, the Training School yes/no and "Area:", "Press the ~B00 button", the save-in-progress lines, two quest
  fields (10035 title "The Desert Plesioth", 60029 goal "Slay the Fatalis"), the options / gallery save questions.
- **How often a player sees what is left:** a hidden tour of every screen tools/g_eng_tour.py reaches (title, new
  game, options, gallery, network options, the four shops, forge, item box, elder talk, quest board, pause menu,
  data page, quest info, quest clear and reward) with `RT_TEXT_JP=1` (logs every string still drawn with two-byte
  characters) now finds no Japanese text at all: only full-width digits and symbols (★, ：, ×, －, "１／５"), which
  the PC font draws half-width in English lines. The 676 strings still without English (docs/survey/mhg_text_untranslated.txt,
  not "ours") are 415 keyboard kana tables, the online word filter, ~120 scan false hits and one-character code
  literals, 32 "dummy" talk slots, three debug prints, and full-width digit / kanji-number tables: none is
  text a player reads. What is not checked on screen: the online town and hub (no G online client in the tour), the
  Training School and the memory-card error paths; their own lines were checked by the fit rules only.

**The day Freedom arrives** (kept for reference; the first run gave only 201 strings, see the section below) (section "Monster Hunter Freedom: what to give and what happens" below): put an ISO of
the owner's own Freedom UMD in `disc/mhfr/` and run `python3 tools/text_map.py`. The step is written and tested
(self-test on Portable: 4624 G strings would receive a line), so G should go from 53% to about 88-90% official
English in that one run, if Freedom's DATA.BIN mirrors Portable's as expected.

## Our own translation (agent A, 23 Oct 2026)

What the owner's plan (docs/DECISIONS.md) calls "our own translation in Freedom's style": English written by us for
the G strings that no official release has, because they are neither on the MH1 US disc nor (as Japanese) in Monster
Hunter Portable, so Freedom will not have them either.

**File:** `text/mhg_en_own.tsv` (committed; the rest of text/ stays ignored because tools/text_dump.py puts game text
there). One line per string: `ID <TAB> CRC <TAB> FLAGS <TAB> ENGLISH`.
- `ID` is the tool's string id (`space:0xADDRESS`, `quest:N:0xOFFSET`), as in docs/survey/mhg_text_untranslated.txt.
- `CRC` is the crc32 of the Japanese bytes the line was written for. It stands in for the Japanese (which is never
  committed) and lets text_map.py refuse a line whose id no longer holds that text ("stale").
- `FLAGS`: `-` none; `t` a talk-window line (text_map.py word-wraps it at the window's 22 columns, then talk_layout pads
  the rows); `p` a tutorial page line (40 columns, as the US pages); `d` one line of a description (30 columns); `m` one
  line of a line-split dialog message (50 columns); `n` the line uses a G-only name that Freedom will decide
  (provisional, see below).
- `ENGLISH` uses the table escaping (`\n` line feed). No Japanese anywhere in the file.

**How it is applied** (`run_own` in tools/text_map.py): after MH1 US, the text rules and the Freedom step. Official
English always wins: a line whose id already has official English is not used, and when Freedom is added, every own
line it replaces is named in the log and in docs/text_coverage.md ("*Ours* ... own lines replaced"). A line is
refused when its printf conversions or `~Xnn` codes differ from the Japanese, or when it holds a stray `~` (the game
reads `~` as a code; two-byte characters like × whose second byte is 0x7E are fine). The report has a column *ours*
and the ids file marks own lines `ours`.
One special case: a string whose US English is only right in one slot of a re-flowed US message (`gk_slot`, see the
memory-card fix above) keeps that US line in its slot, and our line becomes the string's text everywhere else (15
such fallbacks, the MH1-to-G transfer and NET-file dialogs, where G reuses MH1's lines in new messages).

**Adding lines:** write a batch file `ID<TAB>FLAGS<TAB>ENGLISH` and run `python3 tools/text_own.py add BATCH` (it reads
the Japanese from disc/mhg only to compute the crc and to check the fit; `--dry` checks only). `tools/text_own.py check`
re-checks the whole file. The fit check: no more lines than the Japanese, each line no wider than the Japanese box
(two-byte character = 2 columns, ~Bnn icon = 2, other ~ codes 0; 15% extra for boxes of 8 columns or more, because the
English is drawn proportionally); talk lines no more rows than the Japanese takes in the window plus one, and at most 7.
Then re-run text_map.py.

**What was translated (1043 strings, 15 more as fallbacks):**

| batch | strings |
|---|---:|
| town (online hub) and village NPC talk, Felyne recipe hints, guild / shop talk | 428 |
| online-hub menus, help lines, search / Friend List / Black List / Short Mail screens and messages | 176 |
| NETWORK MODE, NET file, memory-card, MH1-to-G character transfer and network-settings messages | 277 |
| weapon and armour descriptions G added (one pointer per line), quest entry rules | 139 |
| Training School briefings, quest-board labels, G quest texts, items, three Felyne names, tutorial pages | rest |

**Style:** MH1 US's terms and names exactly as MH1 US writes them (Bishop / Queen / King Room, hunter's shoppe, Veggie
Elders, Farcaster, Rathalos Webbing, Lao-Shan Lung, Fatalis), MH1 US's button wording
("~B03 or ~B00: Select item"), MH1 US's NET-file and memory-card wording ("memory card (8MB) (for PlayStation(R)2)",
"MEMORY CARD slot"), the US quest form ("Requestor: ..."). NPC voices kept: the Felynes say "meow", the Melynx "gob",
the Red / Black Ogres laugh "Dohahaha" / "Bahahaha". Written by Claude (agent A), not a native translator: worth a
person's read before release, like the hand-checked pairs.

**Freedom's names (24 Oct 2026):** the provisional names below were checked against Freedom: 48 lines now use
Freedom's names (Firecell Stone, Pale Khezu Steak, Paw Pass Ticket, Lao-Shan's Ruby, Torso Increased, Gluttony,
Protection, Sharp Sword, Ice S, Snakebee Larva, Alchemy Guide, Organizer / Backpacker Guide, Khezu Hide - Tan, Expert
Blade/Hammer/Lance/Sword/Bowgun, Gradios, Djinn, Seiglinde+, Great Golem+, Teddybear, Velocidrome Bite, Freedom's
suffix order "Khezu Helm S"; Freedom's abbreviated item names written out, e.g. "Silver Rathalos Claw"). Names that
already matched lost the `n` flag; 24 lines keep it, now meaning "a G-only name no official release has" (Torso
Plus, Guard +1, Crag S, Demonize, Normal S Up, the online room names, ...). The 31 G-only weapon / armour names are
now ours too (flag `n`), in Freedom's style: Freedom writes 改 as "+" and a G version as "+" (Seiglinde+, Silver
Boots+); where Freedom already has "X+" for X改 (Iron Sword+, Longhorn+), the G version is "X G" and its 改 "X G+".
"Dual Blades" everywhere in G (owner, 25 Oct 2026): MH1 US wrote "Dual Swords", Freedom and later releases "Dual Blades";
the series standard wins. tools/text_map.py G_TERMS rewrites MH1 US-sourced lines when it writes en_mhg.txt; our own lines
say Dual Blades directly. Same length, so no box or wrapping changes. MH1's own English keeps the US disc's word.

**Waiting for Freedom** (as written before Freedom arrived; kept for the record):
- 31 G-only weapon / armour **names** left untranslated on purpose (G-rank "G" and "改" versions: Guild Rapier, Gladius,
  G Sword/Hammer/Lance/Knife Origin, Bone Sword G, Iron Sword G, Atlas Hammer G, Longhorn G, and six other new
  weapons, five armour pieces). Their ids are the `weapons` /
  `armour and skills` rows of docs/survey/mhg_text_untranslated.txt that are neither `ours` nor `in Portable`.
- 97 own lines carry flag `n`: they use a provisional English name for a G-only item, skill, weapon, armour piece or
  ammo (e.g. Carbalite Ore, Hellfire Stone, Razorwing, Carapace, Ruby, "S" armour, Sieglinde G, Honeycomber, the
  Master weapons, Sharpness +1, Razor Sharp, Rapid Fire, Guard +1, Alchemy, Glutton, Divine Protection, Torso Up,
  Storage Expert, Throwing Knife, Ice Crystal, Hot Pepper, Crag S / Clust S, Felyne Cart Ticket, Training School,
  Azure Rathalos, Pink Rathian). When Freedom is in, `awk -F'\t' '$3 ~ /n/' text/mhg_en_own.tsv` lists them; replace
  each provisional name with Freedom's (the line stays ours, only the name changes).
- Everything "in Portable" (4703 strings, including the gallery labels, NETWORK OPTIONS rows, the monster-record
  window, the combine window and the item shop's count label seen on screen) is left for Freedom, as planned.

**Display fixes in this round:**
- PC font (src/pc/rt/rt_font.c, both games): the game fills numbers into English lines as full-width digits (the
  item shop's "The price is $z." showed "2 0z"). In English text (a line with a half-width letter and no other
  two-byte character) full-width digits and letters are drawn as the half-width ones, proportionally. Japanese text
  and number-only prints keep the fixed grid. Seen fixed in G's item shop (build/A/eng/after/item_shop).
- Line-split messages that reuse an MH1 line in a new G message showed Japanese for the reused lines (their US line is
  kept to its own slot since agent B's memory-card fix): own fallbacks (above). Seen: the save message
  ("Saving... Do not turn off the console or / remove the memory card (8MB) (for PlayStation(R)2).") fits its box.
- Checked on screen (hidden runs, build/A/eng/before and after, tools/g_eng_tour.py with `G_ENG_OUT`): title, game
  menu, character select / save, item and weapon shop, forge, item box, elder talk, quest board and card, pause menu,
  quest info, quest clear and reward. The town talk and the online-hub screens are not reachable on the PC for G yet
  (no G online client in the tour), so the 600 own lines there were checked by the fit rules and the talk-row
  layout in the written table (all 356 talk lines: even-length rows of at most 22 bytes), not on screen.
- Still open (not changed: official English, owner decides): the quest-info values that overlap "Total Reward" /
  "Reward Remaining" / "Time Remaining", and the other short-form proposals in the table at the end of this file.

## What MH1 US covers (by category)

| category | G strings | official English | by the new text rules | left | left, in Portable | left, not in Portable |
|---|---:|---:|---:|---:|---:|---:|
| items | 972 | 560 (57%) | 0 | 412 | 408 | 4 |
| weapons | 1804 | 934 (51%) | 0 | 870 | 759 | 111 |
| armour and skills | 3615 | 1063 (29%) | 33 | 2552 | 2493 | 59 |
| monsters | 74 | 56 (75%) | 0 | 18 | 15 | 3 |
| quests | 1199 | 1012 (84%) | 365 | 187 | 170 | 17 |
| NPC dialogue and tutorials | 2498 | 1929 (77%) | 54 | 569 | 141 | 428 |
| menus and UI | 1803 | 1158 (64%) | 3 | 645 | 460 | 185 |
| system (memory card, online, keyboard, code literals) | 877 | 348 (39%) | 6 | 529 | 142 | 387 |
| keyboard kana tables (not text) | 421 | 4 | 0 | 417 | 142 | 275 |
| **total** | **13263** | **7064 (53%)** | **461** | **6199** | **4730** | **1469** |

How to read it: everything MH1 had and G kept word for word (or nearly) already has its US line: MH1's items,
weapons, armour, monsters, skills, its quests and village talk. What is left is what G added or rewrote.
Armour is the lowest because G rewrote almost every armour description (only 42 descriptions are still MH1's)
and added hundreds of new pieces; MH1 names that G kept are covered.

Categories are a grouping of the areas in docs/text_coverage.md. For G-only tables the area is taken from the
nearest pointer in the same table that leads to an MH1 string, so it is approximate (e.g. the training-school
entry rules are counted under "armour and skills" because they sit next to the skill names).

## What changed in tools/text_map.py

New step `g_rules` (after the structural pairing and the identical-text step). Each rule says exactly what may differ
between an MH1 JP text that has US English and the G text, and nothing else may differ:

| rule | what may differ | strings gained |
|---|---|---:|
| exact | nothing (blanks at the ends) - reaches the quest fields of G quests the quest tie did not find | 359 |
| layout | full-/half-width letters and digits, blanks, line breaks (the English keeps its own breaks) | 29 |
| marks | + kanji digits (三 = ３), punctuation and quote brackets (。、！？・…「」≪≫【】) | 4 |
| buttons | G wrote a pad symbol (○ □ △ ×) as an icon code ~Bnn; the US line's own icon code is re-pointed to G's code for the same symbol | 28 |
| numbers | G changed a number: replaced in the English when it is written there once as digits and neither the old nor the new number is 1 | 8 |
| whole description | a G description (one pointer per line) whose whole text matches an MH1 description gets the US lines in order, when the line count is the same | 42 |

Safety rules kept from before: the English must not still be Japanese, its printf conversions must match, and a
Japanese text with two different English texts is left out. New: an English text whose digits disagree with the
Japanese is not used (this caught a US mis-tie, 1 egg vs 3 eggs).

Two fixes to what was already there:
- **Description lines are no longer matched one by one.** The US re-wrapped every description, so US line 2 is
  not the translation of JP line 2. Before, a G line that happened to equal one JP line got that US line, which
  could be half of another sentence. Now descriptions are only matched whole (rule above). Net effect: 33 such
  line pairs dropped, 42 lines gained by whole matches.
- **Tutorial pages after the first weapon page were shifted.** The US build inserted pages (Dual Swords and
  others), so the Great Sword page had the Dual Swords text, the Lance page the Great Sword text, and four later
  tutorial pages the wrong page too (checked by eye on both discs). Fixed in the second round, see below.

### Second round (same day)

- **Tutorial pages paired by content** (`page_pairs`). A US page is a run of line pointers in the US tutorial
  region; a JP page is scored against each US page with the same number of lines on the order of colour, icon and
  number codes in each line and on how many of the page's MH1 terms (item, monster, weapon names with a US
  translation) have their English on the US page. Best score first, a near tie settled by page order. 22 of 23 JP
  pages paired, all checked by eye against the US text (the Great Sword page now has the Great Sword text, the
  Hammer page is paired for the first time; the US also swapped the egg and Velocidrome pages, which content
  pairing handles). The 7 tables are out of SHIFTED_US_TABLES again. One page (tuto08_msg02) has no US twin.
- **Yes/no and network-setting dialogs as whole messages** (`group_pairs`): the line-split tables of the yn overlay
  (yn_dialog, yn_mess, yn_help ...) that the US has at a located address. Labels (はい/いいえ) go to the US Yes/No,
  the other lines one to one or re-wrapped at the US width; the %s / ~A00 tokens and the numbers of the message must
  agree (this dropped a dialog where the US needs 239 KB and the JP 122 KB). 161 slots, about two thirds read by eye
  (where the US put lines in another order, the message reads right as a whole but a line can sit one row off).
  The network and error messages in main (net_con_msg, ms_err_str, netDialogMessage) are not at a located address in
  the US build and I found no safe way to tie them: a content search over all US messages paired wrong messages
  (tried, thrown away). They stay without English.
- **Quests by data and map, not number.** The quest's map is word 5 of the first header section (2 forest and hills,
  3 desert, 4 swamp, 5 volcano, 6 jungle; the same in JP, US and G). JP-US ties now need the same map (all 128 already
  had it: the US moved no quest; JP quest 19's text says forest but the quest is in the swamp in JP and US alike, and
  the US text says swamp). G-JP ties need the same map too, and a G quest field takes English from a JP quest on
  another map only when neither text names a place. G did move quests: e.g. G 10018 is the same quest text as JP 19
  but in the forest, so the US "Swamp" English is now kept away from it. 22 G quests are flagged this way (listed in
  docs/text_coverage.md, rule 3).
- **US number changes dropped.** In quest text and NPC talk the English must not carry a number the Japanese lacks:
  the US rebalanced some village quests (15 Velociprey became 20), counts 56 skills where MH1 JP has 53, and changed
  a ticket schedule. 14 pairs dropped (9 talk/quest-table lines, 5 quest fields).
- **NPC talk checked line by line** (automatically: for every talk table, the share of MH1 terms whose English is on
  the paired US line, at offsets -2..+2, plus numbers and Dual Swords mentions). No table is shifted as a whole; two
  tables have a block the US rewrote for its new weapon (npc_dialog_031 rows 6-10, npc_dialog_057 rows 17-22: the US
  lines talk about Dual Swords where the JP talks about other things). Those 7 lines are dropped
  (`us_rewritten_talk`). Lines with no names or numbers in them cannot be checked this way; the rest of the 2145
  talk lines showed nothing wrong.

### Third round: hand-checked pairs (tools/text_pairs_manual.txt)

Pairs I made by reading both discs side by side, written as addresses only. A pair went in only when the US line
says the same thing as the Japanese. The US calls the JP "server" a "land"; it pairs サーバー選択 with "Land
Selection" itself, so I followed that.
- **NET-file, network and error messages in main** (net_con_msg, ms_err_str): 81 messages, new kind `msg JP-table
  US-first-line`. The tool re-flows the US message over the JP lines (as for the memory-card dialogs) and checks
  the %s / %1 / ~A00 tokens and the numbers; 4 were refused that way (e.g. the JP names the slot twice). 262 line
  slots. Left out on purpose: phone-fee and DNAS-text messages (the US has none), messages where the US left out a
  line, error messages whose US version adds an ERROR CODE line.
- **Lobby network dialogs** (netDialogMessage, one string each): 46 of 63. Left out: short-mail and ID messages the
  US does not have, a line whose US version has a stray "C00~" typo, one whose US colour code differs.
- **Plaza menu help** (plaza_main_msg, position pairing shifted): 21 lines that say the same; the short-mail and
  "Capcom page" help are not in the US.
- **Title, game and gallery menus, network menus, memory-card slot names, lobby button help, one quest timer line,
  one town shop line**: 49 pairs.
- **Tool fix:** the meal-result tables (eat_result_lv3-5) were thrown out by the prose length check although their
  rows carry the same ingredient codes in both builds; a table whose small integer words agree (>= 90%) now skips
  that check (`ints_agree`).
- Checked and not on the US disc: the event-result lines that the US left in Japanese (fishing, item-received
  messages in info_str), the game overlay's elder lines, the content-warning lines (the US splits them differently).

### Town NPC talk and the MH1 US lobby

The US disc's lobby.bin does hold MH1's town (online hub) NPC talk in English: those are the npc_dialog tables the
tool already pairs (village/town talk is 97% English for MH1 JP), and every G town line whose Japanese equals an
MH1 JP line gets it (part of the 4094 identical-text pairs). Of the 569 G NPC/tutorial lines still without English,
I compared each with all MH1 JP text: 19 equal an MH1 line that has no US line either (labels like the HR range,
plus 3 elder lines in the game overlay that the US build lacks), about 70 are close edits of MH1 lines (numbers
changed: "the second time" became "the third time", "two kinds of tickets" became "three"; trailing "hic!" removed;
the Friend List lines reused for G's new Black List), and the rest (about 480) share nothing with MH1. The edits
cannot take the US line safely (the US writes those numbers as words), so the town talk really is G's own text:
Freedom has some of it (141), the rest is for our own translation.

### Ambiguous (a rule found English but it was left out): 48 strings (first round 53)

Listed with the reason in docs/survey/mhg_text_untranslated.txt (last column):
- 19 MH1 names plus a G-rank letter (G, S, 改 suffix, e.g. a barrel bomb "G"): how to write the suffix in English is a
  naming choice; Freedom's names decide it.
- 14 number changes the English cannot take safely: 10 where the number changes to or from 1 (singular/plural
  would change), 4 where the English writes the number as a word.
- 12 texts whose Japanese has two different US translations (labels such as "none" and the quest-info captions,
  which the US wrote differently on different screens).
- 3 tutorial texts where the US line names a button the Japanese does not.

## What Freedom is expected to cover

Monster Hunter Portable (PSP, JP, ULJM-05066, disc/mhp) is the PSP release of G and Monster Hunter Freedom
(ULUS-10084) is its US release. The report now reads Portable's text by structure (next section), not by a raw
byte scan: every text entry of DATA.BIN and BOOT.BIN, 10828 distinct Japanese texts. None of Portable's text is
compressed. The earlier raw scan had counted some false hits (single kana inside audio files matching G's
keyboard tables and one-character labels), so the estimate went down a little, not up:

4730 of the 6199 G strings still without English are in Portable:
- items: 408 of 412 (new items, G-rank materials, new item help)
- weapons: 759 of 870 (new weapons and their descriptions, dual swords)
- armour and skills: 2493 of 2552 (nearly all new armour and its descriptions)
- monsters: 15 of 18 (subspecies, new monsters)
- quests: 170 of 187 (new village and G-rank quests, training school)
- NPC dialogue and tutorials: 141 of 569 (village talk; the town talk mostly is not in Portable)
- menus and UI: 460 of 645
- system: 142 of 529 (only the messages that are not about the PS2's memory card or network)
- keyboard kana tables: 142 of 417 (single kana; not text that needs translating)

Without the kana tables that is 4588 real strings: G would go from 7060 of 12842 (55%) to about 11650 (91%), or
88% counting the kana tables, if Freedom's layout mirrors Portable's everywhere. The self-test below gives the same
figure from the actual pairing code (4624, a few line-count mismatches between G's and Portable's descriptions
fewer).

## Portable's DATA.BIN (PSP_GAME/USRDIR/DATA.BIN, 700 MB)

Checked on the owner's disc, 20 Oct 2026, with tools/text_map.py (PSPDisc, psp_slots, psp_text_entries).
- **File table** at offset 0: little-endian u32 sector numbers (2048-byte sectors). Entry i spans sector w[i] to
  w[i+1]; the list ends where the numbers stop rising, and the last number is the file's size (0x537D7 sectors =
  700,364,800 bytes). 5821 entries. The game opens it as `disc0:/PSP_GAME/USRDIR/DATA.BIN` and reads by sector
  (`disc0:/sce_lbn0x%x_size0x%x` in BOOT.BIN).
- **Compression:** none for text. I tried the MH LZSS (tools/mwo_unpack.py, the PS2 G overlay scheme) on every
  entry without a known header: no real hits. The code overlays (105 entries, 'MWo3', stage00.ovl ...) are
  uncompressed on the PSP. Entry 0 (sectors 12 to 0x2580C, 300 MB) has uniform high-entropy bytes (7.95 bits per byte
  from start to end) and no header: most likely UMD filler, possibly encrypted streams; not text either way
  [guess]. Other non-text entries: RIFF audio (120), PSMF movies (15), .TMH textures (1574), PNG, 9 encrypted
  modules ('~PSP').
- **Text entries** (entry numbers are Portable's):
  - entry 3 (0x12C40800): a string pack, u32 count (22), u32 8, then 22 block offsets; a block is u32 offsets to
    NUL-ended strings, relative to the block, count = first offset / 4, 0xFFFFFFFF = empty. Blocks in order:
    general and NPC messages (1174), item names (670) and descriptions (670), blade weapon names (440) and
    descriptions (440), bowgun names (97) and descriptions (97), head, body, arm, waist, leg armour names and
    descriptions (298/283/277/255/254 each), monster notes (32), save/load and system messages (44, 44, 69), and a
    681-string block of short labels. Descriptions are one string with line breaks (G has one pointer per line).
  - entries 4672 and 4673: string packs (tutorial and help titles, gallery unlock lines; 1089 UI words, the farm,
    the in-game magazine pages).
  - entries 4674-4913: the 240 quest files (header word 0 = 0x3C; text inline).
  - entries 4914-4922: record files with inline text (quest-like records, fixed-size rows).
  - entries 4958-4961: offset-list files of the village: guild counter talk, village NPC talk, farm and kitchen
    Felynes.
  - 7 code overlays with strings, and SYSDIR/BOOT.BIN (an unencrypted ELF, 121 strings; EBOOT.BIN is the
    encrypted copy).
- The ISO (`Monster Hunter Portable (Japan).iso`) and the extracted folder read the same (checked: same file table,
  same entries, same BOOT.BIN), so either form works; UMD_DATA.BIN holds the game id (ULJM-05066).

## Monster Hunter Freedom: what to give and what happens

**What the owner gives:** an ISO image of their own Monster Hunter Freedom UMD, US release, game id **ULUS-10084**
(dumped from their own disc with their own PSP, e.g. with a UMD dumping homebrew). Put it in **`disc/mhfr/`**
(any file name ending in `.iso`; an extracted copy with `disc/mhfr/PSP_GAME/` works too). `disc/` is gitignored,
so nothing of it can be committed. Then run:

    python3 tools/text_map.py --report docs/text_coverage.md --ids docs/survey/mhg_text_untranslated.txt

(or `--freedom PATH` to point elsewhere; an explicit path that does not exist stops with an error saying what is
needed; without Freedom the step logs "Freedom: not found ... skipped" and the rest runs as before).

**What the step does** (`run_freedom`, `freedom_pairs`, `freedom_for_g`):
1. Reads Portable's and Freedom's DATA.BIN file tables and the same entry numbers on both sides.
2. Pairs each text entry slot by slot: string packs and blocks by (block, index), the record and offset-list files
   by the order of their text runs (only when both have the same number of runs). An entry is used only if it has
   the same kind on both discs and at least 80% of its pairs look like translations (English, same printf
   conversions); otherwise it is skipped whole and named in the log.
3. Gives English to a G string that still has none (MH1 US English always comes first) when its Japanese equals a
   Portable string (normal form of the *marks* rule) and every Portable slot with that text has the same Freedom
   line. A G description (one pointer per line) takes Freedom's description split at its line breaks when the line
   counts agree.

**What the owner's Freedom turned out to be (24 Oct 2026, agent A).** The first run paired 36 of 784 entries and gave
201 G strings, because Freedom does NOT mirror Portable entry by entry:
- DATA.BIN has 5829 entries (Portable 5821); 4887 have the same size. The 240 quest files are in another order (and
  6 more), the record files after them are shifted by 7, a few entries are added near the end.
- Quest files: word 0 = 0x3C, quest id at 0x58, text fields from 0x5C to word 1, each NUL-ended and padded to 4 bytes:
  16 fields in both versions; 239 of 240 Portable quests are in Freedom under the same id (the odd one has a different
  id; not matched).
- Record files (training and arena quest records): u32 offset table (first offset = table size, 0 = end); each record is
  a quest file without its first 0x3C bytes (id at +0x1C, text fields from +0x20 to the offset in word 6). Freedom has
  more records in two of them; matched by record id.
- Village talk files: (section id, offset) pairs up to two 0xFFFFFFFF words, each section a table of (u32, offset)
  pairs relative to the section. Same sections and counts in both.
- The big string pack (entry 3: 22 blocks of names / descriptions / messages): the same in both except one more string
  at the end of block 18. Blocks 19 and 20 (save / load and system messages, 111 strings) are still Japanese in
  Freedom (the US build must draw those from somewhere else), so Freedom gives nothing for them.
- Code overlays (MWo3) and BOOT.BIN: their Japanese strings are still Japanese in Freedom (the US text comes from the
  string packs), so nothing to pair there.
- The record and talk files' raw text runs differed in number because English lines are longer than the Japanese and
  the binary around them yields other runs: the old pairing "by text run order" could not work there.

**How it pairs now** (`psp_entry_map`, `psp_slots`, `freedom_pairs`, `freedom_ok`): entries are matched by their first
8 bytes in order (difflib), then checked by a skeleton (quest id, first record id, talk section ids); a mismatch is
looked up by skeleton among Freedom's entries nearby. Then: packs by (block, index), quests by field, records by
(record id, field), talk by (section, index), anything else by aligning the runs that are the same bytes on both discs.
A unit (one block, one talk section, one file) is used when 80% of its Japanese pairs look translated and when MH1 US
English for the same Japanese (anchors: from G strings that already have US English) shares a word with Freedom's in
place at least as often as one slot off (entry 3: in place 312/290/287/253... of the anchors per block, one slot off
far fewer). A unit whose string count differs needs 5 anchors and 60% agreement. Then each G line passes `freedom_ok`:
English, same printf conversions, the same ~B / icon codes as G's Japanese (Freedom names the PSP's buttons: 6 lines
refused), colour codes only of G's colours (Freedom highlights other words; the count may differ), no stray '~', no
number the Japanese lacks. A sentence (8+ characters) that Freedom wrote two ways (the same talk for two NPCs) takes
the more used wording (26); a short label with two wordings is left out (54). A G description (one pointer per line)
takes Freedom's whole description split at its line breaks (2628 lines), also where some lines had MH1 US English
line by line (192 lines replaced, so no description is half one translation and half Japanese).

**What is still in Portable but not given (319, by the report):** 140 keyboard kana; 98 system: the save / load
messages Freedom keeps in Japanese and code-overlay literals; 32 NPC lines and 32 menu labels: short labels with two
Freedom wordings or found only in data / overlay files; 11 armour, 4 items, 2 quests (numbers or ~ codes refused).

**Fit on the PS2 screen:** G's descriptions break at the same places as Portable's, so Freedom's lines keep G's line
count; 98% of the description lines are 31 columns or less (MH1 US's run to 28; the description window is as wide as
its longest line). Names are Freedom's 16-character forms (abbreviations such as "Slvr Rthalos Clw" are Freedom's
own). Checked on screen: section "In game" (Freedom round) below.

**On screen (hidden PC G tour, 24 Oct 2026, build/A/eng/freedom, compared with build/A/eng/after):** item shop,
weapon / armour shop, forge, item box, quest board and card, pause menu, equipment detail, data page, quest info:
the pictures are the same as before Freedom (only animation timing differs), because the tour's screens show the
starting village's MH1-era items, weapons and quests, whose English already came from MH1 US. The new Freedom lines
(G's new items, weapons, armour, G-rank quests) are not on those screens yet; they were checked by width instead
(above). Still Japanese on the tour's screens, not Freedom's to fix: the item shop's count label (a G code literal,
not the paired "Amount" string), the talk box title in the quest board scene (drawn from another copy than the
paired string), and the element label on the equipment page.
The self-test (Portable paired with itself, the ceiling of this method) gives 4757; the real Freedom gives 4453
(including the 192 description lines that replace MH1 US lines), 94% of it.

**Tested without Freedom:** `--freedom-selftest` pairs Portable with itself: 784 of 784 text entries paired, 18677
strings, 4624 G strings would receive a line (2625 description lines, the rest single strings). Pointing
`--freedom` at the Portable ISO (Japanese on both sides) correctly uses nothing: every entry fails the "looks
translated" check.

**Assumptions to check on the day** (the log shows each one):
- Freedom's DATA.BIN has the same entry numbering as Portable's. If the US build added or removed entries, the
  paired-entry count in the log will be low; the fix is to map entries by content (names in the first strings)
  instead of by number.
- Freedom's string packs keep Portable's block and string counts (a localisation usually does).
- Freedom's BOOT.BIN is unencrypted like Portable's (if not, its ~120 strings are skipped).
- Only ULUS-10084 is expected; another id is tried but logged.

## What neither covers (truly G-only text): about 1470 strings

| kind | strings | notes |
|---|---:|---|
| town (online hub) NPC talk, G rewrites of MH1 talk, a few tutorial lines | 428 | 318 new, 95 rewritten from MH1, 11 MH1 lines with no US line, 4 tutorial |
| online-hub menus, shop / guild screens, other G tables | 185 | room, ticket, black-list and gathering-hall screens |
| memory card and yes/no dialogs | 120 | PS2 memory-card wording (Portable has Memory Stick wording instead); 68 are MH1 lines with no US message |
| PS2 online messages | 134 | 99 are the chat's bad-word filter (not shown, not translated) |
| weapon / armour lines and quest entry rules not found in Portable | 170 | partly PS2-only collaboration items, partly a matching gap (some lines may still be in Portable) |
| code literals (HUD banners, debug, results) | 133 | 64 more than before: they had matched false hits of the old raw Portable scan |
| items, monsters, quests | 24 | |
| keyboard kana tables | 275 | not text: needs no translation |

This is what "our own translation in Freedom's style" would have to write once Freedom is in: about 1090 strings
(without the kana tables and the word filter).

## Known limits (worth a person's look later)

- NPC talk lines without names or numbers can only be checked by a person reading both texts (the automatic check
  needs something to compare). The two US rewrites it found were both about Dual Swords.
- The hand-checked pairs were read by me (agent B), not by a native speaker; they are conservative (left out when
  in doubt) but a second reader would be good before release.
- The US text of JP quest 19/69 says swamp while the JP says forest; both are swamp quests, so for MH1 the English
  is right and the JP text was the odd one.
- disc/mhf is Monster Hunter Frontier (PC), not Freedom; Freedom (PSP US) is not on the machine yet.

## In game (agent B, 20 Oct 2026)

How it was looked at: `python3 tools/g_eng_tour.py` runs the G PC build hidden with `RT_TEXT_TABLE=build/text/en_mhg.txt`
(1.5 GB cap, about 1 minute for all 14 scenes) and writes pictures and an HTML contact sheet per screen to
`build/B/eng/<scene>.html` (index: `build/B/eng/index.html`; not committed). Scenes: newgame (title, game menu,
character creation, card dialogs), net_options, gallery, options, network (continue / card dialogs), item_shop,
weapon_shop, armour_shop, forge, item_box, elder_talk, pause (village pause menu, status, equipment), quest_info (pause
menu in a quest), reward (quest 4 cleared with the test aids: banner, quest score, monster record). With
`G_ENG_TABLE=/dev/null G_ENG_SUFFIX=_jp` the same scene runs without the table, to tell our text from the port's own
drawing. The binary was the main checkout's current `build/mhg/pc/mhview_g` (copied, not rebuilt). In the menus, n
presses of ddown in a script move n+1 rows (noted in the tool).

### Found, ranked (worst first)

1. **NPC talk rows overlapped: fixed.** Every talk line whose English had a line of odd length was drawn on top of the
   next one ("bu" + "t here's a little" on one row). Cause: the talk window (chat_nm.c Disp_NPC_message, the same code
   in MH1 and G) cuts the typed text into rows of 11 two-byte characters and only sees a line feed where a two-byte
   character would start. The US build drew 22 one-byte columns instead. Fix in the text pipeline
   (`talk_layout` in tools/text_map.py, applied when the tables are written, to the strings of the talk tables:
   npc_*, talk_*, tl_* and G's lines next to them): lines longer than 22 are cut at 22 as the US window did, each line
   is padded to an even length, and a full 22-byte line ends without a line feed. 1722 G lines and 2089 MH1 lines.
   Checked in G (elder_talk): rows clean, the long story pages fit (7 rows). MH1 uses the same window and the same
   table rule; not run in MH1.
2. **A memory-card line in the wrong place: fixed.** "inserted in MEMORY CARD slots 1 and 2." stood alone in a one-line
   dialog. Lines of the re-flowed memory-card messages had been given to their Japanese string everywhere; the string is
   also used alone. They now go only to their own message slot (the `0xTABLE[i]` ids), not to the string or to
   same-text copies. Cost: 7 G strings, 4 MH1 strings (fragments that were only right in one place).
3. **Japanese next to English on the same screen** (no English on the US disc; Freedom or our own translation):
   the talk window title 会話イベント (MH1 JP has it, the US build has no line for it), the NETWORK OPTIONS page (all
   rows and help), the gallery (G's movie name, the L1 / play labels), the character-save and welcome dialogs, the
   combine window's 素材A label, the element label "Dragon属性" in the equipment details (English element name +
   Japanese suffix), G-only item names in lists (調合書①入門編), the monster record window after a quest (G only).
4. **Labels G drew with icons: fixed for 5 rows.** G wrote MH1's "　選択　　　キャンセル" rows with ~Bnn icons in front
   of each word; a new text rule (*button labels*) gives them the US words with G's icons ("○Enter Data ✕Cancel" in
   character creation). 11 other icon lines have no US line ("~B00ボタンを押して下さい" ...).
5. **English that does not fit G's layout** (text content, not the pipeline): the side tabs of the forge and the
   shops ("Improve Wpn", "Weapons") run past the right edge of the screen; the armour stats "Thunder Res:" /
   "Dragon Res" touch their values; in the game menu help "○ Confirm ✕ Cancel" the ✕ icon (at a fixed place)
   covers the end of "Confirm"; 22-character talk rows touch the window's right border. Shorter wording (Freedom's,
   or ours) is the fix.
6. **The table has English, the screen showed Japanese: fixed (both games).** The game prints many strings as an
   argument: `font_print("%s", str)`, the frame titles of DispFrameListA (`font_print_sp(fmt, title)`), the info
   banners of set01.c. The text layer only looked up the format. Now `rt_text_vsnprintf` (src/pc/rt/rt_text.c) looks up
   every `%s` argument too; it is used by flfntPrintf, font_print*, font_print_sp and the game's sprintf. With no table
   it is plain vsnprintf. The info banners also size their frame with `strlen` of the shown string: set01.c is built
   with `-Dstrlen=rt_text_strlen` (tools/build_pc.sh), which measures the English. Seen in G: "Equip Details" and
   "View Quest Details" window titles, "Returning to village in 1 min." in a frame that fits.
7. **The item shop's count line "所持数×~C00C02( 0)": fixed (PC port, both games, with or without English).** G prints
   "~C02%2d~C00(%3d)" with a plain print; the PC's glyph loop drew the colour codes as letters. font_puts
   (src/pc/rt/rt_font.c) now reads ~Cnn as a palette change. Proportional English spacing is now used only for text
   with a letter in it, so numbers the game draws over each other keep the fixed grid (the red "   ~C02%3d" over the
   box count lines up again).
8. **Font:** nothing wrong seen in the narrow ASCII set (proportional widths, quotes and apostrophes draw); no
   character fell back to a missing glyph in these screens.

Added to the tour (second pass): **quest_board** (the elder's quest levels, list and quest card after his first talk)
and **quest_info** now reaches the four quest-info pages in a quest. Found there: the quest card's labels were
Japanese (報酬金, 契約金 ...): the US has two spellings of each label (the village "Reward:  " and the board's
"Reward："), so the text rules left them out; a G string now takes the version from its own overlay when that leaves one
(*same overlay*, 5 labels). Third pass (21 Oct): G split MH1's "クエスト詳細　　　　／３" into a label and separate
page counters ("１／３" stays as digits); the label "Quest Info", "Quest Level ★" and the special-conditions "なし"
("None") are hand pairs now. MH1 in English is in docs/english.md section 8. The quest-info pages: "Total Reward", "Reward Remaining", "Time Remaining" run into their values
(short-form table below).
**The town** (network mode) is not reachable on the PC for G: the PC's online town has been built and tested for MH1 only
(ONLINE=1, docs/network.md); a G online client is not set up, so the town NPC and plaza screens were not looked at.

### Short-form wording (ours, not official)

English that does not fit G's layout. These are **our proposals in US style**, not text from any official release;
Freedom's wording, once paired, takes precedence where it exists. Not applied to the tables (the owner decides).

| where (G id) | US / table text | problem seen | proposed short form |
|---|---|---|---|
| quest-info page 1 (main:0x5388F8) | "Total Reward:" | runs into the value at a fixed column | "Reward:" |
| quest-info page 1 (main:0x538908) | "Reward Remaining:" | runs into the value | "Left:" |
| quest-info page 1 (main:0x538918) | "Time Remaining:" | runs into the value | "Time Left:" |
| forge tab (lobby:0x63F210) | "Improve Wpn" | past the right edge of the screen | "Upgrade" |
| shop / forge side tabs | "Weapons", "Create Wpn" | at the right edge | "Wpns", "Make" |
| armour stats (main:0x538E50 / 0x538E70 / 0x539180) | "Thunder Res:", "Dragon Res:" | touch their values | "Thunder:", "Dragon:" (and "Fire:", "Water:" to match) |
| game menu help (main:0x53A900) | "  Confirm    Cancel" | the ✕ icon (fixed place) covers "Confirm" | "  OK         Cancel" (Cancel at the same column) |
| quest card (lobby:0x640138) | "Special Conditions：" | longest label on the card, close to the edge | "Conditions：" |

