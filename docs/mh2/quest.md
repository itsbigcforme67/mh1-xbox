# MH Dos: game task and quest core (QUEST agent, 2 Nov 2026)

Written from our own disassembly of the owner's disc. Community mh2-decomp not opened. Proof is bytes only: nothing here has run.

## Byte-matched C (in the MH2 build, tools/mh2_check.py 13 of 13)

| function | address | bytes | where |
|---|---|---|---|
| next_cmd_search | game 0x731DA0 | 5,032 | src/mh2/game/next_cmd_search.c; em_cmd_r23.c includes it |
| cmd_target_kind_set | game 0x704120 | 2,672 | include/em_core_g_mh2.h (src/mh2/game/cmd_target.c), with jump tables 0x8EC4E0-0x8EC5F8 |
| target_kind_set | game 0x704B90 | 2,440 | same |
| em_cmd_ck | game 0x726380 | 4,676 | src/mh2/game/em_cmd_ck.c; em_cmd_r01.c includes it |

## Near-matches (`*_nm.c`, same shape, not in the MH2 build)

Logic read from the code; registers or a few instructions differ, so the bytes do not prove them.

| function | address | bytes | state |
|---|---|---|---|
| Em_Dmg_Sys | game 0x725860 | 2,668 | src/mh2/game/em_dmg_sys_nm.c, em_taisei.c includes it for the PC. em/hit/first land one register off |
| quest script step | sub_main 0x3A9230 | 3,520 | src/mh2/sub_main/p003A9230_nm.c, 11 instructions differ, two register swaps |
| quest state machine | sub_main 0x3A2E30 | 2,588 | src/mh2/sub_main/p003A2E30_nm.c, about 8 bytes short, scheduling. The call to 0x3A5880 has a0 = 1 left from a loop: the PC needs that 1 |

## What was learned

- **0x3A2E30 is the quest's per-frame state machine**, called from the game task at 0x326E64. It runs by D_5D8260 (0 running, 1 and 2 ending, 3 result) and returns 0, 1, -1, -2.
- **0x3A9230 steps a quest event script:** 8-byte commands {s16 op, a, b, c} from the pointer at 0x5D82B4, the index in 0x5D827E; op 0x1A is a label, 0x1C jumps to a label, 6/7, 0x28/0x29, 0x2A/0x2B are if/endif pairs, 9/10 a wait, 0x1B a timer with a message. G has no such thing: this script is new in MH2, and the quest flow depends on it. Names are placeholders and every op meaning is a **guess** from the code. Where the script comes from was not looked at.
- 0x378A30 is per-stage set-up by stage number (a table at 0x4485D0, then calls by stage). 0x33E730 is the pit menu's frame (Game_clear_ck, lpPit). Neither written.
- em01_senkai_sub (0x70A290) is rewritten: it takes a parameter table as third argument, shared by the flying monsters. MH1's C does not carry. Not written.
- game_func_007262D0(em, p, v) stores v in byte 0x95E of EMW (0x9DA in MH1's numbering): the command error. It is called with 2 and 4.

## EMW fields MH2 changed that the header has wrong (for STRUCTS)

Read from cmd_target_kind_set against its C, all u16: x828 at 0x77A, x829 at 0x77C, x882 at 0x998, x883 at 0x99A, x92F at 0x986, x95A at 0x98C (holds x883 << 2 in target_kind_set case 12). cmd_idx at 0x774 is written as a byte by em_cmd_ck. EMW + 0x3D0 is a pointer with a function at +0x18. The header has `u8 x828` at 0x77C, `u8 x829` at 0x77D, x882/x883 at 0x7D6. A cast to any other struct makes MWCC keep the address in a register, so a field must be in EMW itself or reached through a union parameter, which is what EMT in em_core_g_mh2.h does. When the header has them, EMT can go.

## Tools

- config/mh2/hand_parts.txt: hand C of which only the named functions stay in the link. Needed for a **same-file static**: MWCC keeps a caller-saved register across a call only when it sees the callee as a static earlier in the file (agent-C notes, seen again at 0x3A8990 and game 0x7262D0). The static is compiled, compared by mh2_own, and dropped; its bytes stay in the asm.
- tools/mh2_own.py takes functions by map name as well as by address suffix.
- MWCC does not open `#include "../../x"` paths here. C that a shared file and a hand file both need goes in include/. has_raw files (em_cmd_r01, r23, em_taisei) never reach MWCC with VERSION_MH2, so relative includes are safe there.
- `#pragma divbyzerocheck off` is needed around 0x3A2E30.
- Hand C in src/mh2 is not in the PC build's source list unless a shared file includes it. p003A9230_nm.c and p003A2E30_nm.c are not linked yet.

## Name map: em_cmd opcode handlers

em_cmd_ck calls each handler by opcode, so MH1's and MH2's tables can be laid side by side. Where they differ, either the map's name is wrong or the opcode moved, not decided here. Many are `game_func_` where a twin is likely. New opcodes in MH2: 0x1F, 0x36, 0x37, 0x69, 0x70-0x7C (from the table).

| opcode | MH1 handler | MH2 name now | address |
|---|---|---|---|
| 0x03 | em_cmd_area_move_ck | game_func_00727840 | 0x727840 |
| 0x06 | em_cmd_target_set | game_func_00730540 | 0x730540 |
| 0x07 | em_cmd_main_jump | game_func_007279C0 | 0x7279C0 |
| 0x08 | em_cmd_stand_ck | game_func_007279D0 | 0x7279D0 |
| 0x0A | em_cmd_body_status_set | game_func_00727D00 | 0x727D00 |
| 0x0E | em_cmd_stage_no_ck | em_cmd_emtype_ck | 0x7281A0 |
| 0x13 | em_cmd_pl_target_set | game_func_007287A0 | 0x7287A0 |
| 0x15 | em_cmd_stage_no_sel | game_func_00728AD0 | 0x728AD0 |
| 0x19 | em_cmd_area_route_ck | game_func_00728E80 | 0x728E80 |
| 0x1A | em_cmd_escape_area_set | game_func_00728EA0 | 0x728EA0 |
| 0x1B | em_cmd_mind_ck | game_func_007292F0 | 0x7292F0 |
| 0x1C | em_cmd_mind_no_ck | game_func_00729400 | 0x729400 |
| 0x1F | - | em_cmd_stand_ck | 0x727AE0 |
| 0x20 | em_cmd_pl_ang_sel | game_func_007297A0 | 0x7297A0 |
| 0x23 | em_cmd_emtype_ck | game_func_00729E60 | 0x729E60 |
| 0x26 | em_cmd_demo_flag_set | game_func_0072A180 | 0x72A180 |
| 0x27 | em_cmd_type_sel | game_func_0072A190 | 0x72A190 |
| 0x2C | em_cmd_myemtype_sel | game_func_0072B240 | 0x72B240 |
| 0x2D | em_cmd_smell_set | game_func_0072B580 | 0x72B580 |
| 0x36 | - | em_cmd_near_pos_ck_mh2_00729CE0 | 0x729CE0 |
| 0x37 | - | em_cmd_male_ck | 0x72BC40 |
| 0x3D | em_cmd_before_stage_ck | game_func_0072C300 | 0x72C300 |
| 0x3E | em_cmd_before_stage_sel | game_func_0072C470 | 0x72C470 |
| 0x41 | em_cmd_em_hp_vital_add | game_func_0072CB70 | 0x72CB70 |
| 0x48 | em_cmd_timer_set | game_func_0072D780 | 0x72D780 |
| 0x49 | em_cmd_pl_land_target | game_func_0072D790 | 0x72D790 |
| 0x58 | em_cmd_boss_pl_target_set | game_func_0072EA00 | 0x72EA00 |
| 0x5E | em_cmd_my_floor_ck | game_func_0072EFC0 | 0x72EFC0 |
| 0x61 | em_cmd_runaway_timer_set | game_func_0072F650 | 0x72F650 |
| 0x63 | em_cmd_male_ck | em_cmd_stage_no_ck | 0x72F8A0 |
| 0x69 | - | game_func_0072CF20 | 0x72CF20 |
| 0x70 | - | game_func_0072B3A0 | 0x72B3A0 |
| 0x71 | - | game_func_007316A0 | 0x7316A0 |
| 0x72 | - | game_func_007317C0 | 0x7317C0 |
| 0x73 | - | em_cmd_stage_no_sel | 0x7318F0 |
| 0x74 | - | game_func_00731AF0 | 0x731AF0 |
| 0x75 | - | game_func_0072A370 | 0x72A370 |
| 0x76 | - | em_cmd_type_sel | 0x72A550 |
| 0x77 | - | game_func_0072F9C0 | 0x72F9C0 |
| 0x78 | - | game_func_0072FC20 | 0x72FC20 |
| 0x79 | - | game_func_0072A970 | 0x72A970 |
| 0x7A | - | game_func_0072A760 | 0x72A760 |
| 0x7C | - | game_func_0072FAF0 | 0x72FAF0 |
| 0x81 | em_cmd_contents | game_func_00730490 | 0x730490 |
| 0x92 | em_cmd_demo_start | game_func_007314A0 | 0x7314A0 |
| 0x93 | em_cmd_wait_set | game_func_007314B0 | 0x7314B0 |
| 0x99 | - | game_func_00731480 | 0x731480 |
