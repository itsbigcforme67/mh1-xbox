# Agent A notes (game overlay: shell08, set09)

## set09 (0x618CA0-0x61ED08) - whole file matches
src/game/set/set09.c, 52 functions, jump tables 0x689E00-0x689EC8.
Verified: tools/check.py all OK, `tools/rebuild.sh game` = game OK.
A stage effect manager: kind 0 is a controller whose work holds 32 child
pointers and per-kind counts; kinds 1-11 are small ambient objects (see the
file header; what each kind looks like in game is a guess). Types are
file-local (SET09W, SET09W1, SET09W5). Functions that are LOCAL in the
original are `static` here (checked: still matches).

## shell08 (0x62D4D0-0x6331C0) - split
Matching and built:
- shell08.c   0x62D4D0-0x62EA1C Shell08_set_ang .. shell08_i (+ rodata 0x68A0D0-0x68A120)
- shell08b.c  0x6307C0-0x63099C shell08_h .. shell08_trans_sub
- shell08c.c  0x632250-0x632AB8 shell08_impact_set .. shell08_z_adj (+ rodata 0x68A210-0x68A240)
- shell08d.c  0x632E70-0x6331C0 shell08_type6_init .. shell08_type8_pos_set
Types in include/shell08.h (new header, SH08W at sh->senko, SH08P particles).
Verified: check.py OK for all, rebuild game OK.
Still assembly:
- shell08_m (7584 bytes): near-match in shell08_nm.c, ~1065/1898 differ.
  Control flow / structure matches (sdiff shows only prologue and a few
  spots); the rest is register allocation: original keeps `flag` in fp,
  em spilled at sp+0xB0, w->p spilled at sp+0xC0, num s6, all s7, idx s0,
  step s5, t s1, y f20, d f21. Declaration-order searches did not find it.
- shell08_rgba (944 bytes): near-match in shell08_nm.c, 48/236 differ
  (order of the channel extraction; permuter found nothing).
- shell08_trans (6320 bytes): not written. It spills many byte locals to
  16-byte stack slots; expect a long job.

## Matching lessons (each confirmed by a match)
- `if (a == 0 || a == 5) return;` gives the "beqz -> stub `b end`;
  bne -> body" shape (set09_i); a switch or `&&` form does not.
- `case 8: return;` keeps its own `b end` stub; `case 8: break;` gets
  merged away (set09_i00).
- MWCC b52 folds `0.0f * x`; the original kept a multiply by a zero
  register. A local `f32 spread = 0.0f;` reproduces it (set09_09_pos_reset).
- u16 fields compared with negative constants (`rot > -0x1000`, `rot < 0`)
  explain odd `slti at, x, -4095` / `bgez` on an lhu (set09_m02); the
  emitted slti immediate is the sign-extended constant.
- A float `a > b` gives `c.le a,b; bc1t skip`, `a < b` gives `c.lt; bc1f`;
  `if (10000.0f > d)` was needed for `c.le 10000,d` (shell08_m).
- `x = (c) ? a : b` vs if/else: if/else with an assignment in each branch
  stops the next statement's load being hoisted into the delay slots
  (set09_i07).
- A field accessed both signed and unsigned: declare it u16 and cast
  `(s16)` at the signed uses (set09 kind 5 `bank`).
- `for` with explicit `p++; continue;` at every continue (not `i++, p++`
  in the header) matches shell08_m's duplicated pointer increments.
- Set09_set_ex's arg is s16 (no andi), passed straight on.
- set09_i00 clears cnt[0] then loops `cnt[i + 1] = 0` for 11 entries (the
  compiler unrolls it by 5 and peels the rest).

## tutorial / tuto (0x63ACA0-0x63BB50)
- src/game/tuto/tutorial.c 0x63ACA0-0x63B020: all 7 functions match
  (Tutorial_prog .. soncho_no_oshie).
- src/game/tuto/tuto.c 0x63B020-0x63BA78 (SonchoInit .. tuto_msg_flag_set)
  and tutob.c 0x63BB20-0x63BB50 (tuto_read_flag_set) match.
- print_tuto_message (0x63BA80) is assembly; near-match in tuto_nm.c,
  8/40 off (the line counter and y position swap s1/s2).
- Shared header edit: include/game.h gets `u16 quest` at 0x2C (carved
  from padding). quest_w (0x3C7440) is a file-local type in tutorial.c.
- The main data object `soncho_no_oshie` (0x389C54) has the same name as the
  tutorial function, so the split calls it soncho_no_oshie_00389C54;
  tuto.c references it by that name.

## pl damage (0x639EB0-0x63AC9C)
- src/game/pl/pl_damage.c 0x639EB0-0x63A008 (mahi_dm_ck .. Guard_dir_ck)
  and pl_damageb.c 0x63A0F0-0x63AC9C (pl_guard_set, Pl_damage_sub, jump
  table 0x68A4F0-0x68A52C) match.
- pl_guard_ck (0x63A010) is assembly; near-match in pl_damage_nm.c, 1 off.
  It returns a u8 and Pl_damage_sub masks the result, which our compiler
  only does when the caller sees an int return. In C the
  function would have to be u8 and int at once, so it stays split out.
- Shared header edit: include/pl.h, fields carved from padding: kind 0x002,
  ang_y 0x00E, vital 0x302, dm_flag 0x38D, dm_ang/dm_pow/dm_type 0x3EC-0x3F0,
  x43E, x738, stamina 0x748 (guess), dm_vital 0x766, vital_red 0x790,
  x7AA..x7C6 ailment gauges (names are guesses). sizeof(PLW) still 0xA00.

## More matching lessons
- `x <= 0xE39` gives `slti at, x, 0xE3A`; `x < 0xE3A` puts the result in
  v0/v1 (Guard_dir_ck). `(u32)(f - 4) > 1` gives the `sltiu at` range test.
- A `return` in the middle of a switch or a nested block makes the compiler
  emit a `b epilogue` stub. The original jumps straight to the epilogue.
  To get that, Pl_damage_sub needed `break`/`goto vib`/`goto end`
  (shared tail labels) instead of returns.
- `a != X && a != Y ...` chains stay separate beq tests; `a == X || a == Y`
  chains with consecutive constants get folded into range checks.
- Fixed-size sprite structs: one field written with `sw` in one place and
  as halves with `sh` in another needs `*(s16 *)&spr.uv0` (DispTutorial).
- Statement order of plain stores matters even with no dependencies
  (DispTutorial's second sprite: w, y, h, col, x).

## Assignment 3: fl graphics library (5 Oct 2026)
- docs/formats/graphics.md: Meltw, link files, AMO chunk tree, MLCLAY,
  clay handles, shader params, VU1 programs, flSetRenderState map.
- tools/clay_dump.py (AFS -> .obj, em01 renders as Rathalos) and
  tools/vu_dis.py (VU1 microcode disassembler, output only to build/vu1).

## Assignment 4: textures, skeletons, motions, Wii MHG (5 Oct 2026)
- APX texture format + material->texture chain in docs/formats/graphics.md
  section 7; Wii MHG fpk notes in 7b.
- docs/formats/motion.md: AHI bones, skinning, *_tbl.bin banks, AAN curves.
- tools/clay_dump.py: PNG/.mtl/UV export, --motion/--frame posing.

## Assignment 5: hunters, stages, Xbox HUD art (5 Oct 2026)
- docs/formats/player.md + tools/pl_dump.py: six armour parts bound to the
  legs skeleton through ptmat_tbl (read from main.bin at run time); motion
  ids (bank = id%1000/100, slot = id%100; >=1000 own table).
- docs/formats/stage.md + tools/stage_dump.py: stage tables, attribute
  chunk -> render states, HITS collision. st04 = base camp (render).
- graphics.md 7a: cpit1xb/cpit2xb decoded (cpit2xb = real Xbox buttons).
- Wii FPK LZ: still not cracked (two short attempts).

## Assignment 6: native host for game C (5 Oct 2026)
Done (commit "PC port runtime: run decompiled set14 natively"):
- 32-bit build: tools/build_pc.sh uses gcc-multilib if `gcc -m32` links,
  else the build/sysroot32 from tools/setup_pc32.sh (relative paths: the
  checkout path has spaces). DECISIONS "Open" entry written.
- src/pc/rt/ (rt.h, rt_mem.c, rt_data.c, rt_game.c, rt_fl.c): see
  docs/pc.md "Port runtime". set14_nm.c compiles unchanged with -Iinclude
  and runs; st04 waterfalls scroll (shots build/show/rt_set14_1.0/1.5.png
  differ only in the waterfall/mist pixels).
- game.bin in AFS_DATA is stored raw (fmt_afs_read, not fmt_afs_load).
- Static asserts confirm PLW 0xA00, GAME_W 0x224, CLAY 0x8C under -m32.
Where I stopped / next ideas:
1. More game C on the runtime: other set*.c for stage 4 (set09 controller
   spawns nothing there), or eft/shell files with few dependencies.
2. Real stage set spawn list instead of calling set14_set() by hand.
3. clay_attr_set: map the 0xF0000 attribute chunk to gfx states.

## Assignment 7: grow the port runtime (5 Oct 2026)
Done (each step committed, details in docs/pc.md "Port runtime"):
- Render states: clay_attr_set/reset, SetTrnslMode/SetOpeMode/SetFilterMode
  ported to src/pc/rt/rt_fl.c; new gfx states 0x0D/0x5E/0x63/0x64. Blend
  factor codes decoded from flPS2SendRenderState_ALPHA. Host draws apply
  each part's attribute word too (st04_1 parts 0/1 are additive).
- stage_set_set matched: src/main/stage/stage_set.c, main
  0x15BBE0-0x15C210 + rodata 0x35B870-0x35B9A0, `tools/rebuild.sh` all OK.
  Signature is `void stage_set_set(int stage)` with `switch ((u8)stage)`:
  a u8 parameter gave a masked copy in a0 instead of passing s0 through.
  Agent E has f_stage.c (WIP, not linked) for the rest of f_stage; it must
  not define stage_set_set again.
- Runtime runs stage_set_set, set00 (light shafts), set13 (sun glare,
  with hit2/hit2c) and set14 on stage 4. Shots in build/show/rt_set00_*.png,
  rt_set13_sun.png, rt_spawn_set14.png.
- Shared code edited: src/main/set/set13c.c and set13_nm.c declare
  hit_cap_sphr_m with its real argument order (check.py still OK). No
  include/ headers touched.
- Viewer `--stage N` (files via main's per-stage tables). All decompiled
  set files now run (whole-file _nm for set05/set20); data tables are
  listed in src/pc/rt/tables.txt and generated by tools/gen_rt_tables.py.
  All 88 stage numbers start without errors (shots build/show/stages/).
  set09 butterflies seen on stage 0x10 (rt_set09_st16_close.png), set17
  plant tiles on stage 1 (rt_set17_st01.png).
Ideas next: the eft/shell spawns set code asks for (Eft14_set2,
Shell10_set, Shell22_set2, Eft17_set_ex, Eft13_set_pos); area-model
placement offset for stages like st05; hit_point_cyl; fade alpha scale
(PS2 0x80 = 1.0?).

## Assignment 8: effects/shells, stage drawing, player plan (5 Oct 2026)
Done (each step committed; details in docs/pc.md "Port runtime"):
- All decompiled eft*/shell* C builds into the PC port (list EFT= in
  tools/build_pc.sh) on a native effect/shell runtime (src/pc/rt/rt_eft.c).
  The spawn stubs are gone; Eft14_set2 camp fire on stage 0x15
  (build/show/A/st21_eft14_close_1.3.png), Shell10 barrels on 0x11,
  Eft17 dust via RT_SPAWN (spawn_sheet.png).
- Pointers inside imported tables are translated through the ELF's own
  R_MIPS_32 relocations (+ dlsym of host symbols): rt_data.c / rt_mem.c.
- Porting hazard fixed in game C: callers declaring Eft13_set_pos,
  Eft20_set2, Eft14_set3, eft14_set, Eft13_set_pos2 with a different
  argument order than the definition (eft04*, eft22*, eft23*, shell03,
  shell10, shell12, shell14, shell22*, em29). check.py unchanged
  (shell22c shell22_trans was already 1 off before), rebuild all OK.
- Fade colour (fl state 0x67) is 0xAARRGGBB: fire was blue before.
- Floor holes: the area model is drawn by trans_stage (main 0x15CD90),
  which places/spins/scrolls per-stage parts (st05 parts 2/3 are modelled
  at the origin). C written from an m2c draft + asm in
  src/main/stage/trans_stage.c (was trans_stage_nm.c) (not matched, not in c_files). All 88
  stages shot: build/show/A/stages/sheet0/1.png.
- Plan for player + monster with input: docs/pc.md "Plan".
Lessons:
- m2c drops a float argument it thinks unused (f13 when 0 or computed in a
  delay slot): check every flmat call's f12/f13/f14 in the asm.
- m2c needs jump-table targets labelled (.L%08X:) and the table renamed
  jtbl_* in a .rodata section of the temporary .s that draft.py feeds it.

## Assignment 9: motion system, pad, hunter on the runtime (5 Oct 2026)
### main f_frame (0x125340-0x1267BC), 18 functions
- 16 of 18 match: src/main/frame/f_frame.c 0x125340-0x125730
  (create_plcom_motion, create_pl_motion, create_em_motion), f_frameb.c
  0x1257D0-0x125F08 (aan_ctr_get, calc_ofs_velocity, calc_velocity,
  pl_velocity_sub, frame_init, frame_init_b) and f_framec.c
  0x1263F0-0x1267BC (frame_check, em_frame_check, frame_check2/3,
  em_frame_check2/3, move). check.py OK; `tools/rebuild.sh` all OK.
- Near-matches, whole file in f_frame_nm.c:
  - frame_move 12/311 off: only `end` (fp vs s7) and the address of
    sub_on[n] (s7 vs fp) swap registers, plus the f20/f21 restore order.
    Declaration-order search and a 15-minute permuter run found nothing.
  - aan_ofs_calc 35/40: the original keeps `aan` in v0 early (return value
    set before the second test); permuter found nothing usable.
- New header include/frame.h (FRW: the motion part of PLW/EMW, FRMT: one
  0x50-byte layer at +0x194, model work FRMDL). No shared header edited.
- Lessons:
  - frame_init: two identical `han += no % 100` branches in the original
    (the compiler kept both). Writing one as
    `han = (u32 *)((u8 *)han + (no % 100) * 4)` stops MWCC from merging
    them; that, `int` params (frame/blend/n: no masking in the callee) and
    indexing `w->mt[n].x` directly (the compiler strength-reduces n * 0x50)
    made it match.
  - Argument order of float-taking callees shows in which load comes first:
    flPlayMotionExSI is (f32 frame, node, u16 group): `lwc1 f12` before
    `lw a0`.
  - A u16 loop counter passed to a u16 parameter is passed unmasked; to
    an int parameter it is masked (andi) - frame_move's calls show which
    callees take u16.
  - create_*_motion: found by a scripted search over declaration order
    (random + local moves) and then the order of the init statements.
    create_em_motion only matched once its per-bank pointer and bank*100
    were written as `em_mot_han_ofs[no][bank]` / `bank * 100` (the
    compiler makes the induction variables itself, inside the loop
    guard) and `em` was an int passed to Em_max_parts_get(s16) (the
    caller sign-extends).
  - pl_velocity_sub: stack order of FLMAT/vec locals follows declaration
    order (declare v[4] first to get it at the top).
### PC runtime
- rt_motion.c: fl motion layer (handles, motion players, blend, root
  velocity, Hermite 0x192E40, cpApplyMatrix); hunter posed by the game's
  frame_init/frame_move (build/show/A/motion_c*.png).
- rt_pad.c + src/pc/pad/: Psw like ioRead_sub, then swset()/pl_sw_set as
  game C (pad_get.c, pl_normal2.c built for the PC).
- rt_player.c: host stand-in for pl_normal (turn/run/idle); viewer --play,
  --input SCRIPT, --sw-trace (build/show/A/play_run.png).
- Porting hazard to fix when em code is built for the PC: em04*/em19b/em18b
  declare em_frame_check as (EMW *, f32, int); the definition is
  (FRW *, int n, f32 frame). Fine on the PS2 (separate register files),
  wrong on x86.
- Rathian also animated by create_em_motion/frame_move (em_work[0]);
  Em_max_parts_get ported natively (it takes an int and casts to s16
  itself). Viewer fix: hunter.game was uninitialised.
- trans_stage: not edited in this assignment (coordinator note: agent E
  consolidates src/main/stage/trans_stage_nm.c and f_stage_nm.c).

## Assignment 10: game collision on the PC, em_frame_check (5 Oct 2026)
- Stage collision C (agent D's src/main/hit/shit*) runs in the PC port:
  ground, walls (HitWallPlayer -> GetWallHitBitPl/Em -> sphr_face_o3/o4 ->
  PushAdjust3), floor slide. Details and checks in docs/pc.md "Collision".
  rt_hit.c holds load_file_mdl / rt_load_stage_hit and helpers written
  from the asm (NormalClipF3/CheckF3, PointHitCheckF3, UnitNormalVectorCCW,
  NvecFloatAdjust, cpRotMatrixYXZ2, flConvertRtoS, Stage_data_get).
- Hunter: pl_move_sub's collision order in rt_player.c. Rathian:
  em_move's collision tail; em_work[0] placed on the stage and drawn
  where the game moves it.
- Bugs found: host PointToPoint had the operands swapped (it is a - b);
  data-table pointers into PS2 .bss were NULL (now zeroed host memory).
- em_frame_check: em03.c, em33.c, em04*.c, em04_nm.c, include/em04.h now
  declare/call (EMW *, int n, f32 frame) like the definition. check.py
  output unchanged for each file, tools/rebuild.sh all OK.
- Camera: the game's CameraMove + camera slots run on the PC in --play
  (docs/pc.md "Camera"). New src/main/cam/camarea_nm.c (main
  0x222E20-0x223B50, 13 functions, written from the asm, not built for the
  PS2): check.py OK for CameraAreaCheck, GetPanTarget, GetRailTarget,
  nlCalcPoint; GetRailCamPos 1/33 off, default_area_data 7/117,
  SetAreaData 12/71, get_near_point_sub 18/66, GetNearSection 28/67;
  StageCamInit, Get_cam_grid_XZ, CamAreaAttribChk, Area_XZ_Check,
  GetNearPoint mostly off (not worked on). cam_nm.c (not built for the
  PS2) now calls cpInterVector / CamRailPoint in their real argument order.
- Lesson (x86): float-returning callees declared void leak x87 stack
  slots; prototypes must match the definition's return type too.

## Assignment 11: audio platform layer (5 Oct 2026)
Done (each step committed; details docs/formats/audio.md, docs/pc.md "Sound"):
- Formats: AFS01 "MOMO" packs = SCEI HD + BD (PS2 ADPCM) + Capcom Tseq +
  TSBD (SE code -> program, note, volume, pan, randomness, chain); AFS00
  = CRI ADX (48 kHz stereo, loops in the header). BGM id = AFS00 index;
  load_bin_req 0x10000|n = AFS01 entry n.
- Key finding: the third argument of se_req/se_req2 is a program offset
  (Em_se_req2 passes Snd_em_id_conv_tbl[kind] = the snd_emNN program;
  footsteps pass the ground material pl+0x70D = map program 1..7).
- tools/snd_dump.py: --list, --pack N [--vags], --adx N, --adx-list
  (wav to build/audio/).
- src/pc/audio/ (audio.h, audio_mix.c, audio_sdl.c), src/pc/fmt/snd.c,
  src/pc/rt/rt_snd.c (se_req*, flSndRequest/Change from the asm; str_*;
  stage packs + stream; stage_se_move; player run and Rathian walk
  footsteps). Viewer: --audio-dump, --mute.
- Removed stubs: se_req2 (rt_game.c), Em_se_req2 (rt_main.c),
  Pl_se_req2 (rt_eft.c). No include/ headers or PS2-built C touched (the
  PS2 rebuild is unaffected).
- Checked by numbers only (cannot listen): C stream == Python decode,
  footstep timing, loops start/persist, SDL device consumes samples.
Next ideas: the player's ef_move_sub lists for other motions (needs
actions), joint positions for em sounds, quest BGM switching (fight /
clear), TSNDDRV.IRX disassembly to replace the [guess] parts (slot,
priority, SdrSeChg semantics, pitch bend).

## Assignment 12: the game's player code on the PC (6 Oct 2026)
Done (each step committed; details in docs/pc.md "Player"):
- All of agent F's src/main/pl (plNN.c + pl_nm.c + pl_normal_nm.c), the
  game.bin damage files, hit_nm.c, weapon_nm.c (weapon_joint_calc) run on
  the PC: rt_player_tick = rt_pad_tick + pl_move (pl48.c). Walk/run (0/1,
  0/3), roll (cross, 0/0x1C), draw (right stick, 0/4), attacks and combos
  (right stick, 1/0x30 -> 1/0x37 for sword and shield), guard (R1, 2/3),
  sheathe (circle) all come from the decompiled state machines.
- src/pc/rt/rt_pl.c: the helpers that are not decompiled yet, written from
  the asm (Pl_act_set, pl_flag_*, pl_chr_set*/pl_chr_sub, stamina, vital,
  sharpness, rates, front_land_ck*, World_calc, item counts, parts_init,
  attack-data helpers, Code_Make, Pl_poison_add ...) plus stubs (network,
  items/gathering, quest, messages, parts_chg). Request for agent F: these
  are main 0x14D1D0-0x155000 (g_act_set, g_pl_voice_req, Pl_bari_ck,
  World_calc, f_pl's 0x1510xx tail, g_Pl_hold_item_ck); rt_pl.c can be
  the starting point.
- src/main/sound/f_sound_nm.c: pl01_effect_move / ef_move_sub (per-motion
  sounds and dust), from an m2c draft with call arguments cleaned.
- Weapon: w<job>_tbl.bin motions via create_pl_motion; the weapon model
  (weapon_model_data[PLW+0x34C]) placed like weapon_trans.
- x86 porting hazards fixed without touching matched files: rt_abi.c +
  per-file -D renames in build_pc.sh (frame_check*, Eft06_set, Eft02_set6
  declared with the float first in plf.h; hit_point_cbd in f_stage.c;
  pl_move_sub's 4-argument GetGroundHitStatusAreaPl call). Found with an
  -flto build (-Wlto-type-mismatch); script in docs/pc.md.
- Get_Active_itemnum relies on a0 = pl left by its caller (pl10.c calls it
  without arguments): the host version uses the master player.
Shared header edit: include/plf.h pl_mv060/pl_at009/pl_at012 now K&R.
- Hits: hit_check (hit_nm.c) now runs each tick after sync_joints (host
  skeletons -> joint matrices for parts / get_joint_pos / hit_data_expand).
  The SnS sword shell (shell00) hits the Rathian: damage, hit stop, hit
  sounds, eft16 marks, eft05 slash trail (skinned ef_01 via
  flSetSkinTrans). She does not react: needs enemy_mv / em_move (main,
  not decompiled) and em01's damage states (agent B) on the PC.
- Requests: agent F - main 0x14D1D0-0x155000 helpers (see rt_pl.c list),
  parts_chg, Get_equip_value, Get_atk_value (stub returns 0: no element /
  ailment on hits). Agent B/C - enemy_mv, em_move, em01 damage handling so
  the Rathian can react. Anyone: f_sound (src/main/sound/f_sound_nm.c) is
  a fresh m2c-based file, nothing compared with check.py yet.
- Not done: items (square), gathering, carving, the quest's own monster
  set-up (Rathian HP is a stand-in), pad vibration, parts_chg (hand model
  swaps), weapon_dat_make node scaling (great sword / lance / hammer /
  bowgun blade extension during some motions).

## Assignment 13: the Rathian as a real opponent on the PC (6 Oct 2026)
Done (each step committed; details in docs/pc.md "Monster"):
- The PC runs enemy_mv -> em_move -> em01_main / em_cmd (agent B's
  em01_ai_nm.c from main, agent D's em_cmd_nm.c exported from branch
  agent-D at build time) -> frame_move -> collision, every tick.
- `--quest N`: mission file -> quest_w tables -> the quest's own monster
  spawned like Em_direct_set (quest 10 = Rathian nest, stage 40; HP 2500
  from em01_init).
- Verified with scripted runs: she notices the hunter (roar, mode 1),
  walks/turns, charges and bites, hits him (100 -> 51, knock-down), takes
  his sword damage (3 a slash) and flinches (4/2) when a part's durability
  runs out (tested with RT_DMG_MUL=40).
- eft16: the C matches the asm (check.py OK but eft16_m's spill order); the
  screen-wide streak was the host make_mat_srt with X/Z flags swapped.
- x86 hazards found and fixed (list in docs/pc.md "Monster"); LTO script
  for arg-count and float-order mismatches in docs/pc.md "Player".
- Also: Get_atk_value ported (element/ailment); the leftover rt_pl.c
  stand-ins replaced by agent F's pl49..pl83 (pre-outage work committed).
- No shared include/ headers edited. f_em_nm.c (not in c_files) got the
  GetGroundHitStatusAreaEm 5th argument and Em_Master_Change(em).
Lessons:
- On x86, "a0 left over" calls (callee declared without the argument the
  asm passes in a0) read garbage: Em_Master_Change, NextStage_No_Set,
  GetWaterData. Fixed with per-file -D macros in build_pc.sh.
- gcc's loop optimiser trusts array bounds: decompiled loops that index
  one past a struct array need -fno-aggressive-loop-optimizations.
Next ideas: carving and quest clear (f_quest_nm.c on the PC), the other
monsters of a quest (per-stage QEM lists), stage changes (area exits),
Quest_restart, a scripted "bot" for longer fight tests.

## Assignment 14: a complete quest loop on the PC (6 Oct 2026)
Done (each step committed; details and verification in docs/pc.md "Quest
loop"; shots in build/show/A/quest/):
- `--quest N` runs the game's own quest start (Quest_init, Quest_start,
  station_em_set / Quest_next_em_set -> Em_direct_set) and game modes:
  rt_flow.c runs game2/game3/game5 (f_game.c, matched) each tick with
  game_core = the viewer's host tick. Quest 10: kill -> Quest_enemy_die ->
  Quest_condition_judging (clear, 60 s carve time) -> game3 -> game5
  result_prog -> reward screen -> money screen -> mode 6 (host restarts
  the quest: the village is not ported).
- Carving: the hunter's own carve action (0/0x4A) -> Ext_pick_point_ck2 ->
  ItemStockRequest gives Rathian Scale, Spike, Flame Sac; reward items are
  taken into the pouch with the pad; potion use works (10 -> 9).
- Hunter faints: death 3/0 -> game2 steps 2-6 -> st_model_load = the
  viewer's load_stage_models (stage, set, collision, camera, sound) ->
  pl_init(1): carted back to base camp (stage 21). The cart is em18
  (em18_init.c / em18b.c built); its model is not drawn.
- HUD: menu_nm.c / menu_disp_nm.c / chat_nm.c / set01.c (+ f_reward*,
  ud_nm, disp1/2_nm) run on the PC; rt_2d.c = fl screen prims + APX
  textures (load_pit), rt_font.c = fl font system + f_font prints.
- New C written from the asm (not built for the PS2, not checked):
  src/main/quest/f_quest0_nm.c (main 0x2267F0-0x226C24).
- Fixes in not-built files: f_quest_nm.c Item_regained takes the unused
  2nd argument; chat_nm.c DispFrameListA steps its string list by one
  pointer (was 4: asm addiu 4) and a block-scope redeclaration gcc
  rejects was dropped. No include/ headers edited. tools/rebuild.sh all OK.
- x86 "a0 left over" calls fixed at build time (sed copies in
  build_pc.sh): menu_nm.c ItemPickingDeclaration -> Pl_master_ck(arg),
  pl10.c item_action_set -> Get_Active_itemnum(pl).
- Lesson: objcopy --weaken also weakens a file's undefined references:
  a weakened whole-file _nm.c that calls a missing function links and
  jumps to 0 (gfs_nm's stage_free). Host replacements of functions such a
  file also defines must be strong. Check with `nm mhview | grep " w "`.
- Lesson: the game caches the texture stage (SetTextureStage); host draws
  that bind other textures must be followed by InitRenderState(1) (trans()
  ends with it) or the cache goes stale.
Requests (not decompiled; written natively for the PC from the asm, can
be the starting point): main 0x274E10-0x2755C0 (load_pit, UseItemChk,
Item_valid_chk, Item_ok_chk, Pit_shot_ok_chk), the fl font system
0x216490-0x217760 and f_font 0x161970-0x162640 (rt_font.c), flps prims
0x175290-0x176470 (rt_2d.c), Get_hunter_rank 0x272320, Gold_add 0x2722C0,
Quest_price_return 0x290E50 (rt_quest.c).
Not done / next: SpritePut (0x15A6D0) + sprite prims flps0D00/0F00/1300/
1400/1600 (game3's darkening quad, Put_sprite_rotate effects), models for
em18 (cart) and other small monsters, pause-menu list selection
(ListSelect/PageSelect/Menu_select_mv), item combining
(Item_preparation*), map markers (flvecrRotTransPers), quest failure after
three faints (untested), the village.

## Assignment 15: village, pause menu, small monsters, quest failure (6 Oct 2026)
Done (committed step by step; scripted --input runs, traces and --shot
screenshots in build/show/A/; details in docs/pc.md "Village"):
1. Village: after the money screen (game mode 6) the PC runs the lobby
   overlay's own offline loop: Clear_lobby_ram + Local_main each tick
   (lobby.bin's step table vs_square_*), Kokoto (stage 87) and the house
   (86) with the intro and the wake-up. The hunter walks with the game's
   player code (village lbcom motions), NPCs walk and talk (lbnpc_nm.c),
   the Village Elder is the quest counter: gift talk, level list, quest
   list, accept (quest 131 checked: cw+0x35D3 set), leave through the gate
   (square at the spot kind 6) -> Local_main returns 1 -> the quest starts
   (stage 21 base camp). Script: build/lbvil/accept_script.txt.
2. Pause menu in the field: start -> pages 1/2, item list with cursor,
   discard with confirm (pouch count drops), quest info, combine/data
   screens, retire -> game3 -> game5 -> village.
3. Small monsters: Velociprey (em16) AI built (em16_nm.c / em16.c); every
   monster slot is ticked and drawn with its own model/motions per kind
   (quest 10, stage 40: four Velocipreys notice, run, jump, attack,
   v_6.png). The cart (em18) ticks too.
4. Quest failure: three faints (RT_PL_DIE test aid = the game's
   Pl_die_set) -> carted twice -> third: D5 5 -> game3 -> game5 "quest
   failed" score screen (0z) -> circle -> village (house wake-up).
Native PC replacements (platform parts, not game logic):
- rt_village.c: rt_village_enter/tick (what Game_task mode 6 and
  move()/trans() do around Local_main), Disp_NowLoading no-op,
  com_motion_load / em_motion_load / npc_create_model (file loading for
  the host's model/motion loaders), clr_set_work on the host set pool.
- rt_lb_mem: lobby.bin data+bss as one host block (vram shared with
  game.bin); tools/gen_rt_auto.py makes weak stand-ins for missing
  functions and aliases lobby data into it; tools/pc_abs.py maps absolute
  addresses in C to host symbols; tools/pc_patch.py fixes x86 argument
  passing in matched files at build time (list in the file).
- Lbc_set_prim's stack words 0/1/2 for lobby prims [guess].
New C from the asm (not built for the PS2, not checked):
src/lobby/f/lb_village_nm.c (Local_main and the village/guild/NPC/target
functions listed in its header), src/main/chat/dispframe_nm.c
(DispFrameMessageA), src/main/menu/listsel_nm.c (ListSelect, PageSelect,
Menu_select_mv). Edits to near-match files: chat_nm.c (disp_cursorC
arguments, DispFrameListA tile height), menu_disp_nm.c / menu_nm.c
(stack text buffers 64 bytes: sprintf overran them), em01_ai_nm.c
(Em_Next_Stage_Pos / Quest_enemy_capture get em, em_char_set2 gets the
part number; checked in the asm: a0/t0 left over). No include/ headers
edited.
New tools: tools/argregs.py (which argument registers each function
reads, --check FILE.c lists calls with too few arguments),
tools/gen_rt_auto.py, tools/pc_abs.py, tools/pc_patch.py.
Not done: SpritePut + sprite prims (0x15A6D0, ~0x1A00 bytes of asm with
flps0D00..1600: builds a sprite list via CalcPoint; not cheap, game3's
darkening quad still missing), the cart's model, other small monster
kinds' tables (only em16's game.bin tables are imported:
src/pc/rt/tables.txt), set01 field messages on faint not checked on
screen, nobody compared any of it with the PS2.

## Assignment 16: windowed crash, determinism, village menu, sprites (7 Oct 2026)
Done (each step committed; details in docs/pc.md "Windowed = headless"):
- Windowed segfault ~63 s into the accept script: a drawn frame between
  the village's last tick and the quest start drew eft13 prims of effects
  rt_eft_init had just cleared (eft13_t, ew->work NULL). rt_game_init now
  empties the prim queues. The "hunter not at the gate" report was the
  run being mid-walk when it crashed: the traces match tick for tick.
- Windowed and --shot runs now run the same per-tick host work (joint
  sync after every tick); RT_TICK_TRACE=1 prints a per-tick line to diff.
- Free roam fixes: village start menu (was a NULL lbmw crash), eft06_m
  null-table test, unported monster programs not spawned, area exits.
- Cart (em18) already drawn; SpritePut + flps0D00 for game3's fade.
- Shared files: no include/ edits. Near-match files edited: eft06_nm.c
  (tt0), DispLobbyMenu.c (took main's version at merge). New: 
  src/lobby/b/lb_menu_nm.c, src/main/sprite/spriteput_nm.c (both from the
  asm, not linked for the PS2). build_pc.sh: main's hit2 restructure
  (hit2.c/hit2b.c/hit2c.c gone, hit2all.c has raw asm functions) -> the PC
  takes the f_hit_28CE00 tests from hit2_nm.c alone (no longer weak).
- Lesson: an m2c pointer that is stepped in a loop and also tested for
  NULL needs two variables when the asm keeps the base in another
  register (eft06_m: s8 base, s6 cursor); on the PS2 the bad read is
  harmless (address 2), on the PC it segfaults.
- Lesson: headless --time runs do every tick before the first frame, so
  host work done per drawn frame (joint sync) made windowed runs
  diverge; anything that writes game state must run per tick.

## Assignment 17: quest start at camp, supply box, playability pass (7 Oct 2026)
Done (each step committed; details in docs/pc.md "Quest start, supply box,
playability pass"; shots in build/show/A/r17/):
- `--quest N` starts at the quest's start stage (base camp) as on the PS2;
  `RT_QUEST_STAGE=1` keeps the old monster-stage start for scripted tests.
- Supply box filled (Start_item_init after Quest_start) and usable: box
  screen, items into the pouch, item bar. A new character's pouch is empty
  in the game too (select.bin user_data_copy), so nothing was invented.
- Monster sound packs follow the model slots (Velociprey sounds).
- Guard knock-back crash/blank screen: pl_dm001 reads an unset local;
  game C now built with -ftrivial-auto-var-init=zero.
- Test aids RT_SPOT_TRACE, RT_PL_WARP; em trace shows motion state.
No include/ headers edited; no PS2-built files edited.
Lesson: matching C that reads a never-written local is fine on the PS2
(stale stack slot, usually small) and garbage on x86; gcc's
maybe-uninitialized warning misses arrays passed by pointer. Zero-init
for all game C was the cheap global fix (run time unchanged on x86).
Not checked / for the owner to judge on the real thing:
- ARM speed with the zero-init flag (expected small; not measured).
- Roar: in scripted fights the Rathian charged (atk 18) and shot fireballs
  (atk 4, 23) but no roar / ear-cover reaction was seen; whether her
  command program should roar on first sight was not compared with the PS2.
- Damage numbers (a charge took a fresh hunter from 88 to 25; guarded
  hits chip ~7) look plausible for no armour but were not compared.
- Supply box reach: the spot (r 200) is only reachable from one corner of
  the crates; if the PS2 lets you open it from the front, the wall data or
  the hunter's push radius (push00: 48) differ.
- The camera can sit inside camp bushes (no collision with plants; the PS2
  likely does the same).
- Hunter armour: the host always draws armour set 1 with head part 0
  (viewer.c parts[]), whatever the game's equipment says; a new character
  in the game wears no armour. Not changed (how the PS2 draws "no armour"
  was not checked).

## Assignment 18: power-on to the village, saves, village features (8 Oct 2026)
Details in docs/pc.md "Power-on, new game / continue, memory card, village
features". All PC-side (src/pc, tools/build_pc.sh, tools/pc_patch.py,
tools/gen_rt_auto.py); no include/ or PS2-built files edited.
- Boot runs the game's own tasks (select.bin + main omake/fade/tsk/option);
  their draws are recorded per tick and replayed per frame (gfx_rec.c).
- libmc on host files (rt_mc.c): the game's card C saves/loads unchanged.
- Hunter look from the save (Pl_model_id_set from the asm).
- Village: item box, item shop (buy list), forge/armour shop pieces linked;
  agent B's matched lobby round-7 functions (lb_by135-152) now win over the
  stand-ins (BMATCH in build_pc.sh weakens other copies).
- Lessons: `objcopy --weaken` also weakens undefined references (a weak
  undefined data table is NULL, no stand-in gets generated): weaken only
  defined symbols. Main data tables that point into lobby.bin
  (shop_default_tag_00389E90, pit_help_str_tbl) need mapping by hand, since
  game.bin shares the vram.
Not done / not checked: the character screen's 3D preview (editpl models
are not drawn), hair colour, the soft keyboard (typing stand-in), opening
movie, options screen not tried, shop purchase with money, forge/armour
screens not opened, the stale talk window stays under the shop list, save
after a quest (op 9) not exercised, ARM speed of the boot not measured.

## Assignment 19: the first quest loop, shops, hair colour, matched lobby code (6 Oct 2026)
Details in docs/pc.md "First quest loop, shops, matched lobby code". PC-side
files plus PC-only fixes in not-built near-match files (f_reward_nm.c,
lbshop2_nm.c, lb_mix_nm.c, b/nm/lb_process_drawHelp.c, lb_village_nm.c);
no include/ headers edited, no PS2-built file changed.
- Quest 131 (the Elder's first quest, deliver 2 raw meat) playable end to end:
  Aptonoth (em12_nm.c) and em29 run on the PC, carve, delivery box, reward
  screen, money, back to the village, bed save, CONTINUE keeps money, pouch
  and the quest's CLEAR!! mark. tools/test_quest_loop.sh checks it (~5 s).
- Fixes: reward screen menu (ListSelect count in a2), lobby.bin reloaded per
  village entry (hunter walked on the spot after a quest), shop owned counts,
  forge crash (16-bit shop fields read as s32), hint_tbl mapped (spot hints).
- 63 matched lobby files now linked instead of near-match copies / stand-ins
  (tools/pc_lobby_matched.txt, LOBBY3, PICK in build_pc.sh).
- Hair colour: PLW+0x5FC tints the head part's first material (player_trans
  0x167C38), as the PS2 does; not compared with the PS2 picture.
- Lessons: a matched C file can still carry an x86 trap that the near-match
  copy did not (a0 left over: lb_by71 Lb_npc_mv; float argument order:
  lb_check_target(f32, ...) vs the near-match's (..., f32)). Run
  tools/argregs.py --check on every file newly linked for the PC, and check
  that callers in other PC files declare float arguments in the same order.
  Overlay data must be reloaded when the PS2 reloads the overlay
  (Load_overlay = load_bin + mwOverlayInit). An m2c draft reading a 16-bit
  field as s32 at an odd half-word offset reads its neighbour too
  (lb_process_drawHelp +0x24/+0x26).
Not done: opening movie (Sofdec MPEG: needs a video decoder, left skipped),
character-screen 3D preview, forge list glitches (weapon-class icons and the
yellow page title are garbage), the item shop's buy list still shows the
greeting talk window on its left (sell list does not; whether the PS2 keeps
it is unknown), colour streaks over a quest card marked CLEAR!!.

## Assignment 20: village glitches, character-screen hunter, monster breadth (6 Oct 2026)
Details in docs/pc.md "Village glitches, character screen hunter, monster
breadth". PC-side files, PC-only fixes in not-built drafts (em04_nm,
em08_ai_nm, em15_nm, em20_ai_nm, em21_nm, nm/lb_process_drawHelp) and in
#else / #ifndef __MWERKS__ branches (lb_by165, lb_by131); no include/ edits;
rebuild all five OK.
- Forge title/icons/contents, item icons fixed; 32 more matched lobby files
  linked; main->lobby pointers from the ELF's .relmain symbols.
- 3D hunter on character creation and continue screens.
- Kut-Ku, Basarios, Gravios, dromes, Cephadrome, Diablos/Monoblos, Khezu,
  Plesioth, Gypceros and all village small monsters run; event demos linked.
Lessons: see docs/pc.md (data stand-ins read as code, address-suffixed
statics in pointer tables, per-kind file tables, shared joint buffer).
A sed that adds an argument to calls also hits K&R declarations
(`void f(em);`): MWCC rejects it — the PS2 rebuild compiles *_nm.c files too.
Not done: Cephadrome/Plesioth hit while submerged (sound bomb not tried),
drome carving not seen, the demo camera during intro demos not checked,
em02/em07/em10/em33, ARM frame rate.

## Assignment 21: last monsters, bombs, intro demos, gathering, fishing (6 Oct 2026)
Details in docs/pc.md "Last monsters, items, intro demos, gathering and
fishing". PC side only; no include/ edits; no PS2-built file changed.
- Fatalis, Lao-Shan Lung, the em10 trader and Kirin run (all kinds linked).
- Flash bombs (push_senko/move_senko/move_smoke), sound bomb on Cephadrome,
  intro demo camera, carving Velocidrome and Fatalis, gathering (herbs,
  mining, bugs) and fishing checked.
Lessons: grep rt_quest.c / rt_main.c for no-op stand-ins of real game
functions (func_XXXXXX wrappers, WEAK move_*): three gameplay features
(fish, flashes, Lao-Shan quest condition) were missing only because of
them. Shots with RT_SHOTS land on drawn frames, which can lag ticks by one.
Not done: Plesioth with a sound bomb, Lao-Shan kill, em10 trading, forge
greeting linger (not reproduced on x86).

## Assignment 22: progression, trader, open items, music, movie estimate (6 Oct 2026)
Details in docs/pc.md "Progression, trader, demo camera, Lao-Shan kill, music".
PC side only; no include/ edits, no PS2-built file changed.
- Star levels 1 -> 2 -> 3 walked with RT_QCLEAR + bed save + CONTINUE
  (tools/test_progression.sh); urgent quests 136/137 offered at the right time.
- Trader talk and one trade checked. Intro-demo grey ground fixed (EMW/PLW+0x60
  world matrices). Flash bomb facing checked. Lao-Shan killed on stage 12
  (Eft10_set ABI adaptor; RT_PL_GOTO aid). Plesioth reacts to a sound bomb
  once surfaced. Fight / clear / death music via the game's bgm_server.
- Movie: trade-off written in docs/DECISIONS.md (open).
Lesson: any matrix the PS2 builds in trans() (enemy_mk, player_modify) is
read by move-side code too (demo cameras, em10). When the host replaces the
draw path, rebuild those fields per tick.

## Assignment 23: urgent quests for real, Plesioth, reverb (7 Oct 2026)
Details in docs/pc.md (handover summary at the top, round 23 at the end).
- tools/test_urgent.sh: real clears of 136 and 137 open 2 and 3 stars.
- Plesioth swims, attacks from water and ashore, can be hit ashore; frog-bait
  check (Kaeru_ck) and its camera were stand-ins, now linked.
- Reverb added (approximation). No include/ edits, no PS2-built file changed.

## Xbox round (7 Oct 2026, continued)

- Memory report (RT_MEM): numbers for title / village / Rathian against 64 MB
  are in docs/xbox.md (commit 4990f881).
- Xbox link works: `python3 tools/build_xbox.py [--gfx nv2a]` after
  tools/build_pc.sh -> build/xbox/default.xbe (3.13 MB, null graphics) and
  build/xbox/nv2a/default.xbe (3.19 MB, pbkit), each with an ISO (3.7 MB).
  Scheme in docs/xbox.md "Linking for the Xbox": #pragma weak headers,
  tools/coff_weak.py (GNU weak rules for lld-link), per-object tag symbol,
  generated symbol table, prefixed asm labels.
- Checked: PC build + test_quest_loop / test_progression / test_urgent pass
  with the shared link scheme; tools/rebuild.sh all OK. Not run on xemu or
  hardware (no files yet). gfx_nv2a.c is a first version, never displayed.
- Lesson: lld-link 18 errors on two COFF weak definitions of one symbol seen
  before a strong one; and clang's `.weak.X.default.Y` names collide when Y is
  a shared `__real@` constant. Both handled in build_xbox.py.

## Round 3 (7 Oct 2026): memory trims, nv2a gaps, stack, build fixes

- Memory: CPU heap title / village / Rathian 57.7 / 69.9 / 76.9 MB ->
  18.4 / 23.6 / 18.4 MB; Xbox textures 3-7 MB as P8. Details and the ~51 of
  64 MB estimate in docs/xbox.md. Checks: PC tests pass, five screenshots
  byte-identical, 60 s audio dump byte-identical.
- gfx_nv2a.c: P8 textures, near-plane clipping, fog (untested on xemu).
- Stack: RT_STACK=1 watermark, 35 KB used; XBE stack 256 KB.
- build_pc.sh flake ("no compile command for build/pc/rt_gen.o"): the first
  pc_link_adapt pass included the previous build's rt_gen.o; when its
  header changed and cmd/rt_gen.sh did not exist it failed (reproduced).
  rt_gen.o now joins objs.txt only after it is generated. build_xbox.py
  silently skipped objects without a recorded command (rt_gen -> the 20
  undefined symbols); it now stops with a message.
- Lesson: never `pgrep -f` a pattern in a wait loop: the loop's own command
  line matches it (my wait loop never ended; killed by PID).

## Round 4 (7 Oct 2026): CPU profile, projection, GPU skinning

- RT_PROF per subsystem (rt_prof.c, thread CPU time), RT_STEP, tools/prof_scenes.sh.
- Projection at an assumed 20x slowdown and the reasoning: docs/xbox.md "CPU budget".
- GPU skinning for nv2a (skin.vs.cg + gfx_skin.c), checked by RT_SKIN_CHECK against the CPU path
  (39 M vertices); PC keeps CPU skinning.
- Village skinned every villager body variant (16 -> 4 ms on the PC); mixer reverb hit denormals
  (lesson: any float feedback loop - reverb, filters - needs an anti-denormal offset on x87/P3).
- Lesson: contention from other agents makes wall-clock profiles useless here; measure thread
  CPU time (CLOCK_THREAD_CPUTIME_ID).

## Round 5 (7 Oct 2026)

- Village sound: Snd_server now per host tick in every mode (as ACRMain); --quest runs set the
  option defaults (volumes were 0 since the options-sound merge).
- RT_PROF sub-zones (motion, joints, stage draw, prims, 2D) + skeleton-evaluation count; pose
  cache in fl_skel_pose_groups (19 -> 10 evaluations per frame), checked identical with RT_POSE_ALL.
- Xbox: static clays in GPU buffers, sphere near-plane test, fade as a shader constant; libmpeg2 MMX
  (vendored from the same tree agent B used, /tmp/claude-1000/mpg), movie YUY2 on the GPU; RT_PROF
  report to DbgPrint + E:\mh1_prof.txt.
- Lesson: wall clock and even thread CPU time are inflated on this shared machine (load ~7.5/8);
  compare counts (evaluations, vertices) and A/B runs taken back to back.

## mh1-server: design and first code (9 Oct 2026)

- docs/server.md: the design (services from the client code, architecture, security, hosting, privacy, PS2 patch
  path per the owner's 9 Oct decision, milestones, owner decisions). Code in tools/server/ (Python stdlib):
  relay.py (net_peer-frame session relay, works with today's `--join` unchanged), accounts.py (SQLite), lobby_stub.py
  (subclasses tools/mh1_testserver.Client: account check at 6101, optional 6914/6916 relay hand-off), mcs.py (mcsls
  relay skeleton, synthetic tests only), mh1_server.py (CLI), test_server.py (unit tests).
- Verified: tools/test_coop.sh relay (2 and 4 walking, hunt2, hunt4, leave through the relay); the stub against the
  real client's --nettest login (right / wrong password).
- Found: rt_np_init_slots sets the first monsters' owner field +0x88E to this machine's slot instead of the host's,
  so followers never notice the host leaving (hostleave scenario fails; docs/server.md 11). Agent B's file, not changed.
- Lesson: the lobby key is mmbbc_encode(8 digits, seq of the 6101 request); non-digits are skipped, so an account id
  on the wire is 8 digits.

## G round 2: the G quest path on the PC (14 Oct 2026)

- Goal: a G quest from `--quest N` (tools/build_pc_mhg.sh). Reached: quest 1 loads, base camp + hunter +
  HUD (build/A/g/q1.png). docs/mhg.md section 5 "Quest path" lists what was done and what is open.
- Approach: G's versions as `#ifdef VERSION_MHG` blocks in the shared files, written against the header
  field names (so they follow C's G struct layouts when those land). Not byte-checked: G's Quest_start
  writes game_w fields G moved (x0D0 -> 0xF0, x1E7 -> 0x18B, x0D5 -> 0x3E) that only C's G GAME_W has.
- Found: G's quest_w keeps MH1's layout up to 0x150, then adds its own (random bytes 0x155, seed
  0x16A, two s16[52] arrays) and moves MH1's 0x180 block to 0x280. G mission header bytes 0x40 (base
  size %), 0x42 (fish set), 0x43 (start-area set), 0x44 (size class), 0x45-0x47.
- Found: G quests use a per-quest generator (seed from 15 random bytes; online the host sends them), so
  start areas, monster sizes and the reward bonus agree on every machine.
- Found: G's game_w.x2E is the map (11 values): em_cmd tables, and the mission's stage tables are per
  map (sub_main 0x4A1970 maps a stage to the map's index).
- Lesson: the G pointer scan rewrote words inside em byte code (0x00280100 looked like a pointer into
  main's data): pointers into another module now must hit a label start; the G split's own `.word
  SYMBOL` words are taken as pointers. A hang in next_cmd_search/cmd_end_search on the PC = a corrupt or
  unknown em command.
- Tool kept in scratch (not committed): an aligned lui/lo resolver for MH1 table names over edited G
  twins (votes in the commit message of pc_data entries "by vote").

## G round 3: monster state, reward list, armour, stage timers (14 Oct 2026)

- EMW (include/em.h): the PC G build now keeps MH1's EMW layout everywhere (the G -8 shift of the tail and
  EM_FIELD's shift are MWCC-only): the PC's C mixes named fields and raw MH1 offsets, so the shift had split
  them. G's new fields: size (G 0x9F8, %), meat_no (G 0x9FB, hit-zone table), lv (G 0x9FC, level); on the PC
  at 0xA02/0xA04/0xA05; base_scale at 0x600.
- Writers ported (G asm, under VERSION_MHG): em_init lv = quest_w.x14E; em_status_init meat_no = 0, base
  scale + em_size_scale_set (sub_main 0x4E92D0); Em_direct_set size from QEM+0x30; Quest_enemy_revival_set
  (+5% size up to em_size_max_tbl[species], level +1 to 14); Quest_enemy_die (kill-count message 0x4B55F0,
  "N of kind" counter, per-kind kill counts and biggest size); em_hagitori_lv_up (G table 14 x 0x14, species
  compare, state byte -> meat_no 1: Gravios belly); em02_main kind 0x24 meat_no 1 between 20-50% hp.
  rt_pull_enemy_work sets size 100 (G's spawners all set it).
- Subspecies: em_cmd_flag_set/clear/ck kind 7|0x32, 0xF|0x2D; Em_Dmg_Sys kind 2|0x24, 7|0x32.
- Reward: remuneration_item_set cleared MH1's game_w+0x128 (G list is at 0x100): supply-box items stayed in
  the reward list; now cleared through GWX. trans_box (menu_disp_nm.c) read the list at MH1 offsets: GWX.
- Armour: G records are 0x20 (pc_data: all five tables at G sizes); ud_nm.c / rt_pl.c strides (ARMOR_SZ,
  GUN_SZ). Stay/run-away timers: G keeps {stage, value} lists ending {-1, default} (game 0x54A1D0/0x54A230);
  rt_data.c g_stage_lists expands them (21 lists, pc_data). gen_rt_tables.py: pc_data now wins over a
  tables.txt MH1-address line (em12_scale_tbl, st45/st56_pos_tbl were silently zero).
- Checked: G quest 10 without RT_EM_HP/RT_DMG_MUL (GOD + warp/follow aids): Rathian flies between areas,
  takes 1-5 per hit (MH1 same script: 1-6), 900 s not won. With the aids: kill -> reward list without supply
  items (205, 205, 212; a "1:0" first entry left to look at) -> money -> mode 6.
- Open: G em_dur_init/em_dur_set level bonus (needs G em*_init part flags EMW+0x7D4); reward screen not
  looked at in a picture; Shell_data, St_data, Equip_Bonus, em_hp_vital_tbl, monster_data still zero
  (G changed them; need G code copies); no quest failure run; no G-only monster quest tried.

## G round 4: G's own tables for fair hunts (14 Oct 2026)

Goal: G hunts without test aids. All G-only code is under VERSION_MHG; the PS2 G rebuild stays 7 OK (clean), MH1 5 OK.
- Monster hit points (em_hp_vital_tbl is not used by G): hp = em_vital_tbl[kind][variant] (s16, sub_main 0x5098B0)
  x em_lv_rate_tbl[kind][lv].hp (16 levels of {hp, attack, defence}, 0x5097E0); game 0x5499A0 (C near-match 18/29,
  raw on the PS2), em_def_attack_set / em_def_defence_set (G versions match, em_master_b.c; raw .inc dropped).
  The PC's MH1 em*_init call em_hp_vital_set(2): em_master_nm.c maps them to G's variant per kind (em03 x11,
  em04 kinds 5/32 type, em12 type 0/5, em19/24 type&1, else 0; from the G inits' calls).
- Logged (RT_EM_TRACE / RT_HIT_TRACE=1: one `hit:` line per hit on a monster): quest 10 Rathian lv 7: 5000 x 1.1 =
  5500 hp; quest 77 kind 49 lv 10: 4500 x 1.6 = 7200; quest 98 kind 38: 6300; quest 119 kind 45: 8100 (all = G's
  tables). Blade damage = atk_rate x motion x sharpness/hit zone: the sword (Ken 156, attack 60) does 1-6 per hit.
- Found and fixed on the way (they made G hunts unfair on the PC):
  - the matched MH1 emu01/emu02/ems01.c (em_init_sub, em_init, em_status_init) and f_ud.c were linked into the G PC
    build and hid the G code of the near-match copies: no level, size or base scale (scale 0: an invisible Rathian
    hit only by accident), Get_equip_value(pl, kind) called with MH1's signature (atk_rate 0 -> every hit 1).
    tools/pc_g_exclude.txt now keeps them out of the G PC build.
  - Get_equip_value: G takes the hunter and reads User_data+0xC74 (G User_data is 0xE00 bytes; the PC host array
    was MH1's 0x480: G code wrote past it). rt_player writes G's offsets.
  - D_ names with G addresses (pl53 skills, pl73/pl77 ammo, pl_slash messages) read MH1 addresses on the PC: named
    (Skill_data, armor_kind_tbl, Shell_data, Shell_max_tbl, slash_msg_*; config/mhg/g_names.txt + pc_data.txt).
  - Ammo: G Shell_data is 2 bytes {recoil, pow}; ammo per load from Shell_max_tbl[Gun_data+0xD][type]; firing
    sound/motion class from sub_main 0x4CE8E0 (pl77.c, matches); Pl_shell_set's mask from 0x4CD030 (skills add
    shot types); Gun_data rows 0x18 (GUN_ROW), Ken/Gun/Item_data at G sizes.
  - Blade hits (G hit_hit_sub_em): six sharpness levels (s_gauge_tbl/slash_adj_tbl at G sizes), ailment rate per
    sharpness (hit_ailment_rate_tbl), skill 0x43, se kind 7, x3EB flag.
  - Crashes once the hunter is hit / the Rathian moves: eft20_data has 176 lists in G; st_mv_ptr_ck's longer lists
    (245/92); the stay/run-away timer lists' 0x100 host size covered G's neighbours (pointers into em01's stage-move
    lists read the expanded timers): pc_data `list:N` = the G list's length; pointer words of em command / route
    lists that point at label-less main data (mhg_pcmeta neighbour rule, 34 words).
  - G kinds 35-50 now get their init/main (f_em_nm.c, G's jump tables); kind 40's own code is not ported.
  - Reward list: G's remuneration_item_set (f_quest_nm.c): list at +0x100, G's condition ids, 7 chance lists; an
    item id 0 adds nothing (that was the "1:0").
- Checked (hidden): quest 10 with GOD/warp/aim aids only: 50 hits 1-5, no crash 200 s; time-out: idle in camp to
  the end: time -1 -> x06 6 -> クエスト失敗 screen (build/A/g/t6/to.png); quest 10 slayed: rewards 197:1 205:1 ...
  (no 1:0); G-only kinds 49/38/45 (quests 77/98/119) 200 s each.
- Open: quests on map 6 (game_w.x2E 6, stage 19: quests 89, 129) hang in kind 30's em program (cmd 0x2C species
  block, cmd_end_search loops on a misaligned byte; likely a G opcode length in next_cmd_search for 0x80-0x83).
  monster_data (G 0x5278D0, 12-byte rows; G rewrote disp_monster_list) still zero. func_0012D170 (G-only main) not
  ported (Get_equip_value / remuneration / Pl_item_charge take its 0 branch). Carts not tried naturally (the
  warp aid keeps the hunter inside the monster).

## G round 5: every G offline quest (14 Oct 2026)

`tools/test_all_quests_g.sh` (G=1 mode of tools/test_all_quests.py; hidden, test aids): G's offline quests are
the village group 00xxx (`--quest 1`..`50`) and training 10xxx (`10001`..`10048`) (quest_file_tbl groups 0 and 4;
01xxx-03xxx are the town, 20xxx the 50 event files). Per quest: the monsters spawn, clear with the aids, reward
list; a second run with RT_QUEST_TIME=600 must fail by time-out (x06 6); without arguments also a carts check
(quest 9, no GOD/warp, RT_PL_HP 5: the Rathian's hits faint the hunter, two camp returns, failed on the third
with time left). Result: **96/98 pass + carts** (KNOWN: 10030, 10048). MH1 test_all_quests 38/38, quest loop OK.
- Note: "quests 89 / 129" of round 4 were not offline quests: G's group-0 list has 56 entries, so `--quest 89`
  read group 1 (m01033). Their hang was real all the same (below).
- em commands: G's 0x80 random select takes entries 11-13 (next_cmd_search 0x58EFA8, em_cmd_rnd32 0x58CD90).
  next_cmd_search checked against an emulation of G's asm for every (op, sub, fill) - only this differed.
- PC tables at G sizes (config/mhg/pc_data.txt): item_str 524, Item_get_tbl 376 pick ids, em_alpha_clay /
  em_ride_data (52 kinds), eft06/eft13, em01/09/11/15/16/17/20/21 contents/stage-move/route tables; the 17
  one-item pick rows G keeps in main .data (item_get_rows_main, `noptr`: the scan had "converted" their
  {chance, item} words). The 92-stage tables (stage_fog_tbl, ground_tbl_add, st_sun_rgba_tbl, pl_light_tbl2) are
  left at MH1 size: grown, the village crashed in GetGroundHitStatusAreaPl (agent B/D to look at).
- mhg_pcmeta: pointer runs a pointer word points at (em29 command list 0x6BE4F0: kind 22 crashed); no scan
  words inside em byte-code objects (em01_main_cmd09 0x68996C: a quest number + 0x2C read as 0x2C0008).
- Monster kinds 3/4/5/32 never spawned on G (em_create_model refuses a kind whose program entries 0/3 are NULL):
  game 0x5B7010 (= eft01_set(em, 0)) and sub_main 0x4EDCD0 (em04 effect move) are host functions now
  (rt_quest.c); G's em04 sound script 0x4ECD00 stands in as MH1's ef_move_sub (stand-in logged), 0x4EDAA0
  (Eft13 dust) left out. Only kinds 40 and 51 still have NULL entries.
- em20 (Kut-Ku kinds 6/20...): G revival (act 5 no 3, die00-02 timers, die act 3 = game 0x622FE0, start points
  game 0x618A20) written from the asm. G added the same revival to em01/em15/em17/em27 act_set and die
  functions (callers of Quest_enemy_revival_ck in game.bin): not ported yet.
- G tools: pickaxes 112-114, bug nets 115-117 (f_pl.c). G's item_action_set (game 0x54D8C0, 640 lines) uses
  G's item ids throughout; the PC runs MH1's, so most item uses on G are wrong (player lane, listed in mhg.md).
- func_0012D170 = game_w 0x2F3 && 0x2C0, func_0012D1B0 = game_w 0x2F4 (main). Only mode_sel_end writes them and
  never both of 0x2F3/0x2C0: 0 in every shipped path; the G Get_equip_value / reward list call it now.
- Test aids: RT_QUEST_TIME offline, RT_PICK_ALL (nests: 100% points and carried items 512..), RT_PL_TARGET
  `kA,B` (first living kind: bosses in turn), RT_PL_GOTO lists skip a goal with no path, RT_PL_GOD 30000 on G,
  host model cache 64 kinds (G kinds 40-51 had none).
- Open (mine): 10030's 20th Kut-Ku does not return (19/20; ops 36/47 at 18 kills); 10048 Ashen Lao-Shan floors
  at hp 1000 on stage 14; kind 40's own code (game 0x647060-0x648610, ~30 functions) not ported: no quest on the
  disc uses kind 40 (dry runs of all 245 mission files; em40 has only a tbl file); monster_data / G
  disp_monster_list (main 0x125CE0) not ported: G rewrote it into a hunting log (a second page of 6 per page with
  kill counts and crown sizes: main 0x126100/0x126160/0x126340, sub_main 0x4C8580/0x4C85A0/0x4C8750/0x4C87C0/
  0x4C8870 over User_data records and the per-species lists at 0x4FEE90, crown sizes 0x4FEF60; names 0x527A40,
  frame lists 0x25E860..), it needs the G User_data record writers too.

## G round 6: hit crash, items, revival, stage tables (14 Oct 2026)

- **hit_calc_shl crash (quests 25/41/10046)**: G grew shell16_body_tbl to 21 entries; em15's attack 27 uses body 20,
  and the PC copy at MH1 size (20) made hit_calc_shl walk a neighbouring host table (ASLR-dependent, about 1 run in 3;
  reproduced only with ASLR on: gdb `set disable-randomization off`). A scan of G's pointer runs found the same for
  shell04/09/11/19_body_tbl and fade74_data: all at G size in config/mhg/pc_data.txt. 8 replays clean after.
- **Items**: G's item_action_set (game 0x54D8C0), Basic_item_set (0x4F27B0, + food bonus 0x4F2730),
  Fue_item_set (0x4F3610) and the acts with item ids (pl_mv014 throw, 054/072 ballista 512, 071 gather skills,
  079 frog, 085 BBQ with the gourmet spit and well-done steak G, 095 horn, 097 whetstone, egg_com_ck cannon 513)
  ported under VERSION_MHG (f_pl.c, pl79.c, pl80.c; not in the G PS2 build). eft12_data (smoke keys) had no G
  address (zero-filled): smoke/dung bombs crashed; now G 0x67B230. Hidden check (build/A/itest.sh pattern: quest 10/1,
  RT_PL_ITEMS id:3, square at a tick, RT_PL_TRACE acts): every item starts its act (drink 88, steak 90, whetstone 97,
  horn 94/98, throw 14, drop 11, meat/small barrel 25, large barrel 111, pitfall 46, farcaster 114, binoculars 101,
  BBQ 76/85); potion +30 hp. Trap/barrel/farcaster refuse in the base camp (as the game). Not done: shell03's G kinds
  11-15 (poison smoke, knives) and G's eft12 edits run MH1's C (no crash seen).
- **Revival**: em01/em15/em17 G act_set + die functions + die act 3 (em17: 4) with their start points
  (include/em_revive_g.h). em27: G only moved MH1's die04 revival into act_set (same behaviour, not ported). The
  G revival code path is built and runs the G sweep; no offline quest revives these kinds (not observed).
- **10030**: not a revival problem: the PC's quest_condition_prog lacked G command 0x2F (time-out section), so the
  program stopped after 18 kills. Added; 10030 passes.
- **10048 (Lao-Shan, kind 7)**: G Em_Dmg_Sys keeps kinds 7 and 50 at >= 1000 hp unless on stage 12 (game 0x583F30),
  faithful. Unattended, the PC's Lao-Shan walks from 14 to stage 28 and stays (checked with gdb at quest end). G's
  em07_main replaces MH1's quest-number test (101/107/184/206) with event flags 24 (kind 7) / 35: ported, the
  monster then waits on stage 14 for the quest's event demo, which the G PC does not run (evdemo stand-ins), so
  reverted. Needs: G evdemo for the fortress quests, G em07_main/em07_init, then check its route to stage 12.
- **92-stage tables** at G size: their words into sub_main's bss (village ground 0x541EE0..) were not taken as
  pointers, so the village faulted at 0x541F1C (= ground_tbl_add[87]+0xC). tools/mhg_pcmeta.py `allptr` flag.
- Jungle stage 74 exits and monster_data / hunting log: not reached.
- Tools: tools/mhg_fdiff.py reads the G counterpart from its own module (main -> sub_main/game).
- Verification: G PC build OK; MH1 PC build, test_quest_loop OK, test_all_quests 38/38; G sweep 97/98 + carts
  (KNOWN 10048) once at the end, plus a clean sweep after the step-1 fix. PS2 rebuilds could not run under the
  1.5 GB cap (wibo: "heap: failed reserve memory"); instead every changed MH1-registered file preprocesses to the
  same text as before (gcc -E with __MWERKS__, MH1 and G), and none is in config/mhg/c_files.txt.

## G round 7: Lao-Shan, thrown items, hunting log, jungle 74, revival tests (15 Oct 2026)

All G code below is PC-only (`#if VERSION_MHG && !__MWERKS__`, or nm / PC files); the PS2 builds keep their bytes.
- **10048 Lao-Shan passes.** G picks event demos by G quest number and, first, by the quest's monster kinds (game
  0x54A5D0: kind 7/50 -> the Lao-Shan demo, flags 24/35, stage 14; kinds 2/36 -> flags 27/36); the G PC ran MH1's
  EvDemoInitialize (MH1 quest numbers): no G demo ever ran. Ported in evdemo.c (+ species match in
  evdemo_camera_request, evdemo_g_tbl = G 0x678280). em07 (the PC links em07_ai_nm.c, not em07_ai_r01.c): G em07_main
  without the quest test, em_demo00's flag by kind, em_atk04/05 set EM07W x18 = 2/3 (em command 0x2B sub 5 waits for
  it: without it the Lao-Shan rammed the stage-28 gate for ever) and the gate flag (G game_w+0x189 = flag1B3).
  Unattended: 14 (demo, ~2000 ticks) -> 30 -> 28 -> 11 -> 12 at ~58000 ticks. test_all_quests.py: the hunter waits on 12.
- **Thrown items** (shell03 G 0x65AA90.., shell09 G 0x665920.., eft12 G 0x56FE70..): knives 12-15, poison smoke 11,
  placed bomb 16, skills 80/83; shell09 kinds 18-23, attack data arg+1 (MH1's atck_data_no is an empty work area on
  the G PC: every blast had attack data 0 until now), bomber x1.5; eft12 arg 6 puff, 4th smoke colour.
- **Hunting log** (main 0x125CE0.., sub_main 0x4C8510-0x4C8910, 0x4F44F0, result_prog 0x4F4D00): PIT_W 0x83 carved as
  hlog_page; User_data+0xD76 kills per kind, +0xD10 size record per kind, +0xC98 monster-list bits (MH1 +0x3F0: the G
  PC's Monster_list_* read the wrong word). G monster pictures are filedef 7 (rt_2d.c).
- **Jungle 74**: real data, a start-only area (FINDS). Walking out of it works (74 -> 73, 74 -> 75 checked).
- **Revival**: RT_QEM_LIVES test aid + tools/test_revival_g.sh: em01 (quest 10: back on 40, killed, clear), em15
  (quest 40: back on 19, followed, killed), em17 (quests 23/26: back at G's fixed points 64 / 90).
- Build note: a stale build/mhg/pctree/build/pc/rt_gen.defsym* made the G PC link fail (D_3F360A defined twice after
  agent D's lb_u.c); deleting the two files fixed it. Not in git.
- Open: em07_init's G calls (sub_main 0x4DC5D0: em+0x7D4 status-immunity bits?, game 0x549FE0 char set); point_cam_sub
  case 25 (G sets demo x66 instead of point_cam_hit); G result_prog's other changes (0x4F4900 etc.); the poison
  knife's ailment value on hit not checked.

## G round 8: release checklist for G, coverage, five player-visible fixes (15 Oct 2026)

docs/RELEASE_G.md (criteria, gaps by player impact, coverage) and docs/survey/mhg_cov_g.txt (generated list).
- **tools/cov_report_g.py**: a COV=1 G build's coverage mapped to docs/survey/mhg_map.csv; lists entered functions whose
  G twin is missing/rewritten/edited/shape and whose PC body is not a G version (no G marker in the body, not in the G
  branch of an `#if` on VERSION_MHG), plus the zero-filled G tables entered functions read (objdump -dr of the .o files).
- **CONTINUE crash**: omake.c's G disp_mode_menu reads D_528538 / D_528550 / D_002EC668; the first two were zero arrays,
  the third was aliased by gen_rt_auto.py into MH1's weapon_model_data (its hex read as an MH1 address). Now in
  pc_names.txt, and gen_rt_auto.py takes a D_ name's G address from G's symbol view when it has one. Only D_002EC668 and
  D_004FEA50 changed in the generated aliases (diffed); D_004FEA50 (G story-gate quest list) was page_tab+0x1140 before.
- **Elder lists**: G's vs_square_init_init now calls Lb_make_quest_tbl_local (lb_guild_g.c) instead of a stand-in
  (it predates agent D's G version): "1 / 0" -> "1 / 5".
- **Reward screen**: reward_itembox / disp_reward formats (lit_1001/1002/843) were zero; G reads them through cells at
  0x5262BC..0x5262C8. pc_names.txt lines ending in `+` add a name beside an existing one (mhg_pcmeta.add_hand_names).
- **em12 revival points** st49/51/53_pos_tbl (G 0x69D5F0/650/6B0, pc_data.txt); **flfntLocate_i** aliased (build_pc.sh).
- Build note: a COV=1 build leaves instrumented objects behind: build_pc.sh does not recompile when only the flags change,
  and the next plain build links them (gen_rt_auto turns __cyg_profile_func_enter/exit into stand-ins; the nxdk link
  fails on them). After a coverage run: delete each build/mhg/pctree/build/pc/cmd/X.sh that holds -finstrument-functions
  and its X.o, then build again.
- Seen, not mine (in RELEASE_G.md with lanes): EXTRA (omake) row crash (omake_str / check_bit_no), elder fee number missing
  from the talk text, equip_exp_core strings zero, set19 position tables, em14/em17 G act selection (GN6, mine, open).

## G kind 40 on screen (15 Oct 2026)

- Kind 40 = "Higeruu", a cut-down em09 (Felyne) with its own 16-bone em40_tbl and no model; details in docs/FINDS.md.
- src/game/em/em40_g.c (PC only, VERSION_MHG, written from G game 0x647060-0x648E30, not byte-checked): init, main,
  acts, sound script; PC-only program table (G's em_prog_tbl[40][3] is named em04_effect_move_0058F3E0 by the G map,
  so the host would run the Mosswine's script; em40_init installs em40_prog_pc). game_func_00648E20 resolves [0]/[1].
- Fixes found on the way: em_parts_num imported at 0x28 (kinds 40-51 had 0 motion layers or junk; now 0x34 in
  config/mhg/pc_data.txt); em_type_act_set lacked G kinds 35-50 (G jump table lit_1223_00685080: each takes its base
  monster's act set, 40 -> em09); em09_act_set act 1/2 off stage 0x35 sets no position in G (MH1 read one from a
  stray register; crashed the PC with kind 40).
- Test aids: RT_QEM_KIND=from:to (rt_em.c + em_create_model in rt_quest.c), RT_EM_MODEL=kind:emNN[:emMM] (viewer.c).
  Run: `RT_QEM_KIND=23:40 RT_EM_MODEL=40:em09 RT_QUEST_STAGE=1 RT_CAM_EM=k40,350,320,0.6 mhview_g disc/mhg --quest 1`.
- Checked: G PC build, Xbox G build (nxdk), MH1 PC build + test_quest_loop OK, test_dbgmenu OK, test_all_quests_g
  10033 10038 10042 10044 28 33 (subspecies quests, after the act-set fix) all PASS.
- Open: G shell11_i (index and table edited) not ported, so kind 40's swing (and G Aptonoth hits) use MH1's data on
  the PC; the kind-40 hit does no damage.

## G round 9: em14/em17/em08 move choice, em04 sounds, durability bonus, shell attacks (15 Oct 2026)

All PC-only G code (`#ifdef VERSION_MHG` in *_nm.c / PC files, or `VERSION_MHG && !__MWERKS__` in registered files); the PS2 builds keep their bytes.
- **GN6**: G's em14_to_normal / em17_to_normal are `j act_dist_select`; G's act_dist_select only handles mode 3 (both inits set mode 3, so
  MH1's mode 0-2 paths with em*_act_add / rail tables were dead in practice but read zero tables). em_act01 (both) waits for x194 in every mode.
  em17_to_fly lost the mode test. em14_main / em17_main: case 11 guards, kinds 43 / 47 / 48 in part-break reactions, demo stops of 10041 / 10043
  (G quest numbers, game_w.info_stop = G 0x1F5). em08_main: a whole G copy (no trap / sleep2 / x9EA reactions, swim stagger by depth, part 3 -> 4/20).
- Test: scratch script acthunt.sh (god, follow + warp to the boss every tick, attack pad cycle, RT_EM_TRACE, now with `kind N` at the line end):
  act sequences per monster; every act returns to 0/1 through em_cmd_ck (no freeze).
- **GN15**: decoded G 0x4ECD00 with a small register simulator over the asm (calls Code_Make / sound_call per case): same calls as MH1 except
  kind 32 in motion 0x3F7 and no Eft13; the dust moved to 0x4EDAA0, run only when func_001F19C0(2) == 2. func_001F19C0(i) = the byte
  Load_overlay(value, slot) stores for slot i (Arrangement+3+i in G's symbol view); Load_overlay(2, 2) only in the gallery's event scenes.
- **Durability bonus**: sub_main 0x4DC5D0 sets EMW 0x7D4 bits (em*_init), 0x4DC5F0 adds base x (0.33 + 0.08 (lv-9)/3) to a marked part
  at level 9+ (em_dur_init and em_dur_set). Offline G quests are level 2-7 (scan of 30 quests), so it shows after revivals / online.
- **shell11**: G passes kind+1 from its callers and indexes the attack list by kind; table at G 0x2EEA48 is a pointer cell to the list
  (pl_atck_data_set_shl stores the table, shl2 reads entry idx * 24). Lesson: when a G shell's index changes, compare every caller's
  argument in both games (objdump grep of `jal Shell11_set` with the a1 load before it).
- **Poison knife**: G entry (shell03 arg 13 -> atk_sel 6) has ailment bit 2, value 50; dm_value_add adds it to EMW 0x7BC. Hidden hits not
  achieved with the warp aid (the hunter stands inside the body or the boss flies / burrows); thrown knives fly straight (traced, trace removed).
- G result module (sub_main 0x4F3A50-0x4F8030) mapped: 20-step result_prog, item hand-in (0x4F4900, five G item ids), crown/record and
  hunting-log announcements (0x4F4620 / 0x4F4780), gallery unlocks (0x4F41B0), new result screens. Not ported (RELEASE_G GN17).

## G round 10: the quest result module, Plesioth / Cephadrome G moves, poison knife (15 Oct 2026)

- **f_reward_g.c** (PC only, VERSION_MHG): G's whole result module (sub_main 0x4F3A50-0x4F7FDC) written from gdraft drafts read against
  `mhg_fdiff.py -g` listings (m2c dropped call arguments and merged jump-table cases: check `case 0` of every gold/sale/point display:
  it is the footer only). tools/build_pc.sh swaps f_reward.c / f_reward2.c / f_reward3.c for it (G only). G strings and tables through
  pc_data.txt allptr tables (result_msg / result_str / result_str2 / movie_add_str_tbl at G size 0x54); G's result_w is a host static.
- **func_0012D170 bug** (rt_quest.c): it read game_w+0x2F3 / +0x2C0 (past the host's game_w) and tested the second byte the wrong way;
  G's code reads system_w+0x43 / +0x10 (1 = 0x43 set, network off). It returned 1 in test runs: pouch restored after each quest,
  no item sale. Lesson: a G helper that reads "game_w+0x2xx" past G's 0x240-byte game_w is reading another object; take the name from
  G's symbol view (system_w here).
- Shots (hidden, RT_SHOTS with tools/test_all_quests_g.sh): build/A/gn17/1_reward_list .. 7_gallery_title.png. RT_PL_ITEMS="145:3,178:2"
  with quest 10 shows the sale; quest 10015 shows record + training + gallery title; quest 10043 the ecology movie.
- **em21 (Plesioth)**: game 0x62DB60 replaces `x388 = v` in 63 acts and picks em_range_set (scratch rng2.py found each call's value and
  flag by following li/move to a1/a2 from the function start: the flag is often a leftover `li a2,1` from a case compare). Part-3 damage
  act 23. **em08 is Cephadrome**, not Plesioth (round 9's note was wrong; G's symbol view gives G addresses MH1 names like BsDummyTask,
  which made fly07/10/16 look rewritten). em08 atk03/04 ported.
- **Poison knife**: tools/test_activities.py's pinned-Rathian setup with mhview_g (scratch knife.py): one knife hits (hp -4), stock +50,
  tolerance 200, decays 10 a step; later knives miss once the Rathian turns to attack.
- Open: em21 die case 4 (game 0x60CF20), the quest-10035 stage-54 demo (em21_main -> 0x60C280), em21/em08 init char-set (0x549FE0).

## G on the Xbox: owner's bugs GB5 1-10 (16 Oct 2026)

Reproduced on the PC G build first (hidden, --shot/--time/--input, RT_SHOTS), then in xemu (own copy of the HDD image in
build/A/xf, `flock xemu.lock`, build/A/gb5/x/xrun.sh records each run). Pictures and logs in build/A/gb5/.
- **1 draw order after the debug menu**: dbgmenu.c states_on() turned GFX_RS_ZTEST off and states_off() never turned it on;
  nothing else sets it, so every frame after the menu had been open drew without the depth test (PC GL too, MH1 too). Shots
  z/a_400 (no menu), b_400 (menu opened and closed: villagers over the gate beam), c_400 (fixed). Also dbg_leave_village():
  Pit_reset + Lbc_set_prim(0,0,0) before the debug menu leaves the village, as Local_main does.
- **2 village music at the title**: dbg_title stops both streams and the SE voices. Note: Local_main's -1 (leaving the village
  "another way") makes viewer.c run quest_back (restart the quest), not the title [code read, not run]; the bed's
  "quit the game" would end there. Left for later.
- **3 no villagers**: G's Lb_reset (lobby 0x59CCA0) returns right after Online_ck() offline; MH1's runs push_em_work_all, and
  G's vs_square_init calls Lb_npc_set only while lb_sys x00 == 0 (the first load of a visit). Leaving the house emptied the
  village (the debug menu's village row reloads from step 0, so the NPCs came back there). lb_z133.c G branch.
- **5 weapon swap**: rt_player_game_init (rt_player.c) wrote its default weapon (156, or RT_WEAPON) into User_data at every
  quest start, so any equipped weapon reverted; it now takes the save's (Ken ids < 381 / Gun < 78 in G). The sound packs
  had followed User_data already, which is why the owner heard the new weapon.
- **6 gallery movie**: gfx_update_texture (gl / nv2a / null) and gfx_tex_rgba_hint; rt_movie.c rewrites the oldest of its three
  textures. xemu: free memory flat at 11 MB during movie 9 (was 4.7-5.8 MB), no black frames. xemu still shows only ~2
  frames a second during the movie (the decoder in emulation); real hardware speed unknown.
- **8 bed save prompt invisible**: G's Lbc_init_network_work had no callback on the text prim (MH1's alias has no G address);
  wiring text_lobby_trans_ot3 crashed in DispDialogData because tl_etc was a generated function stand-in in G. tl_etc = G
  lobby 0x636E70 (config/mhg/pc_names.txt), G dialog kind 4 reads entries 3/4 (lb_by180.c TL_SAVE). PC and xemu shots.
- **10 one frame after retire**: viewer.c draws black while game mode is 6 and the village has not been entered (the quest
  stage is gone). New test aid RT_RETIRE=tick (rt_dbg.c, offline Quest_retire_set). PC RT_SHOTS: tick 477 sky -> black.
- **4 sky in the wake-up**: not reproduced (PC every 4 ticks, xemu every recorded frame) on the current build.
- **9 kill-all**: not reproduced on PC G (areas 32/34/35/36 of quest 10: only the hunter's area's monsters hit, second pass
  shows them gone; kill in 33 then moving to 32 leaves 32's alive). rt_dbg_hits now logs each monster (slot, kind, area, hp).
- Found: garbage line "慰･$d" at 140,360 after NEW GAME's save (select/edit_nm.c disp_edinfo_g with an item index past
  G's help table) [seen in PC font trace and xemu video; not fixed].

## G round 11: release checklist, fourth pass (16 Oct 2026)

Hidden PC G runs only (pictures build/A/text, build/A/rwbg, build/A/q10035, build/A/ts). RELEASE_G.md section 8.
- **D_FFFF pointer words**: splat writes the data word 0x0000FFFF as `.word D_FFFF`; mhg_pcmeta.split_data counted every
  `.word SYMBOL` as a pointer, the loader turned the words into NULL. 78 words in G main, each an s16 list's -1 end
  (Sel_menu_disp's title strips: "CHARACTER CREATION" + zeros drawn as 'A'). Lesson: a split's `.word D_xxxx` with an
  address below 0x100000 is a number. Found by tracing the strip in Sel_menu_disp (p pointed into the G image, not a
  host copy: the importer was not at fault).
- **~Bnn**: G queues {x, y, glyph height, icon, 0xFF} per font stack (0x12B8F0) and draws the queue with PutButtonICON
  after the stack (0x12B530); rt_font draws it at once (same entry).
- **em21 / quest 10035**: the map pairs MH1 em_move05/06 of em21 with another monster's (G 0x60D700/0x60D7B0: wrong);
  G's em21 move05 (0x635EB0) has the MH1 four die acts, move06 (0x635F40) adds act 1 = game 0x6345E0 (the opening's
  scripted swim). Lesson: check a `mhg_map.csv` "order" match against the neighbours' addresses before porting.
  m2c drafts of 0x6345E0 lost a0 in two calls; read em_frame_check / Shell08_set_ang_time args from the asm.
- **Frame copy**: G asks in quest_condition_prog (0x4B8AA0: system_w+0x33 = 0x81), copies in trans (0x12E8A0 ->
  0x12EC90), starts the clear camera when 0x80 is seen (0x4B905C). The PC's quest flow stops calling
  quest_condition_prog after the clear (x00 -> 1), so the camera stays immediate there. Fast-forwarded test runs draw
  no frame in between: RT_STEP=1 shows it.
- **Pl_damage_sub / pl_guard_set** (G C in pl_damageb.c, `#elif defined(VERSION_MHG)`): every G Pl_act_set passes 0xC;
  sub_main 0x4CC810 = stun gauge / +0x93E / +0x93D test. Eft16_set_impact's float is the last C argument on the PC.
- **trans_stage**: m2c's flmatMakeTrans shows f12 and f14 only (x, z): a UV matrix's v (f13) must be read from the asm
  for every new case. Diffing the m2c drafts of MH1 and G (addresses stripped) lists the changed cases quickly.
- Seen, not fixed: `mhview_g --stage 88/89` crashes in load_stage_hit (f_sphr.c:186), not from this change.
- Progress (VERSION=mhg tools/progress.py): 20.161% before and after (this round is PC-only C, no new PS2 match).

## G round 12: real-G reference shots, stage fog, Dual Swords sounds, NextStage_Dir_Set (16 Oct 2026)

- **tools/g_ref_shot.sh + tools/pcsx2/pine.py** (RELEASE_G section 9). PCSX2 2.8.2's PINE: `EnablePINE`/`PINESlot` in
  [EmuCore], socket $XDG_RUNTIME_DIR/pcsx2.sock.SLOT; ops 0-7 read/write 8-64 bit, 9/10 save/load state slot, 11 title,
  15 status. run_pcsx2.sh: `PINE=slot` env, left stick on i/j/k/l, right stick t/f/g/h (the d-pad does not walk).
  Village warp for the real game: lobby player_work (G 0x34D450) +0xAC / +0xB4 floats, as RT_LB_WARP. Quest accepted
  = select_w+0xD0 (G 0x35EC30) holds the quest number (10001 for the elder's first quest); writing it alone does not
  open the gate. Area warp = PLW+0x73A stage, +0x73C pos, +0x570 angle, +0x738 = 1 (stage_mv_ck's own fields).
  G's camera: lpView (0x2EF28C) -> eye[3], target[3], ..., +0x2C angle of view. Gotcha: `kill $!` of a `flock ...
  run_pcsx2.sh &` kills flock only; the emulator keeps the lock: kill run_pcsx2.sh (pgrep -P flockpid).
- **Fog**: fl fogs per clay (RenderState 0x8000 from Attribute_from_amo's aa_fog[a[8]], aa_fog = {0x8000, 0} in both
  games), FOGCOL takes the low 24 bits only, start/end are flSetRenderState 0x10/0x11 (stage_fog_set per stage; G reads
  stage_work+0x50 instead when +0x4D > 0, set by func_0010F9F0, not ported: never set offline [inferred]). The PC
  now does it per clay (gfx_clay_desc.fog). Fog strength vs the PS2 not calibrated (VU1 code not read).
- **flPS2SendRenderState_ALPHA** (0x178C90) decoded for src 2: dst 0x10 -> ALPHA 0x48 = Cs*As + Cd (additive),
  dst 0x30 -> 0x44 (normal blend). Confirms SetTrnslMode(4, 1) is additive (stage 27's layers).
- **Battle_type at MH1 size on G** (0x7C of 0xE1): a G table that keeps its MH1 name but grew in G is imported at the
  MH1 size unless pc_data.txt says otherwise; look for `extern X name[MH1 size]` with G indexes past it.
- **ef_move_sub comparison method** (scratch, not committed): a ~250-line MIPS interpreter over the objdump of both
  functions, pointer values symbolic (pl, w), loads from pl/w taken from an input dict (kind, anim), frame_check
  returning 1, Code_Make returning a marker; per (kind, anim) the call multisets are compared. Guards are found by
  re-running with one frame_check returning 0. The generated C was checked the same way (harness compiling the
  case block with recording stubs).
- **eft09 (G)**: G's eft09_set keeps the quest entry's lives (em_work_serch(em)->x04) in ew->timer; eft09_m ends the
  tail when the entry's lives drop below it and +0x2E = 2; subspecies kinds in the tail_off switch. f_quest.c's statics
  are global on the PC (one TU), so G code that calls them (em_work_serch) links.
- Checks: see the commit.

## G round 13: the Dual Swords (job 6) on PC/Xbox G (16 Oct 2026)

- **What G adds for job 6** (all read from G's asm): pl_attack cases 0x53-0x67 (0x67 = pl_at024(pl, 3), another class);
  eight G-only handlers at game 0x55C160-0x55D36C, named pl_at083/084/085/089/091/093/094/102 after their first action;
  ds_atk_set (game 0x54CBA0: next attack from the stick and the demon mode); job-6 cases in basic_atack_ck (stick:
  forward 0x55, else ds_atk_set), ex_atk_ck (evades as sword and shield, R1 during a move = 0x63 on / 0x62 off) and
  job_special_com_ck (R1 = 0x53). PLW+0x93D = demon mode ("kijinka"): stamina drains 1 a tick (timer_calc_sub_pl, was
  already ported), knock-downs end it (pl_dm002 / pl_dm005), shell00_i gives x1.35 attack and hit flags 0 / 0x12.
  shell00_set2 (game 0x6690D0) = shell00_set with the blade in shell x07; shell00_i then swaps body 9 <-> 10 (left hand).
- **Tables at G size** (config/mhg/pc_data.txt): Pl_slash_tbl, mv001_tbl, job_atk_adj_tbl (7 jobs + pad), ds_chr_tbl
  (G only), weapon_model_data / WEAPON_TEX (225 models; was 124: no G-new weapon of any class had a model on the PC).
- **Weapon drawing**: G's weapon_joint_calc takes the blade as a second argument (case 6 only; weapon_joint_calc2 on
  the PC); weapon_trans places the second hierarchy in the left hand (joint 0x12, offset -7.4/-0.4/-6.5, XYZ(0,0,pi)) or
  on the back next to the first (part 9). rt_player_weapon does the same.
- **Trails**: G's eft05 keeps up to two trails per effect (count per arg 1,1,1,2,2; history interleaved per trail),
  arg 1/2 right/left blade, 3 both blades in red for the demon mode (pl_init creates it for the quest, timer -1, drawn
  only while the mode is on), per-weapon tip offset and hand-row scale tables (ds_type_index = G main 0x101F10). The
  demon-mode glow (G game 0x567F70..: effect type 3, clay 59 of eft_mdlw[0], every other frame) is eft_ds_aura_*.
  The glow's update passes an unset register as the frame mode (G bug or leftover); the PC uses 1 as the draw does
  [guess].
- **Matching**: pl_at084..pl_at102 (7 functions, 4320 bytes, part 48) and ds_atk_set (316 bytes, part 49) match G
  (VERSION=mhg check.py, G game rebuild OK). pl_at083 near-match (one extra empty `beq arg, 2` in G; switch orders and
  an empty case tried), PC only. tools/check.py now also finds a G-only function by a name PROVIDEd in
  config/mhg/g_names.txt. G matched: game 26.34% -> 26.72%, all G 20.04% -> 20.16%.
- **Tools**: tools/gdraft.py drafts raw G functions too (c_rawfuncs, from the split image through objdump);
  tools/g_ref_shot.sh WEAPON=KIND:ID (the equipped item-box entry User_data+0xCFE -> +0x44 + 6n is poked before the
  gate; G copies it to +0xC75 at departure) and --combo "KEYS" (records the screen while pressing keys; peek:OFF reads
  a PLW byte). tools/test_all_quests_g.sh has a Dual Swords hunt (g_dual; DUAL=1 with quest numbers).
- **Checked**: real G in PCSX2 with Dual Swords 234: forward combo, R1 demon mode (PLW+0x93D read 1), demon attacks with
  red trails on both blades; PC side by side build/A/dual/cmp_combo.png, cmp_demon.png. PC hunt quest 10 with 234:
  hits on the Rathian (1-5 per hit, no aid), quest clear with the test aids; all four stick directions and the demon
  versions run (0x5B 0x5C 0x5D 0x5A 0x54 0x59). Not checked: hit values against the real game, sounds by ear,
  pl_at089 (demon dance) frame by frame, the white (non-demon) trails' look.
- **Xbox**: G nv2a build in xemu, RT_ARGS=--quest 10 with RT_WEAPON=234: quest starts, the hunter holds the Dual
  Swords (build/A/dual/xemu.png, xemu.mp4), 20-25 fps, no crash in 240 s. Checks: tools/full_check.sh all ok (emulator
  tests skipped as usual), VERSION=mhg rebuild 7 OK, G Xbox build ok.

## G round 14: picture match with the real G, quest lane (16 Oct 2026)

- **Method**: tools/g_ref_shot.sh gained `POKE="pine commands"` (a second PS2 picture after a memory poke, output in
  OUT/poke_TAG.txt: used to switch the PS2's fog off and to read stage_work / fl state) and `PCQUEST=10001` (the PC
  picture from a quest after the same area change, test aid RT_AREA_GO=tick,stage[,x,z,ang], instead of the --stage
  viewer: set objects, effects, HUD and the screen gradient only run in a quest). Each run writes OUT/fl_TAG.txt
  (flFogColor/End/Start, flPS2FadeColor, flPS2Ambient read from the PS2). Picture difference: a small python script
  over ffmpeg raw frames (scratch; numbers in RELEASE_G section 10).
- **Lesson: compare before modelling.** The "brighter PC" looked like lighting / fog; reading the PS2's fl state showed
  fog and fade identical, a fog-off poke changed almost nothing, and the per-pixel ratio was a multiply -> a 2D overlay.
  trans() calls stage_spr_disp between ot4 and the sprites; the PC's rt_game_draw had replaced trans() and left it out.
- **VU1 notes**: stage area parts use shader family 0x19 (Vu1Code_0019_0002, AddMatrix_0007: matrices + FOG at mem 9):
  vertex colour goes to the GS unscaled (ftoi0 of the float colour, flPS2HalfColorSub halves the bytes at clay build:
  255 -> 128 = 1.0), fog F = 255 (end - w) / (end - start), clamped. The param tables (param_XXXX_YYYY, 0x18 input type,
  0x24 colour scale) map families to programs; families 0x10-0x12 are unlit without fog, 0x19/0x1A the fogged ones.
- **G stage environment block** (main 0x10FD10 and helpers 0x10F9F0 / 0x10FA40 / 0x10FAA0, read from the asm):
  layout in rt_game.c rt_stage_env. stage_work+0x4D is the version; stage_fog_set / light_init / stage_set_set test it.
- **Matched for G this round** (VERSION=mhg check.py OK, G rebuild 7 OK): pl_at024 (f_pl part 50), stage_set_set
  (stage_set_g.c, jump table sub_main 0x529300), set19_i + set19_trans (removed from c_rawfuncs), set06_i / set06_m
  (part 4), set06_trans (part 5), stage_fog_set (putspr2 part 2), eft13_t (eft13b part 3, jump tables 0x52BEB0-0x52BF70).
  Matching tricks: G's stage_set_set wants Set06_set(s16) (passes the loop variable unextended) and an unprototyped
  Set05_set; set06_trans needed its early returns split into two ifs (`if (!p) return; if (!p->flag) return;` instead of
  `||`, which made MWCC emit bne + b to a second epilogue) and stw declared before mw; eft13_t needed a random-swap search
  of its declaration order (scratch tryorder.py: 162 -> 84 -> 20 differing) and then statement order inside the new cases.
- **Names**: config/mhg/g_names.txt gained set19g_* (mhg_map put set19_s11/s12/s21_mask_tbl one table off: the G
  names sit on stages 15/21/24's masks), set06_st76/77_*, stage_fog_tbl, the overlay set functions and sun tables for
  sub_main's stage_set_set. check.py finds a G-only function with no type in the symbol file through a g_names PROVIDE
  (set06_trans).
- **PC-only ports** (G code, not matched): stage_spr_disp G branch (f_stage_nm.c), set13_i / set13_m / set13_trans kind 8
  and new stages, eft20_pos_set, em08 dmg05 / move04, em16_to_normal, light_init's environment override.
- **Lesson**: a G function that drops a call (em27 die04 without Quest_enemy_revival_set) can rely on a caller G
  changed too; porting it alone made the Velocidrome family revive forever (test_all_quests_g caught it). Reverted.
- **Fixed on the way**: Em_data_com_adrs_get with no quest (PC only: the viewer's monsters crashed in G's eft09_set);
  stale set clays after an area change to a stage without set model (rt_bind_set_model n = 0 now unbinds); 3D clear
  colour black.
- **Checks**: tools/full_check.sh (see the commit), VERSION=mhg rebuild 7 OK, G Xbox build. Pictures: build/A/look.
- Progress (VERSION=mhg tools/progress.py): all G 20.16% (round 13) -> 20.575%.

## G Xbox bugs 11-14 and the tint triangle (17 Oct 2026)

- **13 (both games)**: the host weapon model followed PLW+0x34C only in the online town. Village equip (shop
  shop_armor2_stack / armor_set_myArmor -> Set_userdata; the PS2 also Lb_player_release/Lb_player_load) changes it:
  weapon_follow now runs in the offline village too. Checked with tools/gvil_ev.py (W="x,z,0;tick,x,z" puts a second
  warp after the shop, to see the hunter): Iron Sword model 10, Bone Lance (G id 103) model 6.
- **11**: the owner's report was the weapon shop's comparison with the just-bought Bone Lance as current: correct.
  Top box = current weapon, bottom = the one to buy (attack red/blue lower/higher).
- **14 (G)**: G vs_square_init (lobby 0x59EF00) -> lobby_bgm_set with no argument (m2c draft); the asm keeps the stage
  byte in a0. Garbage 0 -> Snd_bgm_tbl[0] = M6_TAKI1 (waterfall) at 127. G Snd_bgm_tbl = main 0x288E40 (house 86 ->
  stream 0x1C vol 0x37). Wrong lead first: an x87-stack-leak theory (cos/sqrt of the ADX filter going NaN); tested with
  a fnstenv tag-word check in xemu: no leak; dropped. All G AFS00 ADX are 2 ch 48 kHz (format not the cause).
- **12**: pdclib fwrite = byte copy + 1 KB flushes. PNG now built in memory, one write (xbox_write_file). Message frame
  first. xemu save time: 5832 ms before (pictures 5791 ms), 247 ms after (pictures 219 ms); xemu_pick_test G 15/15 PASS
  (one earlier run froze at ~40 s with the host at load 8 and other agents' builds; the rerun passed: not explained).
- **1 again**: not reproduced. Owner's session had debug-menu money and three reporter freezes. Fixed: reporter's
  end_session restores the game's texture (rt_2d_restore_texture; the game caches its bound texture); stage_spr_disp
  skipped in the id pass (xemu_pick_test G failed 'stage' pick: the full-screen layer covered everything). PC GL
  near-plane trace (temporary) at the weapon shop: skinned clays fully behind the eye only, none crossing.

## Cross-game audit (18 Oct 2026; COORDINATOR queue 0d)
Full list: docs/audit_cross_game.md (738 commit/file lines + 30 hand-checked fixes from commit messages).
- Method: all 166 G-touching commit messages since 13 Oct read; every VERSION_MHG / rt_ver_mhg / !VERSION_MHG line in
  src/pc and tools read in its current context (grep -B/-A); the 396 G branches in decompiled files judged from the commit
  message plus a scan of their added comments for PC / host / crash / stand-in wording (121 lines). A reverse sweep of
  src/pc for raw MH1 User_data / select_w offsets found the pouch test (#4).
- Most G platform fixes were already shared: the GB5 bug fixes, the reporter, fog, the black quest-start frame, rt_mem.
- G-only pointer / size fixes (mhg_pcmeta, pc_data sizes) cannot apply to MH1: MH1's pointers and sizes come from its own
  ELF symbols and relocations (tools/gen_rt_meta.py says so); only three MH1 tables are hand-sized (tables.txt).
- Lesson: the life bar showed why RT_PL_GOD's 30000 was G-only (the bar ran off the screen); setting it only around
  pl_move (where Pl_damage_sub takes the hit of the last hit_check) and the maximum PLW+0x792 after keeps both right.
- G's quest_bit_tbl (sub_main 0x4FECA0) read with gdb: 0-50, 1001-1039, 2001-2050, 3001-3007, 10001-10048, 20001-20050.

## G round 15: the rest of the quest lane (20 Oct 2026)
All PC C from G's asm (VERSION_MHG in *_nm.c / PC-only branches; the PS2 builds keep their bytes). Not match-checked:
check.py needs wibo, which this round could not run (PC-only round). Commits on agent-A, "G quest lane: ...".
- **Method**: tools/gdraft.py (G m2c draft) next to tools/draft.py (MH1 draft, where MH1 asm still exists) or the PC C,
  normalised (temp names, D_/func_ names, the G 0x960+ EMW shift) and diffed; per-case call multisets for the big
  sound/effect scripts (scratch casecmp.py: split on `case 0x3E8+`, calls with float args sorted out of the m2c order);
  every hit read in the asm (`tools/mhg_fdiff.py game F -g`) for breaks, jump tables (read from build/mhg/split) and
  registers m2c lost (em_char_set2's 5th arg, em_frame_check3's 2nd float). Then hidden god-mode hunts with a temporary
  act trace (mode.x15 per change) and tools/test_all_quests_g.sh.
- **Lesson (EMW in G)**: every EMW field from 0x960 on sits 8 bytes lower in G (include/em.h). An fdiff line
  `sb x,2538` vs `sb x,2530` is the same field (x9EA), not a new one; I first "ported" em17's 0x9E2 writes and reverted.
- **Lesson (names)**: mhg_map pairs G functions by similarity, so a G function can carry another MH1 function's name
  (G em17's 0x60A480 is listed as em_atk07 but G calls it for x15 1; MH1's atk07 is unreachable in G). Always read the
  dispatcher's jump table (em_move03) before porting by name.
- **em17** (Gravios/Basarios): see the commit; new: G em_move03 table, atk01_g / atk02_g (a jump), atk04_g / atk05_g
  variants, the 10043 demo (G rewrote it: a scripted walk, 3 idle motions on layer 1), ef_move_sub's new motions.
  Hunts (quests 23 / 26 / 10043, 200-400 s): acts 3.0 / 3.1 / 3.5-3.9 seen, all return to 0.1.
- **em27** (Velocidrome family): G moved the revival into em27_act_set(5, 4) (Quest_enemy_revival_set, em_status_init,
  em27_revive_init = em27_init + MH1's die04 start points); die04 only picks the act. Last round's "revives forever"
  came from porting die04 alone. Quest 7 now: three kills, two revivals, clear. Also em27_main's "face the hit" turn
  (G sets ang[1] by the hit quadrant before each flinch/fall act), G quest 10014 for the demo stop.
- **em16** (Velociprey family): same face-the-hit turn, demo stops 10006/10010/10041 + the new 10035 (demo act 6.6,
  G 0x6025F0: hidden until event flag 0x16). That showed a **platform bug (both games)**: viewer.c drew monsters whose
  x01 is 0; the game's em_move only queues the monster's prim when x01 != 0 (src/main/em/f_em_nm.c). Fixed in
  em_slot_sync (one line). Pictures: build/A/q10035b/grid2.png (Velociprey gone in the opening scene), grid3.png (they
  come after it).
- **em01** (Rathian/Rathalos + G variants 0x25/0x2A, 0x29/0x31): macros EM01_KIND1 / EM01_KIND11; G em01_main, four new
  attacks (x15 26-29), atk09/10/17/22/20, flight curves 15/16 (em01.c + pc_data sizes), ef_move_sub. Not seen in the
  hunts: x15 26-29 (chosen by G's act tables in situations the god-mode hunts did not hit).
- **eft20** (monster dust/splash effects): i/m/t as G (kind 0x24 as 2, the nine kinds' landing shells, args 13/21 with
  G's fade keys imported by name, arg 29 scaled). Args 13/21 only come from Gravios motion 0x45A: not seen in a hunt.
- **em07** (Lao-Shan): pick ids by kind and quest rank (mission header 0x42). **set04**: G's i/m/trans (stages
  0x59-0x5B, which G's 88-entry stage tables do not have: dead in G; default branches differ).
- **eft05 for the other weapons**: compared by code, not pictures: G's callers pass the same args (0 blood, 1 white;
  arg 2/3 are Dual Swords only); G's Eft05_set skips single trails in demon mode; eft05_t's row-2 length scale is 1 for
  an unscaled hand joint. Nothing to port.
- **Coverage** (COV=1 G build, the full tools/test_all_quests_g.sh run with RT_COV, build/A/cov2/report.txt): 5579
  functions entered, 1010 flagged "G twin differs, no G marker" (many false positives: G code in #elif branches, PC
  shims, abs files). Ranked list of the real ones in RELEASE_G section 13.
- tools/mhg_fdiff.py: per-process temp file (agents ran it at the same time and read each other's bytes).

## The Xbox G crash at the Elder's quest list (21 Oct 2026, coordinator's top priority)
- **Owner**: on the real Xbox (G, fresh save) the Elder's level list (only 緊急クエスト selectable, which is right for a
  new save: check_questLevelSelect allows only the urgent row while the star level is -1; the urgent quest is key quest
  10001) -> "おぬしに頼めそうな依頼はこれぐらいじゃ" -> next -> crash.
- **Reproduced in xemu** (build/A/xurg: fresh disk, RT_ARGS=--quest 1 --play + RT_VILLAGE_START, the PC pad script):
  `Fatal System Error 0x1E (0xC0000005, 0x0023B7C0, 0, 0x54484749)`: in _xfmt (vsprintf) reading a format at
  0x54484749 ("IGHT" as bytes). A temporary rt_log in lb_set_questpage_info: Xbox `type 16385 attr 0x54484749`, PC
  `type 1`. **Cause**: lb_guild_g.c declared `int Lb_get_quest_type();` but it returns s8 (lb_t01.c); clang's callee
  returns the whole register (`mov %ecx,%eax; ret`, upper bytes from the row's flag word 0x4001), gcc's happened to be
  clean; lb_quest_attribute[16385] was text. **Fix**: the s8 declaration (lb_guild_g.c, and #ifndef __MWERKS__ in
  lb_x01.c / lb_v06.c for MH1). Checked: the new XBE's caller now has `movsbl %al,%eax` after the call. Not yet re-run in
  xemu (the owner's other session had xemu and little memory left).
- **Same hazard elsewhere**: tools/check_kr_returns.py lists every `int f();` of a function defined with a char/short
  return (907 lines, many of them old asm-era names): each is a possible Xbox-only bug. Not fixed in bulk.
- **tools/test_urgent_g.sh** (new, in full_check): NEW hunter in the village -> Elder -> urgent 10001 -> depart ->
  deliver -> clear -> back in the village at star level 0. It passes on the PC; it cannot see the Xbox bug itself (gcc).
- **Xbox log** (both games): mh1log.txt and logs\*.log were only fflush'ed into the kernel's file cache, which a crash
  never writes out (the owner's log ended long before the crash). xbox_log_commit (xbox_log.c) does fflush +
  NtFlushBuffersFile: every rt_log line after 0.5 s, every warning / error at once, and stderr once a second.
- **Follow-up (21 Oct, coordinator): the whole class fixed, both games.** tools/check_kr_returns.py --fix gave 906
  declarations their definition's return type (plain C types, so no header needed); in byte-matching files (config/
  c_files.txt, config/mhg/c_files.txt) and in every header the original line stays for MWCC under `#ifdef __MWERKS__`.
  The checker knows those MWCC-only branches. A file that used the function before its declaration got the declaration
  at the top. `--implicit CMDDIR` compiles every linked PC object's recorded command with gcc -fsyntax-only
  -Wimplicit-function-declaration: 60 calls of char/short functions without any declaration got one. Both checks are
  steps of tools/full_check.sh (kr_returns, kr_returns_g) and report 0. Not reported on purpose: a narrower or other
  char/short declaration of a function (the caller converts the same way on both compilers) and int vs pointer (same
  register and width on both 32-bit targets). int vs float was reported and fixed too (flSin declared int in the yn
  files; flps0009 declared f32 but returning int: an x87 stack pop of nothing).
  Checks: MH1 and G PC builds, MH1 and G Xbox builds, test_all_quests_g (98 + carts + dual), test_all_quests (38),
  test_urgent, test_urgent_g. **The PS2 byte rebuild (5 OK / 7 OK) was not run** (free memory stayed under 1 GB): left
  for the coordinator's check.
- **Rebuild break (21 Oct, fixed on main by agent D) and the tool made safe:** my --fix guarded only c_files-listed files
  and headers, but the MH1 rebuild (tools/build.py) compiles every file under src/ (not pc/, mhg/, ps2patch/): three
  lobby files got a `signed char SoftKeyboard_alive_check();` MWCC saw next to the header's int. --fix now guards every
  change. New `--mwcc mh1|mhg`: reads what MWCC sees (each compiled unit plus the headers it includes, with __MWERKS__
  and, for G, VERSION_MHG defined; #define renames skipped) and reports a function declared with two return types.
  Checked: it reports the lb_ac.c case when that line is put back, and 0 on main for both versions. full_check's
  kr_returns / kr_returns_g steps run it.

## Owner's Xbox report: saving, shop icons (21 Oct 2026)
- Save round trip reproduced as the owner did it on PC G (tools/test_save_g.sh): works; Xbox-only cause still unknown.
  mc_xbox.c now logs saves/loads/failures. G's CONTINUE path: power-on autoloads the card ("loaded" right after the
  reset), the GAME MENU cursor starts on CONTINUE, CardConld03 says "no character data" (msg 26) when every slot's byte
  0 is 0 (save_data_sub(0, 15) == 7).
- Weapon shop icons: G Lb_put_job (0x593100) uses e + 0x19 and remaps 5/6 -> 1/4 in the small sheet; lb_ag01.c.
- G texture slots: menu slots are MH1 + 0x27 (0x118 -> 0x13F, 0x119 -> 0x140, 0x11A -> 0x141, 0x157 -> 0x180); the PC
  keeps MH1's (GSLOT in include/g_alias.h).
- Save slot cut at 0x480 (udmisc_nm.c Load/Save_userdata): fixed (UD_SLOT); the select screen's TIME reads slot +0xC1C
  (hours'minutes", so under a minute shows 0'00"). test_save_g.sh has a new-process CONTINUE leg.
- G sharpness level (Pl_slash_lv_ck): six levels, 12-byte rows (G 0x4CCAB0); the HUD showed a full blade as red.
- Xbox boot steps logged + committed at once (rt_log_commit); AC97 init: PI/MC bus masters reset, GIE off, GPI/resume
  status cleared, ISR clears them too. Waiting for the owner's log of a hung second start.
- Open: quest supply box cursor (camp box at stage 21 spot kind 3, 9500,9500: RT_PL_WARP's angle did not turn the
  hunter, box not opened in a scripted run yet); reward screen stuck on its timer (not looked at).

## G English, own translation (23 Oct 2026)
- **Numbers:** G official English (MH1 US) 7087 of 13263 (53.4%, unchanged by this round apart from the refresh);
  **ours 1043** (+15 fallbacks); left 5133 = 4703 waiting for Freedom (same Japanese in Portable) + 430 not in
  Portable: 275 keyboard kana tables, 31 G-only weapon/armour names left for Freedom's naming, ~124 non-text
  (character tables, scan false hits such as `ｑ\n` runs and strings starting mid-character, debug prints).
  The ~1090 "our own translation" strings of docs/english_g.md are all done.
- **Design** (docs/english_g.md "Our own translation"): text/mhg_en_own.tsv keyed by id + crc32 of the Japanese (no
  game text in the repo), flags t/p/d/m/n, applied last by text_map.py `run_own` (official always wins; Freedom's
  replacements are listed). tools/text_own.py adds batches and checks the fit. .gitignore: `text/*` +
  `!text/mhg_en_own.tsv`.
- **Lessons:** (1) A G line-split message often reuses MH1 lines whose US English is kept to MH1's message slot
  (gk_slot); the report counts those strings as English, but in G's new messages they show Japanese. Own lines for
  such ids become the string's fallback while the slot keeps the US line. The report's "official English" therefore
  over-counts slightly for line-split dialogs. (2) cp932 × is 0x81 0x7E: a byte-level '~' check must skip two-byte
  characters. (3) An empty table value means "keep the Japanese": own lines must never be empty. (4) My scratch
  context tool (pointer runs that use a string, with each slot's Japanese and English) was the only way to translate
  line-split messages sensibly; worth turning into a tool if someone continues (kept out of the repo for now: it
  prints Japanese).
- **Display:** rt_font.c draws full-width digits/letters inside English lines as half-width (item shop price). Hidden
  before/after tours: build/A/eng/before, build/A/eng/after (tools/g_eng_tour.py, new `G_ENG_OUT`).
- **Not checked on screen:** the town talk and online-hub lines (no G online client in the tour).

## Freedom pairing (24 Oct 2026)
- **Problem:** the first run with the owner's Freedom (ULUS-10084) gave 201 G strings: Freedom's DATA.BIN has 8 more
  entries, its 240 quest files are reordered (matched by quest id at 0x58), the record files are shifted by 7, and
  the "same number of text runs" rule could never hold for record / talk files (English is longer, the binary around
  it yields other runs). Entry 3 (the big pack) was dropped whole for one extra string at the end of block 18.
- **Fix** (tools/text_map.py): psp_entry_map (difflib over the entries' first 8 bytes + skeleton check: quest id,
  first record id, talk section ids; skeleton search nearby when they differ), new psp_slots kinds quest / records /
  talk, pairing per unit (block / section / file), _align_runs for overlays and BOOT.BIN (unique identical runs as
  anchors, LIS), the MH1-US anchor check (_anchor_shift: words shared with US English in place vs one slot off),
  freedom_ok (~B codes equal, colour codes only G's colours, stray ~, numbers). Descriptions take Freedom's whole text
  also over MH1 lines matched line by line (192). Sentences with two Freedom wordings take the more used (26).
- **Numbers:** G official English 7087 -> 11474 (86.5%); ours 1043 -> 1071 (31 G-only names added, 3 replaced by
  Freedom); left 718 (415 kana tables). Per category (official): items 964/972, weapons 1693/1804, armour 3545/3615,
  monsters 71/74, quests 1183/1199, NPC talk 2038/2498, menus 1587/1803, system 387/877.
- **Still not given (319 "in Portable"):** 140 kana; Freedom keeps entry 3 blocks 19/20 (111 save / system messages)
  and the code overlays' literals in Japanese; 54 short labels with two Freedom wordings; 13 refused by freedom_ok
  (PSP buttons, number checks); one Portable quest not in Freedom.
- **Names:** 48 own lines now use Freedom's names; 24 keep flag n (G-only names); 31 G-only weapon / armour names
  written in Freedom's style (X改 = X+; XG = X+ unless Freedom already has X+, then "X G" / "X G+").
- **Lesson:** an independent anchor (MH1 US English for the same Japanese) proves an alignment much better than
  "looks translated": in entry 3 the in-place agreement was 290/297 vs ~150 one slot off.
- **Not checked on screen:** the tour's screens show only MH1-era content (identical pictures before / after);
  Freedom's G-new lines were checked by widths (98% of description lines <= 31 columns).
