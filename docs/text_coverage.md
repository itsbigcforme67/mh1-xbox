# Text coverage: official English for MH1 JP and Monster Hunter G

Made by `tools/text_map.py --report` from the three discs (disc/mh1, disc/mh1_us, disc/mhg). Counts and ids only; no game text is quoted. Re-run it to refresh the numbers (about 3 minutes and 1 GB of RAM the first time, under a minute with the cache in build/text/). Ids are `space:0xADDRESS` (PS2 address) and `quest:N:0xFILEOFFSET`, as in docs/english.md.

## How the English is tied to the Japanese (by structure)

1. **Code reference.** Each MH1 JP function is looked up in the other build by its masked instruction n-grams (addresses and 16-bit immediates masked). Where the code is the same, the same `lui/addiu` pair loads the same object, so JP address -> other address (2433 US and 2451 G addresses). Edited functions are aligned on the n-grams that still match.
2. **Table index.** A JP pointer table is a symbol of the JP build. Its place in the other build comes from (1), from the MH1 names that tools/mhg_map.py put at G addresses, from pointers inside tables already found, or from the neighbouring table's shift; an object is accepted only if the word classes (string pointer / zero / pointer / integer) of the whole object agree with the JP layout (95% for layout guesses, 25-80% for code or name hits because the tables were edited). Rows are then paired by position, or, where rows were inserted, by their number fields (US) or by their Japanese text (G); the item-id arrays follow the Item_data record alignment.
3. **Quest data, not number.** MH1 JP quest files are tied to US files by their non-text data, and only on the same map (128 of 153 JP quests, 45 with an identical data block; the village quests 131-177 and a few others were rebalanced and are left out). G quests are tied to JP quests by identical title/goal/failure/details fields, else by the non-text data (monster, map and reward sections, rule words), again only on the same map (131 of 245 G quests). A G quest field that takes English by text alone (rule 6) takes it from a JP quest on another map only when neither text names a place. G quests whose text matches a JP quest on another map (G moved them) get no English by tie (flagged: G 2037 / JP 9, G 2038 / JP 9, G 10018 / JP 19, G 10025 / JP 64, G 10027 / JP 81, G 10028 / JP 89, G 10035 / JP 43, G 20011 / JP 5, G 20012 / JP 5, G 20013 / JP 5, G 20014 / JP 5, G 20015 / JP 5, G 20021 / JP 41, G 20022 / JP 41, G 20023 / JP 41, G 20024 / JP 41, G 20025 / JP 41, G 20026 / JP 10, G 20027 / JP 10, G 20028 / JP 10, G 20029 / JP 10, G 20030 / JP 10).
4. **Content anchor (G only).** A JP string at least 12 bytes long whose bytes are identical and unique in JP and in G gives its table a shift vote (3211 such anchors); the anchor itself pairs nothing (content rules: 6).
5. **Same Japanese text (MH1 JP).** A JP string left unpaired (its US table not located, or the US code no longer reads it, e.g. the item list's title) takes the English of identical Japanese text elsewhere when all paired occurrences agree.
6. **Text rules (G only, `g_rules`).** A G string still without English takes the English of MH1 JP text that differs from it only as a rule allows: *exact* (same bytes; reaches the quest fields of G quests no quest tie found), *layout* (full/half-width letters, blanks, line breaks), *marks* (kanji digits, punctuation, quote brackets), *buttons* (G wrote a pad symbol as a ~Bnn icon code; the US code is re-pointed to G's code for the same symbol), *numbers* (a changed number is replaced in the English when it is written there once as digits and neither number is 1), and *whole description* (a G description, one pointer per line, whose whole text matches an MH1 description gets its US lines in order when the line count is the same). Sources are whole messages only: single lines of the description tables and of the line-split dialogs are never used, and identical-text propagation (5, and its G form) no longer uses description lines either (a US description line is a re-wrap, not the translation of the JP line beside it). See docs/english_g.md.

A pair is dropped when its printf conversions differ, when the 'English' is still Japanese, or when it equals the JP bytes.
G strings whose Japanese text changed from MH1 get the English of the old text only if the change is small (text similarity >= 0.8, same digits and conversions); otherwise they are listed as *edited in G*.

Objects located in the other build: US 457 of 689 string tables (graph 5009, code 2318, layout 395, adjacent 93); G 557 (graph 4893, name 4329, name+code 1318, code 1009, adjacent 127, layout 104). Not located: US 1348 string cells, G 744.

## Monster Hunter G by category

Categories group the areas below (armour includes skills; NPC dialogue includes tutorials; menus and UI include the shop / guild screens and the unnamed 'other tables'; system is memory card, online, keyboard and code literals). *Text rules* are the G-only rules of tools/text_map.py (g_rules: whole descriptions, exact quest fields, layout, marks, buttons, numbers): whole description 42, buttons 29, exact 359, numbers 9, layout 31, same overlay 5, marks 3, button labels 5; 1060 more by identical text elsewhere. *Ambiguous* means a rule found MH1 English but it was left out (MH1 name plus a G-rank suffix (naming left for Freedom) 19, numbers: number changed to or from 1 (singular/plural) 11, the same Japanese has two different English texts 4, the English names a button the Japanese does not 3, numbers: number not written as digits in the English 3, number not written as digits in the English 3, numbers: two different English texts 2). *Portable* is Monster Hunter Portable (PSP JP, disc/mhp), the Japanese of Monster Hunter Freedom: a G string whose Japanese is in Portable word for word (same normal form as the *marks* rule) is one Freedom should have an official English line for; all text entries of its DATA.BIN (string packs, record files, code overlays; none is compressed) and BOOT.BIN were read (10740 distinct texts).

| category | G strings | official English | of which by the text rules | ours (own translation) | left | left, ambiguous for a rule | left, same Japanese in Portable | left, not in Portable |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| items | 972 | 964 (99%) | 0 | 7 | 1 | 0 | 1 | 0 |
| weapons | 1804 | 1693 (93%) | 0 | 111 | 0 | 0 | 0 | 0 |
| armour and skills | 3615 | 3545 (98%) | 33 | 70 | 0 | 0 | 0 | 0 |
| monsters | 74 | 71 (95%) | 0 | 3 | 0 | 0 | 0 | 0 |
| quests | 1528 | 1509 (98%) | 365 | 19 | 0 | 0 | 0 | 0 |
| NPC dialogue and tutorials | 2498 | 2038 (81%) | 54 | 428 | 32 | 0 | 32 | 0 |
| menus and UI | 1803 | 1588 (88%) | 15 | 196 | 19 | 0 | 11 | 8 |
| system (memory card, online, keyboard, code literals) | 877 | 387 (44%) | 6 | 281 | 209 | 0 | 93 | 116 |
| keyboard kana tables (not text) | 421 | 6 (1%) | 0 | 0 | 415 | 0 | 140 | 275 |
| **total** | **13592** | **11801 (86%)** | 473 | 1115 | 676 | 0 | 277 | 399 |

*Ours* is our own English (text/mhg_en_own.tsv, docs/english_g.md "Our own translation"), only for strings no official source has; official English always wins, and Freedom's replaces ours where it has a line (4 own lines replaced: quest:10029:0x68 quest:10044:0x68 main:0x2EA360 sub_main:0x539330).

## MH1 JP strings with official US English

| area | strings | official English | none |
|---|---:|---:|---:|
| village NPC lines and talk | 2172 | 2132 (98%) | 40 |
| armour | 1105 | 1105 (100%) | 0 |
| weapons | 941 | 937 (99%) | 4 |
| quests | 868 | 712 (82%) | 156 |
| online, keyboard, word filter | 765 | 174 (22%) | 591 |
| items (names, help) | 601 | 597 (99%) | 4 |
| code literals | 469 | 178 (37%) | 291 |
| memory card and yes/no dialogs | 367 | 225 (61%) | 142 |
| quest board / quest text tables | 333 | 320 (96%) | 13 |
| village / plaza / guild / shop UI | 236 | 188 (79%) | 48 |
| menus, options, HUD | 145 | 130 (89%) | 15 |
| tutorial pages | 116 | 113 (97%) | 3 |
| monsters | 97 | 60 (61%) | 37 |
| skills | 37 | 37 (100%) | 0 |
| other tables (pit_help_itembox_str) | 27 | 24 (88%) | 3 |
| other tables (map_name) | 6 | 6 (100%) | 0 |
| other tables (game_menu_msg) | 6 | 6 (100%) | 0 |
| other tables (movie_add_str_tbl) | 6 | 6 (100%) | 0 |
| other tables (gh_boardStr) | 6 | 6 (100%) | 0 |
| other tables (pit_help_chat_str) | 5 | 5 (100%) | 0 |
| other tables (reload_level_str) | 5 | 5 (100%) | 0 |
| other tables (itembox_cmd_str) | 5 | 5 (100%) | 0 |
| other tables (menu02_str_tbl) | 4 | 3 (75%) | 1 |
| other tables (num_chars) | 3 | 0 (0%) | 3 |
| other tables (mix_list_str) | 3 | 3 (100%) | 0 |
| other tables (menu00_str_tbl) | 3 | 2 (66%) | 1 |
| other tables (menu08_str_tbl) | 3 | 3 (100%) | 0 |
| other tables (str$) | 3 | 0 (0%) | 3 |
| other tables (equip_exp_str_gun) | 3 | 3 (100%) | 0 |
| other tables (equip_exp_str_armor) | 3 | 3 (100%) | 0 |
| other tables (wearable_tbl) | 3 | 3 (100%) | 0 |
| other tables (pit_help_reward_str) | 3 | 3 (100%) | 0 |
| other tables (yes_or_no_str) | 2 | 2 (100%) | 0 |
| other tables (data_list_str) | 2 | 2 (100%) | 0 |
| other tables (chat_cfg_str) | 2 | 2 (100%) | 0 |
| other tables (retire_yesno_str) | 2 | 2 (100%) | 0 |
| other tables (load_msg) | 2 | 0 (0%) | 2 |
| other tables (connect_msg) | 2 | 0 (0%) | 2 |
| other tables (nn_tbl) | 2 | 0 (0%) | 2 |
| other tables (menu01_str_tbl) | 2 | 2 (100%) | 0 |
| other tables (menu06_str_tbl) | 2 | 2 (100%) | 0 |
| other tables (menu04_str_tbl) | 2 | 2 (100%) | 0 |
| other tables (menu05_str_tbl) | 2 | 0 (0%) | 2 |
| other tables (menu09_str_tbl) | 2 | 2 (100%) | 0 |
| other tables (menu0a_str_tbl) | 2 | 2 (100%) | 0 |
| other tables (pit_help_data_str) | 2 | 2 (100%) | 0 |
| other tables (demo_cam) | 2 | 2 (100%) | 0 |
| other tables (receive_mes_str) | 2 | 2 (100%) | 0 |
| other tables (status_sub_str) | 2 | 1 (50%) | 1 |
| other tables (equip_exp_str_sword) | 2 | 2 (100%) | 0 |
| other tables (equip_exp_str_sw_attr) | 2 | 2 (100%) | 0 |
| other tables (sex_char_tbl) | 2 | 2 (100%) | 0 |
| other tables (reward_menu_str) | 2 | 2 (100%) | 0 |
| other tables (select_id_msg) | 2 | 0 (0%) | 2 |
| other tables (special_tag_dat) | 2 | 0 (0%) | 2 |
| other tables (mix_cmd_str) | 1 | 1 (100%) | 0 |
| other tables (retire_str) | 1 | 0 (0%) | 1 |
| other tables (q_sendpl_list) | 1 | 1 (100%) | 0 |
| other tables (zen_tbl) | 1 | 0 (0%) | 1 |
| other tables (ten_moji) | 1 | 0 (0%) | 1 |
| other tables (maru_moji) | 1 | 0 (0%) | 1 |
| other tables (bou_sei_tbl) | 1 | 1 (100%) | 0 |
| other tables (zen_num) | 1 | 0 (0%) | 1 |
| other tables (sn_objs) | 1 | 0 (0%) | 1 |
| **total** | **8404** | **7030 (83%)** | 1374 |

(6318 of the 7536 JP strings found in the executables and overlays, plus 712 quest fields.)

Dropped pairs: line-split dialog (needs per-message alignment) 258, same as JP 178, US changed a number (quest / talk text) 9, US rewrote the talk (Dual Swords) 7, quest US changed a number 5, format mismatch 1, still Japanese 1.

## Monster Hunter G strings with official US English

| area | strings | official English | ours | twin without US English | edited in G | G only |
|---|---:|---:|---:|---:|---:|---:|
| armour [G table] | 2641 | 2607 (98%) | 34 | 0 | 0 | 0 |
| village NPC lines and talk | 1883 | 1777 (94%) | 106 | 0 | 0 | 0 |
| quests | 1309 | 1296 (99%) | 13 | 0 | 0 | 0 |
| weapons | 940 | 939 (99%) | 1 | 0 | 0 | 0 |
| weapons [G table] | 864 | 754 (87%) | 110 | 0 | 0 | 0 |
| armour | 816 | 814 (99%) | 2 | 0 | 0 | 0 |
| online, keyboard, word filter | 755 | 162 (21%) | 98 | 495 | 0 | 0 |
| items (names, help) | 533 | 531 (99%) | 1 | 1 | 0 | 0 |
| village NPC lines and talk [G table] | 498 | 148 (29%) | 318 | 0 | 0 | 32 |
| items (names, help) [G table] | 439 | 433 (98%) | 6 | 0 | 0 | 0 |
| memory card and yes/no dialogs | 217 | 138 (63%) | 78 | 1 | 0 | 0 |
| village / plaza / guild / shop UI [G table] | 182 | 106 (58%) | 75 | 0 | 0 | 1 |
| village / plaza / guild / shop UI | 181 | 166 (91%) | 14 | 1 | 0 | 0 |
| menus, options, HUD [G table] | 179 | 131 (73%) | 45 | 0 | 0 | 3 |
| quest board / quest text tables | 158 | 156 (98%) | 2 | 0 | 0 | 0 |
| other tables (lit_445) [G table] | 153 | 148 (96%) | 5 | 0 | 0 | 0 |
| menus, options, HUD | 134 | 132 (98%) | 2 | 0 | 0 | 0 |
| skills [G table] | 124 | 90 (72%) | 34 | 0 | 0 | 0 |
| tutorial pages | 117 | 113 (96%) | 4 | 0 | 0 | 0 |
| other tables (map_name) [G table] | 100 | 71 (71%) | 27 | 0 | 0 | 2 |
| other tables (lit_329_00661C) [G table] | 96 | 96 (100%) | 0 | 0 | 0 | 0 |
| code literals (main) | 76 | 14 (18%) | 15 | 0 | 0 | 47 |
| other tables (lit_333_00661DF) [G table] | 64 | 64 (100%) | 0 | 0 | 0 | 0 |
| code literals (lobby) | 63 | 15 (23%) | 2 | 0 | 0 | 46 |
| memory card and yes/no dialogs [G table] | 62 | 17 (27%) | 45 | 0 | 0 | 0 |
| quest board / quest text tables [G table] | 61 | 57 (93%) | 4 | 0 | 0 | 0 |
| other tables (lit_322_006619E) [G table] | 52 | 52 (100%) | 0 | 0 | 0 | 0 |
| code literals (game) | 50 | 32 (64%) | 5 | 0 | 0 | 13 |
| online, keyboard, word filter [G table] | 45 | 10 (22%) | 30 | 0 | 0 | 5 |
| other tables (lit_326_00661B) [G table] | 40 | 40 (100%) | 0 | 0 | 0 | 0 |
| monsters | 38 | 36 (94%) | 2 | 0 | 0 | 0 |
| other tables (lit_309) [G table] | 36 | 36 (100%) | 0 | 0 | 0 | 0 |
| monsters [G table] | 36 | 35 (97%) | 1 | 0 | 0 | 0 |
| skills | 34 | 34 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_198_0065F) [G table] | 32 | 32 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_175_00359E) [G table] | 30 | 30 (100%) | 0 | 0 | 0 | 0 |
| other tables (pit_help_reward_str) [G table] | 28 | 26 (92%) | 2 | 0 | 0 | 0 |
| other tables (pit_help_itembox_str) | 27 | 27 (100%) | 0 | 0 | 0 | 0 |
| other tables (wearable_tbl) [G table] | 25 | 21 (84%) | 4 | 0 | 0 | 0 |
| other tables (lit_262_006606D) [G table] | 24 | 24 (100%) | 0 | 0 | 0 | 0 |
| code literals (sub_main) | 23 | 5 (21%) | 2 | 0 | 0 | 16 |
| other tables (lit_290_00660FA) [G table] | 20 | 20 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_451_006647A) [G table] | 18 | 18 (100%) | 0 | 0 | 0 | 0 |
| other tables (movie_add_str_tbl) [G table] | 17 | 17 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_243) [G table] | 16 | 16 (100%) | 0 | 0 | 0 | 0 |
| other tables (reward_menu_str) [G table] | 13 | 12 (92%) | 1 | 0 | 0 | 0 |
| other tables (lit_188_0065EF) [G table] | 12 | 11 (91%) | 1 | 0 | 0 | 0 |
| other tables (lit_256) [G table] | 12 | 12 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_319) [G table] | 12 | 12 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_324_00661AB) [G table] | 12 | 12 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_327_00661C) [G table] | 12 | 12 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_420_00663C) [G table] | 12 | 12 (100%) | 0 | 0 | 0 | 0 |
| other tables (pit_help_itembox_str) [G table] | 12 | 12 (100%) | 0 | 0 | 0 | 0 |
| other tables (yes_or_no_str) [G table] | 11 | 11 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_224_0065FA) [G table] | 8 | 8 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_236_0065FDE) [G table] | 8 | 8 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_281_00660C) [G table] | 8 | 7 (87%) | 1 | 0 | 0 | 0 |
| other tables (lit_285_00660DF) [G table] | 8 | 7 (87%) | 1 | 0 | 0 | 0 |
| other tables (lit_337_00661F) [G table] | 8 | 8 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_205_0068EEE) [G table] | 7 | 6 (85%) | 1 | 0 | 0 | 0 |
| other tables (data_list_str) [G table] | 7 | 7 (100%) | 0 | 0 | 0 | 0 |
| code literals (yn) | 7 | 0 (0%) | 6 | 0 | 0 | 1 |
| other tables (lit_441_006643B) [G table] | 6 | 6 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_450_006646F) [G table] | 6 | 6 (100%) | 0 | 0 | 0 | 0 |
| other tables (gh_boardStr) | 6 | 6 (100%) | 0 | 0 | 0 | 0 |
| other tables (equip_exp_str_armor) [G table] | 6 | 6 (100%) | 0 | 0 | 0 | 0 |
| other tables (map_name) | 6 | 6 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_222_0068F) [G table] | 5 | 5 (100%) | 0 | 0 | 0 | 0 |
| other tables (itembox_cmd_str) | 5 | 5 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_183_0065ED) [G table] | 4 | 4 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_221_0065F) [G table] | 4 | 4 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_218_0065F) [G table] | 4 | 4 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_232_0065FCD) [G table] | 4 | 2 (50%) | 2 | 0 | 0 | 0 |
| other tables (lit_279_00660B) [G table] | 4 | 4 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_357) [G table] | 4 | 4 (100%) | 0 | 0 | 0 | 0 |
| other tables (menu02_str_tbl) | 4 | 3 (75%) | 1 | 0 | 0 | 0 |
| other tables (game_menu_msg) | 4 | 4 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_190_0068ED) [G table] | 3 | 0 (0%) | 3 | 0 | 0 | 0 |
| other tables (lit_214_0068EFC) [G table] | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_231_0065FCB) [G table] | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_408_006637E) [G table] | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_442) [G table] | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (lit_384_00662F) [G table] | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (gh_boardStr) [G table] | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (num_chars) | 3 | 0 (0%) | 0 | 3 | 0 | 0 |
| other tables (menu00_str_tbl) | 3 | 2 (66%) | 1 | 0 | 0 | 0 |
| other tables (menu08_str_tbl) | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (demo_cam) | 3 | 2 (66%) | 0 | 0 | 1 | 0 |
| other tables (movie_add_str_tbl) | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (pit_help_reward_str) | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (mix_list_str) | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (equip_exp_str_armor) | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (wearable_tbl) | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (default_reibun) [G table] | 3 | 2 (66%) | 0 | 0 | 0 | 1 |
| other tables (load_msg) [G table] | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (game_menu_msg) [G table] | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (pit_help_chat_str) | 3 | 3 (100%) | 0 | 0 | 0 | 0 |
| other tables (select_id_msg) | 2 | 0 (0%) | 2 | 0 | 0 | 0 |
| other tables (special_tag_dat) | 2 | 0 (0%) | 2 | 0 | 0 | 0 |
| other tables (nn_tbl) | 2 | 0 (0%) | 0 | 2 | 0 | 0 |
| other tables (menu01_str_tbl) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (menu06_str_tbl) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (menu04_str_tbl) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (menu05_str_tbl) | 2 | 0 (0%) | 2 | 0 | 0 | 0 |
| other tables (menu09_str_tbl) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (menu0a_str_tbl) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (demo_cam) [G table] | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (zen_num) [G table] | 2 | 1 (50%) | 1 | 0 | 0 | 0 |
| other tables (reward_menu_str) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (yes_or_no_str) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (data_list_str) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (equip_exp_str_sword) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (equip_exp_str_gun) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (edit_menu_msg) [G table] | 2 | 0 (0%) | 2 | 0 | 0 | 0 |
| other tables (load_msg) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (connect_msg) | 2 | 2 (100%) | 0 | 0 | 0 | 0 |
| other tables (itembox_cmd_str) [G table] | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (zen_tbl) | 1 | 0 (0%) | 0 | 1 | 0 | 0 |
| other tables (ten_moji) | 1 | 0 (0%) | 0 | 1 | 0 | 0 |
| other tables (maru_moji) | 1 | 0 (0%) | 0 | 1 | 0 | 0 |
| other tables (num_tbl) [G table] | 1 | 0 (0%) | 0 | 0 | 0 | 1 |
| other tables (equip_icon_u_tbl) [G table] | 1 | 0 (0%) | 1 | 0 | 0 | 0 |
| other tables (zen_num) | 1 | 0 (0%) | 0 | 1 | 0 | 0 |
| other tables (push) [G table] | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (srcfile) [G table] | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (mix_cmd_str) | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (mix_cmd_str) [G table] | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (mix_list_str) [G table] | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (receive_mes_str) | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (equip_exp_str_gun) [G table] | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (verify_button_00387D) [G table] | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (Snd_weapon_tbl) [G table] | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| other tables (pit_help_data_str) | 1 | 1 (100%) | 0 | 0 | 0 | 0 |
| **total** | **13592** | **11801 (86%)** | 1115 | 507 | 1 | 168 |

G strings are counted from a scan of main and the overlays (Shift-JIS runs, as tools/text_dump.py; a few hundred false hits are possible) plus the four text fields of each quest file. Status of the pairs between a JP string and its G twin: identical text 3935, near text (edited a little) 1260, no US English for the JP string 1095, text edited in G (left out) 266.

### What is left, and where it can come from

- **G only** (no MH1 twin: G's new quests, items, monsters, the plaza/gathering hall code and tables): no official English exists on the US disc. The owner's plan (docs/DECISIONS.md): Monster Hunter Freedom (PSP US) next, then our own translation in Freedom's style; the *Portable* column above counts what Freedom should cover (docs/english_g.md). The MHG 'English Remix' patch (Amaillo) is a community source, not used.
- **edited in G** and **twin without US English**: the Remix again, or the US text if a person checks it.
- **Code literals** that no table points to (HUD banners, result lines): not paired in this version (the code-reference pairs reach few of them because most literals are reached through pointer cells in data).
- The full list of ids without English, with area, reason, 'in Portable' and why a text rule left it out: `docs/survey/mhg_text_untranslated.txt`.
