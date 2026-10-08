# Targeted decompilation: the game overlay and the village lobby (agent E, 8 Oct 2026)

Scope: `src/game` (game.bin) and the single-player lobby/village code (`src/lobby`, not plaza or online).
Agent B writes the `main` section of this file; this section is the part for the two overlays.

## How the list was made

* `tools/unmatched.py game|lobby` lists every function that is not inside a registered (byte-matched)
  range of `config/c_files.txt`. These are the only functions where the PC can run something that is
  not proven.  game: 40 functions, 48 KB (95.5 % of the overlay is matched).  lobby: 1740 functions,
  506 KB, but only 15 of them have any C at all (the lobby near-match files are mostly stale copies of
  functions that are matched in other files; the PC links the matched version, see
  `tools/pc_lobby_matched.txt`).
* Which of them really run: `COV=1 tools/build_pc.sh` builds with `-finstrument-functions`
  (`src/pc/rt/rt_cov.c`); with `RT_COV=file` every process appends the set of functions it entered.
  `tools/cov_report.py COVFILE BIN LIST...` maps that to names and marks the unmatched ones (RAN / ---).
  Not part of a normal build.
* Which of them are *wrong*: `tools/semdiff.py FILE.c [FUNC..]` compares the original and our compile
  of every near-match function as multisets of instructions with register names, relocation
  targets and branch targets erased. EQUIV = only register allocation / scheduling / block order
  differ. DIFFERS lists the instructions only one side has (a different constant, offset, compare,
  a missing or extra operation). It is a screen with false positives (jump-table bounds, signedness
  of promoted u16, inlining), not a proof; every DIFFERS was read against the asm.
* Host code that stands in for game/lobby functions: `nm` of `rt_em.o` against the other objects
  shows which of the weak `WSTUB`/`WEAK` definitions in `src/pc/rt/rt_em.c` are actually used. Only
  `src/pc/rt/rt_em.c` (`em_sleep_eff_set` ABI shim) and `rt_snd.c` (`ashi_sd_req`) define a
  game/lobby symbol strongly; everything else the host provides for these two overlays is a weak
  fall-back that a real definition overrides.

## What the numbers say

* game.bin: 2640 functions, 95.48 % of the bytes matched. The 40 unmatched functions are all
  near-match C (`*_nm.c`) the PC links; there is no game.bin function without C. 31 of them ran in the
  headless tests (quest loop, progression, urgent, all 38 offline quests, 34 village activities, frog).
* lobby.bin: 3272 functions, 38.1 % matched, but the unmatched rest is online code (cnLBS, browser,
  plaza, zlib, OpenSSL, 231 stand-ins in `build/pc/rt_gen.c`) and 15 near-match functions; only four of
  those run offline (`lb_disp_name`, `lb_pl_turn_sub`, `lb_insert_target_list`, `Lb_set_mini_data`).
  Most of what `src/lobby/**/*_nm.c` shows as "differing" are stale copies of functions that are
  matched in other files (the PC links the matched ones, `tools/pc_lobby_matched.txt`).
* Host (`src/pc`) definitions of game/lobby symbols: `em_sleep_eff_set` (ABI shim, fine),
  `ashi_sd_req`; weak fall-backs in `rt_em.c` that were still in use for game functions that have C:
  `em01_local_area_move_init` and `Em_Mode_Chg` (now linked, see below).

## Fixed: the PC ran something different from the original

Found by reading the asm against the near-match C after `semdiff.py` flagged it, or from the coverage run.

1. `em_cmd_ground_area_move` (monster command: walk to the next stage along an area route). The copy
   cast the wall test ray flat on the xz plane (`SetVector(v, x, z, 0)`) instead of from
   `(x, y + 10 * byte, z)` to the route point at the same height, and tested the wrong halfword of the
   route point (+0x12, the move index, instead of +0x10, "needs a wall test"). So a monster that had to
   cross a wall-blocked route tested a ray in the wrong plane and used the wrong flag. Now follows the asm
   (field `hit_ck` named in `include/em_cmd.h`); 152 of 253 instructions differ from the original only by
   saved-register allocation.
2. `em12_main` (the Em_Dmg_Sys result switch). The original's jump table has case 14 (the 12 reaction
   when the monster is not kind 0x19) and case 12 *without* the kind test; the copy applied the kind test
   to 12 and ignored 14. A 0x19 monster now reacts to result 12 and a result 14 is no longer dropped.
3. `em01_local_area_move_init` (kinds 1 and 11, 0x5665F0) was not linked: the PC ran the weak no-op of
   `rt_em.c`, so em01/em11 kept the stay and run-away timers of the previous area when moving to a new
   one. `src/game/em/em_modechg.c` (matched) is now picked in (`PICK_X` in `tools/build_pc.sh`), with
   `Em_Mode_Chg` (the host copy in `rt_em.c` was equivalent; the matched one wins now).
4. Lobby effect 25 (`eft25_i`, `_m`, `_d`, `_e`, `_t`: spark/aura particles on a joint; the Felyne
   canteen / NPC emotes call `Eft25_set`) was missing: only `Eft25_set` and `eft25_move` were linked, so
   the effect started and did nothing. The matched `eft25_i/_d/_e/_t` files (`lb_ge2502-05.c`) are
   linked now and `eft25_m` (near-match in `lb_e25.c`, written from the asm) is picked in.
5. `lb_disp_name` (name above each avatar): the job icon x was computed from `(s32)` truncated
   `spF0[0]` before the 1.25 scale; the original scales the float first (one of the three uses
   truncates, two do not). Up to 1.25 pixels.

Checked against the asm and found equivalent (so the "differing" figure is only allocation/scheduling):
`em_char_set` (monster motion request), `em_cmd_escape_area_set`, `em_cmd_dansa_sel`,
`em_cmd_angle_ck` (its `!= -1U` test on a byte is dead in the original too), `lb_target_angle`,
`Em_Mode_Chg`; the neck code (em_neck_move_sub, neck_ang_set) was read for logic only, the original uses unsigned shifts and compares where the copy has u16 promoted to int (same values).

## Ranked list: unmatched game functions (what the PC runs from `*_nm.c`)

RAN = entered in the headless tests. EQ = `semdiff.py` says only register allocation / scheduling /
block order differ. Diff counts are `check.py` (inflated by shifts).

| # | function | bytes | ran | state |
|---|----------|-------|-----|-------|
| 1 | `em_neck_move_sub`, `neck_ang_set` (em_core_nm.c) | 1956, 1392 | yes | head turning every tick; unsigned/signed shift noise only, logic read OK; 35 / 73 real hunks |
| 2 | `em_char_set` | 936 | yes | motion request; read against asm, equivalent (stack spill vs fp) |
| 3 | `em_cmd_end_command`, `em_cmd_samestage_pl_target_sel`, `em_cmd_all_pl_target_sel` | 2276, 1052, 900 | yes | AI program interpreter / target pick; semdiff differences are frame size, u8 loop temps and a duplicated constant load; only the area-route case of end_command and the first half of samestage_pl_target_sel were read against the asm |
| 4 | `em_cmd_ground_area_move` | 1012 | yes | FIXED (real bug), allocation left |
| 5 | `em12_main` | 1660 | yes | FIXED (switch), jump table vs ladder left |
| 6 | `em_cmd_escape_area_set`, `NextStage_No_Set`, `NextStage_Dir_Set` (EQ, 3 instr off) | 812, 856, 540 | yes | area change; read OK |
| 7 | `em_cmd_flag_set/_clear/_ck`, `em_cmd_range_ck` (EQ), `em_cmd_angle_ck`, `em_cmd_horm_pos_ang_ck` | | yes | 5-7 instr off each, scheduling |
| 8 | `Em_Master_Change`, `Em_Taisei_Set` (EQ), `em_range_set` (EQ) | 1224, 108, 180 | yes | allocation only |
| 9 | `em09_act_set`, `em20_act_set` (1 instr: `daddiu`), `em09_material_sub` (agent D) | | yes | |
| 10 | `eft04_t` (6 instr), `eft05_t` (spline scheduling, flag is s16 in the original), `eft11_i`, `eft18_set_com` (EQ, one delay slot), `eft22_end_init` (EQ, 4 instr) | | yes | |
| 11 | `set17_trans` (EQ) | 872 | yes | |
| 12 | `shell08_m`, `shell08_trans` (written from the asm, 1544 differ, stack layout) | 7584, 6320 | yes | no semantic difference found, not re-read in full |
| 13 | `set05_m` (2 instr), `Set20_set` (EQ, block layout), `shell22_i`, `shell08_rgba` (EQ), `em_cmd_st25_pl_target_sel`, `em_cmd_pl_ang_sel`, `em09_effect_move`, `print_tuto_message` (EQ, register swap) | | no | |

Byte matches reached in this session: none. Every function above is 1 to 8 instructions off or a pure
allocation difference. What was tried (beyond the previous agents' work): `declbf.py` (NextStage_Dir_Set:
8 -> 3 instructions), 20 minute permuter runs on NextStage_Dir_Set, em20_act_set, set05_m,
eft22_end_init, print_tuto_message (no improvement), expression-order and type variants listed in
`docs/agents/agent-E.md`.

## Lobby (single-player village)

* Unmatched with C: `lb_disp_name` (FIXED, float scale), `lb_pl_turn_sub` (EQ), `lb_insert_target_list`
  (EQ, 8 instr: `c` lands in t2 instead of a2), `Lb_set_mini_data` (builds the 24 byte member record for the
  network; ran once, no offline effect).
* Matched in C but not linked until now: `eft25_*` (see above). The remaining lobby stand-ins that ran are
  all online (cnLBS_*, Bs*, sceHTTP*, plaza, lm_*, `lb_member_*Check`).
* Stand-ins in `rt_gen.c` that fire in offline play and belong to main are agent B's list.

## Tools added

* `tools/semdiff.py FILE.c [FUNC..]`: the equivalence screen described above.
* `COV=1 tools/build_pc.sh` + `RT_COV=file` + `tools/cov_report.py`: function coverage.


## Semantic review table (round 2, agent E)

Screen: `tools/semdiff.py` over every `*_nm.c` (game, lobby, main), restricted to functions that are still unmatched,
ranked by coverage (RAN) and by mismatching memory-op offsets/widths/signs (stack accesses ignored). About 370 functions
are flagged; most are inlining, macro expansion or scheduling. Only the ones below were read against the asm.
main files claimed by E for this review (B: please skip): pl_nm.c (pl_horm_sub, Pl_item_stack, gun_adj_sub), camr5_nm.c,
camarea_nm.c, eft20_nm.c, eft06_nm.c, set13_nm.c, menu_disp_nm.c, light_nm.c. Left to B: shit*_nm.c, cp*, hit*_nm.c.

| function | verdict | note |
|---|---|---|
| `pl_horm_sub` (main/pl_nm.c) | FIXED | `work81A` (the hunter's lean toward a locked-on monster) was read as s16; the original reads it as u16, so `v >= 0x8000` was never true for negative values and the near-zero band was snapped to 0 instead of stepping by 0x400 |
| `em_cmd_ground_area_move`, `em12_main`, `lb_disp_name` | FIXED | see above |
| `k_HitEmCamera` | equivalent | the original expands the sign test by hand (4 compares incl. -0.0); same result as `PUSH_ACC` |
| `Get_cam_grid_XZ`, `NextStage_No_Set` (lh/lhu of x73A), `Pl_item_stack` (lh/lhu of work88E), `em_cmd_dansa_sel`, `em_cmd_angle_ck`, `em_cmd_escape_area_set`, `em_char_set` | equivalent | scheduling / sign of values that never reach 0x8000 |
| `em_cmd_end_command` | area-route case equivalent | rest not re-read |
| `eft20_m`, `eft06_m`, `set13_trans`, `Pl_light_set`, `light_init` | not decided | layout differs (switch order, unrolling, helper inlined); no constant or offset mismatch found yet |
| `em_neck_move_sub`, `neck_ang_set`, `Em_Master_Change`, `shell08_m`, `shell08_trans` | not decided | logic read once, constants and offsets agree with m2c of the asm; not proven |

Quest 154 (`tools/test_all_quests.sh 154`): not a game-logic regression. With `RT_BODY_HIT=0` it passes on the merged tree
and on main's own binary, with the default (body_hit on since main 22db5619) it fails: the quest's boss is kind 8 on stage 56,
the hunter reaches it, and the test aid then warps the hunter to the monster ("carve point -1", every tick) while the
monster never takes damage and never dies. Most likely the test aid's warp into the monster and body_hit pushing the hunter
out of the body cancel each other for that monster's body size (same family as the tail_cut fix in c3983857), not a monster AI bug.


---

# Agent B: main module, PC copies against matched C

## What the PC actually runs (agent B, 8 Oct 2026)

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
2. em_material_sub (0x10CEA0): when this work started only the raptor case was ported; agent E ported the other kinds by hand
   in rt_em.c at the same time (merged: theirs is linked). My independent m2c-based copy of the whole function is kept as
   src/main/emw/emmat_nm.c (compiles on the PS2 side, 1875 instructions like the original, not linked on the PC): it can serve to
   cross-check the hand port, or as the start of a byte match.
2b. flvecApplyMat (0x172EE0, the 4x4 vector transform) was a no-op stand-in, called from the first frame on. The matched
   calc_mat_angY (cp01) calls it, so linking the matched cp01 broke the tranquilizer test (the Rathian never fell asleep); host
   implementation added (rt_flmat.c), along with flMemcpy (no-op: the village's net/chat buffers) and flExp (no-op: sysw.c's gauss
   table); flmatAddTrans2 and ride_ofs_calc are linked from their matched files.
2c. Stage code (f_stage_nm.c / f_stageb.c) calls Set09_set_ex (game.bin) and Eft25_set_pos (lobby overlay) by address: both ran as
   no-op stand-ins (the stage item-point glitter and the village effect). Wired with -Dfunc_618F00= / -Dfunc_60E330=.
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
