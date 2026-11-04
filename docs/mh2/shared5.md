# MH Dos: shared C, round 5 (SHARED agent, 3-4 Nov 2026)

Shared game C and config/mh2 names, byte-checked where it says "matched" (tools/mh2_diff.py -g shows 0 differing instructions and the
carry registered it). Every MH2 address below was read from our own disassembly of the owner's disc. Community mh2-decomp not
opened. "Guess" marks what was not checked.

## The Rathian moves (with RT_MH2_EM=1)

Cause of round 4's "motion layer end 0": frame_init and frame_init_b (src/main/frame/f_frameb.c) took a monster's motion handles
from `motion_set_handle_tbl[mdl_no * 600 + 1700]` and a hunter's from `[id * 300 + 500]`, MH1's bases. MH2's frame_init (sub_main
0x2E5190) uses 1910 and 710 (`addiu 0x776`, `addiu 0x2C6`), the same bases f_frame.c's create_em_motion / create_pl_motion already
used under VERSION_MH2. So em_char_set did set chr (1001, 1003...), but frame_init read empty handles. Now `MH2V(1700, 1910)` /
`MH2V(500, 710)`; frame_init, frame_init_b and em_frame_check (speed at MH2 0x884, sub_main 0x2E5D30) byte-match and carry.

Seen on the PC, `RT_MH2_EM=1 RT_MH2_EM_HERE=1 RT_STEP=1 RT_QUEST_TRACE=1 build/mh2/pc/mhview_2 disc/mh2 --quest 1014 --input
"idle*30,up*120" --time 30`: the Rathian goes act 0 -> 16 -> 22 -> 1, rises to y 825 / 1703, flies from (1500, 1500) to (5067,
5067), lands (mode 0) and takes off again; 900 ticks, no crash. gdb at its 300th em_frame_check: chr 1015 / 1215 / 1415, 3
layers, layer 0 frame 202 of end 226, looping. It is not on screen in the shot (it flew off; drawing is the host's). The kind-10
monster stays in act 0: its idle pose x2D4 is 0 (stand), which em10's act00 treats as "stay" (looks like normal behaviour, not
checked further).

## QEM (include/quest.h) and the model slots

MH2's quest monster entry, 0x3C bytes, from em_work_serch 0x3A7BF0, Quest_enemy_escape 0x3A2660, Quest_enemy_revival_ck 0x3A2150,
Quest_enemy_revival_set 0x3A21F0, Quest_enemy_die 0x3A2460 and Em_direct_set 0x3A3E80. G's names at MH2's places:

| field | G | MH2 | note |
|---|---|---|---|
| id | 0x00 | 0x00 | kind |
| x04 | 0x04 s8 | 0x04 s8 | lives left |
| x06 | 0x06 s8 | 0x06 u16 | Em_direct_set stores a byte of it to EMW+0x8AF |
| x07 (stage) | 0x07 s8 | 0x08 u16 | |
| m0A | - | 0x0A s8 | new: 1 = never revives (Quest_enemy_revival_ck) |
| m0B | - | 0x0B s8 | new: revival place mode (Quest_enemy_revival_set; 1 asks sub_main 0x3A2070) |
| x18 | pad | 0x18 s32 | Em_direct_set: a byte to EMW+0x964 |
| x30 (size) | 0x30 | 0x30 | |
| x08 (hit points) | 0x08 s16 | 0x32 s16 | |
| x0A (em_work id) | 0x0A s16 | 0x34 s8 | |
| x05 (counts) | 0x05 s8 | 0x35 s8 | |
| x36 | - | 0x36 s8 | level (Em_direct_set: EMW+0x98A) |

MH2 ends a QEM list at `id <= 0` (blez), MH1 and G at `id < 0`: QEM_LAST(e). game_w's model slots are at +0x2A (GW_EMSLOT;
Quest_next_em_clr 0x3A4228 `lbu 0x2A`): f_quest.c and f_quest_nm.c scan them through it. Matched and carried: em_work_serch,
enemy_insurance_sub, Quest_enemy_revival_ck (MH2's m0A test under VERSION_MH2), Quest_enemy_escape.

For HOST: tools/build_pc.sh renames G's Quest_em_init_set / Quest_next_em_set away on MH2 because they "scan the model slots at G's
game_w+0x28"; they read +0x2A now. The host copies in src/pc/rt/rt_mh2_em.c can stay (they also do MH2's station set), but the
reason given in the script no longer holds. Not changed (HOST's file).

## Stage 120's camera

MH2's default_area_data (sub_main 0x3204B0) fills the same one-area block as G's, then calls 0x320470, which copies 0x100 bytes
from cam_cnf_chs (0x41C830; a CAMCNF) into the area's config at +0x50. No stage_camera_data_tbl at all. src/main/cam/camarea03.c
does that under VERSION_MH2; the copy helper (sub_main_func_00320470) is matched and carried; default_area_data and StageCamInit
are not (they store the block in CameraWork, which on the PC is still G's layout; MH2 has it at +0x388 and copies to +0x390..).
`RT_QUEST_STAGE=1 RT_STEP=1 ... --quest 1014` now starts on stage 120 (the ruins) with the camera behind the hunter; it crashed in
default_area_data before.

## em_effect_move's symbol

tools/mh2_map.py ends a function only at jr ra or j, so gm_sub 0x90A990 (em_effect_move: `lw 0x3D0; lw 0xC; jr v0`) ran into the
2-instruction wrapper after it (j eft01_set with a1 = 0; G's twin is sub_main_func_004EDD70). tools/mh2_funcfix.py now takes
`split` lines in config/mh2/func_fix.txt: the function holding the address is cut there, in docs/survey/mh2_map.csv and
config/mh2/symbols (idempotent; mh2_map.py runs funcfix last, so the split survives a rerun). src/main/emw/emt02.c carries whole.

## em_cmd on stage 120 (next blocker, not fixed)

With `RT_QUEST_STAGE=1 RT_MH2_EM=1` the Rathian, now in the hunter's stage, runs em01_main -> em_cmd_ck (G's, em_cmd_nm.c):
1. It crashed in em_cmd_find_ck on `*(u8 *)0x3F34C3` (MH1's game_w.pl_num, outside what tools/pc_abs.py maps). em_cmd_nm.c's 13
   reads now say `MH2V(0x3F34C3, 0x6ADCEE)` (MH2 em_cmd_find_ck game 0x728710: `lbu -0x2312(0x6B0000)`).
2. It then hangs in em_cmd_stage_no_ck (opcode 0x0E) -> cmd_end_search: MH2's handler (game 0x7281A0) reads the stage as 2 bytes,
   big-endian, maps it through sub_main 0x379B20 and compares with EMW+0x680; opcode 0x15 (0x728AD0) the same through 0x379AC0, with
   its own loop. G's read one byte, so the program pointer drifts. MH2's next_cmd_search (src/mh2/game/next_cmd_search.c, matched)
   also has other operand lengths than G's (0x0E case 0: 3 bytes, not 2; 0x15 case 0: 3+2). Still to write.
   The stage maps (read from the asm, not written as C):
   - 0x379A30(s): 0xFFFF -> 1; else table 0x440E30[s] (s16) != 1.
   - 0x379A70(s): 0xFFFF -> 0xFFFF; t = table 0x440FC0[s] (u16); t == 0xFFFF ? s : t (the stage's other variant).
   - 0x379AC0(s): 0xFFFF -> 0xFFFF; 0x379A30(s) ? s : 0x379A70(s).
   - 0x379B20(s): 0xFFFF -> 0xFFFF; d = game_w+0x41 (byte), or main 0x18DA70() when that is 0 (bit 3 / bit 4 of the byte at
     0x5D4910); d == table 0x440E30[s] ? s : 0x379A70(s).
   The two tables run 200 entries (0x190 bytes each, 0x440E30 + 0x190 = 0x440FC0). 0x440E30 holds 0, 1 or 2 per stage (a guess:
   time of day of the stage), 0x440FC0 the paired stage (stage 110 -> 1, 120 -> none). The PC needs both imported (HOST:
   config/mh2/pc_data.txt or src/pc/rt/tables.txt; no names for them yet), then em_cmd_nm.c can take MH2's next_cmd_search
   (`#include "../../mh2/game/next_cmd_search.c"` as em_cmd_r23.c does) and the two handlers. MH2's own em_cmd_ck
   (src/mh2/game/em_cmd_ck.c) calls MH2-only handlers (em_cmd_op1F, op37, op73, op76, game_func_*) the PC has no C for.

## MH2's CameraWork (not done)

Unchanged from docs/mh2/structs.md: 6 slots of 0x80 at +0x80 (no per-mode union in the slot), init 0x380, pl 0x384, data 0x388,
area 0x38C, then 0x390..0x3A4 (StageCamInit's copy, sub_main 0x320580 confirms: G's 0x590..0x5A4 less 0x200), mode work at 0x3B0,
0x4B0, 0x530, 0x580, quakes 0x610 / 0x624 / 0x638 (0x14 each), reset 0x668, demo_arg 0x66C, demo_req 0x670, zoom 0x672. The PC
runs G's camera on G's CAMW; moving it is a rewrite of camm.c / cam*.c plus src/pc/rt/rt_cam.c (HOST's).

## Not done

- MH2's CameraWork and the 6-slot camera.
- hit_hit_sub_em / hit_hit_sub_pl as MH2's own logic.
- em_cmd for MH2 (above), Quest_enemy_revival_set / Quest_enemy_die / Em_direct_set as MH2's own logic (read, see the QEM table).
