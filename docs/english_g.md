# English for Monster Hunter G: what official text covers, and what is left

Agent B, 20 Oct 2026. Input for the owner's decision on G's English (docs/DECISIONS.md: official sources first,
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
| official English from MH1 US, now | 6776 | **51%** |
| left, but the same Japanese is in Monster Hunter Portable (so Freedom should have it) | 4935 | 37% |
| left, not in Portable either | 1552 | 12% |

Of the 1552 "neither", 204 are the software keyboard's romaji-to-kana tables (not text to translate), so about
**1350 strings** would have to be written by us: mostly town (online hub) NPC talk (~440), PS2 online, network and
memory-card messages (~430), and online-hub menus (~215).

421 of the 13263 are keyboard kana tables (204 of them in the 'neither' row). Leaving them out, official English is 6772 of
12842 = 53%.

## What MH1 US covers (by category)

| category | G strings | official English | by the new text rules | left | left, in Portable | left, not in Portable |
|---|---:|---:|---:|---:|---:|---:|
| items | 972 | 560 (57%) | 0 | 412 | 408 | 4 |
| weapons | 1804 | 934 (51%) | 0 | 870 | 759 | 111 |
| armour and skills | 3615 | 1063 (29%) | 33 | 2552 | 2493 | 59 |
| monsters | 74 | 56 (75%) | 0 | 18 | 15 | 3 |
| quests | 1199 | 1010 (84%) | 370 | 189 | 172 | 17 |
| NPC dialogue and tutorials | 2498 | 1878 (75%) | 49 | 620 | 181 | 439 |
| menus and UI | 1803 | 1107 (61%) | 4 | 696 | 482 | 214 |
| system (memory card, online, keyboard, code literals) | 877 | 164 (18%) | 7 | 713 | 212 | 501 |
| keyboard kana tables (not text) | 421 | 4 | 0 | 417 | 213 | 204 |
| **total** | **13263** | **6776 (51%)** | **463** | **6487** | **4935** | **1552** |

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
| exact | nothing (blanks at the ends) - reaches the quest fields of G quests the quest tie did not find | 364 |
| layout | full-/half-width letters and digits, blanks, line breaks (the English keeps its own breaks) | 30 |
| marks | + kanji digits (三 = ３), punctuation and quote brackets (。、！？・…「」≪≫【】) | 4 |
| buttons | G wrote a pad symbol (○ □ △ ×) as an icon code ~Bnn; the US line's own icon code is re-pointed to G's code for the same symbol | 24 |
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
  tutorial pages the wrong page too (checked by eye on both discs). Those 7 tables are now in
  `SHIFTED_US_TABLES` (no English rather than the wrong one). MH1 JP's own coverage moves from 6272 to 6249 for this.
  Re-pairing them properly (by page content) is a small follow-up.

### Ambiguous (a rule found English but it was left out): 53 strings

Listed with the reason in docs/survey/mhg_text_untranslated.txt (last column):
- 19 MH1 names plus a G-rank letter (G, S, 改 suffix, e.g. a barrel bomb "G"): how to write the suffix in English is a
  naming choice; Freedom's names decide it.
- 18 number changes the English cannot take safely: 10 where the number changes to or from 1 (singular/plural
  would change), 9 where the English writes the number as a word.
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

4935 of the 6487 G strings still without English are in Portable:
- items: 408 of 412 (new items, G-rank materials, new item help)
- weapons: 759 of 870 (new weapons and their descriptions, dual swords)
- armour and skills: 2493 of 2552 (nearly all new armour and its descriptions)
- monsters: 15 of 18 (subspecies, new monsters)
- quests: 172 of 189 (new village and G-rank quests, training school)
- NPC dialogue and tutorials: 181 of 620 (village talk; the town talk mostly is not in Portable)
- menus and UI: 482 of 696
- system: 212 of 713 (only the messages that are not about the PS2's memory card or network)

So Freedom should take G from 51% to roughly 88% official English. Once the owner supplies Freedom, the way in is
the same as MH1 JP -> MH1 US: Portable JP -> Freedom US by structure (same data file layout, same table order),
then Portable text -> G text by the rules above (most are identical text).

## What neither covers (truly G-only text): about 1550 strings

| kind | strings | notes |
|---|---:|---|
| town (online hub) NPC talk, G rewrites of MH1 talk, a few tutorial lines | 439 | shopkeepers, the ticket and room-rank talk, plaza people (320 new, 96 rewritten from MH1, 23 MH1 lines with no US line) |
| PS2 online and network messages | 270 | 239 are MH1 JP text with no paired US line: the US online code is different, so some may still exist on the US disc unpaired |
| memory card and yes/no dialogs | 162 | PS2 memory-card wording (Portable has Memory Stick wording instead); 118 are MH1 lines whose US lines are split differently |
| online-hub menus, shop / guild screens, other G tables | 214 | room, ticket and gathering-hall screens |
| weapon / armour lines and quest entry rules not found in Portable | 170 | partly PS2-only collaboration items, partly a matching gap (some lines may still be in Portable) |
| code literals (HUD banners, debug, results) | 69 | |
| items, monsters, quests | 25 | |
| keyboard kana tables | 204 | not text: needs no translation |

This is what "our own translation in Freedom's style" would have to write once Freedom is in: about 1350 strings,
fewer if the online and memory-card lines can be paired with the US disc after all (a per-message alignment like the
one MH1 JP's memory-card dialogs already use).

## Known limits (worth a person's look later)

- The US moved a few MH1 quests: e.g. JP quests 19 and 69 (forest, woodcutters) are "Trouble in the Swamp" with a
  swamp requester in the US files, so G's copy of that quest gets an English line naming the wrong place. Only 45 of
  the 128 JP-US quest ties have an identical data block; the others may have similar US edits.
- The village/town NPC tables were checked only by the length correlation, not page by page as the tutorial was.
- disc/mhf is Monster Hunter Frontier (PC), not Freedom; Freedom (PSP US) is not on the machine yet.
