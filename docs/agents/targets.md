# Targeted decompilation: what the PC actually runs (agent B, 8 Oct 2026)

Method (tools/targets/): `nm.py` lists main functions the PC build links that are NOT inside a
registered (byte-matched) range; `nm2.py` lists functions that ARE matched but whose PC definition
comes from some other object (an `*_nm.c` copy or a hand-written `src/pc/rt/rt_*.c`). Stand-ins that
really fire were collected from ~/.local/share/mh1pc/logs ("stand-in called").

## A. Matched in C, but the PC ran a copy (996 functions, 219 KB; switch = free proof)
Biggest groups by the object the PC linked instead of the matched file:
chat_nm 68, hit2_nm 11 (13.7 KB), f_quest_nm 63, menu_nm 54, menu_disp_nm 20, ud_nm 38, sk_all/hk_all,
pl_nm 24, f_frame_nm 17, cam_nm 29, cmd_nm, eft02_nm, mcact/mclow, omake_nm, bgm_nm, option_nm,
and hand-written rt_*: rt_gen (63 no-op stand-ins whose C is matched: trans_eft, SetPartsTrans,
weapon_dat_make, release_stage_model ...), rt_flmat 42 (math), rt_pl 23, rt_font 20, rt_flow 22,
rt_main 14, rt_eft 20, rt_snd 22.

## B. Not matched, PC runs a copy (233 functions, 248 KB)
Ranked by player impact:
1. Player: pl_nm.c (basic_com_ck, pl_move_sub, pl_turn_sub, pl_horm_sub, pl_mv021, pl_at008/9/12,
   pl_dm003/8, pl_egg05, Pl_item_stack, Pl_horm_adj, Pl_box_select, body_hit*, timer_calc_sub_pl, ...)
2. Collision: shit*_nm.c (GetWallHitBitPl/Em/2, sphr_face_o3/o4, PushAdjust3, GetGround*, GetWallHitLine ...), hitw_nm, tri_nm
3. Monster: em_move (f_em_nm), em_ride_sub (NO-OP in rt_main.c), em_material_sub (host stand-in, only raptors),
   em_work_set / pull_enemy_work / em_create_model (host), em_search_set, Game_task (rt_boot.c), round_init (NO-OP)
4. Camera: cam_nm.c / camr*_nm.c
5. Items/quest flow: f_quest_nm.c (quest_condition_prog, remuneration_item_set, Share_item_*), item_nm.c, reward_itembox
6. HUD/menu: menu_nm.c, menu_disp_nm.c (disp_item_sub_select ...), omake_nm.c
7. Effects: eft06_m, eft13_*, eft20_* (nm), light_*, set13_*
8. Draw: weapon3_nm.c (weapon_trans, pl_item_trans), trans_stage.c (PC rewrite, 15 KB)

## C. Stand-ins that fire in real play (log "stand-in called")
load_bin_req, load_busy_ck, stage_free, ot_init, view_reset, vib_*, clr_item_work, init_set_work,
em_effect_pull, round_init, load_shadow, load_eft, set_viewproj, flFlip, flCompact, flCalcTrans...
Most are platform (loader, GS, SPU) and correct to skip; the game-logic ones are round_init,
em_effect_pull, clr_item_work/init_set_work/init_item_work (matched C exists, item pool unused).

## Done (agent B, 8 Oct 2026)

Wiring (tools/build_pc.sh, tools/pc_patch.py, tools/targets/):
- cp01-cp03 (math library) linked from the matched files; the rt_*.c copies are weak.
- 92 + 68 matched main files (batch A: pl, frame, hit, quest, item, em/emw, weapon, light, stage, sound (not bgm), eft02/20;
  batch B: camera, hit2, menu, option, omake, ud) replace their `*_nm.c` near-match copies. The nm files stay linked weak for
  the functions that are still unmatched. A host-owned symbol (src/pc/rt/rt_*.o strong definition) is weakened in the matched
  object automatically, so the host version still wins where the game memory model is replaced (joint matrices, models,
  sound, files). The build needs `tools/targets/pcbuild.sh` (loops until it links).
- `func_XXXXXX` / `D_XXXXXX` names in matched main C (calls into game.bin, tables) are tail-jump shims / aliases to the
  game module's real names (tools/targets/dalias.py).
- argregs check of the newly linked files: calls that rely on "a0 left over" are patched for the PC in tools/pc_patch.py
  (Item_preparation_adrs, adx_se_set, Pl_stg_ck/Em_stg_ck, Pl_master_ck).

Byte matches: NormalClipCheckF3 and PointHitCheckF3 (cp01, now 0x120240-0x120C7C), both used by every ground query.
Near-match improved: hit_sphr_sphr2 18 -> 11 of 64 instructions (declaring `d` reused for dx / dz*t).
Not matched after the time cap: hit_cap_cap2_m (5012 B), hit_cap_cap3_m (3780 B), em_ride_sub (written, 448 vs 528 byte frame).

## Behaviour differences found (the "bugs just fixed")
1. em_ride_sub (0x10B060, Lao-Shan Lung): was a weak empty function in rt_main.c. The Lao's back was never a floor, no hunter
   ever got the "riding" state PLW+0x604 = 1/3 that the Lao AI (em_cmd_pl_ride_ck) reads, and nothing saved the joints' old
   matrices (sys_old_mat, filled by old_pos_save in move()). Now written from the asm (src/main/em/emride_nm.c) and run for kind 7
   from rt_monster_tick together with the old-matrix save. Not byte-exact; checked only for crashes (quest 101 smoke run).
2. em_material_sub (0x10CEA0): only the raptor case was ported by hand; the other 24 kinds (blinking eyes, the cut tail, broken
   parts, hair/wings per state, Fatalis/Lao colour fades, ...) drew every material. The game's own function (m2c output of the asm,
   src/main/emw/emmat_nm.c) now runs on a scratch material array and gives the hide mask for every kind.
3. System_timer never counted on the PC outside online play (the PS2 Scheduler does System_timer++ every frame): the map's boss icon
   pulse, the extras menu glow, and the em_material_sub blink table of kinds 9/18/23 read it. rt_sys_tick counts it now.
4. Wiring pitfalls found while switching (not game bugs): matched C that calls a file-static of its near-match
   (hit_hit_sub_em, eft02_move) ran a no-op stand-in; an `ALIASES` alias binds to the object that defines the target, which was
   the weak near-match copy (Plesioth em21_init: monster stuck idle); a symbol that is already weak must still be requested weak.
5. Near-match copy vs matched C, text comparison (tools/targets/cmpnm.py): QuestClearCameraRequest, reward_key_repeat,
   Menu_select_mv / ListSelect, Pl_light_set, parts_chg: same behaviour. The bgm_nm.c copy is NOT the same as the matched
   bgm01-04 on purpose: it checks str_getstat before keeping a BGM stream (the PS2 code leaves the village silent after the house),
   so bgm stays on the near-match (test_audio catches it).

## Still ranked, not done
hit_cap_cap2_m / cap3_m / sphr_sphr2 (monster attack capsules), the pl_nm.c player functions (46-60 % of the instructions
differ at scheduling level, the C logic was compared), cam_nm.c (cam_sub_std ...), eft06_m/eft13/eft20 (effects), trans_stage.c
(PC rewrite of 15 KB), Game_task (rt_boot.c), se_req2 (rt_snd.c), the chat/sk/hk/cmd/mc matched files (kept on nm: online chat,
soft keyboard with its own entry points, memory card on host files).
