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
