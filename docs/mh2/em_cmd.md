# MH Dos: the monster command scripts (em_cmd) against G's

Research only (4 Nov 2026), for the em_cmd blocker in docs/mh2/shared5.md: on stage 120 with `RT_MH2_EM=1` the Rathian's
program hangs in em_cmd_stage_no_ck -> cmd_end_search. Written from our own disassembly of the owner's discs (build/mh2/*.elf,
build/mhg/game.elf) and our C (src/game/em/em_cmd_nm.c, src/mh2/game/em_cmd_ck.c, src/mh2/game/next_cmd_search.c). Community
mh2-decomp not opened. Nothing was built or run. Names are from our MH1/G sources; unnamed functions are sub_<addr> (the
symbol files call them game_func_/sub_main_func_<addr>; em_cmd_ck.c's placeholders em_cmd_op1F/op37/op73/op76 are kept).

How each claim was checked, so the reader knows what to trust:
- **[len]**: operand lengths from MH2's next_cmd_search (game 0x731DA0, byte-matched C) and G's (game 0x58E170). Both were run
  through a small scratch MIPS interpreter on every opcode x sub-byte (0-15, 255) and the results diffed; MH2's agrees with its
  matched C. Solid.
- **[ck]**: opcode -> handler from MH2's em_cmd_ck (game 0x726380, byte-matched C) and G's (game 0x584780), both read off the
  compare chain in the asm. Solid.
- **[carry]**: the handler's address range is in config/mh2/c_files.txt, i.e. our shared MH1/G C rebuilds MH2's bytes
  unchanged (tools/mh2_carry.py). Solid: "same as G" in logic, with MH2's EMW offsets.
- **[shape]**: MH2's and G's handlers compared by instruction mnemonics (difflib ratio). 1.00 means same code shape with other
  offsets; it is good evidence, not proof.
- **[read]**: read by hand from the asm for this doc. Plain-language summary; details marked "guess" were not traced.

## 1. The command stream

A program is a byte stream (EMW.cmd_pc); every command is an opcode byte, then operands. Conditions and selects use a
sub-byte after the opcode: 0 = the test (with its operands), 1 = else / next entry, 2 = end (selects: 2 = else, 3 = end).
cmd_end_search (MH2 0x731CA0, G 0x58E070) and else_ck (0x733150 / 0x58F1A0) are the same in both games [shape 1.00]; they
step over commands with next_cmd_search, so **every operand length must be exactly right or the pointer drifts**. An opcode
next_cmd_search does not know returns the pointer unchanged (it only sets the abort byte EMW+0x95E, MH1 0x9DA, to 4), so
cmd_end_search's inner `while (p[0] != code)` never ends: that is the hang.

**Two-byte stages are big-endian in the stream** (first byte << 8 | second byte, read byte by byte, so alignment does not
matter) [read: 0x0E, 0x15, 0x06 case 3, 0x1A, 0x3D, 0x3E, 0x3F]. G's quest numbers (0x56, 0x57) were already 2 bytes, same way.
Area-route stage lists that the program tables point to (not the byte stream) are read as native u16 (lhu, little-endian on
the PS2) [read: 0x18].

On stage 120 the maps below are the identity (stage 120 has class 0 and no partner), so the Rathian's hang is purely the
operand widths: G's next_cmd_search takes 0x0E case 0 as 3 bytes, MH2's data has 4.

## 2. Opcode table

Lengths are whole commands including the opcode byte, "c0/c1/..." by sub-byte (other sub-bytes as in the last group). "same"
means same length in G. EMW offsets are MH2's (MH1's in brackets where the C knows the field).

### Same in both (length and logic)

| op | handler (MH2) | G | length | what it does | how known |
|---|---|---|---|---|---|
| 01 | em_cmd_kehai_ck 0x7275E0 | 0x5857F0 | 2 | presence check on players | shape 1.00 |
| 02 | em_cmd_ninshiki_ck 0x727730 | 0x585940 | 2 | recognition check | shape 1.00 |
| 04 | (inline) em_cmd_top 0x731C70 | 0x58E040 | 1 | back to the program's top (cmd_idx byte 0) | ck |
| 08 | em_cmd_stand_ck 0x7279D0 | 0x585BE0 | c0 2 | posture x388 (0x390) == 0 | carry |
| 09 | em_cmd_fly_ck 0x727BF0 | 0x585CF0 | 2 | posture == 2 | carry |
| 0A | em_cmd_body_status_set 0x727D00 | 0x585E00 | 2 | | carry |
| 0B | em_cmd_mode_ck 0x727D10 | 0x585E10 | c0 3, c1/c2 2 | | carry |
| 0F | em_cmd_route_set 0x7282F0 | 0x5862D0 | 6 | | shape 0.96 (a u16 field) |
| 10 | em_cmd_route_ck 0x728410 | 0x5863E0 | 2 | | shape 0.98 |
| 11 | em_cmd_kehai_pl_set 0x7286A0 | 0x586670 | 2 | | shape 0.93 (pl_num read at MH2's game_w) |
| 12 | em_cmd_find_ck 0x728710 | 0x5866E0 | 2 | | shape 0.91 (same) |
| 16, 17 | action_set 0x728C70, area_route_set 0x728CB0 | | 3, 6 | | carry |
| 1B, 1D, 1E | mind_ck 0x7292F0, mind_sel 0x729580, mind_move_end 0x729760 | | c0 3 / c0 6 c1 3 / 2 | mind x88A | carry |
| 21, 22 | thirst_ck 0x729A20, near_pos_ck 0x729B60 | | c0 3 | 22: XZ distance to the target position <= 100 x byte | carry |
| 24-27 | repeat_cnt_set 0x72A060, repeat_cnt_clr 0x72A170, demo_flag_set 0x72A180, type_sel 0x72A190 | | as G | | shape 1.00 / carry |
| 28-2A | all_pl_same_stage_ck 0x72ABA0, stay_timer_ck 0x72AD40, runaway_timer_ck 0x72AE50 | | 3 | | shape 1.00 |
| 2C | sub_72B240 (G's game_func_005885E0) | 0x5885E0 | c0 6, c1 3 | select by species | shape 1.00 |
| 2D-35 | smell_set .. ikari_ck (0x72B580-0x72BB30) | | as G | | carry |
| 38-3C | water_ck .. boss_atk_ck (0x72BD60-0x72C1B0) | | 3 | | carry |
| 40 | em_mode_change 0x72CB00 | | 3 | | carry |
| 44, 46, 47 | boss_same_stage_ck 0x72D1A0, target_pl_act_ck 0x72D510, fish_ok_ck 0x72D660 | | as G | | carry |
| 4A-51 | pl_look_ck .. swim_ck (0x72D800-0x72DAE0) | | as G | | carry |
| 55, 56, 57 | target_pl_hate_high_ck 0x72E5B0, quest_no_ck 0x72E6E0, quest_no_sel 0x72E810 | | as G (2-byte quest numbers) | | shape 1.00 / 0.98 |
| 59, 5B, 5D, 5F, 60 | tenjo_ck, ninshiki_timer_sub, smell_set_ck, st25_pl_target_sel, st25_gate_ck | | as G | | carry / shape 1.00 |
| 64, 65, 67, 68 | target_pl_hate_ck, all_pl_hate_clear, em_master_ck, em_cmd_reset | | as G | | carry |
| 66 | pl_ride_ck 0x730020 | | 3 | | shape 1.00 |
| 70 | em_cmd_myemtype_sel 0x72B3A0 | 0x588740 | c0 6, c1 3 | select by kind | shape 1.00 |
| 71 | sub_7316A0 (G game_func_0058DF20) | 0x58DF20 | c0 4, c1/c2 2 | | shape 1.00 |
| 80 | em_cmd_rnd32 0x7302D0 | | as G | random select | shape 1.00 |
| 82 | em_cmd_sub_contents 0x7304E0 | | 3 | sub-program call | carry |
| 90-94 | position_set, vec_set, demo_start, wait_set, em_atk_bit | | as G | | carry |
| 99 | sub_731480 (G game_func_0058DD00) | 0x58DD00 | 3 | | shape 1.00 |

### Changed in MH2

| op | handler (MH2) | G | length MH2 (G) | what changed (plain words) |
|---|---|---|---|---|
| 03 | em_cmd_area_move_ck 0x727840 | 0x585A50 | 2 (same) | G looked up em_area_mv_tbl[kind][stage]. MH2 asks sub_734260(em, target stage EMW+0x684): passes when the kind may go there. sub_734260 reads a per-kind table at game 0x8BF840, then a per-rank pointer by sub_3799A0(game_w+0x30), then a u16 list ended by 0xFFFF; entries >= 10000 name one exact stage (minus 10000), others match by stage group (0x379AC0). Same convention as the host's stage_value lists. [read] |
| 05 | (inline) sub_702EE0 | em_type_act_set | 4 (same) | MH2 first refreshes the target position (cmd_target_kind_set or target_kind_set, by EMW+0x817), then calls sub_702CD0 (guess: MH2's form of em_type_act_set). [read] |
| 06 | em_cmd_target_set 0x730540 | 0x58D000 | c3 5, else 4 (4) | Mode 3 now takes a 2-byte stage: x828 = byte, x829 = big-endian stage mapped by **0x379B20**, then x828 = x73A (0x684) = x829, x92F (0x986) = 0xFFFF. New mode 13: x829 = index of the first other live monster on the same stage whose byte EMW+0x966 is set, 0xFFFF if none (guess: a monster-targets-monster mode). x828/x829 are u16. [read] |
| 07 | em_cmd_main_jump | | 2 | cmd_idx stored as a byte. [shape] |
| 0C, 0D | flag_set 0x727E30, flag_clear 0x727FF0 | | 3, 2 (same) | Flag numbers 1-9 (G 1-7; 0 = all). What flags 8 and 9 set was not traced. [read] |
| **0E** | **em_cmd_stage_no_ck 0x7281A0** | 0x5861C0 | **c0 4** (3), c1/c2 2 | Test: stage (2 bytes, big-endian) mapped by **0x379B20** == em->stg (0x680, u16); else CMD_SKIP to the 0x0E else/end as G. [read, the blocker] |
| 13, 58, 19 | pl_target_set, boss_pl_target_set, area_route_ck | | same | Only x828/x829/x92F widened to u16, "none" = 0xFFFF. [read] |
| 14 | em_cmd_angle_ck 0x7287E0 | | same | Small changes (angle byte unsigned, target index); not traced. [shape 0.86] |
| **15** | **em_cmd_stage_no_sel 0x728AD0** | 0x586A70 | **c0 7, c1 4** (6, 3), c2/c3 2 | Select on stage: n entries, each a 2-byte big-endian stage; an entry runs when **0x379AC0(entry) == 0x379AC0(em->stg)** (both sides grouped). G's "values sorted, stop early when past" path is gone: MH2 only tests equality and walks every entry. Case 1 skips its 2-byte value, then skips to the 0x15 end. [read, the blocker] |
| 18 | em_cmd_area_route_move 0x728D20 | | 1 (same) | The route's stage list is u16 (random pick flag 0x8000, G 0x80) and each stage goes through 0x379B20 into x92F. x92E "none" = 0xFFFF. [read] |
| 1A | em_cmd_escape_area_set 0x728EA0 | 0x586E60 | 3 (2) | Target stage is 2 bytes, big-endian, through 0x379B20. The neighbour list (sub_7083B0, G st_mv_ptr_ck) has u16 entries. MH2 walks up to 20 hops (sub_7084B0 gives the next stage's neighbours, sub_734260 says when a hop is allowed) where G looked one step. Ends with x92F = x828 = x829 = the chosen stage. Hop logic not fully traced. [read] |
| **1C** | **em_cmd_mind_no_ck 0x729400** | 0x5872A0 | **c0 6, c1 3** (c0 3, c1/c2 2) | **Different command now**: a select on hit-point percent (x302 x 100 / x792 = current / max HP). Entries are 1-byte thresholds; the first entry whose threshold is above the percent runs. Name kept from G's opcode; it no longer reads the mind. [read] |
| 20 | em_cmd_pl_ang_sel 0x7297A0 | | same | Computes the angle to the target player itself (needs target mode 1 / x828 0 / x829 a player), G used the stored x904[]. [read] |
| **23** | **em_cmd_emtype_ck 0x729E60** | 0x587AA0 | **c0 6, c1 3** (c0 4, c1/c2 2) | **Now a select**: entries are species/kind bytes; an entry runs when a live monster of the same species (sub_7097A0, G's game_func_0054A190) is on this monster's stage (Pl_stg_ck_tw). Kind 74 means "any monster with EMW+0x966 set". No type byte; no pointer stored. [read] |
| 2B | em_cmd_flag_ck 0x72AF60 | | same | Probably flags 8-9 as in 0x0C (guess). [shape 0.92] |
| 36 | em_cmd_near_pos_ck_mh2_00729CE0 | - | c0 3 | **new**: 0x22 with a 3D distance (flvecCalcDistance) instead of XZ. [read: same code but that call] |
| 3D | em_cmd_before_stage_ck 0x72C300 | 0x589580 | c0 4 (3) | 2-byte big-endian stage, 0xFFFF = always false (G 0xFF); compares 0x379B20(stage) with x92E (0x882, **u16** in MH2). [read] |
| 3E | em_cmd_before_stage_sel 0x72C470 | 0x5896C0 | c0 7, c1 4 (6, 3) | Like 0x15: 2-byte entries through 0x379B20 against x92E, never matches when x92E = 0xFFFF, equality only. [read] |
| 3F | em_cmd_ground_area_move 0x72C610 | 0x5898C0 | 4 (3) | Target stage 2 bytes big-endian through 0x379B20 (the early "already there" test and the rest as G, with u16 stages). [read, start only] |
| 41 | em_cmd_em_hp_vital_add 0x72CB70 | | same | More kinds; heals 15% or 10% of max HP by kind. [read, partly] |
| 42, 69 | horm_pos_ang_ck 0x72CCA0, sub_72CF20 (G game_func_0058A020) | | same | First refresh the target position with cmd_target_kind_set; angle byte unsigned. [read, start] |
| 45, 52, 53, 54 | pl_fishing_ck, all_pl_target_sel, samestage_pl_target_sel, target_pl_samestage_ck | | same | Extra player tests (sub_3B5A40; 52 also sub_734260 per player). Not traced. [shape 0.77-0.98] |
| 48 | em_cmd_timer_set 0x72D780 | 0x58A810 | 2 | MH2 stores the byte as is in a u16 at EMW+0x994; G stored byte x 30 (frames) in work08. reset_flag_ck clears 0x994. [read] |
| 49 | em_cmd_pl_land_target 0x72D790 | | 2 | x828/x829 u16. [shape] |
| 5A, 5C, 5E, 62 | target_land_no_ck, tenjostage_ck, my_floor_ck, dansa_sel | | same | Stage / index fields widened; player work stride 0x970. [shape 0.94-0.99] |
| 61 | em_cmd_runaway_timer_set 0x72F650 | | 1 | G took em02_runaway_timer_tbl[stage]; MH2 takes the kind's {stage, value} list at game 0x8BEB60 through sub_7098A0 (the host's stage_value). [read] |
| **63** | em_cmd_male_ck 0x72F8A0 | 0x58C860 | **c0 3** (2) | Test now takes a mask byte: passes when (em->type (0x1B) & mask) != 0. G tested x11 == 0 with no operand. [read] |
| 81 | em_cmd_contents 0x730490 | | 2 | x822 is a byte at 0x775 (include/mh2_emw.h has a pad there). [read] |
| 83 | em_cmd_range_ck 0x7307D0 | | same | Has a 3D-distance path (flvecCalcDistance). Not traced. [shape 0.86] |
| 84 | (inline) sub_7275D0 | (nothing) | 1 | MH2 adds 1 to the u16 at EMW+0x3A2 (x39A); G did nothing. [read] |
| FF | em_cmd_end_command 0x7309F0 | 0x58D330 | 2 | Case 4 (end of a ground route) moves the monster to the route's stage through 0x379B20 and MH2 helpers (sub_705B80, sub_2EE740, sub_32DD20) instead of em_area_move_init. Not traced in full. [read, calls only] |

### New in MH2 (no G handler)

| op | handler | length | what it does |
|---|---|---|---|
| 1F | em_cmd_op1F 0x727AE0 | c0 2 | test, no operand: posture x388 (0x390) == 1 (08 is 0, 09 is 2). [read] |
| 37 | em_cmd_op37 0x72BC40 | c0 2 | test, no operand: bit 0x200000 of the word at 0x5D8288 (quest_w+0x28; meaning unknown). [read] |
| 72 | sub_7317C0 | c0 2 | test, no operand: sub_39E400() == 1 (host notes: 0 offline; guess: "online"). [read] |
| 73 | em_cmd_op73 0x7318F0 | c0 6, c1 3 | select on EMW+0x89D (x949) & 0x7F, ordered like G's selects; a match clears those 7 bits. Meaning unknown. [read] |
| 74 | sub_731AF0 | c0 3 | test with a count byte against the same x949 & 0x7F (online path through sub_39F150); a pass clears it. Partly guessed. [read] |
| 75 | sub_72A370 | c0 6, c1 3 | select on byte EMW+0x8AF, ordered. Field unknown. [read] |
| 76 | em_cmd_op76 0x72A550 | c0 6, c1 3 | select on main sub_18DA00(): 0/1/2 from bits 0/1 of byte 0x5D87A9. Meaning unknown. [read] |
| 77 | sub_72F9C0 | 2 | test? reads one byte and compares it with main sub_18DA70() (the 0/1 used by 0x379B20 below). Guess: "is it day / night". [read, start] |
| 78 | sub_72FC20 | c0 4 | test: angle to the target player inside a range of two bytes (like 0x14 / 0x20). [read, partly] |
| 79 | sub_72A970 | c0 7, c1 3 | select on a value from the kind's function table (EMW+0x3D0, slot +0x10, given one byte). Guess: a per-kind state. [read] |
| 7A | sub_72A760 | c0 6, c1 3 | select on sub_3799A0(game_w+0x30), the mapped em_cmd level (rank). [read] |
| 7B | sub_7275D0 (as 0x84) | 1 | counter EMW+0x3A2 += 1. [ck, read] |
| 7C | sub_72FAF0 | c0 2 | test, no operand: sub_3799A0(game_w+0x30) is 9 or 10 (guess: the top ranks). [read] |

Not in either game: 0x00, 0x43, 0x6A-0x6F, 0x7D-0x7F, 0x85-0x8F, 0x95-0x98, 0x9A-0xFE (em_cmd_ck resets the program; MH2's
next_cmd_search sets the abort byte).

### next_cmd_search / cmd_end_search: every length that differs

MH2 against G's real next_cmd_search [len]. Everything else is identical, including G's own additions (0x56, 0x57 2-byte
quest numbers; 0x69, 0x70, 0x71, 0x99). Our G C for the PC (em_cmd_nm.c, VERSION_MHG branch) matches G's real lengths for all
the rows below.

| op | G | MH2 | why |
|---|---|---|---|
| 06 | 4 | 5 for mode 3, else 4 | 2-byte stage in mode 3 |
| 0E | c0 3 | c0 4 | 2-byte stage |
| 15 | c0 6, c1 3 | c0 7, c1 4 | 2-byte stages |
| 1A | 2 | 3 | 2-byte stage |
| 1C | c0 3, c1/c2 2, else 1 | c0 6, c1 3, else 2 | condition became a select |
| 1F | unknown | c0 2 | new |
| 23 | c0 4, c1/c2 2, else 1 | c0 6, c1 3, else 2 | condition became a select |
| 36 | unknown | c0 3, c1/c2 2 | new |
| 37 | unknown | 2 | new |
| 3D | c0 3 | c0 4 | 2-byte stage |
| 3E | c0 6, c1 3 | c0 7, c1 4 | 2-byte stages |
| 3F | 3 | 4 | 2-byte stage |
| 63 | 2 | c0 3, c1/c2 2 | mask byte |
| 72-7C | unknown | as in the table above | new |

cmd_end_search and else_ck: same code in both [shape 1.00]; they only depend on next_cmd_search.

## 3. The stage maps

Four small leaf functions in sub_main, two in main, and three per-stage u16 tables in a row (0x440CA0, 0x440E30, 0x440FC0,
0x190 bytes each = 200 entries; the symbol file calls 0x440CA0 Stg_env_type, MH1's per-stage environment table, read by
sub_main 0x379980). Only these functions read 0x440E30 / 0x440FC0 (scan of every overlay's code for lui/offset pairs).

- **0x440E30 (s16 per stage), "class"**: 0, 1 or 2. Counts over the 200 entries: 106 zeros, 79 ones, 15 twos.
- **0x440FC0 (u16 per stage), "partner"**: another stage, or 0xFFFF. 158 entries pair up both ways, always one class-0 stage
  with one class-1 stage (79 pairs). Class-2 stages have no partner. Entries 197-199 hold 0 (class 0): reads as padding past
  the last stage (guess). For the test quest: stage 110 is class 0 with partner stage 1 (class 1); stage 120 is class 0 with no
  partner.
- **Guess, not verified: class 0 and 1 are the day and night forms of one area, class 2 an area without that split.** The 0/1
  that 0x379B20 compares with (below) is set at quest start (sub_main 0x3A11A0) from a quest flag bit, and forced to the first
  value for five quest-type values (7, 8, 13, 14, 15 of the u16 at 0x6ADB4A). That fits "the quest's time of day", but it was
  not seen in play.

The functions (s is a stage; 0xFFFF means none and comes back as 0xFFFF from all but the first):
- **sub_379A30(s)**: 1 for 0xFFFF; otherwise 1 when class[s] is not 1. ("s is not a class-1 stage".)
- **sub_379A70(s)**: partner[s], or s itself when the partner is 0xFFFF.
- **sub_379AC0(s), the group**: s when sub_379A30(s), else the partner. So a class-1 stage counts as its class-0 partner; every
  other stage as itself. Used where "the same area" is meant: 0x15, sub_734260's lists, the host's stage_value (game 0x7098A0).
- **sub_379B20(s), the current form**: d = 0 when game_w+0x41 is nonzero, else main sub_18DA70(); then s when class[s] == d,
  else the partner (or s). So it turns a stage number from a script into the form of that area that is in use now. Used for
  every stage a command writes or compares with em->stg / x92E (0x06, 0x0E, 0x18, 0x1A, 0x3D, 0x3E, 0x3F, 0xFF).
  **Correction to docs/mh2/shared5.md:** that note says d = game_w+0x41 when it is nonzero. The load of game_w+0x41 is
  followed by a branch whose delay slot clears d, so a nonzero byte gives d = 0, not the byte. game_w+0x41 is G's x1DC (MH1
  0x1DC, include/game.h; MH2 game_w is G's + 2 from 0x16); MH1/G code loads the quest map only when x1DC is 0 (pit01.c), so
  nonzero looks like "not in a hunt" (guess).
- **main sub_18DA70()**: byte 0x5D4910: 0 when bit 3 is set, else 1 when bit 4 is set, else 0. Its setter sub_18DAA0(n) clears
  bits 3-4 and sets bit n (n is 3 or 4). Bits 0-2 of the same byte are a separate three-way value (getter sub_18D970 -> 0/1/2,
  setter sub_18D9C0); the lobby writes the whole byte too (0x779460). Nothing else is known about the byte.
- Related, not a stage map: **sub_3799A0(n)** = table 0x43ED80[n] (u16) or n when 0xFFFF; the em_cmd code uses it on
  game_w+0x30 (the em_cmd level) to get a rank index (0x03 via sub_734260, 0x7A, 0x7C). The host already reads it
  (src/pc/rt/rt_mh2_em.c).

Field widths found on the way (MH2 EMW): stg 0x680 u16, x73A 0x684 u16, x828 0x77A u16, x829 0x77C u16, x92F 0x986 u16, and
**x92E 0x882 is u16** (every reader uses lhu; include/mh2_emw.h has it as u8 with a pad byte after). x822 is a byte at
0x775 (the header has a pad there). "None" for all the stage fields is 0xFFFF, not 0xFF.

## 4. Plan for the change (for a later agent; in this order)

1. **Headers.** include/mh2_emw.h: x92E -> u16 at 0x882 (drop _pad883); x822 as a byte at 0x775. If the file is generated
   (tools/mh2_structs.py, the "votes" comments), put the fix where the generator takes hand entries. Check em_cmd_nm.c's
   `em->x92E` / `x92F` / `x829` writes of 0xFF under MH2: they must write 0xFFFF (reset_flag_ck, em_cmd_init's G copy is
   already renamed away; em_cmd_area_route_move, before_stage_*). A `#define EM_STG_NONE MH2V(0xFF, 0xFFFF)` keeps it short.
2. **The stage maps as shared C.** Six tiny leaf functions, good first byte-match candidates in the MH2 build: sub_main
   0x379A30, 0x379A70, 0x379AC0, 0x379B20 and main 0x18DA70 (keep the sub_<addr> / func_<addr> names). The PC needs the two
   tables: add `D_00440E30 = 0x00440E30; // size:0x190 noptr` and `D_00440FC0 = 0x00440FC0; // size:0x190 noptr` (asm-label
   names; no real names known) to config/mh2/pc_data.txt, or keep reading them through rt_addr as rt_mh2_em.c's stage_group
   does. The byte 0x5D4910 and game_w.x1DC are plain RAM; on the PC the byte is 0 until the quest start code sets it, so d = 0
   (class-0 forms) until then. rt_mh2_em.c's stage_group should then call the shared 0x379AC0 (one copy).
3. **next_cmd_search.** In em_cmd_nm.c, wrap the G next_cmd_search in `#ifndef VERSION_MH2` and `#include
   "../../mh2/game/next_cmd_search.c"` under VERSION_MH2, as em_cmd_r23.c does (it brings its own static stand-in for
   0x7262D0 writing EMW+0x95E). This alone stops the hang for stage 120, but then the handlers below read the wrong bytes.
4. **The two blockers, under VERSION_MH2 in em_cmd_nm.c:** em_cmd_stage_no_ck (0x0E: 2-byte big-endian, 0x379B20, compare with
   em->stg) and em_cmd_stage_no_sel (0x15: 2-byte entries, both sides through 0x379AC0, equality only, case 1 skips 2 bytes).
   Read the stage as `(p[0] << 8) | p[1]`.
5. **The other length changes** (all in the stream; each is wrong on MH2 data now): 0x06 mode 3 (+ mode 13), 0x1A, 0x3D,
   0x3E, 0x3F, 0x63 (mask on em->type), 0x1C (HP-percent select), 0x23 (species select). 0x1C and 0x23 fit CMD_SEL_FUNC's
   shape poorly (threshold "<" for 0x1C; species + stage test for 0x23); write them out.
6. **em_cmd_ck for MH2.** Add MH2's cases to em_cmd_nm.c's switch under VERSION_MH2 (or include src/mh2/game/em_cmd_ck.c, which
   is byte-matched, and provide every name it calls). Without this the new opcodes reset the program each tick (G's default
   case) and 0x84 misses its counter. The new handlers can start as their lengths plus "condition false" for the ones not yet
   understood; the lengths above are exact, the meanings partly guessed. Use em_cmd_ck.c's names (em_cmd_op1F, op37, op73,
   op76, game_func_<addr>) so the PC and the matched C agree.
7. **Same-length logic changes**, by how likely the Rathian meets them: 0x18 (u16 route lists, 0x8000 random flag, 0x379B20),
   0x03 with sub_734260 (needs game 0x8BF840: per kind -> per rank (sub_3799A0) -> u16 list; place it in pc_data.txt or read by
   rt_addr; at least 21 kinds have lists, the table's length is not settled), 0x61 (share rt_mh2_em.c's stage_value), 0xFF case
   4, 0x20 / 0x14 (live angle to the target player), 0x0C / 0x0D / 0x2B (flags 8, 9: trace first), 0x48 (u16 timer, no x 30),
   0x05 (refresh the target before the act set), 0x41, 0x42 / 0x69.
8. **Test:** `RT_QUEST_STAGE=1 RT_MH2_EM=1 RT_STEP=1 RT_QUEST_TRACE=1 build/mh2/pc/mhview_2 disc/mh2 --quest 1014 --input
   "idle*30,up*120" --time 30 --shot x.png`. A one-line opcode trace in em_cmd_ck (op, sub-byte, program offset) under an env
   variable would show the next unknown quickly. Stage 120 needs steps 1, 3, 4, 6; stage 110 (class 0 with a class-1 partner)
   is the first place where the maps change anything.

## What is guessed or open

- The meaning of class 0/1/2 (day/night) and of the 0/1 from sub_18DA70; the meaning of game_w+0x41 when nonzero.
- Meanings of the new opcodes 0x37, 0x72-0x7C (fields read are given; names are not).
- 0x1A's multi-hop walk, flags 8/9 of 0x0C/0x0D, 0x45/0x52/0x53/0x54's extra tests, 0x83's 3D path, 0xFF case 4's helpers,
  0x14's small change: only their calls / field widths were read.
- The length of game 0x8BF840 (per-kind area-move lists) and of the 0x440FC0 tail (entries 197-199).
- Nothing here was run; the lengths in section 2 are the solid part.
