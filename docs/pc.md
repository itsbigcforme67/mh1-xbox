# PC viewer (first piece of the PC port)

## Handover summary (agent A, 7 Oct 2026; read this first)

The PC build (`tools/build_pc.sh` -> `build/pc/mhview`, 32-bit x86; ARM via
`tools/build_arm.sh`) plays MH1 offline from power-on: logos, title, new
hunter / continue, memory card on host files, the village (Elder, shops,
forge, item box, house bed save), quests from the Elder, every monster kind,
items (gathering, fishing, bombs), quest clear / failure, reward, and the
star-level progression. Game logic is the decompiled C (matched files and
*_nm near-matches); src/pc/ holds the platform side and the glue.

Checks to run after changes (all headless, about a minute together):
- `tools/test_quest_loop.sh`: power-on -> new game -> quest 131 -> reward ->
  bed save -> CONTINUE (1550z).
- `tools/test_progression.sh`: star levels 1 -> 3 with marked clears, kept by
  the save.
- `tools/test_urgent.sh`: urgent quests 136 and 137 hunted for real; each
  clear opens the next star level.
- `tools/rebuild.sh`: the PS2 rebuild (all five OK) when game C was touched.

How the PC wires game C (where most bugs were): no-op stand-ins generated
for missing functions (build/pc/rt_gen.c, tools/gen_rt_auto.py) — grep them
first when a feature does nothing; per-file ABI adaptors for calls whose
PS2 argument registers differ from the C prototype (src/pc/rt/rt_abi.c, ABI=
lines in build_pc.sh; tools/argregs.py); fields the PS2 fills in trans()
that move-side code reads (world matrices at EMW/PLW+0x60 are rebuilt per
tick). Test aids are environment variables (RT_*), listed in the "Run"
section and the round sections below; RT_PL_GOTO, RT_PL_TARGET=kN and
RT_QCLEAR are the newest.

Known gaps: opening and attract movies (Sofdec, open question in
DECISIONS.md), the soft keyboard (typed-ASCII stand-in), reverb is an
approximation, online play, ARM frame rate measured only up to round 20
(25-28 fps at 960x720), nothing systematically compared with the PS2.

`build/pc/mhview` is a real-time viewer written in C99. It loads MH1 data
straight from the user's disc files at run time, with nothing extracted to
disk, and shows a stage (default 4, st04; `--stage N` for others) with the Rathian
(em01) and a hunter standing in it. Both play their motions in real time. Camera is free-fly.

## Build

The build is 32-bit (`gcc -m32`, see "Port runtime" below and
docs/DECISIONS.md). Needs gcc, the 32-bit runtime libraries (libc6:i386,
libsdl2-2.0-0:i386, libgl1:i386) and either gcc-multilib or the no-root
sysroot made by tools/setup_pc32.sh (downloads libc6-dev-i386 and
lib32gcc-13-dev into build/sysroot32). No other libraries.

    tools/setup_pc32.sh          # once, only without gcc-multilib
    tools/build_pc.sh            # -> build/pc/mhview

## Run

    build/pc/mhview disc/mh1

`disc/mh1` must contain `AFS_DATA.AFS` and `SLPM_654.95` (and `AFS00.AFS` /
`AFS01.AFS` for sound; without them the viewer runs silent). The ELF and the
game.bin overlay (an AFS_DATA entry, stored uncompressed) are read for the
hunter's part-to-bone table (ptmat_tbl, 0x3018F0) and the game data tables
the decompiled C uses (src/pc/rt/rt_data.c).

Controls (free camera, the default):
- WASD move, mouse look.
- Space / C move up / down; Shift moves faster.
- Esc quits.

Controls with `--play` (the pad drives the hunter, camera follows):
- An SDL game controller (Xbox layout: A cross, B circle, X square,
  Y triangle, LB/RB L1/R1, LT/RT L2/R2, Back select, Start start), and the
  keyboard: W/A/S/D left stick, arrow keys right stick (turns the camera),
  K cross, L circle, J square, I triangle, Q L1, E R1, Z L2, C R2, Enter
  start, Backspace select, T/F/G/H d-pad.
- The hunter is driven by the game's own player code (see "Player"
  below), with the PS2 controls: left stick move (push lightly to walk),
  right stick draw / attack (flick direction picks the attack), cross
  roll, R1 guard, circle sheathe (weapon out), square use item. The camera
  is the game's: d-pad left/right turn it, up/down zoom, L1 resets it
  behind the hunter. Note: the arrow keys are the right stick, so they
  attack as on the PS2.

Screenshot mode, for checking without looking at the window: it renders
offscreen in a hidden window, reads the back buffer and writes a PNG.

    build/pc/mhview disc/mh1 --shot build/show/pc_viewer.png --frames 2 --time 1.0
    build/pc/mhview disc/mh1 --shot out.png --size 640x480 --cam 11250,260,8150,0.72,-0.12 --time 2

| option | meaning |
|----|----|
| `--time S` | freeze the animation clock at S seconds |
| `--frames N` | frames to render before the screenshot |
| `--cam x,y,z,yaw,pitch` | camera position and angles (radians) |
| `--size WxH` | window size |
| `--stage N` | stage number (game_w.stage, 0-87, hex with 0x), default 4 |
| `--quest N` | load quest N's mission file (questName[N], 1-0xB1) and start the hunt as the game does: on the quest's start stage (base camp; quest 10: stage 21, the Rathian in her nest on stage 40), the supply box filled. `RT_QUEST_STAGE=1` starts on the stage of the quest's own monster instead (scripted fight tests); `--stage` overrides both |
| `--play` | the pad (controller + keyboard) drives the hunter; follow camera |
| `--input SCRIPT` | scripted pad for tests, implies --play: `idle*10,up*50,left+cross*15` = ticks per step; names in src/pc/pad/pad.h |
| `--follow D,H,P` | `--play` camera: distance D behind, H above the hunter, pitch P (default 900,450,-0.3); the yaw is `--cam`'s |
| `--audio-dump FILE.wav` | no audio device; mix 1/30 s per game tick into a 48 kHz stereo wav (for checking sound offscreen) |
| `--mute` | no sound at all |
| `--sw-trace` | print, per tick, the pad state the game's sw_set_sub gave player 0 and the player's position/angle |

Environment variables for the player: `RT_WEAPON=id` (Ken_data id, default
156, the first sword and shield; 1 = the first great sword), `RT_PL_TRACE=1`
(action, step, motions, frame, position per tick), `RT_PL_STANDIN=1` (the
old host stand-in), `RT_EM_POS=x,z` (put the Rathian there, for hit tests),
`RT_HIT_DM=1` (print damage the Rathian takes; `2` also lists live attack
shells and their hit volumes), `RT_SKIP_TYPE=n` (do not draw prims of
effects/sets of type n).
Monster: `RT_EM_TRACE=1` (per tick: enemy_mv step, action, motion, frame,
position, angle, hit points, mode 0 calm / 1 attack; at spawn the part
durabilities), `RT_EM_STANDIN=1` (the old host stand-in: root motion and
collision only, no AI). Test aids that change the game (scripted fights
only): `RT_PL_GOD=1` (hunter vital back to 100 each tick), `RT_PL_AIM=1`
(hunter faces monster 0 while standing), `RT_DMG_MUL=n` (damage to
monster 0 times n). `RT_EM_POS` also moves a `--quest` monster.

Verified 5 Oct 2026 with build/show/pc_viewer.png and
pc_viewer_close_0.5.png / _2.0.png:
- the stage is textured, with sky, waterfalls and ruins;
- the Rathian stands on the plateau, and the two times show different
  frames of its motion;
- the hunter stands on the stone path in its idle motion.

## Layout

| path | what |
|----|----|
| `src/pc/fmt/` | format readers, pure C, no graphics. Every reader takes a byte order (`FMT_LE` PS2, `FMT_BE` Wii MHG, which uses the same formats word-swapped). Files: `afs.c` AFS archive, `melt.c` Meltw + link files, `amo.c` models, `apx.c` textures, `ahi.c` skeletons, `aan.c` motion tables and curves, `hits.c` HITS collision (ground height). Formats are in docs/formats/. |
| `src/pc/gfx/gfx.h` | the graphics interface: textures, render states, clays |
| `src/pc/gfx/gfx_gl.c` | its OpenGL 1.x fixed-function implementation (SDL2 window) |
| `src/pc/fl/` | the port's "fl" layer: `fl_model` (AMO → clays, CPU skinning, VU1-style lighting), `fl_skel` (AHI + AAN motions → bone matrices), `flmat.h` (fl row-vector matrices) |
| `src/pc/audio/` | the audio interface: `audio.h`, `audio_mix.c` (portable mixer: 48 voices + 2 streams, 48 kHz stereo), `audio_sdl.c` (SDL2 device) |
| `src/pc/fmt/snd.c` | sound packs (SCEI HD/BD + TSBD, PS2 ADPCM) and ADX decoding (docs/formats/audio.md) |
| `src/pc/rt/` | the port runtime: what decompiled game C expects from the PS2 side (see below) |
| `src/pc/viewer.c` | the app: scene setup, hunter assembly (SetPartsTrans), camera, screenshot PNG writer |
| `tools/build_pc.sh` | build script; output in build/pc/ (gitignored) |

`src/pc/` is not part of the matching build and is not in c_files.txt.

## Port runtime (game C running natively)

The decompiled game C is compiled unchanged with the game's own include/
headers and linked into the viewer (list in tools/build_pc.sh, GAME=).
Running natively now:

| file | what it does |
|----|----|
| src/main/stage/stage_set.c | stage_set_set, the per-stage spawn list (matches the PS2 code). For st04 it spawns set00, Set13_set(0) and set14 |
| src/game/set/set00.c | light shafts: st04_1 clay 1, additive, scrolling, turned to the camera (rview_matY) at its two table positions |
| src/main/set/set13*.c (+ set13_nm.c) | sun glare: st04_1 clay 0 drawn towards sun_pos_tbl[4], faded out when the stage_sphr_tbl spheres hide the sun |
| src/game/set/set14_nm.c | UV-scrolled waterfalls (st04_1 clays 2 and 3 at 11060,0,1566) |
| src/game/set/set09.c | ambient creatures (butterflies etc.) on stages 5, 0x10, 0x21, 0x33... |
| src/game/set/set17.c | plant tiles on stages 1, 2, 3, 46 |
| set03/04/05_nm/07/08/10/11/15/16/18/19/20_nm/22.c, main set12.c | every other set object the spawn list can start (see each file's header) |
| src/main/stage/trans_stage.c | trans_stage: draws the area model and the set-model parts the stage places (see "Stage drawing") |
| all decompiled eft*/shell* (game and main), list EFT= in build_pc.sh | effects and shells: what set objects and stage_set_set spawn (Eft14_set2 camp fire on st21, Shell10_set barrels on stage 0x11, Shell22_set2, Eft17_set_ex, Eft13_set_pos ...) now run as the real C |
| src/main/hit/hit2.c, hit2c.c | sphere/capsule tests set13 uses |
| src/main/hit/shit*_nm.c, shit2.c, tri_nm.c, hitw_nm.c | the stage collision (f_sphr, agent D): load_stage_hit, GetGroundHit*, GetWaterHit, GetFloorSlide, HitWallPlayer -> GetWallHitBitPl/Em -> sphr_face_o3/o4 -> PushAdjust3, GetWallHitLine/GetEyeHitLine (see "Collision" below) |

The `_nm.c` files are near-matches on the PS2 side (logic believed
equivalent), so they run here too. For split files the whole-file `_nm.c`
is used when it holds every function; otherwise the matching pieces plus
the `_nm.c` (eft06, eft13, eft20, shell06, shell08). Files in WEAK=
(shell06_nm, eft20_nm) repeat some matching functions, so their symbols are
made weak (objcopy --weaken) and the matching copies win.

src/pc/rt/:
- `rt_game.c`: game_w, player_work, stage_work (timer counts up each tick),
  set_mdlw; the set object pool (pull/push_set_work, 64 entries of 0x80: a
  guess; pull_set_work(n) also gives n 512-byte heap blocks as sw->u.work,
  like 0x155290); prims and ordering tables ot0..ot4 (get_prim,
  release_prim, add_prim; drawn ot0 first, ot4 last, low priority first
  inside a table: a guess); ran_suu (same generator as 0x161230);
  rt_game_init (calls stage_set_set) / move / draw. Static asserts check
  the struct layouts.
- `rt_fl.c`: flSetRenderState (0x0D blend op, 0x19 texture matrix, 0x1A
  world, 0x5E blend factors, 0x60 alpha ref, 0x63 filter, 0x64 texture
  clamp, 0x67 fade, 0x6C z-write; 0x6D accepted and ignored; others print
  a one-time warning), flExecuteClay (handle -> gfx clay), and the clay
  attribute path, ported from the asm: clay_attr_set (0x121E20),
  clay_attr_reset, SetTrnslMode, SetOpeMode, SetFilterMode. Their tables
  (src_mode, dst_mode, ope_mode, filter_mode, aa_alpha_src, aa_alpha_ope,
  aa_filt, aa_addr) are read from the ELF. rt_clay_attr_word() packs the
  CLAY+0x88 word from an AMO part's 0xF0000 chunk like Attribute_from_amo,
  so set-model clays and the host's own draws use the game's blend modes.
- `rt_flmat.c`: fl matrix/vector/maths helpers (flmatRot*33, Mul33_2,
  ScaleFactor33, SetXYZ33, flvec*, flSqrt, flArcCos...) written from the
  VU0 asm, and rview_mat / rview_matY (rt_set_camera, as View_move builds
  them: rview_mat = camera world matrix, rview_matY = Ry(camera yaw + 90°)).
- `rt_main.c`: small main-program functions not decompiled yet, written
  natively from the asm: clr_flash, hit_cap_pk, Pl_stg_ck/Em_stg_ck,
  frame_check2, flvecApplyMat33_2. Stubs: hit_point_cyl, Create_FOV /
  flCheckMeshFOV (everything counts as visible; the GPU clips),
  reload_tex (textures stay resident), camera quake, monster sound.
- `rt_eft.c`: effects and shells. The effect list (eft_work, 128 x 0x40,
  free stack + linked list from eft_w_top: pull_eft_work/2, push_eft_work,
  move_eft, trans_eft/trans_eft_up), the shell list (64 x 0xD4,
  pull/push_shell_work, move_shell with the +0x7B hit-stop counter,
  trans_shell), the second prim pool (get_prim2), the senko/smoke/smell
  stacks (kept, not drawn), the effect models eft_mdlw[0..4] (ef_00,
  kage04-06, ef_01 from main's effect_model_data/EFT_TEX tables, loaded by
  the viewer) and the helpers the eft C calls: eft_vec/alpha/rgba_linear,
  make_mat_srt, eft_trans_sub(_col/_opa), Eft_rendope_set, shell_rate_add,
  vectors, GetGroundHit (host collision callback; GetWaterHit says "no
  water"). Joint queries (get_joint_pos/wmat) return the actor's position:
  no skeletons run as game C yet. Player/monster-only helpers (sound,
  vibration, attack data, skinned-model drawing flCalcTrans/flSetSkinTrans,
  shell08_trans) are stubs; RT_TRACE lists them.
  `RT_SPAWN="eft17:4,eft14:3,..."` spawns test effects at the hunter.
- `rt_overlay.c`: main C calls overlay functions by address
  (func_6229B0 = set14_set, func_54B8C0 = Eft14_set2, ...). Each is routed
  to the ported function in the definition's argument order.
- `rt_data.c` + `tables.txt`: Capcom data tables are declared empty and
  filled at start-up from the user's SLPM_654.95 / game.bin by address
  (nothing copied into the repo). Most are listed by name in
  src/pc/rt/tables.txt; tools/gen_rt_tables.py looks up their address and
  size in config/symbols/ and writes build/pc/rt_tables.c (run by
  build_pc.sh). `NAME work` lines are zeroed work areas (em_work,
  quest_w). Tables in .bss (past the file data of the ELF or overlay)
  start as zeros. **Pointers inside tables:** the ELF keeps its link
  relocations (.relmain, .relgame.bin); every R_MIPS_32 entry is a data
  word holding an address. After the copy, each such word in a host table
  is turned into a host pointer: into the host copy of a table if it points
  into one, else to the host symbol of that name (dlsym; the viewer is
  linked -rdynamic; the ELF's .symtab names the target), else to the same
  bytes in the loaded image. The images themselves are relocated the same
  way, so pointer chains (eft*_data keyframe lists, fade tables) work.
  Pointers to code that is not ported become NULL. `rt_ptr_at(va)` reads a
  relocated pointer (the viewer's ptmat_tbl). Unnamed data (D_3F2090 =
  rview_mat row 3, ...) is defined with --defsym in build_pc.sh.
- `rt_mem.c`: PS2 address lookup in the ELF and the overlay; relocations
  and symbol lookup for the above.

### Motion system (frame_init / frame_move)

The hunter is animated by the game's own motion code: src/main/frame/
f_frame_nm.c (main 0x125340-0x1267BC; 16 of its 18 functions match the PS2
bytes, see docs/agents/agent-A.md) runs unchanged. create_plcom_motion
turns every AAN in plcom_tbl.bin into a motion-set handle
(motion_set_handle_tbl, com_mot_han_ofs); frame_init picks the handle from
PLW.char0/char1, frame_move steps the layers, cross-fades, loops, and moves
the player by the root motion (pl_velocity_sub). Below it, src/pc/rt/
rt_motion.c implements the fl motion layer natively (read from the asm,
main 0x173A50-0x1746A0): a motion-set handle is an index into a host table
of parsed AANs; the two motion players at model work +0x44 / +0x54 are
RT_MPLAYs that record per group the set, the frame and the blend, with the
+0xD0 word flCalcTransVelocity is given pointing back at the player.
flCalcTransVelocity returns how far AAN bone 1 of group 0 (the root's
child node on the PS2) moves between two frames: the run loop plcom 3 moves
it 537 units forward over 78 frames. The host poses the hunter's skeleton
from the player with rt_motion_pose (fl_skel_pose_groups: per-group frame,
channel blend), keeping that bone's X/Z translation at its bind value
because the game moves the actor instead (`root_lock`; how the PS2 cancels
it at draw time is not traced yet [guess]).

The Rathian runs the same way: create_em_motion builds em01_tbl.bin's
handles (Em_max_parts_get, ported in rt_main.c from main 0x10B770: 3 part
groups for kind 1), em_work[0] (not in use, be_flag 0) holds its motion
layers, ids 1003/1203/1403 (slot 3 of banks 0/2/4). Verified: with
`RT_HOST_MOTION=1` (the viewer's old AAN player for both actors) the shot
build/show/A/em_host_view.png matches em_game_view.png except for a
one-frame phase difference.

`RT_MOTION_SCAN=1` lists every common motion with its length, loop and root
travel (how plcom 3 was found).

### Player and pad

- rt_pad.c is the PS2 pad driver step (ioRead_sub, main 0x11FAC0): it turns
  the host pad (fl pad bits + sticks, src/pc/pad/pad.h; SDL backend
  src/pc/pad/pad_sdl.c) into Psw[0] (buttons, triggers, stick direction
  bits, stick angle and power with the 45 dead zone, repeat), then runs the
  decompiled swset() (pad_get.c). pl_sw_set / sw_set_sub (pl_normal2.c)
  then fill player_work[0].sw as on the PS2. fl bit meanings were read
  from ps2pad_hard_to_soft_ds2 (0x306500) [inferred; the mapping to the
  game's bits is ioRead_sub's and is exact]. Stick angle: 0 = right,
  0x4000 = up.
- rt_player.c runs the game's player code (see "Player"); with
  RT_PL_STANDIN=1 the old host stand-in (turn and run only) is used.
- Verified 5 Oct 2026: `--input "idle*10,up*50,left*15" --sw-trace --time
  2.5` prints sw.ang 0x4000 / pow 127 for "up" and 0x8000 for "left", the
  hunter turns to the camera's forward direction and runs about 300 units
  (build/show/A/play_run.png shows it mid-stride, turned left).

### Player (game C)

Since 6 Oct 2026 the hunter is run by the decompiled player code (agent
F's src/main/pl, see docs/agents/agent-F.md); rt_player.c only sets it
up and calls it:
- Set-up (rt_player_game_init) does init_pl_work's offline-master part
  (main 0x1116E0): equipment type/id at PLW+0x35F/+0x360, +0x34C =
  Ken_data[id][0], job PLW+2 = Battle_type[+0x34C] (0 great sword, 1/5
  bowguns, 2 hammer, 3 lance, 4 sword and shield, from menu_stat_job_str),
  User_data +0x3CD/+0x3CE for Get_equip_value; then the game's pl_init(0):
  start position from stage_start_pos, idle motion. The weapon class's
  motion table w<job>_tbl.bin goes through create_pl_motion (ids >= 1000).
- Each tick: rt_pad_tick, then pl_move (pl48.c: pl_sw_set, pl_move_sub
  for 8 players, hit_timer_calc_shl). pl_move_sub (pl_nm.c near-match)
  runs timers, Pl_damage_sub (game.bin pl_damage), the action state
  machines (pl_normal / pl_attack / pl_damage ... through their jump
  tables), pl_turn_sub, the motion step pl_chr_sub (frame_init /
  frame_move at speed 2: the 30 Hz tick plays 60 fps motion data), the
  per-motion hook pl01_effect_move -> ef_move_sub (src/main/sound/
  f_sound_nm.c: footsteps, swing and voice sounds, dust), wall, floor
  and ground collision, World_calc.
- Then (viewer, per tick) sync_joints poses the host skeletons and gives
  the joint world matrices to the game C (part blocks PLW+0x110.. for
  parts_init's 32 slots = skeleton nodes, get_joint_pos, hit_data_expand),
  and rt_hit_check runs hit_check (hit_nm.c), the order of game_core
  (move, trans, hit_check).
- src/pc/rt/rt_pl.c: helpers not decompiled yet, written from the asm with
  their addresses (Pl_act_set and friends, flags, motion requests,
  stamina/vital/sharpness, rates, front_land_ck, item counts, attack data,
  Code_Make ...); network, items picked from the stage, quest and message
  functions are stubs. Get_Active_itemnum reads its player from a0 on the
  PS2 (its caller passes nothing): the host uses the master player.
- Weapon model: weapon_model_data / WEAPON_TEX[PLW+0x34C] (AFS entries,
  weNNN_amh / _tex), posed like weapon_trans (weapon3_nm.c): hand part
  0x12 / 0xE or sheathed part 9 (sword and shield) / 10 with the
  weapon_disp_tbl_r/l/b[job] offset and XYZ rotation; the shield bones
  (AHI group 1) on joint 0x11. Per-motion node scaling of great sword,
  lance, hammer and bowguns (weapon_dat_make) is not done.
- Hits: shell00 (game.bin) is the sword's attack shell; its body volumes
  are expanded on the player's joints, the Rathian's from em_body_tbl on
  hers. A hit fills the monster's damage fields, plays the hit sounds,
  starts the 2-tick hit stop (PLW+0x610: motion speed 0.2) and the hit
  marks (eft16, eft05 slash trail via the skinned ef_01 model). The
  Rathian's HP comes from em01_init and she reacts (see "Monster").
- x86 hazards: several matched files declare a callee with the float
  argument in another position than the definition (fine on the PS2,
  where floats use their own registers). build_pc.sh compiles those files
  with -DNAME=rtabi_NAME and src/pc/rt/rt_abi.c re-orders (frame_check,
  frame_check2/3, Eft06_set, Eft02_set6 from plf.h; hit_point_cbd in
  f_stage.c; pl_move_sub's four-argument GetGroundHitStatusAreaPl). They
  were found with an LTO build: copy tools/build_pc.sh, add -flto to
  CFLAGS/GAMEFLAGS and read the -Wlto-type-mismatch warnings whose
  declarations differ in where the f32 arguments are.
- Verified 6 Oct 2026 (scripted --input, RT_PL_TRACE, shots in
  build/show/A/pl/): run 0/1 with the run loop moving ~11 units a tick;
  roll 0/0x1C (cross); draw 0/4 (right stick) -> attack 1/0x30 -> combo
  1/0x37 (motions 1401/1402 of w04_tbl); weapon-out walk 0/3 (1004);
  guard 2/3 (R1); sword in hand and shield on the arm (ws_sheet.png),
  great sword overhead swing (gs_2.6.png); stages 1, 5, 0x10, 0x21
  (stages_play.png); sound trace: weapon draw (snd_weapon07 code 1), swing
  (code 4) with voice (snd_vo_m00 0x24), footsteps on the ground
  material; hit on the Rathian (hit_sheet2.png, RT_HIT_DM output).
  Nobody has compared any of it with the PS2 side by side.

### Monster (game C, enemy_mv)

The Rathian runs the game's own monster code since 6 Oct 2026: each tick
rt_monster_tick calls enemy_mv (src/main/em/f_em_nm.c, written from the
asm) -> em_move (sight, smell, hate, anger, status upkeep from em_core /
em_master / em_taisei) -> em01_main (agent B's em01_ai_nm.c) and the
command interpreter (agent D's em_cmd_nm.c, still on branch agent-D:
build_pc.sh exports it with that branch's headers to build/pc/ext) ->
frame_move -> HitWallPlayer / GetGroundHitStatusAreaEm. Her per-animation
sound/effect script is the game's (em_prog_tbl[1][3] = em01_effect_move).
- Set-up (rt_em.c): `--quest N` reads the mission file into a host
  mission_area and points quest_w.x64/x74/x78/x80/x94/x14E at its tables
  as Quest_init does; the quest's own monsters are the QEM list at
  Em_data_com_adrs_get(x78, 1) (0x3C bytes each: kind, variant, stage,
  hunger/thirst/sleep, angle, position). rt_monster_spawn does what
  Em_direct_set does (free em_work, fields from the QEM, enemy_mv step 0 =
  em_init; em01_init sets the hit points: 2500 for quest 10). Without
  `--quest` (free hunt, quest_w.no 0) a stand-in QEM at the viewer's spot is
  used; em01 then starts with a fly-in.
- Quests with the Rathian (kind 1) as their own monster: 10 (stage 40),
  12 (52), 21/24/27 (9), 44/45 (19), 60 (40); kind 11 (em01 code too) in
  6-9, 46 and 56-59. Read from the mission files with a throw-away script; stage
  numbers are QEM+7.
- x86 fixes needed on the way (none touch PS2-built code): game_w.pl_num
  = 1 (sight/hate loop over it); Em_Master_Change and NextStage_No_Set get
  em (a0 left over in the asm); GetGroundHitStatusAreaEm's fifth argument
  em+0x7E4 (t0, set in the delay slot); argument-order adaptors in
  rt_abi.c (em_frame_check, Eft13_set_em_scl, Eft15_set3, Eft02_set3);
  em_sleep_eff_set on the PC (rt_em.c, the PS2 one leaves the scale in
  f12); `-fno-aggressive-loop-optimizations` (Em_Dmg_Sys reads
  EMW.hagi[8] with i == 8 and gcc dropped the loop exit); absolute
  game_w/quest_w addresses in m2c-based files rewritten at build time
  (rt_ps2abs.h); weak-NULL data tables (eft20, fade_type25/26, shell06)
  added to tables.txt.
- make_mat_srt (host, rt_eft.c) had the rotation flags swapped (asm: 2 = Z,
  8 = X): eft16's blood streak (flag 2, only rot[2] set) was turned by an
  uninitialised X angle into the screen-wide red smear of the earlier hit
  shots. 71 effect call sites use flag 2. eft16_nm.c itself checks OK
  against the asm except eft16_m's spill order (its float immediates match).
- Verified 6 Oct 2026 (scripted --input, RT_EM_TRACE / RT_PL_TRACE /
  RT_HIT_DM, shots in build/show/A/em/): quest 10, stage 40: she turns
  and walks (1/3, 1/0) while calm; when the hunter comes close she roars
  (1/7; the hunter covers his ears, 2/25), mode 1, then charges and bites
  (3/4, 3/18, 3/6); a hit takes 49 of the hunter's 100 (knock-down 2/2);
  his sword hits take 3 per slash off her 2500; with RT_DMG_MUL=40 a 120
  hit on part 6 (durability 100) makes her flinch (4/2, motion 1063).
  2100-tick runs on stage 40 and stage 4 without crashes. Nobody has
  compared any of it with the PS2 side by side.
- Not done: carving points (Em_hagi_point_set returns -1), quest clear /
  monster death handling (Quest_enemy_die prints), map marker
  (WyvernAreaMove), event flags (no save data), Quest_restart after the
  hunter dies (stub), other quests' small monsters (QEM lists per stage at
  x74) and stage changes.

### Quest loop (game modes, f_quest, HUD, reward)

With `--quest N` the PC runs the game's own quest flow (6 Oct 2026, agent A):
- Start: rt_quest_load does Quest_init + Quest_start (select_w+0xAC = N)
  as game11 does: mission file questName[N] into mission_area, quest_w
  tables, stage, time limit, monster states (quest_em_init). The quest's
  monsters come from station_em_set / Quest_next_em_set -> Em_direct_set
  (src/main/quest/f_quest_nm.c; f_quest0_nm.c holds main
  0x2267F0-0x226C24, written from the asm). Kinds whose program
  (em_prog_tbl) is not ported get no model slot and are not spawned.
- Game modes: rt_flow.c runs game_w.mode each tick through the matched
  f_game.c / f_gameb.c: game2 (quest: game_core, Info_control,
  Quest_condition_judging, Game_clear_ck, stage change steps 2-6), game3
  (the "quest clear" wait), game5 (result_prog). game_core is the viewer's
  host tick (sim_tick: pad, player, camera, hit_check, monsters, HUD).
  Outside game2 the pad is still read every tick.
- Clear: Quest_enemy_die -> quest_condition_prog -> x00 1 (150-tick wait,
  then 60 s to carve, info banner) -> x00 2 -> game_w+0xD5 = 4 ->
  Game_clear_ck(2) -> game3 -> game5 -> result_prog -> remuneration ->
  reward screen (reward_mv/disp_reward, f_reward*): items picked with the
  pad go into the pouch.
- Carving: the hunter's own carve action (0/0x4A, circle at the carcass)
  -> Ext_pick_point_ck2 (Em_hagi_point_set made the point at death) ->
  ItemStockRequest (menu_nm.c) -> Pl_item_stack.
- Hunter faints: Pl_die_set -> death action 3/0 -> pl+0x738 -> game2
  steps 2-6: Quest_next_em_clr, st_model_load (the viewer's
  load_stage_models: area/set models, collision, camera file, sound),
  stage_set_set, Quest_next_em_set (the cart, em18), pl_init(1): the hunter
  is back at the base camp (stage 21 for quest 10).
- HUD ("pit", main f_menu): load_pit (textures), PitWork_init / Pit_init,
  Pit_mv each tick; trans_pit_0/1/2 draw the clock, vital/stamina bars,
  sharpness, map, item bar and its text through the game's own
  menu_disp_nm.c. The info banner is set01.c.
- 2D (src/pc/rt/rt_2d.c): flps0002/4/5/8/9/C screen prims read from the
  asm (layouts in the file header), PS2 frame 512 x 448 stretched over the
  window; textures by handle in mem_tex[] (flCreateTextureFromApx_mem on
  host APX decoding); gfx_draw_2d in the gfx interface. Screen layers in
  trans()'s order: ot5, font 0, ot6, font 1, ot7, font 2, ot8, font 4,
  ot2, font 3 (rt_game_draw_2d).
- Fonts (src/pc/rt/rt_font.c): AFS_DATA 0x6D2, 7808 glyphs of 20 x 20 at
  2 bits (MSB first) in JIS order; flfntPrintf's five stacks by z,
  FontPuts advance rules, Ascii2Sjis (the font's own half-width row
  0x85), palettes from flfntSetPalData; font_print / _ex / _sp (~C / ~A
  codes) / _double / _uf from the asm.
- Test aids (scripted runs only): `RT_QUEST_TRACE=1` (mode/step/clear
  state changes, the pouch when it changes, the reward list),
  `RT_FONT_TRACE=1` (each text drawn), `RT_TEX_TRACE=1`,
  `RT_TEX_DUMP=dir` (raw RGBA of every 2D texture), `RT_EM_HP=n` (monster
  0's hit points), `RT_PL_WARP_EM=tick` (put the hunter at monster 0 at
  that tick), `RT_PL_ITEMS="id:n,..."` (pouch; no save data),
  `RT_PL_GOD` also clears the stun gauge.
- Verified 6 Oct 2026 (quest 10, scripted --input, traces + shots in
  build/show/A/quest/): HUD with items (hud_items.png); kill with
  RT_EM_HP=30 + RT_DMG_MUL=40, carve three times -> Rathian Scale, Spike,
  Flame Sac into the pouch; clear banner (clear_banner.png); 60 s later
  game3 -> game5; reward menu (reward_menu.png) and item grid
  (reward_items.png, icons checked against the decoded icon sheet),
  circle takes a reward into the pouch; potion use 10 -> 9 (drink motion
  406); death -> carted to the base camp, stage 21 loaded
  (carted_to_camp.png); after the reward the money screen (result_prog
  steps 2-3: fee, reward, total, money counted into User_data,
  gold_result.png), then game mode 6, where the host starts the quest
  again (back_to_quest.png; the PS2 goes back to the village, which is not
  ported). Free play (`--play` without `--quest`) runs Quest_init's
  free-hunt tables and shows the HUD too (free_play_hud.png). Nobody
  compared any of it with the PS2 side by side.
- Not done: SpritePut and the sprite prims flps0D00/0F00/1300/1400/1600
  (game3's darkening quad; game3's text sits on the field picture), the
  cart's model, map markers (flvecrRotTransPers), item combining
  (Item_preparation*).

### Village, pause menu, small monsters, quest failure (agent A, 6 Oct 2026)
- Village (game mode 6, offline): rt_village.c does what Game_task does
  (all_reset: monsters and set objects cleared; Clear_lobby_ram) and then
  runs lobby.bin's Local_main every tick (src/lobby/f, src/lobby/lb and
  src/lobby/f/lb_village_nm.c). Local_main returns 1 when a quest was
  accepted and the hunter walked out through the gate; the host then
  starts that quest (select_w+0xAC) as `--quest` does. Kokoto = stage 87,
  the hunter's house = 86; the Village Elder (talk kind 71, npc01) is the
  quest counter; the gate is unique spot kind 6 at (10650, 15225), left
  with square. lobby.bin shares its vram with game.bin: its data and bss
  live in rt_lb_mem (one host block), its symbols are aliases into it
  (tools/gen_rt_auto.py), absolute addresses in C go through
  tools/pc_abs.py. Village motions: com_motion_load(1) (lbcom_tbl); NPC
  models npc00/npc01/em09/em32 (npc_create_model).
- Pause menu (start in the field): menu_nm.c / menu_disp_nm.c with
  ListSelect / PageSelect / Menu_select_mv (listsel_nm.c) and
  DispFrameMessageA (dispframe_nm.c): item list, discard, quest info,
  retire (D5 7 -> game3 -> game5 -> village).
- Small monsters: every em_work slot is ticked (rt_monster_tick) and
  drawn with its own model, texture and motion table (em%02d files by
  kind, loaded from em_create_model). Velociprey (em16) AI is built; its
  game.bin tables are in src/pc/rt/tables.txt.
- Quest failure: the third faint sets D5 5 -> game3 -> game5 shows the
  "quest failed" score -> circle -> village.
- Verified 6 Oct 2026 (scripted runs, shots in build/show/A/): quest 131
  accepted from the Elder and started at the base camp; pause menu pages
  and discard; quest 10 stage 40 Velocipreys attack (small/v_6.png);
  `RT_PL_DIE=60,1400,2700 --quest 10 --input "idle*6600,circle*3,idle*2000"
  --time 250` -> carted twice, failure score (faint/f_200.png), village
  (faint/village.png). Not compared with the PS2.
- Test aids: `RT_VILLAGE_START=1` (start in the village), 
  `RT_VILLAGE_SKIP_INTRO=1` (first-visit event marked seen),
  `RT_VILLAGE_TRACE=1` (NPCs, spots, talk states, camera), `RT_NO_VILLAGE=1`
  (mode 6 restarts the quest as before), `RT_CAM_DEBUG=1`,
  `RT_PL_DIE="t1,t2,..."` (the hunter faints at those player ticks).

### Windowed = headless, village menu, sprites, area exits (agent A, 7 Oct 2026)
- Scripted runs are tick-for-tick the same windowed and with `--shot`:
  the host syncs joint matrices after every tick (not only per drawn
  frame). Check with `RT_TICK_TRACE=1` (per tick: flow mode, stage,
  hunter position/angle, a sum of monster positions) and diff the two
  runs' "T" lines. Checked on: the village accept script, village/quest
  random roams (5 seeds, 2-4 minutes each), village -> quest 131 -> camp
  -> area 1.
- Fixed crashes: windowed segfault at the village -> quest switch
  (rt_game_init now empties the prim queues: a frame drew eft13 prims of
  cleared effects); start in the village (lbmw NULL); eft06_m (the _nm C
  tested the stepped pointer instead of the table, asm s8 vs s6);
  monsters whose program entry [3] is not ported (kind 29 in area 1)
  are no longer spawned.
- Village start menu: Pit_init's lobby branch -> Lb_Menu_Init; Pit_mv_lb
  -> Lb_menu_move_Core, trans_pit_1_lb -> DispLobbyMenu / Disp_lb_menu
  (src/lobby/b/lb_menu_nm.c, from the asm). main's func_5B3D70.. forward
  to the lobby C (rt_menu.c). Quest status, items (discard), combine
  list, data, status and equipment screens checked on screenshots.
  pit_help_str_tbl[4]/[5] point into lobby.bin (mapped in rt_data.c).
  ItemCopy_Pl2Ud / Ud2Pl as udmisc02.c; without save data the user's
  pouch starts as the hunter's (rt_player_game_init).
- Sprites: SpritePut (src/main/sprite/spriteput_nm.c, from the asm) +
  trans_sprite before ot5 + flps0D00: game3's darkening quad fades the
  screen (brightness 46 -> 31 -> 20 over the fade). flps0F00/1300/1400/
  1600 (textured / 3D sprites) are still stubs.
- Area exits: the host calls stage_mv_ck every tick (move_stage's exit
  check; the rest of stage_m, stage sounds and item sparkles, is not run)
  -> pl+0x738 -> game2 loads the next area (camp 21 -> area 1 = 39).
- flSetRenderState 0x0F-0x11 fog values (inert: nothing sets 0x12),
  0x5F Z test (7 = off [guess]); 0x01/0x0E/0x15 known no-ops.
- The cart (em18) is drawn with its Felynes when the hunter is carted
  (cart13.png); nothing more was needed.
- Not checked: comparison with the PS2; textured sprite kinds; other
  areas' exits beyond camp -> area 1; save data.

### Quest start, supply box, playability pass (agent A, 7 Oct 2026)
- `--quest N` starts like the game: Quest_start leaves game_w.stage at the
  quest's start stage (base camp, 21 for quest 10) and the hunter starts
  there (pl_init's start position); the quest's monsters wait on their own
  stages (the Rathian on 40). `RT_QUEST_STAGE=1` keeps the old start on the
  monster's stage for scripted fights. A quest restarted after the reward
  (RT_NO_VILLAGE) also starts at the camp.
- Supply box: rt_quest_load runs Start_item_init after Quest_start (as
  game11 does): the mission file's start items go into game_w+0x128 (32
  slots of {item, count}). At the camp the box is unique spot kind 3
  (stage 21: 9500,40,9500 r 200); circle there -> pl_mv087 ->
  Pl_box_select (menu_nm.c) -> box_get -> Pl_item_stack. Checked with
  RT_PL_WARP=20,9598,9639: box screen with the quest's 22 items, all taken
  into the pouch (stack limits apply: 3 of 4 whetstone stacks fit), item
  bar shows them. Walking there works too (closest reachable point ~170
  from the spot centre, on its +x/+z corner: the crates in front of the box
  keep the hunter ~250 away on the other sides; not compared with the PS2).
- New character: the game's own defaults are select.bin's user_data_copy
  (src/select/edit00.c): User_data cleared, sword and shield 0x9C, money 0,
  pouch empty. So a fresh hunter's pouch is empty on the PS2 too; the items
  come from the supply box. The PC does not run user_data_copy (only the
  weapon matches it); `RT_PL_ITEMS` stays a test aid.
- Monster sounds: rt_snd_stage loads the snd_emNN packs of game_w+0x28's
  kinds (game12's snd_joint_load list) and em_create_model adds a new
  kind's pack (rt_snd_em_add): Velocipreys were silent before.
- Game C is compiled with `-ftrivial-auto-var-init=zero`: matching C can
  read a local the original never wrote on that path (the PS2 then reads a
  stale stack slot). pl_dm001 (guard knock-back, pl33.c) adds sp30[2] to the
  hunter's position after frame 94 without setting it: on the PC the hunter
  was thrown to z = 1e21 and the screen went blank seconds into a guarded
  Rathian attack. gcc's -Wmaybe-uninitialized does not see this case (the
  array goes to flvecApplyMat33 by pointer).
- Test aids: `RT_SPOT_TRACE=1` (each stage's unique spots at stage set-up),
  `RT_PL_WARP=tick,x,z` (move the hunter at that tick); RT_EM_TRACE also
  prints layer 0's motion state (stat/end/blend ticks).
- Checked (scripted, headless and windowed): five 200 s random-input fights
  on stage 40 (no crash, positions sane), a 60 s windowed run at ~60 fps
  (x86), guard blocks (2/3 -> 2/9 / 2/10 with chip damage), SnS chain
  (draw 0/4 -> 1/48 -> 1/55 -> 1/56), Rathian charge (atk 18), single and
  triple fireball (atk 4 / 23, explosions drawn), village -> quest 131
  start with its supply box. Not compared with the PS2.

### Collision (stage HITS, game C)

The game's own collision C (agent D's f_sphr near-matches, list HIT= in
build_pc.sh) runs on the PC; the host reader fmt_hits_ground_y is no
longer used by the viewer.
- Loading: rt_load_stage_hit(stage) runs the game's load_stage_hit
  (shit1_nm.c): load_file_mdl (rt_hit.c) asks the host for the AFS entry
  of stage_hit_data_w / _f[stage] (Meltw-decompressed) and copies it into
  a 4 MB host area (stage_hit_area_w / _f); WallHitInit / GroundHitInit
  then turn the file offsets into pointers (fine in the 32-bit build).
- rt_hit.c also has the small main helpers that are not decompiled,
  written from the asm: NormalClipF3 / NormalClipCheckF3 /
  PointHitCheckF3 (2D point-in-triangle with the original's quirks: one-ulp
  products count as equal, the orientation test truncates to int),
  UnitNormalVectorCCW, NvecFloatAdjust, cpRotMatrixYXZ2, flConvertRtoS,
  Stage_data_get (quest_w+0x80 = St_data, as the default quest setup at
  0x226BD0 sets it; stage_work+0x48 = Stage_data_get(stage) as stage_w_init).
- Player (rt_player.c): pl_move_sub's order (main 0x14C500): old position
  to +0x5A0, move, HitWallPlayer(pl, 0) (one sphere push00: y 60, r 48),
  GetFloorSlide(pl, v, 1), GetGroundHitStatusAreaPl -> +0x5AC; y snaps to
  it when below or less than 30 above, else a host fall (the PS2 starts
  the fall action Pl_act_set(pl, 0, 9)).
- Monster (rt_hit.c rt_monster_collide, from em_move 0x10BF30): old
  position, frame_move (root motion), HitWallPlayer (spheres
  em_hit_push_tbl[kind]: the Rathian, kind 1, has one sphere of radius 500
  at y 160), GetGroundHitStatusAreaEm, y = ground. rt_monster_place puts
  em_work[0] on the stage; the viewer draws the Rathian where the game has
  it (RT_EM_FIXED=1 keeps the old fixed placement).
- Fixes found on the way: PointToPoint is d = a - b (the host had b - a,
  which also affected effect code that uses it); table pointers into PS2
  .bss (wall_tbl_add -> stNN_wall_tbl) now point at zeroed host memory
  (rt_bss_shadow) instead of NULL.
- Verified 5 Oct 2026 (scripted --input, `--sw-trace`, shots in
  build/show/A/hit/): st04 "right" from the start: the hunter runs into the
  invisible wall at the cliff edge (polygon 10919,7513 - 11252,7689) and
  slides along it 48 units (the sphere radius) away instead of dropping to
  y -487 as before (wall_right_top.png); "left": walks up the stone path,
  y 7 -> 306, feet on the ground (st04_slope.png); stage 1 "up": stops at
  the river bank (z 8651, st01_wall.png); stage 5 / 0x21 runs stop or slide
  at walls. Rathian: on stage 0x21 it walks its 1003 loop along its facing
  (34 degrees, matches the angle) on the ground (em_st21_walk.png); on st04
  its 500-radius sphere is pushed out of the camp walls and it stops at the
  cliff wall; on stage 16 it stops at a wall after ~250 units.
- `RT_HIT_TRACE=1` prints the wall polygons of the start cell and, per
  tick, the player's wall sweep (old/new/pushed position, contacts).
  `--follow D,H,P` sets the play camera for such shots.
- Not done: water (GetWaterHit runs but nothing reacts), the fall action,
  the player's pl_wall_mat use (wall-facing actions), monster states 2/4.

### Camera (game C)

With `--play` the view comes from the game's own camera: CameraMove
(src/main/cam/camm.c) and the five camera slots (cam_nm.c / camd.c,
agent D; cam_sub_std, cam_sub_stg are near-matches) run every tick after
the player; cam2view writes eye / target / roll / fov into lpView, and the
viewer builds its look-at camera from that (roll ignored; the game's angle
of view is used as the vertical fov [guess]). `--follow D,H,P` or
`RT_HOST_CAM=1` keep the old host follow camera.
- Stage camera files: LoadCameraData (rt_cam.c, from 0x11F1E0) loads
  camera_data_tbl[stage] (26 stages have one, st04 included) into
  cam_data_area and SetCameraData fixes its pointers; without a file
  default_area_data builds one follow area from stage_camera_data_tbl.
- Camera areas: src/main/cam/camarea_nm.c (main 0x222E20-0x223B50, the
  g_SetAreaData file) written from the asm for this: default_area_data,
  StageCamInit, SetAreaData, Get_cam_grid_XZ, CameraAreaCheck,
  CamAreaAttribChk, Area_XZ_Check, GetPanTarget, GetRailTarget,
  GetRailCamPos, GetNearSection, get_near_point_sub, GetNearPoint. Not
  built for the PS2; check.py: CameraAreaCheck, GetPanTarget,
  GetRailTarget and nlCalcPoint already match, GetRailCamPos 1/33,
  default_area_data 7/117, SetAreaData 12/71 off, the rest further.
- Controls as on the PS2 (read from cam_sub_std): d-pad left/right turn
  the camera, d-pad up/down zoom (4 levels), L1 puts it behind the hunter.
  When the eye-to-target line crosses a wall (GetWallHitLine) the camera
  goes back behind the hunter every tick, so it cannot be turned there
  (the st04 start spot has a wall right behind the camera).
- Verified 5 Oct 2026 (build/show/A/cam/): st04 idle (gc_idle.png, behind
  the hunter, waterfalls ahead); run left, camera follows round the camp
  (gc_run_left.png); after moving off the wall, d-pad left 40 ticks turns
  the camera to the hunter's front (gc_turn_dleft.png); d-pad up zooms in
  (gc_zoom_dup.png); stage 20 (indoor, fov 1.15 from its camera file) and
  stage 5 (jungle) follow without clipping into walls (gc_st20.png,
  gc_st05.png). `RT_CAM_TRACE=1` prints per tick the slot, area, zoom,
  buttons, wanted/current yaw, wall flag and the view.
- Not ported: k_HitEmCamera finds no monster body parts (hit_data_expand
  stub), Game_clear_ck (quest end camera), cockpit chat. Rail / fixed
  stage cameras (area types 1-3) run the near-match cam_sub_stg but were
  not seen in the tested spots.
- x86 hazard found: a callee returning float that a caller declares void
  (k_HitWallCamera in cam_nm.c) leaves a value on the x87 stack; after
  eight calls the FPU stack overflows. Such declarations must match.

### Sound

docs/formats/audio.md has the formats and the game's sound calls;
`tools/snd_dump.py` decodes packs and ADX to .wav (build/audio/).
- src/pc/rt/rt_snd.c: se_req / se_req2 / Pl_se_req2 / Em_se_req2 /
  Pl_se_req2_com and flSndRequest / flSndChange written from the asm
  (distance volume curves, screen pan, random volume/pitch, chained
  codes); host code then does the IOP driver's part (TSBD program + id,
  note -> split -> sample -> VAG, decoded once, played on a mixer voice).
  str_* (ADX streams from AFS00 into mixer streams, fades and str_volume's
  dB table) are host versions of main 0x100910-0x100E18.
- rt_snd_stage loads the ports as game12 does (common00/01, the map pack,
  player 0's weapon + voice, em_blank + snd_em01) and starts the stage
  stream like stage_bgm_set: st04 plays the camp theme S_M6CAMP (its
  stage_bgm_etc_tbl entry, first entry into the stage), other stages
  Snd_bgm_tbl[stage] (ambience such as M6_MORI1, M2_KAZE1).
  `RT_SND_AMBIENT=1` skips the first-entry theme; `RT_SND_MAP=n` forces the
  map pack.
- stage_se_move (from f_stage_nm.c) runs every tick: river / waterfall
  loops on stages 1, 3, 0x1A, 0x30, 0x34, 0x36, 0x3E.
- Footsteps: the host player stand-in calls rt_snd_player_motion before
  frame_move: the run loop's entries of the player's per-motion sound
  list (ef_move_sub, main 0x24A790: ashi_sd_req kind 2 at frames 8, 30,
  54), with the ground material the game's collision wrote to pl+0x70D.
  The Rathian's walk (1003) plays em01's list entries (code 1 at frames 52,
  116) from rt_snd_monster_motion, at the monster's position (no joints).
- `RT_SND_TRACE=1` prints each pack loaded, stream started and sound
  played (port, code, program, note, VAG, volume, pan).
- Verified 5 Oct 2026 (offscreen, `--audio-dump`, nobody listened):
  st04 idle 4 s: the dump equals the Python ADX decode of S_M6CAMP
  sample for sample from 1 s to 4 s (after the 0.5 s fade-in); `--input
  "idle*10,left*110"`: footsteps at ticks 24, 46, 70, 95, 119 (22-25
  ticks apart = the 8/30/54 frames of the 78-frame run loop), programs 3
  then 1 as the hunter crosses from one ground material to another;
  stage 3: waterfall (code 0x22, 3.7 s loop) and river (0x21) start on
  tick 4 and keep playing; stage 0x21: Rathian steps every 64 ticks. The
  SDL device path was checked with a test program (a 1 s tone is consumed
  in real time).
- Not done: attack / weapon / voice sounds (the player has no actions
  yet), other monsters' and motions' lists, joint positions for sound
  sources, reverb, ADSR envelopes, the quest BGM changes (fight,
  clear), menus.

### Stage drawing (trans_stage)

The PS2 draws the area model in trans_stage (main 0x15CD90), not as one
static mesh: world = Trans(stage_work.pos) * Rxyz(stage_work.rot) (both
zero, stage_w_init), but part 0 (sky) and many per-stage parts are
special. Examples [read from the asm]: st04 part 2 (a ring of clouds
around the origin) is drawn at 13200,0,5190 and slowly turned; st05 draws
part 2 twice at set05_pos_tbl1 (10000,0,11500 / 10000,0,14500) and part 3
at 10000,0,8500 - these parts are modelled around the origin, which is why
st05 had "holes" before; skies of stages 0x19/0x3A/0x40-0x42 turn around
a centre point; water and lava parts get UV scrolls from stage_work.timer
or game_w+0x1E (a u16 counter that counts up every tick). After the area
model it draws set-model parts at set??_pos_tbl rows (x, y, z, angle Y).
Set-model parts are no longer drawn by the host at all: on the PS2 only
trans_stage and the set objects draw them. Verified with shots of all 88
stages (build/show/A/stages/sheet0.png, sheet1.png): st05 is a closed
jungle floor (ts_st05_low.png), st04 now shows the camp tent and ruin
wall, which the host's old "background parts first, z-write off" pass had
hidden.

The host's hunter is player_work[0] (rt_set_player: in use, on the stage,
at its position), so set code that follows the master player works.
`RT_TRACE=1` prints each set object as it starts (type, arg) and the prims
queued in the first drawn frame.

Game logic ticks at 30 per second; the host draws its own models (each
part with its clay_attr_set state), then `rt_game_draw()` walks the
ordering tables. Set-model parts the game C has drawn are skipped by the
host's generic draw so they are not drawn twice.

Blend modes (fl state 0x5E, read from flPS2SendRenderState_ALPHA and the
tables above): factor codes 0 zero, 1 one, 2 src alpha, 3 1-src alpha,
4 dst alpha, 5 1-dst alpha; codes 6-9 have no GS form and are ignored, as
on the PS2. Default (clay_attr_reset) is src alpha / 1-src alpha. Blend op
0x400 is subtract (GS A-B with A=Cs); 0x800 is taken as reverse subtract
[guess]. Filter: 0x10000 = point, 0 = bilinear (TEX1 MMAG/MMIN). Clamp:
0 = repeat, else GS REGION_CLAMP (drawn as clamp-to-edge).

Verified 5 Oct 2026 (offscreen shots, `--size 640x480 --frames 3`):
- `--time 1.0 --cam 11060,700,5000,0,-0.1`: identical to the earlier
  hand-spawned set14 shot (build/show/rt_set14_1.0.png), so the spawn list
  reproduces it; between T = 1.0 and 1.5 only waterfall/mist pixels move.
- `--cam 12300,500,8200,4.71,-0.1`: set00's light shafts through the ruin
  arch (build/show/rt_set00_1.0.png); T = 1.0 vs 2.0 differ inside the
  shafts (texture scroll).
- `--cam 11000,500,8000,2.23,0.3`: the sun glare (build/show/rt_set13_sun.png).
  Whether its size and brightness equal the PS2's is not checked.

Porting hazard found: main C sometimes declares a callee with its
arguments in a different order than the callee's own definition (floats
and ints use separate registers on the PS2, so it still matched). On x86
the order matters: set13c.c's hit_cap_sphr_m declaration was fixed for this
(still matches). Check prototypes against the definition when adding C.

Adding more game C: put the file in GAME in tools/build_pc.sh, the data
tables it needs in src/pc/rt/tables.txt (the link errors name them), route
func_XXXXXX calls in rt_overlay.c, and add whatever it calls into rt_*.c.
For split files use the whole-file `_nm.c` (e.g. set05_nm.c), not the
matching pieces.

## Plan: a player and a monster on the runtime with real input

Written 5 Oct 2026 (agent A), not started. Coverage numbers are matched
bytes from config/c_files.txt by address range; near-match `_nm.c` files
add more logic that already runs on the PC.

What the game's own loop does each tick (read from the asm): pad read
(pad_get.c, matched) -> player_mv (pl01.c, matched) -> pl_move (0x14C3E0:
pl_sw_set, pl_move_sub, hit_timer_calc_shl) -> per-weapon state machine
through pl_prog_tbl (0x2F1590) -> motion update (frame_init / frame_move,
0x125920 / 0x125F10) -> enemy_mv (0x10CB20) -> em_move (0x10BF30) ->
per-monster em_prog_tbl (0x2E8330) programs in game.bin -> CameraMove
(0x21F590) -> draw: trans_stage (ported), player_trans (0x1678C0),
enemy_trans (0x168B10), prims (ported), effects/shells (ported).

| piece | where | state |
|----|----|----|
| pad -> sw buffers | main pad_get.c, pl_normal2.c (sw_set_sub) | matched; runs on the PC with the host pad backend (rt_pad.c, src/pc/pad) |
| player loop entry | pl01.c player_mv / pl_init | matched |
| player states (walk, run, roll, weapon, items) | main 0x134950-0x14D1C8 (f_pl) + helpers to 0x155000 | f_pl ~260 functions matched + pl_nm.c (agent F); all of it runs on the PC, the helpers from rt_pl.c (see "Player") |
| motion system | main f_frame 0x125340-0x1267BC (18 functions) + fl motion layer 0x173A50-0x1746A0 | f_frame: 16/18 match, all 18 run on the PC (f_frame_nm.c); fl layer native in rt_motion.c |
| player/monster drawing | player_trans, enemy_trans, 45 functions | ~3 %; the viewer's hunter_pose / fl_model_pose do the same job natively |
| collision | GetGroundHit, wall hits (main 0x111000-0x125000) | f_sphr all in C (agent D, near-matches); runs on the PC for the hunter and the Rathian (see "Collision") |
| monster common (em_core, em_master, em_taisei) | game 0x533980-0x53A000 | ~65 % matched + near-matches |
| Rathian/other monster AI | game em01.. (363 functions, 150 KB) | ~13 % matched; em01.c (Rathian action setters) partly |
| camera | main f_cam, f_cam_223B50 (agent D), g_SetAreaData (camarea_nm.c) | runs on the PC in --play (see "Camera") |

Done (agent A, 5 Oct 2026): steps 1 and 2 below, and a host stand-in for
step 3 (rt_player.c) so the hunter runs and turns with the pad.

Suggested order (each step ends in a screenshot or a short input replay):
1. Host input: map an SDL controller to the PS2 pad bits and fill the
   buffers pad_get.c reads; record/replay pad logs for offscreen tests.
2. Motion bridge: implement frame_init/frame_move/frame_check natively on
   top of fl_skel (same motion ids and frame counters in PLW/EMW), so game
   C that sets char0/char1 animates the viewer's models.
3. Player locomotion first: decompile only the "normal" state family
   (to_normal, walk/run/turn, roll) of f_pl plus pl_move_sub, with
   GetGroundHit on the host collision. Weapons later, one at a time
   (sword and shield first: smallest table).
4. Camera: build cam_t.c into the PC port (CameraMove behind the player)
   instead of the free-fly camera.
5. Monster: em_core/em_master already run; add the Rathian's (em01)
   program table and its action setters, its motion bank, and the
   joint queries (get_joint_pos/wmat) from fl_skel so effects attach.
6. Hits and damage last (pl_damage is matched; attack data tables are
   imported by rt_data already).
No big rewrite is needed: the runtime pattern (decompiled C + rt_* stand-ins
+ imported tables) scales; the work is decompiling the player state and
motion code. Static recompilation of the remaining asm (DECISIONS "Open")
would be the shortcut if steps 3 and 5 turn out too slow.

## Design notes, for the port

- **Small, fixed-function gfx interface.** The original Xbox GPU (NV2A)
  is DirectX 8 class, so gfx.h uses only these:
  - vertex arrays (position, RGBA8 colour, ST), triangle lists, one texture;
  - alpha test/blend, z test/write, fog;
  - three matrices.

  gfx_gl.c uses only GL 1.1-era calls. A D3D8/nxdk backend should be a
  same-size file.
- **Lighting and skinning on the CPU**, as VU1 did them on the PS2. The
  per-vertex lighting is the Vu1Code_0001_0002 model: ambient + 3
  directional lights, each max(0, n·L) × colour, clamped, times the vertex
  colour. Stage parts are pre-lit by their vertex colours. A clay therefore
  only needs pre-lit colours.
- **PS2 names:**
  - A *clay* is one AMO part (flCreateClayHandle / flExecuteClay →
    gfx_create_clay / gfx_execute_clay).
  - `gfx_set_render_state` takes flSetRenderState's numbers where they are
    known: 4 texture, 0xF-0x12 fog, 0x17 view, 0x1A world, 0x60 alpha
    reference, 0x67 fade colour, 0x6C z-write. Port-only states start at
    0x100.

  The game's own calls can later be routed here once the GS register
  meaning of the remaining states is traced.
- **fl matrix layout:** fl matrices are row-vector (`v' = v * M`, translation
  in m[12..14]). This is the same memory layout OpenGL's column-major
  functions take, so they load unchanged.
- **Motion clock:** motions run at 30 frames per second. Motion ids decode
  as in frame_init: bank = (id % 1000) / 100, slot = id % 100.
  - The hunter plays plcom ids 1 (legs, char0) and 101 (upper body, char1).
  - The Rathian plays slot 3 in banks 0/2/4 (body, head, tail).
- **Placement:** each actor is posed at frame 0; its lowest vertex gives
  the offset from the game position (on the ground, GetGroundHit) to the
  model origin.

## Known gaps

- **Render states:** per-part blend, filter and clamp come from the
  0xF0000 chunk (clay_attr_set). Cull, UV-scroll flag, fog and lighting
  type from the same chunk (states 0x00, 0x62, 0x12, 0x01, baked into the
  clay on the PS2) are not applied. Alpha test is > 0x40 for host draws;
  the stage uses the game's own state 0x60 values (0x80 / 0).
- **Rathian:** the tail tip (AHI tree 1) is not attached, so it lies on the
  ground. No blending between motions.
- **Hunter:** no weapon. Hair and cloth bones (ptmat ≥ 64) keep their bind
  offset.
- **Runtime:** fade colour (state 0x67) is 0xAARRGGBB with alpha 0xFF =
  1.0 (eft05 packs r << 16, eft_trans_sub sends 255 * a). No players/monsters run as game C yet, so player_work is
  zero (set13 uses the master player's position on some stages).
- **Scene:** `--stage N` loads any stage (files from main's per-stage
  tables); em01 and one armour set are fixed. On stages other than 4 the
  hunter stands at stage_start_pos[stage] (main 0x2F2620), the camera 2500
  behind it (on a few room stages, e.g. 20, the camera is then inside a
  wall: use --cam). Stage 0x11 (st11 files) has barrels (Shell10) at
  1400..4100 where the area model has no geometry: probably an unused
  stage [guess].

### ARM (Armbian RK3518 box, 6 Oct 2026)

The same port runs as a 32-bit ARM (armhf) program on a 64-bit ARM Linux box, without
root: `tools/build_arm.sh` cross-builds with Debian's gcc-14-arm-linux-gnueabihf and an armhf
sysroot unpacked in ~/mh1arm, and `tools/run_arm.sh` starts it through the sysroot's
loader (Mesa's lima driver from the sysroot). Setup of ~/mh1arm: a user-level apt config
with `APT::Architectures { arm64; armhf; }` and its own lists/status dirs, `apt-get update`,
`apt-get download` of the armhf closure of libsdl2-2.0-0, libsdl2-dev, libgl1, libglx-mesa0,
libgl1-mesa-dri, libc6-dev (apt-cache depends --recurse) into sysroot/, and of
gcc-14-arm-linux-gnueabihf, cpp-14-..., binutils-arm-linux-gnueabihf, libc6(-dev)-armhf-cross,
libgcc-14-dev-armhf-cross, linux-libc-dev-armhf-cross (+ bases) into cross/, all unpacked
with dpkg-deb -x; the cross libc.so linker script is edited to point at cross/. ARM-specific
flags: -fsigned-char (PS2 char is signed), -fpermissive (gcc 14). `RT_FPS=1` prints drawn
frames per second.
- Measured 6 Oct 2026 (H96 Max, RK3518, Mali-450 GL 2.1, quest 10 at the cave, no fight):
  game logic at full speed (30 ticks/s), about 27 fps drawn at 960x720 and 48 fps at 640x480;
  a --shot screenshot looks the same as on x86. Not tested: long play, fights, the village.
- Re-measured 6 Oct 2026 evening (box idle; current build with -ftrivial-auto-var-init=zero):
  base camp 25-26 fps at 960x720 and 48 fps at 640x480, cave 27-28 fps at 960x720. No
  regression; an earlier 18-19 fps reading was another copy of the game left running on the box.

### Power-on, new game / continue, memory card, village features (agent A, 8 Oct 2026)
`tools/play.sh` (no argument) now starts from power-on: `mhview --boot`.
- Boot (src/pc/rt/rt_boot.c): the game's own task scheduler (tsk_nm.c) runs
  select.bin's Init_task (card check, options auto-load CardAtld), Demo_task
  (rating screen, middleware and Capcom logos, title), main's Select_task
  (omake_nm.c: NEW GAME / CONTINUE / GALLERY / OPTIONS, then the village /
  town choice), Edit_task (character creation) or Cont_task (load a hunter),
  plus Fade_task / Card_task. When a task starts Game_task the host takes
  over: game mode 6 = the village (Game_task's offline path). "Go to town"
  (network) falls back to the village.
- select.bin data: rt_sel_mem (like rt_lb_mem), symbols from
  config/symbols/select.txt aliased by tools/gen_rt_auto.py, pointers from
  .relselect.bin. main calls select functions by address: D_533BE0 /
  D_5367F0 / D_5375F0 (defsyms), func_534650 (user_data_copy) and
  func_533A00 (Init_task) in rt_overlay.c.
- The tasks draw while they run (flps0008, font_draw, trans()); one tick's
  gfx calls are recorded (src/pc/gfx/gfx_rec.c, hooks in the backend) and
  every frame replays the last tick, so frames and 30 Hz ticks stay
  independent. trans() (rt_boot.c) only draws during the boot.
- Fades are real now (fade_nm.c + Fade_task, also outside the boot via
  rt_sys_tick; host quest starts call fade_set(2) as game13 does).
  `RT_NO_FADE=1` hides them. all_reset is a host version (trans list, fade,
  sounds, fonts). The opening movie (Sofdec) is not played: its wait ends
  at once. The online patch check after a load (PatchLoadinDNAS) is done
  at once.
- Name entry: the soft keyboard (sk_nm.c, kana/kanji) is not ported; the
  stand-in in rt_menu.c takes typed ASCII (stored full width via han2zen),
  Enter or the pad's start finishes, empty = "HUNTER". `RT_NAME=x` for
  scripted runs.
- Memory card (src/pc/rt/rt_mc.c): libmc (sceMc*) on a host directory,
  `$MH1_SAVE_DIR` or `~/.local/share/mh1pc/memcard0`; port 1 has no card.
  The game's mclow/mcact/mccomb C runs unchanged on it, so the save is the
  PS2's own BISLPM-65495MH directory (data file 0x11450 bytes, icon.sys,
  icon00.ico). `RT_MC_TRACE=1` prints the commands.
- The hunter's look: armor_create_model runs Pl_model_id_set (written from
  the asm, main 0x123F60) and the bare-part rule of 0x124310; the viewer
  reloads m_/f_<part><n> models when the look changes (sex, face, hair,
  skin colour from the face, armour). Hair colour (PLW+0x5FC) is not applied.
- Village: item box (lobby f/lb_ib.c whole file; main's draw call 0x60CE50
  routed), shops/forge/armour pieces from agent B's b/ and b/nm files
  (LOBBY2 in build_pc.sh, linked weak). D_610370 (NPC body volumes for the
  camera) is lobby.bin's table (the zeroed placeholder crashed the camera).
- Test aids: `RT_BOOT_TRACE=1` (task slots), `RT_LB_WARP="tick,x,z[,ang];..."`
  (village warp), `RT_SHOP_TRACE=1`, `RT_VILLAGE_TRACE` lists each stage's
  spots (house door kind 12 at 11225,14350; in the house bed kind 14 at
  2230,745, item box kind 15 at 1950,1160; spots need square).
- Checked (scripted --input, shots in build/show/A/boot/): power-on ->
  logos -> title -> NEW GAME -> name/sex/face/hair -> save (file written) ->
  village with the first-visit event; restart -> auto-load message ->
  CONTINUE -> character select shows the saved hunter -> village; female
  hunter with face 4 in the village; bed save in the house writes the card
  file; item box store works. Not compared with the PS2.

### First quest loop, shops, matched lobby code (agent A, 6 Oct 2026)
- The Elder's first quest (131, "deliver 2 raw meat") from power-on to the
  next CONTINUE: Aptonoth (em12_nm.c, kind 12) and em29 (a breakable target)
  run on the PC; their tables that config/symbols lacks are imported by
  address (`NAME 0xADDR 0xSIZE` lines in src/pc/rt/tables.txt). Carving
  (pl_mv071 arg 3) gives raw meat; the camp's delivery box is unique spot
  kind 21 (10350,40,10640, circle -> Share_item_conv): "all items delivered",
  quest clear, 20 s, reward screen, money screen (+50z, counted up 1z at a
  time then the rest), village.
- Reward screen: ListSelect(&cur, keys, 2) (the count 2 is a2 left over in
  the asm, 0x292DB8); "end receiving" works.
- Village re-entry reloads lobby.bin's data and zeroes its .bss
  (rt_lb_reload = Load_overlay(3)); before, client_work said "village motions
  loaded" while the quest had replaced them and the hunter walked on the spot.
- Shops: item shop buy (-20z, herb to the pouch) and sell (+1z) checked; owned
  counts printed (font_print_ex count in t0); forge weapon list opens (crash
  fixed: lb_process_drawHelp read 16-bit list fields as s32).
- Spot hints ("square: enter house"): Lb_put_unique_act_hint (lb_ah.c, taken
  with PICK) and main's hint_tbl[0] mapped to lobby 0x64F1F0.
- Matched lobby code: tools/pc_lobby_matched.txt (56 files whose functions
  the PC took from *_nm copies) and LOBBY3 (7 that were gen_rt_auto
  stand-ins, e.g. cnWrap_SoundRequest: the village menu sounds) are linked;
  BMATCH weakens the other copies. PICK="file:sym" links single functions of
  a whole-file C.
- Hair colour: player_trans (0x167C38) writes PLW+0x5FC into the head part's
  first clay's first material; the PC multiplies that material's vertices
  (fl_model tint). Not compared with the PS2.
- Test aids: `RT_SHOTS=t1,t2,...` (with --shot X.png also X_<tick>.png),
  `RT_PL_WARP="t,x,z;t,x,z"`, `RT_PL_WARP_EM="t1,t2-t3"` (next to the
  target's carve point or body), `RT_PL_TARGET="tick:slot,..."` (which
  monster AIM / WARP_EM / DMG_MUL use), RT_SPOT_TRACE lists exits too,
  RT_LB_WARP counts village ticks over all visits; RT_QUEST_TRACE prints the
  quest's condition program and every Gold_add. tools/mk_input.py builds
  --input scripts from absolute ticks; tools/test_quest_loop.sh is the loop
  check (tools/pc_scripts/).
- Not done: opening movie (Sofdec decoding is not cheap: left skipped), the
  character screen's 3D hunter, forge list icons / page title (garbage),
  greeting window under the item shop's buy list, colour streaks over a
  CLEAR!! quest card. Nothing here compared with the PS2.

### Village glitches, character screen hunter, monster breadth (agent A, round 20, 6 Oct 2026)
Fixes (all PC side; PS2 rebuild all five OK):
- Forge list: the yellow page title is main's my_job_str, whose pointers go
  into lobby.bin. rt_import_lobby now finds every main data word whose
  ELF relocation symbol lies in the lobby.bin section (72 words: my_job_str,
  shop tags, menu help, armour shop tables, plaza menus ...) instead of
  three hand-mapped tables. Icons: matched Lb_put_job / Lb_put_icon
  (lb_ag01/02) and Lb_put_itemIcon / Lb_put_materialItem (were stand-ins)
  linked; 30 more matched shop/forge/dialog files in
  tools/pc_lobby_matched.txt (the forge list now has 2 pages of early
  weapons instead of 13 pages of everything). lb_process_drawHelp near-
  match: missing arguments added (argregs.py).
- Item shop greeting window and CLEAR!! card streaks: not seen any more
  after the above (shots of the buy list and the Elder's five ★1 cards, one
  marked CLEAR!!). Each card has a green smudge top left; whether the PS2
  card has it was not compared.
- Character creation / continue screens: player_trans called from the
  screens' prims now records a host draw (gfx_rec_call) of player_work[no]
  with the game's view (lpView). Continue: the save's look; creation: bare
  parts of the chosen sex/face/hair (the PS2 uses editpl_*_amh.bin, the
  same parts in one file — the picture was not compared).
- Monsters: em20 (Kut-Ku, Gypceros), em17 (Gravios, Basarios), em27
  (Velocidrome, Gendrome, Iodrome), em19 (Vespoid, Hornetaur), em04
  (Mosswine, Bullfango), em09 (Felyne, Melynx), em08 (Cephadrome,
  Cephalos), em21 (Plesioth), em14 (Diablos, Monoblos), em15 (Khezu), em03
  (Kelbi) linked (build_pc.sh EM, near-match copies weak via WEAK_EM), their
  game.bin tables in tables.txt. Lessons:
  - game.bin data an overlay C file names but tables.txt lacks becomes a
    *function* stand-in in rt_gen.c, read as data (em20 crashed on its fly
    height table). Check after adding files: weak `int NAME()` stand-ins
    whose symbol has no type:func.
  - em_prog_tbl entries point at file statics whose C carries the address
    suffix; map_ptr now tries NAME_ADDR (Mosswine/Melynx were "not ported").
  - Model / texture / motion files per kind come from main's tables
    0x2EC7A0 / 0x2EEE20 / 0x2EC830 (dromes use em16/em13/em30 models and
    em16 motions; Genprey had no motions before).
  - Monster slot 0 was always drawn with the host's em01 object; all
    monsters shared one joint-matrix buffer (rt_actor_joints keeps the
    pointer), so hit checks used the last monster's skeleton.
  - Per-file ABI adaptors (rt_abi.c) for em_frame_check(2), Eft13_set_em_scl,
    Eft15_set3, Eft02_set3; a0-left-over calls fixed in the drafts.
  - Quest event demos (evdemo.c) were NOPs: first-encounter monsters (Kut-Ku
    148, Cephadrome 154, Monoblos 171) wait for game_w+0x21F and never woke.
- Test aids: `RT_CAM_EM=slot,dist,height,yaw` (free camera on a monster),
  `RT_PROF=1` (host ms per game tick and per drawn frame), the monster trace
  shows act/sub/step.

Monster state (scripted runs from `--quest N` with RT_QUEST_STAGE=1, the
hunter warped next to the monster and slashing with RT_DMG_MUL, GOD mode;
"clear" = monster killed with RT_EM_HP/RT_DMG_MUL test aids, quest clear
(D5 3), carving checked through the pouch). Nothing compared with the PS2.

| kind | monster | code | state |
|---|---|---|---|
| 1 | Rathian | em01 | as before (quest 10, 170) |
| 11 | Rathalos | em01 | runs: sleeps in its nest (138), flies, attacks, flinches; kill not tested in a hunt quest |
| 6 | Yian Kut-Ku | em20 | runs (144, 148, 150): attacks, flies, flinches, flees to another area when weak; killed -> quest clear, 3 carves |
| 20 | Gypceros | em20 | runs (159): attacks, takes damage |
| 22 | Basarios | em17 | runs (173): rock disguise, attacks; killed -> clear, carve |
| 17 | Gravios | em17 | runs (172): attacks |
| 27/28/31 | Velocidrome / Gendrome / Iodrome | em27 | run (137, 156, 160): attack, flinch, die; 137 killed -> clear, carved twice (round 21) |
| 8/34 | Cephadrome / Cephalos | em08 | wake after the intro demo (154), swim in sand, attack; a sound bomb drives it out of the sand (round 21) |
| 14/26 | Diablos / Monoblos | em14 | run (174, 171): burrow, attack; little damage taken in the test |
| 15 | Khezu | em15 | runs (175): attacks |
| 21 | Plesioth | em21 | runs (165): swims; a sound bomb from the shore (11800,9300) while it is surfaced at ~10340,8920 makes it leap and fall back (act 4/17), then it swims on (round 22; without the bomb it does not) |
| 19/24, 4/5/32, 9/23, 3, 13/16/30, 12, 29 | small monsters | em19/em04/em09/em03/em16/em12/em29 | spawn and run without crashes in all village quests |
| 2 | Fatalis | em02 | runs (103-106): attacks (killed the god-mode-less hunter in 15 s); killed -> clear; its three pick points carved twice (round 21) |
| 7 | Lao-Shan Lung | em07 | runs (101, 102, 107): walks 14 -> 30 -> 28 -> 11 -> 12 (stage 12 at tick ~44000 in quest 101); its hit points stop at 1000 outside stage 12; killed on stage 12 -> quest clear -> reward (round 22) |
| 10 | trader NPC (red hair, backpack) | em10 | spawns on stages 5, 16, 41 (Quest_next_em_set adds kind 10 there); circle within 300 talks (hints, random gifts); trading checked on stage 41: a herb-class item (71) traded twice for item 77 with yes/no (round 22) |
| 33 | Kirin | em33 | in no quest on the disc (start positions only for quest 0); runs in free play with RT_EM_KIND=33 |

All quests 1-177 start on their monster's stage and run 450 ticks
(village 131-177: 1800-tick fights) without a crash.

Frame rate (x86, this machine, RT_PROF=1): game logic 0.22-0.33 ms per tick
in big-monster fights (Rathian quest 0.23, Kut-Ku 0.33, Basarios 0.32): cheap.
The host's per-frame work is the CPU skinning of every visible model
(fl_model_pose, per monster and per hunter part) plus the GL calls; the
monster count on a stage is what grows it. The character screen poses and
skins its hunter once per drawn frame (replay), not per tick. Not measured on
the ARM box: run with `RT_PROF=1 RT_FPS=1`.

### Last monsters, items, intro demos, gathering and fishing (agent A, round 21, 6 Oct 2026)
All PC side (src/pc, tools/build_pc.sh, tables.txt); no PS2-built file and
no include/ header changed.
- Monsters: em02 (Fatalis), em07 (Lao-Shan Lung), em10 (the trader) and em33
  (Kirin) linked; every monster kind now has its code on the PC (table
  above). PC versions of main's RedDragonEscapeCamera / F_DragonEscapeCamera
  (0x225E90/0x225EA0, from the asm) and Em_se_req2_com. The scan of all
  quests 1-177 (start stage and monster stage, RT_QEM_DUMP) found kind 33 in
  none of them.
- Items: flash bombs. push_senko now runs Em_Senko_Ck as the PS2 does, and
  move_senko / move_smoke (0x16A670 / 0x16A4A0) count their entries down
  each tick (they were no-ops: flashes and smoke never went away). Checked:
  Genprey that face the flash take their damage reaction; the Rathian in the
  test was looking away and was not blinded. Sound bomb (item 33, shell03
  arg 9 -> Shell09 type 13) drives Cephadrome out of the sand (quest 154).
- Intro demos (first sight of a monster): the camera now also ticks on the
  first two ticks of a stage, so a demo requested on tick 0 plays instead of
  ending at once. Quest 154: HUD hidden, the fin pass, the leap, a close-up.
  For ~60 ticks mid-demo the ground is a flat grey plane (camera at sand
  level); not compared with the PS2.
- Forge greeting window: not reproduced on x86. At the weapon-workshop NPC
  (lobby x68 14, at 9960,12120) the greeting closes 1-5 ticks after "next";
  one earlier run showed it for one frame when the sub-menu opened. Since
  the window is only drawn in shop steps 1 and 3 (Lb_shop_talk), a longer
  linger on the ARM box would come from drawn frames lagging ticks, not from
  the game logic. Not changed; PS2 behaviour not checked.
- Single-player content (quests 131/154, --stage, scripted):
  - herbs (circle at a pick point: 3 herbs, item 71), mining (pickaxe 131
    at a kind-3 point: ore 109, pickaxe broke), bug catching (net 134 at a
    kind-4 point: item 91): work. RT_SPOT_TRACE lists the points.
  - fishing: works now. func_5589F0 (Fish_set) was a no-op stand-in, so
    fishing spots had no fish. Stage 54 (desert): bait 122, cast, bite after
    ~370 ticks, circle -> fish 94, one bait used.
  - Also wired: Bdora_hp_ck (Lao-Shan half-HP quest condition) and
    Em09_item_sub (Melynx's stolen item) instead of stand-ins that returned 0.
  - Farm, Poogie and a training school: none in MH1's offline village (no
    such code or NPC found). lobby.bin has a pig NPC (npcPig*, lbnpc*.c),
    not in the village's NPC list; probably the online town [guess].
- Test aids: `RT_QEM_DUMP=1` (each stage's monster list of a quest),
  `RT_EM_KIND=n` (free play with monster kind n and its own model),
  `RT_PL_WARP="t,x,z,ANG"` (optional facing, hex), RT_PL_WARP_EM goes to a dead
  monster's own pick points, RT_PL_TRACE prints the fishing bite timer.
- Not done: Lao-Shan kill on its last stage, trading with the em10 trader,
  sound bomb next to a swimming Plesioth, frame rate on the ARM box (no new
  per-frame host work except the fish effects on fishing stages).

### Progression, trader, demo camera, Lao-Shan kill, music (agent A, round 22)
All PC side; no include/ or PS2-built file changed.
- Star levels: the game's own code (Lb_make_quest_tbl_local, lb_get_quest_level,
  get_flag_quest; main tables quest_local_tbl / flag_quest_tbl_local /
  key_quest_tbl) already runs. How MH1 offline works, read from the tables:
  1 star = 131-135 (0x83-0x87); clearing all five offers the urgent quest 136
  (0x88, "first monster hunt"); clearing 136 opens 2 stars. 2 stars need 138
  and 142 (0x8A, 0x8E) for the urgent 137 (Velocidrome); 3 stars 0x90/0x93/0x94
  for 154 (0x9A); 4 stars 0x96/0x9C/0x9E for 139 (0x8B); 5 stars
  0xA2/0xA6/0xA7/0x8C for 171 (0xAB), which opens the hidden sixth list.
  The Elder shows the urgent quest as a single card; afterwards the level list
  (cleared levels marked CLEAR!!, locked ones grey, "????" last).
- `tools/test_progression.sh` (~10 s, after test_quest_loop.sh): RT_QCLEAR
  marks quests cleared (hex list, "84-87,8a"), the bed save writes them,
  CONTINUE must show the level and the urgent quest (trace lines
  "rt_village: level N, cleared: ..." and "quest list (key XX)" with
  RT_QUEST_TRACE). Walked: 1 star -> urgent 136 -> 2 stars (kept by the save)
  -> urgent 137 -> 3 stars (kept). Screens: build/show/prog/. The real
  clear path (f_reward's Quest_clear_bit_set) is the one test_quest_loop.sh
  checks with quest 131; the urgent quests 136/137 were not played.
- Trader (em10): talk = circle within 300 units (Sansai_talk_ck -> pl_mv091),
  messages page with circle, yes/no with d-pad left/right. Trade tables per map
  (map2/4/6_trade_sp/_nm) work as written; checked one trade path only.
- Intro demo grey ground fixed: demo cuts placed relative to the monster read
  EMW+0x60 (its world matrix), which only enemy_mk in trans() wrote; the PC
  now builds it (and PLW+0x60 as player_modify does) every tick. Quest 154's
  middle cuts now follow the fin through the sand.
- Flash bomb facing: Em_Senko_Ck / senko_ck work from the monster's head
  joint direction and its search fov (Rathian 0x1555 = 30 degrees each side).
  A flash landing in front of her head blinds her (act 4 + the eye damage
  path); one landing behind her head or 31 degrees off does not. Item 27 is
  the flash bomb.
- Lao-Shan: `RT_PL_GOTO="tick,stage"` walks the hunter through the area exits
  (breadth-first over the STG_MV lists). Quest 101: hunter to stage 12, the
  monster arrives at tick ~44000, killed there (RT_EM_HP=1300 to save time)
  -> clear -> reward. The death dust crashed: Eft10_set needed an ABI adaptor
  for em07/em08 (rtabi_Eft10_set).
- Music: bgm_server and the game's stage_bgm_set (src/main/sound/bgm_nm.c)
  are linked: monster-found and fight music (S_FOUND1 -> S_FIGHT2), quest
  clear (S_CLEAR1), faint (S_DEATH1), ADX one-shots (adx_se_set). Still no
  reverb (flSndSetRev is a no-op). Weapon swings, hunter voices, monster
  calls, ambience and village music were already there.
- Not done: Plesioth sound bomb beyond the one case above; whether the PS2
  Plesioth reacts the same; opening movie (see DECISIONS.md); ARM frame rate.

### Urgent quests for real, Plesioth, reverb (agent A, round 23, 7 Oct 2026)
- `tools/test_urgent.sh`: 136 (three Velociprey, area 40) and 137
  (Velocidrome, area 34) accepted at the Elder, hunted, rewarded, saved; a
  real clear moves 1 -> 2 stars and 2 -> 3 stars (CONTINUE checks the save).
  The required non-urgent quests are marked cleared with RT_QCLEAR (setup
  only). New aid: `RT_PL_TARGET=kN` = the nearest living monster of kind N
  on the hunter's stage (for WARP_EM / AIM / DMG_MUL).
- Plesioth (quest 165, stage 54 cave lake): swims deep (y -1990) and near the
  surface (-660), spits at a hunter on the shore (act 3/4), leaps ashore
  (2/14 -> 0/4), walks and attacks on land (1/x, 3/2), goes back (2/16).
  Hits land while it is ashore (starter sword: 1 damage per hit; the Rathian
  takes 1-5, so plausible). It cannot be reached while submerged, as the
  hunter cannot swim. Kaeru_ck (frog-bait check that lets a hooked Plesioth
  be pulled out) and FishWyvernCameraRequest were no-op stand-ins: now the
  game's (PICK_MAIN in build_pc.sh links one function of a main file).
  Frog fishing itself was not reproduced (which bait item is the frog was
  not found; items 124/125 cast but nothing bit in 3000 ticks).
- Reverb: cheap. The game sets it per stage with flSndSetRev(core, type 4,
  depth) from Snd_rev_set_tbl (caves/nests deeper). audio_mix.c now has a
  small Schroeder reverb (4 combs + 2 allpasses per channel, ~12
  multiply-adds per sample) on the sound-effect voices; music streams stay
  dry. Not the SPU2's reverb program; which voices each SPU2 core carries
  was not traced. `RT_NO_REVERB=1` turns it off. Checked offscreen only
  (quest 10 nest: depth 10240 -> wet 0.19); nobody listened.
- Small fix: `--quest` printed a garbage monster kind for quests without a
  big monster.
