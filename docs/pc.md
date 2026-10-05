# PC viewer (first piece of the PC port)

`build/pc/mhview` is a real-time viewer written in C99. It loads MH1 data
straight from the user's disc files at run time, with nothing extracted to
disk, and shows stage 4 (st04) with the Rathian (em01) and a hunter
standing in it. Both play their motions in real time. Camera is free-fly.

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
headers and linked into the viewer. First file: set14 (stage UV-scroll
overlay), from src/game/set/set14_nm.c (whole file; set14_trans is a
near-match on the PS2 side, believed equivalent). On stage 4 it scrolls the
two waterfall layers of the st04_1 set model (clays 2 and 3, placed at
11060,0,1566).

src/pc/rt/:
- `rt_game.c`: game_w, player_work, stage_work (timer counts up each tick),
  set_mdlw; the set object pool (pull/push_set_work, 64 entries of 0x80: a
  guess); prims and ordering tables ot0..ot3 (get_prim, add_prim; prims are
  drawn in table order, low priority first: a guess); ran_suu (same
  generator as 0x161230); rt_game_init/move/draw. Static asserts check the
  struct layouts.
- `rt_fl.c`: flSetRenderState (0x19 texture matrix, 0x1A world, 0x60 alpha
  ref, 0x67 fade, 0x6C z-write; others print a one-time warning),
  flmatMakeTrans, flFloor, flExecuteClay (handle -> gfx clay). Stubs:
  clay_attr_set/reset, SetFilterMode, se_req2.
- `rt_data.c`: Capcom data tables are declared empty and filled at start-up
  from the user's SLPM_654.95 / game.bin by address (nothing copied into
  the repo).
- `rt_mem.c`: PS2 address lookup in the ELF and the overlay.

Game logic ticks at 30 per second; the host draws its own models, then
`rt_game_draw()` walks the ordering tables. Set-model parts the game C has
drawn are skipped by the host's generic draw so they are not drawn twice.
The stage's set spawn list is not decompiled yet, so rt_game_init calls
set14_set() by hand.

Verified 5 Oct 2026: `build/pc/mhview disc/mh1 --shot X.png --size 640x480
--frames 3 --time T --cam 11060,700,5000,0,-0.1` at T = 1.0 and 1.5: the
waterfalls are drawn, and between the two shots only the waterfall and
mist pixels change (the textures scroll).

Adding more game C: put the file in GAME in tools/build_pc.sh, its data
tables in rt_data.c, and whatever it calls into rt_*.c.

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

- **Render states:** no per-part attribute states yet (the 0xF0000 chunk:
  cull, UV scroll, fog, blend modes). Everything is drawn with alpha test >
  0x40 plus alpha blending and no culling. The sky is drawn first with
  z-write off.
- **Rathian:** the tail tip (AHI tree 1) is not attached, so it lies on the
  ground. No blending between motions.
- **Hunter:** no weapon. Hair and cloth bones (ptmat ≥ 64) keep their bind
  offset.
- **Runtime:** clay attributes (0xF0000 chunk) are not applied for game
  draws either; fl fade alpha is passed as-is (PS2 0x80 = 1.0 is not
  handled).
- **Hard-coded scene:** only stage 4, em01 and one armour set, chosen in
  viewer.c.
