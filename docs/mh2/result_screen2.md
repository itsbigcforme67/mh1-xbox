# MH Dos (MH2): result screen, part 2 (the open questions of result_screen.md) (RESULT2 research, 4 Nov 2026)

- **Scope:** answers the "not read" list of docs/mh2/result_screen.md: the page-text builders, remuneration_item_set,
  the score sheet, the flag-0x200000 unlocks, the online event codes, the tail of sub_3A0040, the bounty values at
  0x5D4790, the User_data blocks 0x1C / 0x2928 / 0x27B8 / 0x2CCC, quest flag 0x200000, mode 7, and where kills are
  counted. Nothing was built, run or changed; no C was written. result_screen.md was not edited (its fixes are listed in 9).
- **Source:** our own disassembly of the owner's disc (asm/mh2/plsel, sub_main, main, lobby, select), read by hand with
  throwaway scripts in the scratchpad. Compared with our G C (src/main/quest/f_quest.c, f_quest_nm.c). The community
  mh2-decomp repo was not opened. No game text is quoted: strings are named by their pointer-table slot only.
- **Proof level:** as in result_screen.md. "Read" = followed instruction by instruction; "skimmed" = calls, globals and
  constants only; **guess** = a meaning the code does not name. Address names: `sub_X` / `D_X` as in the asm; "UD" =
  User_data 0x5D4A60, "RW" = the result work (system_w+0x88, 0xBDA200), "QW" = quest_w 0x5D8260, "GW" = game_w 0x6ADCB0.

## 1. Short answers

| question | answer | level |
|---|---|---|
| Where MH2 counts kills | At kill time into QW+0x20C (Quest_enemy_die 0x3A2470 and quest_em_die, cap 9999); captures into QW+0x2AC (sub_3A26B0, the MH2 Quest_enemy_capture). At quest end sub_3A0040 -> sub_39FC00 adds QW+0x20C into UD+0x23D2[kind] (sub_3BBA10, cap 9999) and QW+0x2AC into UD+0x24D2[kind] (no cap). This is the hunting log. Skipped for online quest 25001 | read |
| The "second count" | Captures. sub_3A26B0 has the shape of G's Quest_enemy_capture (QEM x2E bit 8 guard, the 0x81 message byte) and is the only writer of QW+0x2AC | read; the name is a **guess** from the shape match |
| Quest flag 0x200000 | Score-attack quests: a score sheet (time against a par time from the quest number, best time and best score kept per quest, kill points per kind, points per category, leftover pouch items valued at price / 4), the pouch restored afterwards, rewards rolled by score bands. What the game calls them: **guess** training / challenge quests | read (logic); **guess** (name) |
| Mode 7 | Plays a movie from the stream table 0x4683C0 (entry 7 after the quest that sets event flag 0x184, entry 8 after quest 5007), runs a plsel scene (0xD64C80 / 0xD64D40, stops the stream), then shows a centred multi-line message page from the pointer list at 0x447CC4 and goes to mode 6. **Guess:** the ending movies and staff roll | read (flow); **guess** (meaning) |
| 0x5D4790 | s32[74] bounty value per kind for this session. Cleared by main 0x18D5F0 (the config calls it smell_init: a carried name, almost surely wrong), filled offline by main 0x18D6A0 from UD+0x2808 (u16 per kind) and online by lobby 0x7B3880 from a lobby table at 0x8BEDA0. The village code (lobby 0x710E50) moves UD+0x2808 with the hunted ring (below) | read (fills); skimmed (the village recalculation) |
| UD+0x2808 / +0x28C8 | u16[96] current bounty per kind (new save: the middle of its group's band, main 0x18DAE0); byte[96] its band (1-6) and whether it went up or down (main 0x18DD20) | read |
| UD+0x27B7 / +0x27B8 | 5 groups x 16-byte rings of recently hunted kinds (groups from 0x1BB2A0, 44-byte rows: s32 min, s32 max, s32 count, kind list), plus a count byte. Start value copied from 0x1BB380. Read by the village bounty recalculation | read (layout); **guess** (that it lowers the bounty of over-hunted kinds) |
| UD+0x2928 | three 0x1C-byte records of tracked monsters: +0 s32 total time (QW+0x52C / 1800, cap 59999), +4 u16 hunts (cap 9999), +8 s16 strength (x1.08 per offline quest, cap 300; x0.85 floor 50 when hunted in online quest 25001), +0xA kind (0 = empty), +0xC u16 size, +0xE name string, +0x17 s8 type (names table 0x447DF0), +0x18 state rolled from +8 (below 80: 1 at 10 % else 0; below 150: 2; else 4 at 10 % else 3), +0x19. Shown on the online page (builder id 7). **Guess:** a named monster that grows between quests and is fought online in 25001 | read (fields); **guess** (meaning) |
| UD+0x1C | 16 slots x {s16 value, u16 bits}, one per stage of the list 0x1BB9C8, used only offline when quest flag 0x20 is set (sub_3A0820 / sub_3A09F0): value = QW+0x134 at the end, bits = the per-stage bits QW+0x48C[stage] minus QW+0x53A; both cleared when the value is 0 or less. Remuneration conditions 0x0B-0x0F pay when GW+0x13A was above 25600 / 19200 / 12800 / 6400 and QW+0x134 is now at or below it. **Guess:** a monster's HP (and broken parts) carried from quest to quest on siege-style stages | read; **guess** (meaning) |
| UD+0x2CCC | sum of captures of the kinds marked in em_tsuushin_tbl (0x8BEE60), cap 9999 (sub_39FD40). Unlock 0x63 at 100 | read; **guess** that those are the large monsters |
| remuneration_item_set 0x3A9FF0 | MH2's own reward roll, a different design from G's (section 4) | read |
| sub_3A0040's tail | statistics counters, the hunting log, unlock bits (section 5) | read |
| sub_D6AC30 | the score sheet of flag-0x200000 quests (section 6) | read |
| sub_D70810 | unlocks 0x81, 0x88-0x8C and list unlocks for flag-0x200000 quests (section 6) | read |
| sub_D74320 | online event codes 0x700-0xA00 after online quest 15017 (section 7) | read |

Still unknown: what MH2 calls the 0x200000 quests, the tracked monsters and the carried-HP stages; what the QW+0x160
target flags 0x11 / 0x12 / 0x30 / 0x32 mean exactly (counted targets / delivery targets: **guess**); the contents of the
lobby bounty recalculation (0x710E50) and of the movie scene 0xD64C80; who writes QW+0x134 and GW+0x13A; the online step-1
page (D721F0 / D72290) beyond the text it builds.

## 2. The text pages (read)

### 2.1 One line buffer, one drawer

All result pages share a text buffer in RW and one of three drawers. A builder fills the buffer; the drawer prints it.

| RW+ | meaning |
|---|---|
| 0x000 | page kind for the drawer sub_D71660: 0 money pages, 1 hunter point page, 3 record page |
| 0x6B4 | page id given to the builder sub_D6D610 (0-3 money, 4 hunter points, 5 records, 7 the online step-1 page) |
| 0x6B8 | page count (4 on the money pages, 1 otherwise) |
| 0x6BC | s32 timeout (3600 ticks); shown as seconds (value / 60) |
| 0x6C0, 0x6C1 | close state (1 = closing animation running, sub_D72F20; 2 = count down 0x6C1, then the page reports done) |
| 0x748, 0x74C | first line / sub-page of the unlock and event pages |
| 0x750 | line count |
| 0x754 | scroll position of the sale list (money page 3) |
| 0x758 | 80 lines x 0x4C bytes (0x17C0, memset by every builder) |
| 0x1F0 | which record-list entry the record page shows |
| 0x1F6..0x20C | record page scratch: old biggest / smallest (0x1F6 / 0x1F8), this quest's (0x1FA / 0x1FC), their display floats (0x200 / 0x204 / 0x208 / 0x20C), kind (0x210) |

The builders write each line with sprintf from a format taken out of two pointer tables in sub_main's data, then turn the
digits to full width (han2zen). The formats carry the font's colour escapes. Names come from D_445620 (monster names, by
species from sub_3BC090), D_464770 (item names) and D_447DF0 / D_447E30 (the tracked-monster texts). Names are padded to
20 characters with the pad string D_00D7BB88. Below, "F[n]" = the n-th pointer of D_469EB0 and "S[n]" = the n-th pointer of
D_469FA0 (S[0] is the separator line, S[1] a "doubled" mark, S[2..4] marks for bonus entries 0 / 1 / 2).

### 2.2 Layout of sub_D71660 (money, hunter point and record pages)

- Message type: Lb_put_msg_type2(0x31). x of every body line = (640 - 9 x strlen(S[0])) / 2, the same for all lines
  (so the separator sets the column). **Guess:** 9 px per byte at size 18.
- Line 0 (the title): size 24, at y = 82. Lines 1..n: size 18, y = 88 + 22 x line. Empty lines are skipped.
- Always: a prompt string (pointer at 0x469FBC) at (490, 356); the timeout in seconds with the format at 0x469FC0 at
  (440, 380).
- Money pages only: "page n / m" (format D_00D7BBC0, page + 1, count) at (514, 88), printed through sub_3694A0.
  A note (pointer 0x469FB4) at (300, 88) when the online low-rank cut was applied (RW+0x3B80) and RW+0x18 is set and the
  page's own sum is non-zero (page 0: RW+0x04, page 1: RW+0x08, page 2: RW+0x0C).
  Page 3 (sale list): blinking arrows (D_00D7BBB0 up at (500, 120), D_00D7BBB8 down at (500, 290), size 28, shown on frames
  with bit 0x10 of the counter 0x6AE188) when the list scrolls (RW+0x754 > 0; RW+0x754 + 7 < RW+0x39F8).
- Hunter point page: the same note at (410, 88) when RW+0x714 is non-zero and the cut was applied.
- Record page: a "new" mark (pointer 0x469F38) at (472, 176) when this quest's biggest beats the stored biggest, and at
  (472, 198) for the smallest (smaller is better); the same arrows when more than one species is on the list.

### 2.3 What each builder page holds (sub_D6D610, jump table 0xD7BC50)

Lines are listed in order; "blank" = an empty line. Pages are padded to line 10 before the closing separator where noted.

| id | page | lines |
|---|---|---|
| 0 | money summary | F[0] title; S[0]; F[1] RW+0x08 (quest money); F[2] RW+0x0C (bounty money); F[3] RW+0x14 (sales); 3 blank; F[4] RW+0x04 (total paid); S[0]; F[5] UD+0x60 (money now). RW+0x10 is in the total but has no line |
| 1 | quest money | F[12] title; S[0]; F[13] (one player) or F[14] (more); F[15] RW+0x6F4 (+ S[1] if factor RW+0x6FC is not 1.0); F[16] RW+0x6F8 if > 0 (+ S[1] if RW+0x700 is not 1.0); target 0 paid (RW+0x24): F[52] if combined (RW+0x20 = 1) else F[17], value RW+0x704; not combined: F[18] RW+0x708 if RW+0x28, F[19] RW+0x70C if RW+0x2C; pointer 0x469FD4 when the byte QW+0x54A is set; pad to 10; S[0]; F[20] RW+0x08 |
| 2 | bounties | F[6] title; S[0]; F[7]; then each bounty line of type 0-2 whose (type, id) is the quest's bonus entry 0 or 2 (or none): species name + F[8] (entry's per-count QW+0x1F8, at least 1; line value); S[2] after entry 0. Blank; F[9] (one player) or F[10]; then every type 0-2 line again with its money: entry 0: n x per x value / players, entry 1: value / players, else n x value / players, each x 60 / 100 with the low-rank cut (RW+0x3B80); S[2 + entry]. Pad to 10; S[0]; F[11] RW+0x0C |
| 3 | sale list | F[21] title; S[0]; F[22]; 7 rows from RW+0x754 of result_w+0x16 {item, n}: item name + F[23] (n, Item_data price x n); after the last item the delivery line (result_w+0x66 / +0x68: item name + F[23] with QW+0x208 / +0x20A + pointer 0x469FB0). Pad to 10; S[0]; F[24] RW+0x14 |
| 4 | hunter points (online) | F[43] title; S[0]; target 0 done (Quest_clear_ck(1)): RW+0x24 -> F[53] (combined) or F[44] with RW+0x71C; not combined: F[45] RW+0x720 if RW+0x28, F[46] RW+0x724 if RW+0x2C; not done: F[51] if RW+0x71C. Deaths RW+0x710 > 0: F[47] RW+0x718 + D_00D7BBC8. Pad to 8; F[48] RW+0x714; S[0]; F[49] RW+0x738 (total). Rank change: RW+0x73C = 0 and RW+0x734 = 1 -> pointer 0x469FD0; RW+0x73C = 1 -> 0x469FC8; = 2 -> 0x469FCC; else 0x469FC4. F[50] RW+0x730 (rank) |
| 5 | records | F[26] title; S[0]; F[27]; species name (kind RW+0x30[RW+0x1F0]); stored biggest RW+0x33C[kind] -> F[54] with size sub_3BBB50(kind, size) as a float, or F[31] when 0; stored smallest RW+0x3D0[kind] -> F[55] / F[32]; F[28]; species name; this quest's biggest RW+0x3A40[kind] -> F[29] (coloured with D_00D7BC10 when RW+0xC4[kind] is set) or F[56] when 0; smallest RW+0x3AE0[kind] -> F[30] (colour when RW+0x158[kind]) or F[57]. Pad to 10; S[0] |
| 6 | none | returns at once |
| 7 | online step-1 page | F[36] with the name at RW+0x6E6 (default pointer 0x447DC4); S[0]; F[37]; the candidate record at RW+0x6D8 (same 0x1C layout as UD+0x2928): F[39] type name, F[40] rank name (D_447E30[func_0018EB20(record)]), F[41] size; F[38] (+ F[42] when any of RW+0x6CC / 0x6D0 / 0x6D4 is set); then the slot UD+0x2928[GW-0x19C] the same way, each line coloured (D_00D7BC30) when its RW+0x6CC / 0x6D0 / 0x6D4 flag is set. Pad to 10; S[0] |

Money page 0..3 are rebuilt every tick (sub_D71BD0 calls the builder each frame).

### 2.4 The unlock and event pages (sub_D73D10, sub_D73610)

Same x column and message type. Title (unlock: pointer 0x469F14; event: 0x469F3C) at size 24, y = 82; S[0] at y = 110.
Unlock page: 4 lines per sub-page from line RW+0x74C x 4, each centred on its own width, the first at size 24 and
y = 176, then size 18 with 22 px steps; a closing S[0]; arrows at (306, 120) / (306, 290); prompt and timer as above.
Event page: 3 lines per sub-page from RW+0x3980 x 3 (same pattern, not followed to the end).
The unlock texts (sub_D73930) copy each id's name from D_447750 (8-byte rows, +4 a pointer) and its two-part text from
D_447F40 split at the newline; the event texts (sub_D732D0) pick a template by the code's high byte from D_447CB0 and fill
`%m` / `%g` style holes from D_447D80 / D_447CE0 by the low byte (code 0x2xx uses low byte - 9).

### 2.5 Input (sub_D71BD0 money, sub_D71E50 records, MH1 game pad bits, **assumed** the same on MH2)

Trigger pad 0x6AE0B4, repeat 0x6AE0C8. Right (0x400) / left (0x800): next / previous money page with wrap-around, sound
0x17 (sub_D67660), scroll reset. Up (0x2000) / down (0x1000): sale list scroll on page 3, or the previous / next record
species. Circle (0x20) with no close running: sound 0x13 and sub_D6B570 (starts the close). The page returns done when
the close count 0x6C1 runs out.

## 3. Kills, captures and the hunting log (read)

- Quest_enemy_die (sub_main ~0x3A2470, labelled so in the asm): unless online quest 25001 (sub_39E400), QW+0x20C[kind]
  += 1 (cap 9999); also QW+0x34C / +0x3EC biggest / smallest from em+0x984 when sub_3A7480(kind) says the kind has sizes.
  quest_em_die (0x3AAD84 file) does the same count.
- sub_3A26B0 (in config's f_quest_p3 range): QW+0x2AC[kind] += 1 (cap 9999) and the same sizes; sets system_w+0x33 =
  0x81. Called from many game-overlay monster files. Same shape as G's Quest_enemy_capture.
- At the result (both cleared and failed, offline, not 25001): sub_39FC00 adds QW+0x20C[k] to UD+0x23D2[k] (sub_3BBA10,
  cap 9999) and QW+0x2AC[k] to UD+0x24D2[k], for the 74 kinds. So result_screen.md's "this screen does not add kills" is
  wrong: it does, through sub_3A0040.

## 4. remuneration_item_set, sub_main 0x3A9FF0 (read)

Not G's design (G walks QREM rows with quest_w.x13C bits). MH2:
1. Clears GW+0x140..0x1FF (48 entries {u16 item, s16 n}) and GW+0x200 / +0x204.
2. Luck level from the master's skills: Pl_Skill_ck 0x9D -> 1, 0x9C -> 2, 0x9E -> 3, 0x9F -> 4, else 0 (checked in the
   order 0x9C, 0x9E, 0x9D, 0x9F).
3. Rows from the pointer QW+0x70 (8 bytes: u16 condition, u16, u32 offset of an item list into mission_area); 0xFFFF ends.
   Item lists are {u16 weight, u16 item, u16 n}, 0xFFFF ends. Conditions:
   - 0x8000 always; 0x8001 / 0x8002 / 0x8003: target 0 / 1 / 2 done (sub_3A87D0).
   - 0x0B-0x0F: GW+0x13A above 25600 / 19200 / 12800 / 6400 / 6400 and QW+0x134 at or below it (see UD+0x1C).
   - 0x15-0x19: delivered count (sub_3A8B70(sub_3A8B10(0))) inside a [min, max) pair of 0x44B518.
   - 0x1A-0x23: 20 - sub_3A8A30(0) inside a pair of 0x44B530.
   - 0x29-0x2D: the score-sheet score (sub_D6ABF0 = RW+0x20A4, or -1 when not a 0x200000 quest) at least 0x44B430[n].
   - 0x2E: QW+0x1EC (clear time) within the par time (func_0018D410(quest) x 1800 ticks).
   - 0x2F: the byte QW+0x1C is 0 or less (**guess:** no carts).
   - 0x30-0x93 (99 - sub_3A8A30(0) at least n - 0x30), 0x94-0xF7 (delivered count at least n - 0x94), 0xF8-0x10C
     (20 - sub_3A8A30(0) at least n - 0xF8): only the first row of these three ranges that passes is used.
4. A passing row rolls up to 24 times: roll j succeeds while (ran_suu & 31) < chance[j], chance tables by condition:
   0x8000: 0x44B630 (flag 0x200000), 0x44B63C (flag 0x4000000), else 0x44B620 + luck x 13; 0x8001: 0x44B624 + luck x 9;
   0x8002 / 0x8003: 0x44B628 + luck x 6; 0x2F: 0x44B638; others 0x44B62C. A success picks from the list by weight
   (0x8000 / 0x8001 always take the first entry on roll 0). Entries with item 0 are dropped.
   0x8002 picks go to GW+0x1A0 (3 entries), 0x8003 to GW+0x1B0 (3), all others to GW+0x140 (24 entries).
5. Unless sub_39F9F0: monster rewards from the game-overlay table 0x931060 (8-byte rows {u8 kind, u8 mode, u16 list by
   rank...}, 0xFF ends): a row applies when mode 0x80 and the kind was captured (QW+0x2AC), or otherwise when
   sub_3A2BB0(kind) says yes (**guess:** killed and carved); the list index is chosen by the quest's rank byte
   (QW+0x4C -> +0x5C); lists at 0x930B00; chance table 0x44B634 (or 0x44B638 for mode 0x80); picks go to GW+0x1C0 (16).
6. Each of the four lists is sorted by item id (bubble sort, zeros last).

## 5. The tail of sub_3A0040 (read)

Start: online quest 25001 -> sub_39EDB0 (the tracked-monster update and the UD+0x2980 / +0x297C / +0x297E counters, and a
roll on the hunted record's type and size from the table 0x1BB920 by its time band 31 / 61 / 121 / 181);
else every used UD+0x2928 record grows x1.08 (cap 300). Then every used record re-rolls its state +0x18 (section 1).
Then, in order:
- sub_39FB70: online (lm_net_status_i) -> UD+0x2998 += 1 (cap 9999).
- sub_39FBC0: quest flag 0x200000 -> UD+0x2C04 += 1 (cap 9999).
- sub_39FC00 (not 25001): the hunting log (section 3); then, not flag 0x200000 nor 0x4000000 and the end byte QW+0x154
  is 2: each of the target entries 1 and 2 (QW+0x168 + 8 i) whose flags have 0x30 adds 1 to UD+0x2B5C (cap 9999).
- sub_39FD40(arg): for the three targets with flags 0x32 whose id is in the table 0x44B2D0 ({u16 id, u16 group}), UD+0x2CAC
  [group] += 1 (cap 9999); UD+0x2CCC += captures of em_tsuushin_tbl kinds (cap 9999); arg 0 only: quests 20002 / 20003 ->
  UD+0x2CC8 += 1, quests 20007-20010 -> UD+0x2CCA += 1; sub_3799A0(GW+0x30) = 2 / 3 / 4 / 5 / 6 / 11 -> one of UD+0x2CC6
  / 0x2CBE / 0x2CC0 / 0x2CC4 / 0x2CBC / 0x2CC2 += 1 (**guess:** per-weapon-class clear counts).
- arg 0: sub_39FA80 (online only: UD+0x2B60 bits by the table 0x44C1D0 {kind, bit} for killed or captured kinds).
- arg 0: quests 20007-20010 -> UD+0x2724 += 1 (cap 200).

## 6. Flag-0x200000 quests: the score sheet and unlocks (read)

sub_D6AC30(master), only when target 0 is done (SS = RW+0x1F18):
- SS+0x199C = copy of the master's pouch; each item without Item_data bit 2 is worth price x n / 4 (SS+0x19EC[]), sum
  SS+0x1A3C.
- SS+0 / +4 / +8: main reward QW+0x14 and the two secondary rewards (info block +0x14 / +0x18 per player) when their
  target is done. SS+0x18 = QW+0x1EC (clear time). SS+0x1A40 = par (func_0018D410(quest) x 1800); within par -> SS+0x10 = 1.
  Better than the stored best time func_0018D570(quest) (or none stored) -> SS+0x1A54 / +0x1A58 = 1 and sub_3BD430 stores it.
- 17 category points SS+0x1C[i] = QW+0x1D8 byte D_00D78C60[i] x weight D_00D78C30[i] (per player for the ids sub_D6ABB0
  marks); kill points per kind RW+0x464[k] x 0x44B2F0[k] / players into SS+0x60[k], all also added to SS+0x1C and SS+0x188.
  SS+0x4C = the quest's target kind (QW+0x1E8) was killed; SS+0x50 = category QW+0x1EA (+1 if above 12) scored.
- SS+0x0C = sum of the categories except 12 and 13, x 2.0 if SS+0x4C, x 1.5 if SS+0x50. SS+0x14 = SS+0 + 4 + 8 + 0x0C,
  doubled within par. Score SS+0x18C = (SS+0x1A3C + SS+0x14) x GW-0x0E / 100 (at least 0), copied to SS+0x194; above the
  stored best (func_0018D4F0) -> SS+0x1A4C = 1, sub_3BD3F0 stores it. SS+0x190 = sub_3BD3E0() (**guess:** a running score
  total). SS+0x1A8[] = sheet line ids (0 always, 1 categories, 2 first category, 3 pouch value), count SS+0x19C.
- The score is RW+0x20A4 (= SS+0x18C), read by remuneration conditions 0x29-0x2D.

sub_D70810 (flag 0x200000 only, after a clear or a fail): UD+0x2CAC[group] at 30 or more -> the unlock in D_00D78F60
(one per row of 0x44B2D0); the sum of groups 2, 3 (once) and 5 at 30 or more -> unlock 0x81; rows of D_00D78F70 {group,
unlock} at 100 or more; SS+0x190 + SS+0x18C at 200000 -> 0x88; all 12 ids of D_00D79230 pass sub_3BCEC0(UD, id) -> 0x89;
SS+0x190 + SS+0x18C at 15000 -> 0x8A; all quests of D_00D78F90 cleared -> 0x8B; all of D_00D78FB0 -> 0x8B again (same id
in the code); all of D_00D78FD0 -> 0x8C. Each unlock goes through main 0x18E700 and the list push sub_D6FE90.

## 7. sub_D74320 (online, read)

Calls sub_D74260 on three tables (D_00D79310 / 79320 / 79340, ids 0 / 1 / 2; not read). Then, online and quest 15017 and
session bit 0x31 clear: sets bits 0x31-0x34 (sub_3BD970) and queues event codes 0x700, 0x800, 0x900, 0xA00. Mode 7 sub 1
sets the same four bits.

## 8. Mode 7 (sub_main 0x327820, read)

Sub-step GW+1: 0: GW+2 = 0 -> Event_flag_set(0x185), movie 7 (sub_3D4070 -> sub_3D3CD0 with entry 7 of 0x4683C0, rows
{id, 1, 10000, 0x1C0 / 0x200, ...}), bit 0x25; GW+2 = 1 -> movie 8, GW+3 = 1 if bit 0x26 was clear, bits 0x26 and 0x31-0x34.
1: wait for the movie (sub_3D40D0 = 3 ends it; while 2, Start skips), fade, stop. 2: GW+2 = 0 -> fade, mode 6; else
sub_D64C80 (a plsel scene). 3: sub_D64D40 until done; GW+3 -> a 150-tick page (step 4), else mode 6. 4: the lines of the
pointer list 0x447CC4 centred ((640 - 9 x len) / 2), y from 160 by 22, size 18; Circle after 60 ticks ends early; then
fade and mode 6. The movie-table reading is solid (the 0x1C0 = 448-line height); "ending and staff roll" is a **guess**.

## 9. Updates to the port plan of result_screen.md (a list; result_screen.md itself not edited)

1. Fix: section 2's "Kill counts" note is wrong. Port sub_3A0040 whole (offline parts) in step 0, cleared **and**
   failed paths: hunting log UD+0x23D2 / +0x24D2, the counters of section 5, the UD+0x2928 growth and state roll
   (uses ran_suu: route it through the host's ran_suu so runs stay reproducible).
2. Fix: RW+0x464 / +0x4F8's second count is captures (QW+0x2AC from sub_3A26B0). The host must call MH2's
   Quest_enemy_die and capture paths so QW+0x20C / +0x2AC / +0x34C / +0x3EC are filled (test aid: write them directly).
3. remuneration_item_set: new host C from 0x3A9FF0 (section 4), not G's. Needs the game-overlay tables 0x931060 /
   0x930B00 (read with rt_addr from the loaded game image, or listed in config/mh2/pc_data.txt) and 0x44B518 / 0x44B530 /
   0x44B430 / 0x44B620-0x44B63C. The reward list (reward_mv) must read four lists: GW+0x140 (24), +0x1A0, +0x1B0, +0x1C0.
4. Pages: replace the "plain host page" stand-in with one host text-page engine = RW+0x748..0x758 buffer + the three
   drawers of 2.2 / 2.4 + the builder of 2.3, taking formats from the disc's pointer tables (D_469EB0, D_469FA0, D_445620,
   D_464770, D_447750, D_447F40, D_447CB0 / CE0 / D80) by address. Needs host han2zen, sprintf with the game's formats,
   the font's colour escapes, flfntSetSize 18 / 24 / 28 and message type 0x31. Build page ids 0, 1, 2, 3, 5 first; 4 and
   7 are online.
5. Money page input as 2.5 (left / right pages, up / down sale scroll by 7, Circle closes via the 0x6C0 / 0x6C1 states);
   the money page is rebuilt every tick, so UD+0x60 on page 0 always shows the live value.
6. Record page: per-species scroll on RW+0x1F0; sizes shown through sub_3BBB50(kind, size) (host C or the shared C if
   it exists); the "new" marks follow the compare rules of 2.2.
7. Bounty values: before a quest the host must run main 0x18D6A0 (0x5D4790 <- UD+0x2808); on a new save main 0x18DAE0
   (UD+0x2808 and the ring UD+0x27B8 from 0x1BB2A0 / 0x1BB380). Without it every bounty line pays 0. The village
   recalculation (lobby 0x710E50) can wait; log a stand-in.
8. Flag-0x200000 quests: the score sheet can stay a stand-in, but it must leave RW+0x20A4 so that sub_D6ABF0 gives -1 for
   other quests (it checks the flag itself, so a zeroed RW is enough) and should skip sub_D70810's unlocks when absent.
9. Quest flag 0x20 stages: port sub_3A09F0 (UD+0x1C slots) in the cleared path; GW+0x13A / QW+0x134 must come from the
   quest code (writer not found; for now a test aid).
10. Mode 7 stand-in: no movie or scene on the PC yet, but it must do the story-state writes of sub 0 (event flag 0x185,
    bits 0x25 / 0x26 / 0x31-0x34 and GW+3), then optionally the 0x447CC4 page, then mode 6. Log it with rt_log_standin.
11. Online-only, skip with logged stand-ins: sub_D74320, page ids 4 and 7, sub_39EDB0, sub_39FB70, sub_39FA80.
12. Tests to add to result_screen.md's list: after a kill of kind 1 and a capture of kind 2 (test aid writes QW+0x20C[1] /
    +0x2AC[2]), check UD+0x23D2[1] and UD+0x24D2[2] rise by 1 on both a clear and a time-up; check GW+0x140.. is non-empty
    and sorted after a clear of 1014 (rows from QW+0x70); check UD+0x2928 records (if any on the save) grow x1.08; check the
    money page lines against the RW values traced.
