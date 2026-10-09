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

# Agent B: main module, PC copies against matched C (8 Oct 2026)

Method (tools/targets/, see docs/agents/agent-B.md): `nm.py` lists the main functions the PC links that are not inside a
registered (byte-matched) range; `nm2.py` the functions that ARE matched but whose PC definition comes from another object (an
`*_nm.c` copy or a hand-written `src/pc/rt/rt_*.c`); `standins.sh` the no-op stand-ins (build/pc/rt_gen.c) that game objects call.

## 1. Matched in C but run as a copy on the PC: switched
Before: 996 functions (219 KB). After: 544 (124 KB), all of them host replacements that must stay (loader, GS/SPU, memory card,
fonts, sound, movie, model creation, the pools that the host owns) or the files kept on purpose (online chat; soft keyboard with
its own entry points; memory card on host files; bgm, whose near-match has a PC fix). Switched (the near-match stays linked weak
for the functions that are still unmatched): cp01-cp03 (math), pl (plx01-12, pl_wall, pl_itemck, pldmv, plegg, pl_snd01...),
frame, hit (hit, hitb-e, shit1/2a/5/6/7/15/16/401, tri01), hit2/hit3, quest (f_quest* 40 files, qstb*), item, em/emw, emsrch, weapon3, wtrans01,
light01-03, stage, reward key, eft02/eft20b, camera (cam*, camarea*, camr*), menu (menu01-41, pit*), option, omake, ud (f_ud).
The tests pass with all of it. Text comparison of the pairs (tools/targets/cmpnm.py) found no behavioural difference, but the
nm copies carried PC fixes that the matched files needed again (item 4 below).

## 2. Not matched, the PC runs a copy (232 functions, 252 KB), by player impact
1. Monster attack capsules: hit_cap_cap2_m 5012 B (74/1255 instructions off: two float registers swapped, f20/f22), hit_cap_cap3_m 3780 B,
   hit_sphr_sphr2 256 B (11/64). They are `c_rawfuncs` (original bytes) on the PS2 side, near-match C on the PC.
2. Player: pl_nm.c (basic_com_ck 2368 B, pl_move_sub 2144 B, Pl_item_stack, pl_mv021, pl_at008/9/12, pl_dm003/8, pl_egg05, pl_turn_sub,
   pl_horm_sub, Pl_horm_adj, Pl_box_select, Pl_basic_flagset, Pl_slash_*, Pl_shell_set, body_hit*, to_normal, timer_calc_sub_pl, em_ninshiki_ck).
3. Collision: shit8-14 (GetGround*, GetWallHitBit*, sphr_face_o3/4 (1936/2016 B), PushAdjust3 (4024 B), GetWallHitLine, GetEyeHitLine), hitw_nm, tri_nm (VectorHitCheck).
4. Monster: em_move (f_em_nm), em_search_set, em_dur_set, em_ride_sub (written, 448 vs 528 byte frame), em_material_sub (agent E's hand port is linked; emmat_nm.c is the m2c copy).
5. Camera: cam_nm (cam_sub_std 2664 B, point_cam_sub, SetCameraData), camr4/5/6 (Spline, Cardano, k_HitEmCamera, GetOrthogonalPoint).
6. Items/quest: f_quest_nm (quest_condition_prog 3420 B, remuneration_item_set, Quest_next_em_set, Share_item_*), item_nm, reward_itembox.
7. HUD/menu: menu_nm (Pit_mv 4/382 off, Pit_mv_lb 3/104, Pit_init, Pit_reset, Menu_mix_mv), menu_disp_nm (disp_item_sub_select 3872 B ...), omake_nm.
8. Effects/draw: eft06_m, eft13_*, eft20_*, light_*, set13_*, weapon3_nm (weapon_trans, pl_item_trans), trans_stage.c (PC rewrite, 15 KB).
Almost there (instructions differing / total, check.py): Sel_back_disp 2/36 (two instructions swapped), Pit_mv_lb 3/104, Pit_mv 4/382
(`now` goes through a0 in the original), menu_data_monster_sub 5/39, key_rept_du 5/60, GroundHitInit/WallHitInit 6/70, stolen_item_stack 6/138,
ZoomRateCalc 8/34. 45-minute caps were reached on Pit_mv, Pl_slash_lv_ck, flMemcpy, hit_sphr_sphr2 (18 -> 11) and cap2_m.

## 3. Stand-ins that fire or are called (rt_gen.c, 400 names)
Platform (loaders, GS, SPU, vib, online Bs*/CallBack_*/cnLBS_*, IME apiask_*): correct as no-ops. Game-logic ones found and fixed here:
flvecApplyMat, flMemcpy, flExp, flmatAddTrans2, ride_ofs_calc, Set09_set_ex / Eft25_set_pos (stage), frame_check_001263F0 (pl_snd01).
Left: apiask_* (kanji conversion API of the soft keyboard), save_file_req (option menu), flSndChange / flSndStatGet / flSndAllStop
(sndc03 sound parameter changes), Em_Senko_Ck (flash bomb, emw02 push_senko).

## 4. Behaviour differences found (the "bugs just fixed")
1. em_ride_sub (0x10B060, Lao-Shan Lung): was a weak empty function (rt_main.c). The Lao's back was never a floor, no hunter ever got
   the riding state PLW+0x604 = 1/3 that the Lao AI (em_cmd_pl_ride_ck) reads, and nothing saved the joints' old matrices (sys_old_mat,
   filled by old_pos_save in the game's move()). Written from the asm (src/main/em/emride_nm.c), ride_ofs_calc linked from its matched
   file, run for kind 7 from rt_monster_tick with an old-matrix save. Checked for crashes only (quest 101 smoke run), not in a play session.
2. em_material_sub: see agent E's section (hand port of all kinds, merged); my m2c copy src/main/emw/emmat_nm.c is not linked.
3. flvecApplyMat (0x172EE0) was a no-op stand-in from the first frame on; flMemcpy (lobby chat/net buffers) and flExp (sysw.c gauss table)
   too; flmatAddTrans2 (weapon3_nm). Found because linking the matched calc_mat_angY (which calls flvecApplyMat) broke the tranquilizer test.
4. System_timer never counted offline (the PS2 Scheduler counts it every frame): map boss-icon pulse, extras menu glow, the blink tables
   of em_material_sub kinds 9/18/23. rt_sys_tick counts it.
5. Stage code (f_stage_nm / f_stageb) called Set09_set_ex and the lobby's Eft25_set_pos by address into no-op stand-ins; pl_snd01's
   footstep/frame sounds called frame_check through an alias name that was a no-op stand-in.
6. Wiring pitfalls (not game bugs): matched C that calls a file-static of its near-match (hit_hit_sub_em, eft02_move) ran a stand-in;
   a linker alias binds to the object that defines the target, which was a weak near-match copy (Plesioth em21_init stuck idle);
   `a0 left over` calls in matched C need the PC patches again (tools/pc_patch.py: Item_preparation_adrs, adx_se_set, Pl_stg_ck, Pl_master_ck);
   str_gattai (strg01.c) uses the MWCC va_start (stdarg now); the bgm01-04 matched files lack the str_getstat check of bgm_nm.c (village
   silent after the house): kept on bgm_nm.

## 5. Byte matches (6 functions, main 42.054 % -> 42.150 %)
- NormalClipCheckF3, PointHitCheckF3 (cp01, now 0x120240-0x120C7C): used by every ground query (shit8).
- menu_data_monster_sub (menu_dmon.c): Monster_list_search returns s8 and the temp is a `long`.
- maru_disp_sub (menu_maru.c), get_near_point_sub (camarea_gnps.c, camera rail sections), font_print_quest_time (menu_ftime.c, buf[8],
  `(t / 30) & 0xFFFFFFFF`): all three found with the permuter (tools/perm.py, 8-10 minutes each) and then written back naturally.
  The permuter solved 4 of 17 functions that were 2-13 instructions off; it cannot read K&R definitions (omake_nm.c, f_quest_nm.c).
Tried without success within the cap: Pit_mv, Pit_mv_lb, Sel_back_disp, key_rept_du, WallHitInit/GroundHitInit, stolen_item_stack, ZoomRateCalc,
Pl_slash_lv_ck, Pl_slash_calc, pl_at012, aan_ofs_calc, em_dur_set, Get_cam_grid_XZ, disp_others_info, Quest_next_em_set, FaceLinePos, Item_preparation,
point_cam_sub, hit_sphr_sphr2 (18 -> 11 of 64), hit_cap_cap2_m (the two float temps t2 / h swap registers, nothing moves them).

## 6. Second pass (agent B, 8 Oct 2026): hit detection, player, collision
New tools: tools/calldiff.py (ordered call-target diff of a near-match against the original; finds a wrong callee),
tools/sigdiff.py (float-op and constant/offset multiset diff; useful where semdiff is noisy). Method that found the bugs:
`tools/draft.py main FUNC` (m2c) read against the near-match C, then calldiff for the callee list.
Real behaviour differences fixed:
1. GetWallHitBitEm (monster wall push, shit11_nm.c) called sphr_face_o4; the original calls sphr_face_o3, the same
   sphere-vs-wall test as the player (o4 is only used by GetWallHitBit2). Monsters used the radius-adjusted height test and
   the near-point y rewrite of o4.
2. PushAdjust3 (shit14_nm.c, multi-contact wall push, both players and monsters): the two "edge contact is behind the face"
   tests were inverted (`!(dot <= 0)`); the asm records the cover when dot <= 0 (checked in the asm: c.le.s, xori 1, beqz).
   Wrong contacts were dropped from the push sum. A[]/B[] are now zeroed because the original reads A[k] past nA.
Verified equivalent (no change): hit_cap_cap2_m, hit_cap_cap3_m (every differing instruction is the f20/f22 swap of t2 and h,
a commutative float add, call-argument scheduling, or the unreachable default of the inner switch; calls match by name;
no data relocations), hit_sphr_sphr2 (float registers and a commutative add only; 18 -> 11 stays, a declaration-move search
over all 22 locals of cap2_m found nothing), basic_com_ck (switch ladders and branch order only), pl_at009, pl_turn_sub,
Pl_slash_lv_ck, Pl_shell_set, body_hit, body_hit_sub_em/new (aligned diff), sphr_face_o3/o4 (the original duplicates the edge
loop for the two centre points; ours uses a pointer), GetWallHitBit2, GetWallHitBitPl, hosei_sub, GetFloorSlide,
GetGroundHitArea, GetWallHitLine, GetEyeHitLine (a quirk kept: the seen-polygon test only compares seen[0]).
Not reached in the coverage run (test_all_quests + loop + activities): hit_cap_cap2_m/cap3_m, hit_sphr_sphr2, pl_move_sub.
Not done: GetGroundHitStatusAreaPl/Em, GetGroundHitAreaUpper, pl_move_sub (sigdiff shows one c.lt.s the original has and ours lacks),
gun_adj_sub, Pl_item_stack, camera (cam_nm, camr*).
