# PC viewer (first piece of the PC port)

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

`disc/mh1` must contain `AFS_DATA.AFS` and `SLPM_654.95`. The ELF and the
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
- Only running and turning do anything yet (see "Player and pad" below).
  The camera is the game's: d-pad left/right turn it, up/down zoom, L1
  resets it behind the hunter.

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
| `--play` | the pad (controller + keyboard) drives the hunter; follow camera |
| `--input SCRIPT` | scripted pad for tests, implies --play: `idle*10,up*50,left+cross*15` = ticks per step; names in src/pc/pad/pad.h |
| `--follow D,H,P` | `--play` camera: distance D behind, H above the hunter, pitch P (default 900,450,-0.3); the yaw is `--cam`'s |
| `--sw-trace` | print, per tick, the pad state the game's sw_set_sub gave player 0 and the player's position/angle |

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
| src/main/stage/trans_stage_nm.c | trans_stage: draws the area model and the set-model parts the stage places (see "Stage drawing") |
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
- rt_player.c is a host stand-in for the player's normal state (pl_normal
  0x141BA0 and its ~70 pl_mv### actions are not decompiled; agent F's
  area): it turns the hunter towards the left stick relative to the camera
  (0x800 per tick), plays run 3/103 while the stick is pushed and idle
  1/101 otherwise (4-tick cross-fade through frame_init), calls frame_move
  and then the game's wall and ground collision (see "Collision"). No
  actions: replace it with the decompiled pl_move / pl_normal when they
  exist.
- Verified 5 Oct 2026: `--input "idle*10,up*50,left*15" --sw-trace --time
  2.5` prints sw.ang 0x4000 / pow 127 for "up" and 0x8000 for "left", the
  hunter turns to the camera's forward direction and runs about 300 units
  (build/show/A/play_run.png shows it mid-stride, turned left).

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
| player states (walk, run, roll, weapon, items) | main 0x134000-0x15B000, 453 functions, 157 KB | ~10 % matched (pl0x.c, pl_normal*, pl_damage); the big weapon state machines are asm |
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
