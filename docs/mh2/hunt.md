# MH Dos (MH2): `--quest` on the game's own code (HUNT agent, 3 Nov 2026)

- **Scope:** host side only (src/pc, tools/build_pc*.sh, config/mh2/pc_*.txt).
- **Source:** read from the owner's disc with throwaway scripts, and seen in hidden runs of build/mh2/pc/mhview_2. The
  community mh2-decomp repo was not opened.
- **Proof:** "guess" marks what was not checked. Code written here for the PC is not byte-checked.

## What runs

`build/mh2/pc/mhview_2 disc/mh2 --quest 1014 --shot x.png` now takes G's `--quest` path (viewer.c), not the drawing check
(that stays behind `RT_MH2_VIEW=1`): rt_quest_load -> Quest_init (G's C on MH2's quest_w layout) -> **MH2's Quest_start**
(rt_mh2.c, written from sub_main 0x3A11A0 for the PC, not byte-checked) -> Start_item_init, EvDemoInitialize -> the stage load
(load_stage_hit, models, light_init) -> game modes through rt_flow (game13, game2 every tick) with the game camera and HUD.
Seen (hidden shot, looked at): stage 110 (beach), the hunter standing in the game camera's view, HUD drawn (with G's pit textures).
Quest 1014: start stage 110, time 90000, reward 2200, fee 800, flags 0x1A0001, the supply box filled (10 items).

Stops at (in this order once each is passed):
- hunter: frame_init (include/frame.h FRW, blocker 1), then pl_move_sub -> GetGroundHitStatusAreaPl (HENT, blocker 2);
- monsters: Em_direct_set -> em_init -> em_status_init -> em_def_attack_set (per-kind table, blocker 4);
- quest flow: G's Quest_condition_judging reads MH2's program and "clears" the quest at tick 7 (blocker 3).
Monsters are off by default on MH2 until blocker 4 is done: `RT_MH2_EM=1` spawns them (the run then stops in em_status_init).
`RT_MH2_EM_PROG=0`: no G stand-ins in em_prog_tbl.

## Host stand-ins written from MH2's asm (src/pc/rt/rt_mh2.c; replace when CODE has C)

| function | address | note |
|---|---|---|
| Quest_start | sub_main 0x3A11A0 | G's with MH2's .mib header: same offsets to +0x30; x14E from +0x48 (G +0x34); +0x3C/+0x40 into quest_w+0x530/+0x534; +0x38 relocated in place; info +0x0C reward, +0x10 fee, +0x1C time, +0x24 table; bytes +0x5E..+0x60 to x165; 0x9D gets (time-1)/9000 (layouts.txt calls 0x9D x168: swapped with x169, guess; **Conflict** with config/mh2/layouts.txt, not settled), 0x15A from 0x44B450[rnd & 15]. Skipped: 0x18D750, 0x3A7FC0, 0x366950, 0x3A3DF0, 0x36B110, 0x3A8060, 0x3A3E20, NPCZoomInCameraCancel, game 0x6EE380, the d/n and 0-2 file pick (0x18D970, 0x18DA70; RT_MIB) |
| Quest_pl_stage_init(0xFF) | sub_main 0x3A1D00 | inside Quest_start: +0x5D random pick from gm_sub 0x9316B0 by quest_w.rnd[master], else the player start table |
| light_init | sub_main 0x332F80 | MH2 has no pl_light_tbl: set 1 from 0x332C70 (stage_work env block, else rows 0x41EC00/0x41ECF0/0x41EC60) |
| light_change_normal, pl_light_change | 0x333770, 0x333640 | guess that these are the twins (place and shape); 0x333770 passes n as the stage (as the asm does) |
| em_cmd tables | gm_sub 0x90AB00, 0x90A9D0 | per kind from em_cmd.afs (DATA.BIN entry 6587) by 0x932C70[game_w.x2E][kind]; loaded from em_create_model |
| em_cmd_init | game 0x7262E0 | cmd_tbl from those tables (raw MH2 EMW offsets) |

G copies renamed away for MH2 in tools/build_pc.sh: light_init_nm.c light_init, light_nm.c light_change_normal / pl_light_change,
em_cmd_nm.c em_cmd_init. Fish_set (rt_quest.c) is skipped on MH2 (no Fish_hani_tbl).

## Tables and names (config/mh2/pc_data.txt, pc_fn_names.txt)

- MH2 has 199 stages (G 92): stage_hit_data_w, stage_start_pos, stage_start_ang, ground/shadow/light/diffuse/wall_tbl_add at MH2's
  size. stage_start_pos's end is a guess (0x940, up to stage_start_ang).
- Em_Taisei_Set (game 0x7077F0) reads five tables; by the EMW field each fills: sleep 0x8BDA00, poison 0x8BCCB0, mahi 0x8BD180,
  sleep2 0x8BDB50, the new one (+0x7D4) 0x8BD550. data_map.txt has em_sleep2_data_tbl at 0x8BCCB0 and em_sleep_data_tbl at 0x8BD550.
  **Conflict:** this reading (by the EMW field each table fills) and data_map.txt name these addresses differently. Not settled.
- pc_fn_names.txt: 60 MH2 placeholder function names (game_func_..., sub_main_func_...) aliased to the host function that
  config/mh2/func_pairs.txt pairs them with ('table' pairs, score >= 0.9, defined on the host). Function pointers in MH2 data
  otherwise import as NULL: pl_prog_tbl[0] (pl01_adr_tbl) needed pl_local_init / pl01_effect_move.
- em_prog_tbl (gm_sub 0x90DE70, 35 kinds; per kind {local init, local init, ?, effect move, 2 functions MH2 added, data}): MH2's
  monster functions have no names, so rt_mh2.c gives G's emNN_local_init / emNN_effect_move to the kinds the host has (18 kinds).
  That MH2's kind N is G's kind N is checked for kind 1 only (its model is em01). func_pairs pairs MH2 kind 10's local init with
  MH1's em21_local_init: kinds may be renumbered.
- Per-stage tables of sub_main pointing into gm_sub are not pointer words for tools/mh2_pcmeta.py (SEES); rt_mh2.c turns them into
  host pointers. Words pointing into sub_main's .bss (ground_tbl_add[0] = 0x6AEE18) stay PS2 addresses.

## Blockers for CODE, ranked by what they block

1. **include/frame.h FRW has MH1 offsets.** Every frame_init / frame_move (f_frameb.c, f_frame.c) on MH2's PLW and EMW. MH2's places,
   from mh2_plw.h and mh2_emw.h (both agree): mt[4] 0x154 (MH1 0x194), chr[4] 0x29C (0x2DC), layers 0x2C0 (0x300), mdl_no 0x30F
   (0x34F), mdl 0x4EC (0x50C), sub_on 0x66C (0x720), sub_chr 0x670 (0x724), sub_rate 0x678 (0x72C). Tried on the PC only (not
   committed): with it the hunter gets through pl_init and frame_init into pl_move. Callers: pl_init_sub (f_pl.c), every em/pl
   motion set.
2. **HENT in src/main/hit/f_sphr.c is MH1's PLW/EMW view.** GetGroundHitStatusAreaPl (from pl_move_sub, pl_nm.c) reads ent->stg at
   MH1 0x736 (MH2 PLW 0x680): stage 0, a table word that is a PS2 address, crash. Blocks the hunter's first step and every ground
   and wall test of hunter and monsters (GetGroundHitStatusAreaEm, GetWallHitBitPl/Em). The same kind of local view is in light_nm.c
   (PLLC: 0x613, 0x710; MH2 0x597, 0x664) and likely elsewhere: every shared file the PC links that casts PLW/EMW to its own padded
   struct reads MH1 places.
3. **The quest's own flow.** G's Quest_condition_judging on MH2's condition program clears quest 1014 at tick 7. Needed: sub_main
   0x3A2E30 (state machine, called from the game task at 0x326E64; p003A2E30_nm.c, with the explicit 1 for the 0x3A5880 call) and
   0x3A9230 (event script, p003A9230_nm.c) linked in place of G's judging, then Quest_start 0x3A11A0 as C and its callees listed above.
4. **Monster per-kind tables and set-up.** Next crash: em_def_attack_set (from em_status_init) reads a per-kind table with no MH2
   address. Expect the rest of em_status_init's tables (Ikari_Data_Set, em_tsuushin_set, em_search_data_set, em_range_set,
   enemy_scale/size, em_hungry/suimin/thirst, em_pos_tbl ...): check each against MH2's function (game 0x7077F0 had five tables
   where G has four). Then MH2's monster code itself: em_prog_tbl[1] = game 0x79EFC0 / 0x79EFD0 / 0x79EF70 / 0x79EFE0 / 0x79A390 and
   gm_sub 0x90A9C0 have no names; em01_senkai_sub takes a parameter table on MH2 (CODE's note); Em_Dmg_Sys is 214 of 667
   instructions off (untrusted).
5. **func_pairs.txt names do not reach config/mh2/symbols** (1,897 pairs, none in the symbols): function pointers in data import as
   NULL on the PC. pc_fn_names.txt covers 60 by hand; folding the pairs into the symbols (or the symbol view of mh2_pcmeta.py) would
   cover the rest. data_load_ptr: data_map says 0x6AE004, load_stage_hit reads 0x6ADFE8.
6. **PLW part pointers** (MH1 0x110, 32 words): MH2 has other fields there (0x120 is MH1's 0x160): where MH2 keeps them is not known.
   The host keeps its part blocks to itself on MH2 (rt_pl.c); game C that reads them gets nothing.
7. Per-stage tables still at G's 92 entries on the host: stage_fog_tbl, st_sun_rgba_tbl, st_sun_tb_tbl, pl_light_tbl2,
   fishcam_ofs_tbl, St_unique_tbl, area_move_high_y_tbl, sun_pos_tbl, Fish_hani_tbl (no MH2 address for most).

## Host fixes that were MH1 offsets in src/pc

rt_pl.c and rt_player.c raw PLW offsets go through MH2_OFS_PLW on MH2 (only for bases inside player_work). rt_quest.c / rt_em.c
game_w+0x28 by field (MH2 moved it to 0x2A). Other raw offsets in src/pc/rt were not audited.
