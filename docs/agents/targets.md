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

## Done (see bottom)
