# English for Monster Hunter G: what official text covers, and what is left

Agent B, 20 Oct 2026 (second round the same day: tutorial, dialogs, quest maps, talk checks). Input for the owner's decision on G's English (docs/DECISIONS.md: official sources first,
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
| official English from MH1 US, now | 6839 | **51.6%** |
| left, but the same Japanese is in Monster Hunter Portable (so Freedom should have it) | 4913 | 37% |
| left, not in Portable either | 1511 | 11% |

Of the 1511 "neither", 204 are the software keyboard's romaji-to-kana tables and about 110 the online chat's
bad-word filter (neither is text to translate), so about **1200 strings** would have to be written by us: mostly town
(online hub) NPC talk (~430), PS2 online, network and memory-card messages (~290), and online-hub menus (~215).

421 of the 13263 are keyboard kana tables. Leaving them out, official English is 6835 of 12842 = 53%.

MH1 JP's own table (the same tool) went from 6272 to 6379 strings with US English (78%) in the second round.

## What MH1 US covers (by category)

| category | G strings | official English | by the new text rules | left | left, in Portable | left, not in Portable |
|---|---:|---:|---:|---:|---:|---:|
| items | 972 | 560 (57%) | 0 | 412 | 408 | 4 |
| weapons | 1804 | 934 (51%) | 0 | 870 | 759 | 111 |
| armour and skills | 3615 | 1063 (29%) | 33 | 2552 | 2493 | 59 |
| monsters | 74 | 56 (75%) | 0 | 18 | 15 | 3 |
| quests | 1199 | 1012 (84%) | 365 | 187 | 170 | 17 |
| NPC dialogue and tutorials | 2498 | 1902 (76%) | 53 | 596 | 165 | 431 |
| menus and UI | 1803 | 1109 (61%) | 3 | 694 | 480 | 214 |
| system (memory card, online, keyboard, code literals) | 877 | 199 (22%) | 7 | 678 | 210 | 468 |
| keyboard kana tables (not text) | 421 | 4 | 0 | 417 | 213 | 204 |
| **total** | **13263** | **6839 (51%)** | **461** | **6424** | **4913** | **1511** |

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

Monster Hunter Portable (PSP, JP, disc/mhp) is the PSP release of G and Monster Hunter Freedom is its US release.
I scanned the uncompressed Shift-JIS strings of Portable's `PSP_GAME/USRDIR/DATA.BIN` (33463 texts; the text
tables are plain, around offset 0x12C40000, the quest files around 0x1D750000) and counted G strings whose
Japanese appears there word for word (same normal form as the *marks* rule; G's line-per-pointer descriptions
are joined first, since Portable keeps each description as one string with the same line breaks). This is a
lower bound (compressed data was not looked at).

4913 of the 6424 G strings still without English are in Portable:
- items: 408 of 412 (new items, G-rank materials, new item help)
- weapons: 759 of 870 (new weapons and their descriptions, dual swords)
- armour and skills: 2493 of 2552 (nearly all new armour and its descriptions)
- monsters: 15 of 18 (subspecies, new monsters)
- quests: 170 of 187 (new village and G-rank quests, training school)
- NPC dialogue and tutorials: 165 of 596 (village talk; the town talk mostly is not in Portable)
- menus and UI: 480 of 694
- system: 210 of 678 (only the messages that are not about the PS2's memory card or network)

So Freedom should take G from 51.6% to roughly 88% official English. Once the owner supplies Freedom, the way in is
the same as MH1 JP -> MH1 US: Portable JP -> Freedom US by structure (same data file layout, same table order),
then Portable text -> G text by the rules above (most are identical text).

## What neither covers (truly G-only text): about 1510 strings

| kind | strings | notes |
|---|---:|---|
| town (online hub) NPC talk, G rewrites of MH1 talk, a few tutorial lines | 431 | shopkeepers, the ticket and room-rank talk, plaza people (320 new, 95 rewritten from MH1, 12 MH1 lines with no US line, 4 tutorial) |
| PS2 online and network messages | 268 | 237 are MH1 JP text with no paired US line, of which ~110 are the chat's bad-word filter (not shown, not translated); the rest are network messages the US build keeps elsewhere |
| memory card and yes/no dialogs | 131 | PS2 memory-card wording (Portable has Memory Stick wording instead); 79 are MH1 lines with no paired US message |
| online-hub menus, shop / guild screens, other G tables | 214 | room, ticket and gathering-hall screens |
| weapon / armour lines and quest entry rules not found in Portable | 170 | partly PS2-only collaboration items, partly a matching gap (some lines may still be in Portable) |
| code literals (HUD banners, debug, results) | 69 | |
| items, monsters, quests | 24 | |
| keyboard kana tables | 204 | not text: needs no translation |

This is what "our own translation in Freedom's style" would have to write once Freedom is in: about 1200 strings
(without the kana tables and the word filter), fewer if the remaining network messages get tied to the US disc by a
person (the tool found no safe automatic tie for them).

## Known limits (worth a person's look later)

- NPC talk lines without names or numbers can only be checked by a person reading both texts (the automatic check
  needs something to compare). The two US rewrites it found were both about Dual Swords.
- Network and error messages in main (net_con_msg, ms_err_str) and the lobby's netDialogMessage: US twins exist
  (e.g. the NET file messages at US 0x363480) but are not tied; a person-checked list (tools/text_pairs_manual.txt)
  would bring them in.
- The US text of JP quest 19/69 says swamp while the JP says forest; both are swamp quests, so for MH1 the English
  is right and the JP text was the odd one.
- disc/mhf is Monster Hunter Frontier (PC), not Freedom; Freedom (PSP US) is not on the machine yet.
