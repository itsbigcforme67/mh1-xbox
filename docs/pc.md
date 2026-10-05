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

Controls:
- WASD move, mouse look.
- Space / C move up / down; Shift moves faster.
- Esc quits.

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
| set03/04/05_nm/07/08/10/11/15/16/18/19/20_nm/22.c, main set12.c | every other set object the spawn list can start (see each file's header). Effects/shells they spawn are stubs |
| src/main/hit/hit2.c, hit2c.c | sphere/capsule tests set13 uses |

The `_nm.c` files are near-matches on the PS2 side (logic believed
equivalent), so they run here too.

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
  reload_tex (textures stay resident), camera quake, monster sound,
  Shell22_set2 / Eft17_set_ex / Eft13_set_pos.
- `rt_overlay.c`: main C calls overlay functions by address
  (func_6229B0 = set14_set, ...). These names are routed to the ported
  function, or to a stub that prints "not ported yet" once.
- `rt_data.c` + `tables.txt`: Capcom data tables are declared empty and
  filled at start-up from the user's SLPM_654.95 / game.bin by address
  (nothing copied into the repo). Most are listed by name in
  src/pc/rt/tables.txt; tools/gen_rt_tables.py looks up their address and
  size in config/symbols/ and writes build/pc/rt_tables.c (run by
  build_pc.sh). `NAME work` lines are zeroed work areas (em_work,
  quest_w). Tables in .bss (past the file data of the ELF or overlay)
  start as zeros. Pointer tables (sun_pos_tbl, stg_eft_mdl_no, stage_sphr_tbl)
  get each PS2 pointer translated to the same bytes in the loaded image.
- `rt_mem.c`: PS2 address lookup in the ELF and the overlay.

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
- **Placement:** each actor is posed at frame 0, then moved so its lowest
  vertex sits on the ground height from `lg004.bin`.

## Known gaps

- **Render states:** per-part blend, filter and clamp come from the
  0xF0000 chunk (clay_attr_set). Cull, UV-scroll flag, fog and lighting
  type from the same chunk (states 0x00, 0x62, 0x12, 0x01, baked into the
  clay on the PS2) are not applied. Alpha test is > 0x40 for host draws.
  The sky is drawn first with z-write off.
- **Rathian:** the tail tip (AHI tree 1) is not attached, so it lies on the
  ground. No blending between motions.
- **Hunter:** no weapon. Hair and cloth bones (ptmat ≥ 64) keep their bind
  offset.
- **Runtime:** fl fade alpha is passed as-is (PS2 0x80 = 1.0 is not
  handled). No players/monsters run as game C yet, so player_work is
  zero (set13 uses the master player's position on some stages).
- **Scene:** `--stage N` loads any stage (files from main's per-stage
  tables); em01 and one armour set are fixed. On stages other than 4 the
  actors and camera stand at the middle of the ground collision. Some area
  models do not line up with their collision (st05's model is centred on
  the origin, its ground is not), so they need a placement offset that is
  not found yet; a few stages show holes in the floor.
