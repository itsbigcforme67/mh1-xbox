# MH Dos: monsters and the quest's end on the PC (HOST agent, round 4, 3 Nov 2026)

Host side only (src/pc, tools/build_pc.sh, config/mh2/pc_data.txt). Written from our own disassembly of the owner's disc
(asm/mh2); seen in hidden runs of build/mh2/pc/mhview_2. Community mh2-decomp not opened. Nothing here is byte-checked: it is host
C standing in until there is matched C. "Guess" marks what was not checked.

## What runs

    RT_MH2_EM=1 RT_STEP=1 RT_QUEST_TRACE=1 build/mh2/pc/mhview_2 disc/mh2 --quest 1014 --input "idle*30,up*120" --time 30 --shot x.png

900 ticks with the monsters on, no crash (round 3 stopped in the first em_move, Stage_Hate_Add). The hunter walks as in walk.md.
The quest's monsters: the Rathian (kind 1, entry stage 120, 2000 HP, level 0, slot 0, em_cmd.afs entry 3) and a kind-10 monster
on stage 110 (200 HP, slot 1, em_cmd.afs entry 0). Both go through MH2's em_move every tick (trace line `rt_mh2_em: em_move`).
**Neither moves yet** (see "Next").

Both quest ends get past mode 3 (on this branch alone and on a test merge with mh2-shared 3a3b075e, not committed):
- `RT_QUEST_TIME=200 ... --input "idle*30" --time 40`: time up at tick 263, mode 3 at 414, steps 1-3, **mode 5 at tick 758**;
- `RT_MH2_TARGET=0@100 ... --input "idle*30" --time 110`: cleared, mode 3 at 2,059, **mode 5 at tick 2,403**;
- both then stop in game5 -> result_prog -> records_update (src/main/reward/f_reward_g.c): `hunt_group_tbl[kind]` is NULL on MH2
  (G's table; no MH2 address). Without `--input` the viewer never starts the game camera (rt_cam_init needs play), and game3's
  move() then calls CameraMove with CameraWork.pl NULL: use --input for these tests.

## Host C written (src/pc/rt/rt_mh2_em.c)

| function | address | note |
|---|---|---|
| em_move | gm_sub 0x904CF0 | MH2's shape: no Stage_Hate_Add; kinds by jump table (below); new counters 0x96E/0x9A3/0x973/0x9AC/0x9AD; the hagi point; speed from 0x884 x (0x8AE+100)/100. Strong over f_em_nm.c's (weak) |
| (hit stop) | sub_main 0x39C090 | +0x40A added into +0x40B, else +0x40B counts down |
| (end of anger) | game 0x709A40 | attack/defence back to the kind's row (0x708E10 / 0x708EE0 = def_row), speed 1.0 |
| (stage map) | sub_main 0x3799A0 | table 0x43ED80 |
| (side action) | sub_main 0x3B5880 | 1..7 for a same-side hunter in action 0x2B0..0x2B6 |
| (eye reset) | game 0x724880 | em_eye_dmg_reset_act_set's MH2 kinds; 0x7F9080 / 0x7DE790 taken as em01_act_set / em08_act_set (guess from G's arguments) |
| (kind main) | gm_sub 0x904AB0 | jump table 0x932FC0, 74 kinds: the G emNN_main named in it. Kinds 48, 51-68, 72, 73 call MH2-only functions (game 0x7588C0, 0x817CD0, ...): skipped with a warning. Kind 40 gets G's em40_main (guess) |
| (hagi point) | sub_main 0x3A66B0, 0x3A6680 | StiEM_data at 0x5D87C0 read as MH2's 0x20-byte entries; Em_hagi_point_clr uses +0x9A8 (checked in the carried part's object) |
| Em_direct_set | sub_main 0x3A3E80 | MH2's entry (below); the model slot at game_w+0x2A; stage from the entry when em_tsuushin_tbl (0x8BEE60) marks the kind |
| Quest_em_init_set | sub_main 0x3A2030 | station set 0x3A7A00 + Quest_next_em_set; f_quest.c's renamed away for MH2 (tools/build_pc.sh) |
| Quest_next_em_set | sub_main 0x3A4370 | kind 0x12 when the stage is game_w+0x31; kind 0x48 on 0x3AAE80's roll; f_quest_nm.c's (weak) renamed away too |
| rt_mh2_quest_em_init | sub_main 0x3A77E0 / 0x3A7920 / 0x3A73E0 | called by Quest_start (rt_mh2.c) for G's quest_em_init |

Left out (each said in the file): gm_sub 0x90AAF0, the online parts (0x39E400 is 0 offline), sub_main 0x3E5680 (stage Eft23
effects), em_herb_set, game 0x86BBC0, the level roll 0x3A7230 (**level 0 for all**, guess) and the size roll 0x3A7170 (**size
100 for all**), MH2's Em_Taisei_Ck 0x724AB0 (G's host copy runs: four tables, not MH2's five; kinds > 34 get no effects).
smell_ck: MH2's takes the monster only; G's host copy gets G's 0x23.

## Layouts found (MH2, from the asm)

- **game_w model slots are at +0x2A** (Quest_next_em_set 0x3A43B0 `lbu 0x2A`), as include/game.h's MH2 GAME_W has them. The
  G-form f_quest.c / f_quest_nm.c scan them by raw offset `(u8 *)&game_w + 0x28`: the Rathian's model was never made (slot 0
  read kind 0) and kind 10 got kind 18's model. Also game_w+0x30 is the em_cmd level (G's x2E), +0x31 G's x2F, +0x14 the stage.
- **MH2's quest monster entry (0x3C bytes)**: +0 kind, +2 type, +6, +8 stage (u16; G's +7 byte), +0xC/+0x10/+0x14 hunger,
  thirst, sleep, +0x18, +0x1C angle, +0x20 position, +0x2C, +0x2E flags, +0x30 size, +0x32 hit points (G's +8), +0x34 em_work id
  (G's +0xA), +0x35 boss flag (G's +5), +0x36 level. G's quest_em_init wrote -1 to +8 (MH2's stage): the Rathian got stage
  0xFFFF and 0 HP and died at once. Every G-form quest C that reads QEM (quest_enemy_ck, revival, em_work_serch, ...) reads G's
  places on MH2: **for SHARED** (include/quest.h QEM needs an MH2 view).
- **em_prog_tbl is gm_sub 0x90DE10, 74 kinds** (em_init_sub 0x903EE0). data_map.txt's 0x90DE70 is its kind-24 entry: the host's
  kind N read kind N+24's program table and kinds past 49 read the u16 table after it (kind 66 jumped to data). Placed in
  config/mh2/pc_data.txt. The G stand-ins (rt_mh2.c em_prog_fix) now go by the kind's family from the em_move jump table, not
  the same number (13 kinds get them; kinds with family 0 are refused by em_create_model).
- **em_hit_push_tbl is game 0x8B9FC0, 74 kinds** (HitWallPlayer 0x330EB0); the G address read junk (kind 10 crashed in
  HitWallPlayer). Placed in pc_data.txt. The online table D_7F1980 is not.
- em_init_sub's kind switch (jump table 0x932E90) groups the kinds as em_move's does; kind 1's init 0x7E7810 is em01_init, then
  quests 0x3F6/0x3F8/0x434/0x436 move the monster (event flags 0x9C/0xAB), then em01_act_set(em, 0, 1, 0). **Not written:** the
  host's G em_init_sub runs (G's em01_init gives the Rathian 2000 HP through em_hp_vital_set2).
- rt_quest_monster_stage (rt_em.c) reads the entry's stage at +8 on MH2 (RT_QUEST_STAGE=1 now finds stage 120).
- rt_pl.c player_work_ptr and rt_dbg.c step 0x970 per player and 0x9D0 per monster on MH2 (SHARED's include/version.h has
  PLW_STRIDE: switch to it at the merge).

## Test aids

- `RT_MH2_EM_HERE=kind`: that kind's quest monster on the hunter's stage at (1500, 1500) (the hunter is not placed yet when the
  monsters are made).
- The em_move trace (RT_QUEST_TRACE): each monster every 120 ticks, `em_move <slot> kind stage mode act/old at x y z ang`.

## Next

1. **Why the monsters stand still.** With RT_MH2_EM_HERE=1 the Rathian sits in mode 2 act 0 (em_fly00) at x05 = 1 waiting for
   em_frame_check(38): its motion layer 0 has end 0, frame 0, chr 0 (gdb). So the motion of em_char_set(0x12) is not set up on
   MH2's EMW (FRW is MH2's since WALK; look at em_char_set and the monster's motion load in the host). em_frame_check
   (f_framec.c) also reads the speed at MH1's +0x930 raw (MH2 0x884). Kind 10 sits in act 0 with x2D4 0.
2. records_update's hunt_group_tbl (f_reward_g.c) for the result screen; MH2's result code has no host C.
3. Stage 120 (RT_QUEST_STAGE=1): StageCamInit -> G's default_area_data reads stage_camera_data_tbl[4] by stage; MH2's
   default_area_data (sub_main 0x3204B0) does not read it at all (it fills the block and calls yn_proxy_wk_load). For SHARED (camera C).
4. Per-stage host tables still at G's 92 entries (hunt.md blocker 7): stage_fog_tbl, st_sun_rgba_tbl, st_sun_tb_tbl, pl_light_tbl2,
   fishcam_ofs_tbl, St_unique_tbl, area_move_high_y_tbl, sun_pos_tbl import from G addresses in MH2's image (junk past 92).
   data_map.txt's candidates (0x4485B0, 0x41CCC0, 0x4416D0) are 0x10 / 0xD0 / 0x10-byte labels, not 199-entry tables: not used.
   Not done.
5. MH2's Em_Taisei_Ck (0x724AB0), the level / size rolls, MH2-only kinds' mains and inits.

# Round 5 (HOST, 3 Nov 2026)

Host side only; from our own disassembly; seen in hidden runs. Not byte-checked.

## Monster motions play

MH2's create_pl_motion / create_em_motion (f_frame.c, MOT_PL 710 / MOT_EM 1910) put the handles at 710 + 300n and 1910 + 600n
(MH2's frame_init, sub_main 0x2E5190, adds 710 / 1910; it is not carried). f_frameb.c's frame_init read MH1's 500 / 1700: the
monster's layers got handle 0, end 0, and em_frame_check never let it leave its first act (round 4 Next 1). The PC now has MH2
frame_init / frame_init_b in src/pc/rt/rt_motion.c (f_frameb.c's renamed away for MH2 by tools/build_pc.sh) and
motion_set_handle_tbl at MH2's size (0x4360, to em_mot_han_ofs 0x5DE3E0). **For SHARED:** the shared fix is the two constants in
f_frameb.c under VERSION_MH2 (then the host copy can go).

    RT_MH2_EM=1 RT_MH2_EM_HERE=1 RT_STEP=1 RT_QUEST_TRACE=1 build/mh2/pc/mhview_2 disc/mh2 --quest 1014 --input "idle*30,up*120" --time 30 --shot x.png

The Rathian goes act 0 -> 16 -> 22 -> 1, rises to y 1703, flies from (1500, 1500) to (5109, 5109), lands, turns (ang 19924 ->
8192 -> 512), takes off again (trace lines `em_move 0 kind 1 ...`); RT_EM_DRAW_TRACE shows motion 1018 posed with the body at y
1062. Not seen on screen (the camera looks away from where it flies). Kind 10 animates (motion 1001) but stays in act 0. On its
own stage (120, without RT_MH2_EM_HERE) the Rathian stays in act 1: the hunter is not there.

## Both quest ends finish

- hunt_group_tbl is sub_main 0x44C800, 74 pointers (lists of a species' kinds, each starting with the kind): read by the helpers
  0x3BBBC0-0x3BC090. data_map.txt's 0x44C860 is its kind-24 entry. em_base_size 0x44C930 (74 floats). config/mh2/pc_data.txt.
- MH2's result screen is not G's result_prog: game5 (0x3274F0) calls plsel 0xD683C0, a 26-step state machine over plsel's own
  screens. **Not ported.** A host stand-in (rt_mh2_quest.c result_prog) waits 30 ticks and leaves as 0xD683C0's last step does
  (mode 6, step 0, all_reset); no rewards, records, kill counts or clear bits are written.
- MH2's village is not ported: mode 6 stays dark (viewer.c village_step), the run ends at --time.
- `RT_QUEST_TIME=200 ... --input "idle*30" --time 40`: result at tick 758, mode 6 at 789, exit 0 (also with RT_MH2_EM=1).
  `RT_MH2_TARGET=0@100 ... --time 110`: cleared, result at 2,403, mode 6 at 2,434, exit 0.

## Per-stage tables (199 stages)

Found by their readers in MH2's code:
- stage_fog_set 0x3788F0: no per-stage table; stage 0x56 takes the block 0x4485C0, stage 0 (and any stage without an environment
  block) 0x4485B0, else stage_work+0x50. Host C in rt_mh2.c; putspr2.c's renamed away for MH2.
- stage_set_set 0x378A30 (p00378A30.c): fixed sun (5000 x3, lines 0, colours 0xFFFFFF): the sun_pos / st_sun_tb / st_sun_rgba
  rows all point at that block (rt_mh2.c), host arrays 199 rows.
- area_move_high_y_tbl: game 0x8BF9C0, f32 per stage (game 0x7333D0: EMW+0x680 << 2), up to st05_land_pos 0x8BFCE0.
- fishcam_ofs_tbl: MH2's fish_cam_sub (0x31D7F0) keeps a byte per stage (0x41CCC0, 0xFF none) indexing 24-byte rows (0x41C9F0);
  the host's per-stage pointers are built from them. MH2's per-index field of view (0x41CC50) is not used (G's stays).
- Not done: St_unique_tbl (lobby only), pl_light_tbl2 (MH2 has none; rt_mh2.c's lights already), stg_eft_mdl_no /
  stage_sphr_tbl (rt_data.c imports them from addr.txt's mapping of MH1 addresses, 0x441170 / 0x449BB0, unchecked; now 199 rows).

## Host strides

rt.h includes include/version.h: the host uses EMW_STRIDE / PLW_STRIDE / ITEM_ROW (0xA10 / 0xA00 hard-coded before; rt_pl.c's
and rt_dbg.c's own copies are gone). viewer.c reads em_work by MH2 offsets through EMO() (stage 0x736, motion 0x2DC, model
0x34F, ...). Other host em_work offsets (rt_em.c, rt_player.c, rt_pick.c ...) are still MH1's on MH2.

## Next

1. The Rathian on screen: put the hunter where it lands, or the camera on it; then its hits on the hunter.
2. MH2's result screen (plsel 0xD683C0) and the village.
3. Kind 10 in act 0 (its family's G main may not fit MH2's kind 10).
4. Host em_work offsets outside viewer.c (rt_em.c rt_em_materials, rt_player.c test aids) through MH2_OFS_EMW.
