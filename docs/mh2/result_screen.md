# MH Dos (MH2): the quest result screen, plsel 0xD683C0 (RESULT research, 4 Nov 2026)

- **Scope:** a reference for porting MH2's result screen. Nothing was built, run or changed; no C was written.
- **Source:** our own disassembly of the owner's disc (asm/mh2/plsel/text/plsel_D67648.s for 0xD683C0, asm/mh2/sub_main,
  asm/mh2/main), read by hand with throwaway scripts in the scratchpad. Compared with our G C (src/main/reward/f_reward_g.c,
  f_reward4.c) and MH1 C. The community mh2-decomp repo was not opened.
- **Proof level:** everything below is a reading of the code, not seen running. "Read" = followed instruction by instruction;
  "skimmed" = only its calls, globals and constants were looked at; **guess** = a meaning given to something the code does not
  name. Names in `code` without an address are ours (G/MH1 sources); `sub_X` is an unnamed MH2 function.

## 1. Where it sits

game5 (sub_main 0x3274F0) calls plsel 0xD683C0 every tick (config/mh2/c_aliases.txt already names it `result_prog`). It is a
switch on game_w.step (byte game_w+1, 26 cases, jump table plsel 0xD7BAD0; a step above 25 skips to the common tail). Every
tick it first calls font_set_stack_no(4); after the switch, if the frame callback card_prim+0x14 ("TRANS", 0x6A3A24) is set,
it queues card_prim with add_prim2(ot 0x6A9AF0, card_prim, 0, 1). Same frame as G's result_prog.

When it leaves, game_w.mode becomes 6 (sub_main 0x327770: stop sounds, all_reset, back to the village) or 7 (sub_main 0x327820:
sets event flag 0x185 and friends, fades, prints a message from the table at 0x447CC4; **guess:** a one-time story notice after
certain quests). The task switch is jtbl 0x46CD70 (modes 0-5 game0..game5, 6 -> 0x327770, 7 -> 0x327820).

## 2. Data it touches

### Globals (sub_main addresses)

| what | address | used for |
|---|---|---|
| game_w | 0x6ADCB0 | +0 mode, +1 step, +2 sub, +4 x04 (s16 fade wait), +6 x06, +0x0A x0A (cleared at start), +0x3C master, +0x3E players (G pl_num), +0x40 end state (G x0D5; 7 / 8 special ends), +0x41 online flag read by 0x36BA50, +0x140 reward_item[] (read by the reward list), +0x270 a byte read on online paths (**guess:** G's x21A "in a session") |
| quest_w | 0x5D8260 | see the field table below |
| User_data | 0x5D4A60 | see the field table below |
| pouch backup | 0x5D4A00 | 0x50 bytes = 20 pouch slots, just below User_data (G's ud_pouch_backup). In config it carries the byte-match name `Friend_data` |
| card_prim | 0x6A3A10 | two 0x20-byte prims cleared at start; +0x14 the frame callback |
| stage_work | 0x5EC460 | x01 cleared at start (as G) |
| quest_price | 0x5BC4A0 | saved to result_w+8 and zeroed in step 0 (normal ends); Quest_price_return on special ends |
| system_w | 0x6ADF90 | +0x88 (0x6AE018) points at the result work (below); +0xE0 (0x6AE070) = 1 when mode 7 sub 0 is chosen |
| PitMenu | 0x5BB080 | +0x2C/+0x2D/+0x30 window state set by the screen inits (via sub_363190, a window open with a -180 slide) |
| Item_data | 0x45F110 | 0x18-byte rows; +4 flag byte (bit 4: removed at quest end; bit 2: handed in and sold), +0x10 price |
| player_work | 0x5F89D0, 0x970 stride | master's pouch at PLW+0x77C (20 x {u16 id, s16 n}); PLW+0x876 byte read as the death count |
| hunt_group_tbl | 0x44C800 | sub_3BC090(kind) = first kind of the kind's species list (the "species" index used everywhere below) |
| clear-bit quest list | 0x44C3A0 | u16 quest numbers, 60000 ends; a quest's index = its clear bit |
| bit set 0x6ADAB4 | 0x6ADAB4 | sub_3BD9C0 test / sub_3BD970 set. **Guess:** per-session "already announced" bits (it is not inside User_data) |

### The result work ("RW", 0x3B88 bytes)

system_w+0x88 holds a pointer set at boot (sub_main 0x2E4570) to the fixed buffer 0xBDA200; step 0 memsets 0x3B88 bytes of it.
The same pointer is the `PLSEL_W *D_006AE018` of src/mh2/plsel/p00D6FE90.c (that struct only names +0x39FC count / list) and
p00D6F8B0.c (its memset). Offsets seen in this screen (all read unless marked):

| RW+ | type | meaning |
|---|---|---|
| 0x04 | s32 | money total paid on the money page (sum of 0x08, 0x0C, 0x10, 0x14) |
| 0x08 | s32 | quest money (fee part, main reward, secondary rewards; sub_D6F8D0) |
| 0x0C | s32 | bounty money (sub_D6FD00) |
| 0x10 | s32 | added into the total; no writer found in this screen's callees |
| 0x14 | s32 | sale of handed-in items (sub_D6FDB0) |
| 0x18..0x2C | s32 flags | reward-changed, combine (0x20), target 0 / 1 / 2 paid (0x24, 0x28, 0x2C) |
| 0x30 | u16[] | record list: species with a new size record (sub_D67680 adds once); count at 0x1EC |
| 0xC4, 0x158 | u16[74] | per species: biggest / smallest record kind (1 first ever, 2 beaten) |
| 0x1F4 | u16 | any record set |
| 0x33C, 0x3D0 | u16[74] | per species: biggest / smallest record before this quest |
| 0x464, 0x4F8 | u16[74] | per species: kills / second count (**guess:** captures) this quest, from quest_w+0x20C / +0x2AC |
| 0x58C, 0x620 | u16[74] | copies of the two counts, used up by the bounty list |
| 0x6B4..0x6C8 | | page / timer state of the text pages (0x6BC: s32 3600-tick timeout per page; 0x6C0 byte; 0x6B8 page count) |
| 0x6F4..0x70C | s32 | money parts (0x6FC / 0x700 are float factors 1.0 or 2.0) |
| 0x710..0x73C | s32 | hunter points (online only): deaths, penalty, base, rank before / after, up or down, HRP total |
| 0x1F18.. | | score sheet of flag-0x200000 quests (sub_D6AC30; skimmed) |
| 0x397C, 0x3984 | s32, u16[0x39] | event list: codes shown on the event pages (see step 16) |
| 0x39F8 | s32 | sale line count |
| 0x39FC, 0x3A00 | s32, s16[32] | unlock list: ids unlocked this quest (sub_D6FE90 adds, at most 32) |
| 0x3A40, 0x3AE0 | u16[74] | per kind: biggest / smallest size seen this quest (copied out of quest_w) |
| 0x3B80 | u8 | online low-rank cut applied (pay x 60 / 100) |
| 0x3B81 | u8 | the quest's own bounty was met (fee part paid at 2.0) |
| 0x3B82 | u8 | event code 0x500 already queued |
| 0x3B83 | u8 | flag-0x200000 quests: was the quest cleared before (Quest_clear_bit_ck at start) |
| 0x3B84 | s32 | hunter point percent (100) |

### result_w (plsel 0xD7BD90, 0xC4 bytes, memset in step 0)

The MH2 form of G's GRESULT_W (f_reward_g.c `gres`). +0x08 quest_price at departure; +0x10 handed-in flag; +0x11 bounty flag;
+0x12 last species checked by sub_D67B20; +0x16 handed-in items, 20 x {u16 id, s16 n}; +0x66/+0x68 the delivery item shown;
+0x6A bounty lines, 8 x {s16 type, s16 id, s16 n, s16 value} (type 0 species bounty, 1 kills, 2 captures, 3 delivery item).

### reward_w (plsel 0xD7BE60)

The reward list's state, same use as MH1's REWARD_W (include/reward.h) with other offsets: +0 mode, +1 list index, +2 sub,
+3, +4/+5 cursors, +8 s16 time left (3600 at init; the list returns it, <= 0 = done), +0x0A/+0x0B, +0x0C, +0x10 s32 money from
selling, +0x14. Set by sub_D695A0 (reward_init).

### quest_w fields (MH2 offsets; include/mh2_layouts.h names where they exist)

| quest_w+ | meaning here |
|---|---|
| 0x08 no | quest number |
| 0x14 x14 | main reward money now (compared with info+0x0C, the base reward) |
| 0x1EC | a time value compared with a per-quest best time (flag-0x200000 quests; **guess:** clear time) |
| 0x1DC..0x1E1 | per-category kill tallies, filled by sub_3A6E30 for flag-0x200000 quests (category table 0x44B390) |
| 0x1F4..0x20B | three bonus entries {s16 type, s16 id, s16 per, s16 value}: types 1 kills, 2 captures, 3 delivery item. **Guess:** mission-file "sub rewards" |
| 0x20C x1D4 | u16[74] kills per kind this quest |
| 0x2AC | u16[74] a second per-kind count (**guess:** captures) |
| 0x34C x16C | u16[74] biggest size seen per kind this quest (zeroed after use) |
| 0x3EC | u16[74] smallest size seen per kind this quest (new in MH2; zeroed after use) |
| 0x4C | pointer to a block whose +0x4C/+0x50/+0x54/+0x58 are hunter points (target 0 / fail / target 1 / target 2) |
| 0x7C (Q_INFO) | the .mib info block: +0x08 a money value (fee part, **guess**), +0x0C base reward, +0x14 / +0x18 secondary rewards; via the block 0x5D82DC also +4 the quest level |
| 0x28 (Q_FLAGS) | 0x200000 (sub_36BA50), 0x4000000 (sub_39F970), 0x400000 (sub_39F9D0) |
| 0x53C | an id for the online-only step 1 |

### User_data fields (MH2 offsets from 0x5D4A60)

| UD+ | meaning here |
|---|---|
| 0x1C.. | per-slot words written by sub_3A09F0 (**unknown**) |
| 0x5C | hunter points total |
| 0x60 | money (Gold_add_mh2 sub_3B6500: clamps 0..9,999,999; unlock 0x1E at 2,000,000) |
| 0x1FFC | quest clear bits (by the index in 0x44C3A0) |
| 0x2040 | the pouch (UDMO(G 0xC24)); restored from 0x5D4A00 on special ends |
| 0x21D2 / 0x22D2 | u16 per kind: biggest / smallest size record (sub_3BBA50/3BBA90 set, 3BBAF0/3BBB10 get, 3BBBC0/3BBCE0 species best) |
| 0x2724 | a counter raised by one (max 200) after quests 20007-20010 |
| 0x27B4 | u16 title bits (sub_18E810; all 11 low bits set -> bit 11) |
| 0x27B7, 0x27B8 | five 16-byte rings of hunted kinds by group (main 0x18DBB0; groups from the table 0x1BB2A0). **Guess:** a "recently hunted" list |
| 0x2928 | three 0x1C-byte records (in use when +0xA is non-zero; +8 grows x1.08 per quest to 300, +0x18 rolled from it) by sub_3A0040. **Unknown** what they are |
| 0x2CCC | u16 compared with 100 for unlock 0x63 (**unknown**) |
| 0x3660 | the unlock bit set (sub_3BD560 test / sub_3BD550 set). Not Omake_flag_ck (0x3BD4F0) |

**Kill counts:** corrected in docs/mh2/result_screen2.md: step 0's sub_3A0040 -> sub_39FC00 adds this quest's kills
(QW+0x20C) and captures (QW+0x2AC; the "second count", a shape match with G's Quest_enemy_capture) into UD+0x23D2 /
UD+0x24D2 per kind, on both the cleared and the failed path. Its port plan (section 9) supersedes section 4 here where they differ.

### Tables the port reads from the disc (never copied into the repo)

plsel: 0xD78C80 event-flag list (u16 code, u16 event flag; 0x39 rows), 0xD78D10 online bonus chances, 0xD78D80 hunting-log
unlock rows (4 bytes, 0xFF ends), 0xD78E70 quests that queue an event code on first clear, 0xD78F00 / 0xD78F20 hunter-rank
thresholds and their unlock ids, 0xD78F60..0xD79010 / 0xD79230 quest lists for unlocks, 0xD78C30 / 0xD78C60 score weights;
sub_main: 0x44B2F0 score per kind, 0x44B390 kind category, 0x44C3A0 clear-bit list, 0x5D4790 bounty value per species (in RAM,
read by main 0x18D660; who fills it is not known), text tables 0x447750 / 0x447F40 (unlock names), 0x447CB0 / 0x447CE0 /
0x447D80 (event texts), 0x469F14.. (page texts).

## 3. The 26 steps

Helpers with names: Quest_price_return = plsel 0xD67530 (MH1 rwkey01.c, linked in), end_fade_set = 0xD67620 (MH1/G
f_reward.c part 2), trans_result_2 = 0xD675D0, pouch_restore = 0xD67630 (bytes match lb_bz73.c's cpn_PutCNData; it copies the
0x50-byte pouch backup at 0x5D4A00 to UD+0x2040, which is G's pouch_restore, 0x4F4190), reward_init = sub_D695A0,
reward_mv = sub_D69630 (both read), Gold_add = sub_3B6500, Hunter_point_add = sub_3B6B40 (jumps to Hunter_point_add_sub),
Quest_clear_bit_set = sub_3B9480. se_req wrappers: 0xD67650 (sound 0x13), 0xD67660 (0x17), 0xD67670 (0x14).

"Next page" below = the shared pattern: a wait counter game_w+4 runs out, then the page's own timeout RW+0x6BC (3600 ticks)
counts down; the step moves on when the timeout ends or the page's input function says done; then fade_set(1), game_w+4 = 30.

| step | what it does | moves on | G result_prog |
|---|---|---|---|
| 0 | quest end: see 3.1 | to 1, 3, 5, 10, 16, 21, 23 or 25 | 0 (much extended) |
| 1 | online only: a bonus page (sub_D72290, init sub_D721F0; reads quest_w+0x53C and the three UD+0x2928 records). **Guess:** an online-only extra | page done -> 2 | new |
| 2 | fade wait 30; cleared -> reward_init, step 3; else load textures (sub_D72E70), copy the pouch to User_data (ItemCopy_Pl2Ud), money init (sub_D70FD0), step 5 | x04 = 0 | new |
| 3 | the reward list: reward_mv (sub_D69630; list, pick into pouch, box, sell for money with Gold_add); TRANS = sub_D67570 (draws sub_D727E0 while step is 1 or 2, else disp_reward) | reward_mv <= 0: fade 1, str_fadeout(0, 30), ItemCopy_Pl2Ud, step 4 | 1 |
| 4 | fade wait 30; textures; str_fadein(0, 30); flag-0x200000 quest -> step 20; online quest 25001 -> sub_D702F0, then step 10 (game_w-0x19C (0x6ADB14) = -1) or 21 (sub_D720B0); else money init, step 5. TRANS = sub_D71110 (backdrop) | x04 = 0 | 2 |
| 5 | the money page: sub_D71BD0 (input, page flips), sub_D71660 (text). Money was already paid in the init (see 3.2) | next page -> 6 | 3 (G counts the money up) |
| 6 | fade; timeout reset | x04 = 0 -> 7 | 4 |
| 7 | offline -> step 10; online: hunter point page init (sub_D71FB0), jingle 0x1D if the rank went up (RW+0x734 = 1) | at once | (G 4/7) |
| 8 | the hunter point page (sub_D71FE0 input, sub_D71660 text) | next page -> 9 | 7 |
| 9 | fade | -> 10 | 8 |
| 10 | unlock list empty (RW+0x39FC = 0) -> step 13 (TRANS off); else build the unlock texts (sub_D73930 from 0x447750 / 0x447F40), jingle 0x1D, step 11 | at once | new (G's training / gallery announcements are the nearest) |
| 11 | the unlock page (sub_D73BB0 input, sub_D73D10 text) | next page -> 12 | new |
| 12 | fade | -> 13 | new |
| 13 | record list empty (RW+0x1EC = 0) -> step 16; else the record page init (sub_D71DF0: page text, jingle 0x1D), step 14 | at once | 9 (G: "hunting record updated") |
| 14 | the record page (sub_D71E50 input, sub_D71660 text) | next page -> 15 | 9 |
| 15 | fade | -> 16 | 10 |
| 16 | event list empty (sub_D73280: no non-zero RW+0x3984 entry) -> end_fade_set, step 25, TRANS off; else event texts (sub_D732D0), step 17 | at once | 13-16 (G's gallery) are the nearest |
| 17 | the event page (sub_D734B0 input, sub_D73610 text) | next page -> 18 with fade_set(10) | new |
| 18 | x04 wait (drawing) then end_fade_set, step 25 | x04 = 0 | new |
| 19 | sets step 20; nothing in result_prog sets 19 (not searched for elsewhere) | at once | new |
| 20 | flag-0x200000 quests: the score sheet (sub_D6C660 input, pays with Gold_add; sub_D6CCC0 text) | done -> 10 | new |
| 21 | online 25001: a page (sub_D721A0) | next page -> 22 | new |
| 22 | fade | -> 10 | new |
| 23 | error end (state 8): error_disp; x04 (150) wait | -> 24, fade 1 | 17 |
| 24 | error_disp; x04 wait | mode 6, step 0, all_reset | 18 |
| 25 | the last fade: x04 wait (30), then system_w+0xE0 = 0 and: sub_D68270 -> mode 7, sub 0, system_w+0xE0 = 1; else sub_D68330 -> mode 7, sub 1; else mode 6, step 0; all_reset | x04 = 0 | 19 |

sub_D68270 (offline only): quest cleared, event flag 0x184 set, 0x187 and 0x185 clear, and the byte at 0x5D8148 is 0.
sub_D68330 (offline only): quest cleared, quest 5007, event flag 0x58 clear. (Read. Which story notices these are: **guess**.)

### 3.1 Step 0 in order (read)

Common start: memset result_w (0xC4); main 0x195230 (a jump to main 0x18F0C0, not read); game_w.x0A = 0; stage_work.x01 = 0;
result_w+0x12 = 0; card_prim cleared; main 0x18D770 (fills a summary at 0x5D4920: quest number, end state, a stage group, two
bytes from the info blocks, and clears its member lists unless the master cleared it; **guess:** a "last quest" record for the
guild card or the network); sub_3A6E30 (flag-0x200000 quests only: kill tallies by category); RW memset; sub_D74060(RW) (per
species kills and second counts this quest into RW+0x464 / +0x4F8 and copies at +0x58C / +0x620).
Flag-0x200000 quest: RW+0x3B83 = Quest_clear_bit_ck(quest), the score sheet (sub_D6AC30(master), skimmed), ItemCopy_Ud2Pl(master).
Flag-0x4000000 quest: ItemCopy_Ud2Pl(master) (**guess:** the quest's own items are dropped by restoring the pouch).

**End state 8** (error): step 23, sub 0, x04 = 150, x06 = 1, Quest_price_return, fade_set(2), TRANS = trans_result_2,
pouch_restore. Same as G.
**End state 7** (special end): the event list (sub_D72F70), Quest_price_return, pouch_restore; event list empty -> step 25,
x04 = 30, fade_set(10), end_fade_set; else textures, step 16, TRANS = sub_D71110, fade_set(2). G instead went to its gallery
check.

**Every other end:**
1. The event list (sub_D72F70: each table row whose event flag is set and whose bit in 0x6ADAB4 is clear sets the bit and is
   queued), all_sprite_free, TRANS = sub_D67570, result_w+8 = quest_price, quest_price = 0.
2. Pouch items whose Item_data flag has bit 4 are removed (Pl_item_stack with minus their count). G removed by bit 0x10.
3. sub_D67C30: the bounty lines (3 quest bonus entries against the species counts; then every species with a bounty value at
   0x5D4790; then the delivery item taken from the pouch). sub_D67FE0: every pouch item with Item_data bit 2 is taken out and
   listed for sale (G's handin_items, by flag instead of G's five fixed ids).
4. Not flag-0x4000000 and not flag-0x200000: every species with kills or second count > 0 goes into the hunted ring
   (main 0x18DBB0).
5. sub_D676E0: the size records, both biggest and smallest (G had only biggest): old species records kept in RW, this quest's
   sizes copied into RW and cleared in quest_w, User_data records raised (or lowered), each species with a first or beaten
   record goes on the record list.
6. Online 25001: sub_D72C80 (clears RW, an unlock, more), pouch backup copied back into the master's pouch (sub_3BC5A0(pl, 1)),
   sub_39ED30 (one item taken from the pouch).
7. **Cleared:** sub_3A0040(0) (the UD+0x2928 records, the 20007-20010 counter, sub_39FB70 / 39FBC0 / 39FC00 / 39FD40 /
   39FA80, not read); the clear bit (online: only if flag 0x400000 is clear or game_w+0x270 is set; offline always;
   Quest_clear_bit_set sets it only when target 0 is done); flag 0x4000000: sub_D70C00 (all quests in 0xD78FD0 cleared ->
   unlock 0x8C); flag 0x200000: sub_D70810 (quest-list unlocks 0x81, 0x88-0x8C), sub_D67B20, sub_D70CA0 (hunting-log and crown
   count unlocks 0x63-0x6C, title bit 9); sub_D731D0 (event code 0x500, unlock 0x7A); target 0 done: sub_D74320 (online, codes
   0x700-0xA00) and sub_D730F0 (first clear of a listed quest: code 0x300 + row, quest 10103 also 0x400); sub_3A09F0;
   remuneration_item_set (sub_3A9FF0, fills game_w.reward_item; paired with G 0x4B90A0 at score 0.53, not read).
   Then online and sub_D68130 says yes (a chance from 0xD78D10) -> step 1 with sub_D721F0; else step 3 with reward_init.
   x04 = 30, fade_set(2).
8. **Not cleared:** sub_3A0040(0), sub_D731D0. Flag 0x200000 -> textures, sub_D70810, sub_D67B20, sub_D70CA0, step 10, TRANS
   off. Online: quest 25001 -> textures, ItemCopy_Pl2Ud, sub_D70760, sub_D720B0, step 10 or 21; else sub_D68130 -> step 1.
   Otherwise (the offline failed / time-up case): textures, step 5, ItemCopy_Pl2Ud, money init (sub_D70FD0), TRANS =
   sub_D71110, fade_set(2), x04 = 30. G went to step 3 (gold_init) here.

### 3.2 The money init, sub_D70FD0 (read)

MH2 pays the whole sum at once with Gold_add and then only shows the lines (G counted the money up while drawing).
1. sub_D6FF90 (online only): hunter points. Target 0 done: points from the block at quest_w+0x4C (+0x4C), target 1 (+0x54),
   target 2 (+0x58); failed: +0x50. Quest below 50000, hunter rank 31 or more and quest level below 5: x 60 / 100 and RW+0x3B80
   = 1. Minus 20 per death of the master (PLW+0x876 of the master, 0x5F9246 for player 0). Times RW+0x3B84 percent. Hunter_point_add with
   the total; rank before / after; rank thresholds unlock ids (sub_D6FED0).
2. sub_D6FD00: bounty lines of type 0-2: RW+0x0C += n x value / players; RW+0x3B81 = 1 when a line is the quest's first bonus
   entry.
3. sub_D6F8D0 (cleared only): RW+0x6F4 = info+0x08, RW+0x6F8 = departure price minus that (at least 0), factors 1.0 (online
   with game_w+0x270: 2.0; RW+0x3B81: 2.0 for the second part); target 0 done: main reward quest_w+0x14 (RW+0x18 = 1 if it
   differs from the base info+0x0C); targets 1 / 2 done: info+0x14 / +0x18 divided by the players; online low-rank cut x 0.6;
   all into RW+0x08. **Guess:** info+0x08 is a refund of the contract; not checked against a quest file.
4. Online with 2+ players: RW+0x1C = 1; low-rank cut on RW+0x0C.
5. sub_D6FDB0: RW+0x14 = sum of Item_data price x count of the handed-in items, plus the value of a delivery-item line.
6. sub_D702F0 (online 25001 unlocks, skimmed), sub_D67B20, sub_D70CA0 (unlocks, see step 0).
7. RW+0x04 = 0x08 + 0x10 + 0x0C + 0x14; Gold_add(RW+0x04); page 0, 4 pages, timeout 3600; sub_D6D610 builds the page text
   (2,166 instructions, not read).

Unlock (main 0x18E700, read): if the unlock bit is clear, set it and show a message (set01_set2 with the pointer at
0x446A24); returns 1 if newly set. Each caller then adds the id to the unlock list shown at step 11.

## 4. Port plan

The PC is offline, so the first target is the offline paths: cleared (0 -> 3 -> 4 -> 5 -> 6 -> 7 -> 10 -> 13 -> 16 -> 25)
and failed / time-up (0 -> 5 -> 6 -> 7 -> 10 -> 13 -> 16 -> 25). Online steps 1, 2, 8, 9, 21-24 and flag-0x200000 step 20
can stay stand-ins that skip to their next step, each logged with rt_log_standin.

**What can carry from G (f_reward_g.c) with MH2 changes:**
- The step skeleton, `announce`-style page start, `to_village`, the special ends 7 / 8, `pouch_restore` (UD+0x2040 <-
  0x5D4A00 by UDMO), `Quest_price_return`, `end_fade_set` (already shared C).
- `handin_items`: change to the Item_data flag-bit-2 rule and 20 slots; the bit-4 removal loop (G bit 0x10).
- `records_update`: extend to the smallest records, 74 kinds, MH2's quest_w (+0x34C / +0x3EC) and User_data (+0x21D2 /
  +0x22D2) places, and the first / beaten kinds (sub_D676E0).
- reward_init / reward_mv (f_reward4.c, MH1 form): MH2's sub_D69630 has the same state machine shape; reward_w moves to MH2's
  layout and gains the sell-for-money branch and the flag-0x200000 branch. Best done as `#ifdef VERSION_MH2` in f_reward4.c
  once that is checked against 0xD69630 instruction by instruction (not done here).
- Quest_clear_bit_set: MH2's form is small (list 0x44C3A0, bits at UD+0x1FFC, only with target 0 done); host C.

**What needs new C (host C first, in src/pc/rt/rt_mh2_result.c, replacing rt_mh2_quest.c's stand-in):**
1. RW as a host buffer of 0x3B88 bytes (or a struct with the offsets above), and result_w / reward_w as host objects.
2. Step 0 (3.1) without the online parts.
3. The money init (3.2) offline part: sub_D6FD00, sub_D6F8D0, sub_D6FDB0, Gold_add_mh2 (UD+0x60 clamps, unlock 0x1E).
4. The unlock machinery: main 0x18E700, UD+0x3660 bits, the RW+0x39FC list; the offline unlock checks sub_D70CA0,
   sub_D731D0, sub_D730F0, sub_D70C00, sub_D6FED0 (online only, skip).
5. The event list (sub_D72F70, sub_D73280, 0x6ADAB4 bits).
6. The pages: until sub_D6D610 / sub_D71660 / sub_D73D10 / sub_D73610 are read, a plain host page that prints the numbers
   (money lines, record species and sizes, unlock ids, event codes) through the host font is enough to see the logic work.
7. The end choice of step 25 (sub_D68270 / sub_D68330) and a mode-7 stand-in that goes to mode 6.

**What the host must provide:** the plsel image loaded (rt_add_overlay already does) so its tables are read with rt_addr();
the sub_main tables above (config/mh2/pc_data.txt entries: the clear-bit list 0x44C3A0, 0x44B2F0, 0x44B390, text tables);
MH2 places for Event_flag_ck / Pl_item_stack (sub_3B1E80) / ItemCopy_Pl2Ud / ItemCopy_Ud2Pl on MH2's PLW (pouch +0x77C);
quest_w+0x20C / +0x34C / +0x3EC filled during the quest (check that the host's Quest_enemy_die writes MH2's places); the
textures of sub_D72E70 (load_file_mdl 0x26 and 0x22) or a stand-in backdrop.

**Order and tests** (hidden runs, RT_QUEST_TRACE=1, `build/mh2/pc/mhview_2 disc/mh2 --quest 1014`):
1. Step machine with every page as a 60-tick stand-in, tracing each step change. `RT_QUEST_TIME=200 --input "idle*30" --time 40`:
   expect 0 -> 5 -> 6 -> 7 -> 10 -> 13 -> 16 -> 25 -> mode 6. `RT_MH2_TARGET=0@100 --time 110`: expect 0 -> 3 -> 4 -> 5 ...
   (step 3 needs reward_mv to end: let the stand-in end it after its timer).
2. Money: trace RW+0x04..0x14 and UD+0x60 before / after. Time-up: no quest money, only sales of flag-bit-2 items (start with
   none: 0). Clear of 1014: main reward 2200 in RW+0x08 (plus info+0x08 part); check UD+0x60 rises by RW+0x04.
3. Hand-in and removal: put a flag-bit-2 and a flag-bit-4 item in the pouch before the end (test aid), check both leave it
   and the first is listed and paid at its price.
4. Clear bit: after the clear, UD+0x1FFC bit at 1014's index in 0x44C3A0 set; Quest_clear_bit_ck(1014) = 1; not set on time-up.
5. Records: needs a monster kill with a size (RT_MH2_EM=1 and a test aid that writes quest_w+0x34C[1] / +0x3EC[1]); check
   UD+0x21D2[1] / +0x22D2[1] and the record list.
6. Unlocks and events: trace each 18E700 call and list push; on a fresh save the first clear should queue whatever 1014's
   rows give (expect few or none).
7. reward_mv with real input (`--input` presses) once f_reward4.c's MH2 form exists.

## 5. Not known / guessed (in one place)

- What flag 0x200000 quests are (score sheet, best time, rental gear restored): **guess** training / challenge quests.
- What mode 7 shows; what the 0x184 / 0x185 / 0x187 / 0x58 flags and quest 5007 mean.
- info+0x08 as a contract refund; the "second count" as captures; the 0x5D4790 bounty values (who fills them).
- The UD+0x2928 records, UD+0x1C slots (sub_3A09F0), UD+0x2CCC, the hunted ring at UD+0x27B8.
- Step 1 / 21 contents (online); sub_D72290, sub_D6D610, sub_D71660 and the other page builders were not read, only skimmed.
- remuneration_item_set (sub_3A9FF0), sub_3A0040's tail calls, sub_D6AC30, sub_D70810, sub_D74320, sub_D702F0: skimmed.
- Where MH2 adds hunting-log kill counts (not in this screen).
