# Original Xbox port: plan and groundwork

Written by agent A on 7 Oct 2026, before any hardware or xemu files existed. The
PC build (docs/pc.md) is the reference; the Xbox build reuses its game C and
platform interfaces. Estimates are marked as estimates.

## Toolchain (works, no root)

nxdk (github.com/XboxDev/nxdk, cloned with submodules to `~/xboxdev/nxdk`,
outside the repo) needs clang, lld, llvm tools, bison, flex, cmake and make.
This machine has only make/git and no passwordless sudo, so the Ubuntu 24.04
packages were downloaded without installing them and unpacked into a private
root:

    mkdir -p ~/xboxdev/debs ~/xboxdev/root && cd ~/xboxdev/debs
    apt-get download clang-18 lld-18 llvm-18 llvm-18-runtime llvm-18-linker-tools \
        bison flex cmake m4 ... (plus their library dependencies, from
        apt-cache depends --recurse)
    for f in *.deb; do dpkg-deb -x "$f" ../root; done

`~/xboxdev/env.sh` sets PATH (llvm-18/bin, root/usr/bin, nxdk/bin),
LD_LIBRARY_PATH, BISON_PKGDATADIR, M4 and NXDK_DIR. Then:

    . ~/xboxdev/env.sh
    cd ~/xboxdev/nxdk/samples/hello && make      # -> bin/default.xbe + an .iso

Checked 7 Oct 2026: `hello` and `sdl` samples build (default.xbe and an XISO).
This also builds nxdk's libraries: pdclib (C library), winapi subset,
libSDL2 (video, audio, game controllers), pbkit (NV2A push buffers),
zlib, libpng/jpeg, the USB stack and lwIP networking. Not run yet (no xemu
files, no console). clang 18 is deliberate: nxdk warns that clang 19.x up to
20.1.2 miscompiles some code with optimisation on.

## Cross-compiling the game C (done, compiles)

Method (repeatable, `tools/xbox_compile_check.sh`, ~5 min): run `tools/build_pc.sh` with CC set to a wrapper that
records every compile command, then replay the `-c` commands with `nxdk-cc`
(target i386-pc-win32, Pentium III), dropping the Linux-only flags (-m32,
sysroot, SDL include path, -g, -fno-aggressive-loop-optimizations). Objects in
build/xbox/obj (gitignored). Result, 7 Oct 2026:

- 699 of 701 files compile (after main merged the lobby-client files into f/lb_cli.c): all game C the PC links (main, game, lobby, select
  overlays, matched and near-match files, ABI patch copies) and the PC runtime
  glue. The struct-size checks in src/pc/rt/rt_game.c (PLW 0xA00, GAME_W
  0x224, CLAY 0x8C, SETW/PRIM/STAGE_WORK offsets) pass under the MSVC-style
  layout of the win32 target, and include/ has no bit-fields (MSVC packs those
  differently), so the PS2 structs keep their offsets.
- clang 18 turns three gcc warnings into errors by default in C99
  (implicit function declarations, implicit int, int-conversion): the replay
  passes `-Wno-error=` for them. Hundreds of decompiled files rely on
  implicit declarations; adding prototypes is the long-term fix.
- Fixed (PC side): `em_core_nm.c` / `em_cmd_nm.c` used -D macros that expanded
  inside a prototype (clang rejects it); they are now call-site patches in
  tools/pc_patch.py. PC behaviour unchanged (quest loop, progression and
  urgent tests pass).
- Fail (platform files, need Xbox versions, not trivial):
  - `src/pc/rt/rt_data.c` uses `dlsym` to find host symbols by name for the
    ELF data import. On the Xbox: a table generated at build time
    (name -> address) instead of the dynamic symbol table.
  - `src/pc/rt/rt_mc.c` (memory card on a host directory) uses dirent.h. On
    the Xbox: FindFirstFile/CreateFile from nxdk's winapi, under E:\UDATA.
- (The rt_data.c / dlsym and rt_mc.c points above are solved: rt_data.c now
  uses a generated symbol table on both builds, and the Xbox links a null
  memory card for now.)

## Linking for the Xbox (done 7 Oct 2026: links, not run yet)

    tools/build_pc.sh                       # first; the Xbox build reuses its commands
    . ~/xboxdev/env.sh
    python3 tools/build_xbox.py             # null graphics -> build/xbox/default.xbe, mh1.iso
    python3 tools/build_xbox.py --gfx nv2a  # pbkit graphics -> build/xbox/nv2a/default.xbe, mh1.iso

About 2 minutes. build_xbox.py takes the objects the PC build links
(build/pc/objs.txt, in link order) with their recorded compile commands
(build/pc/cmd/), compiles them again with nxdk-cc, compiles the PC front-end
(viewer, fl_model, formats, SDL pad, audio mixer + SDL audio output) with the
Xbox stand-ins from src/pc/xbox/, makes the symbol table for rt_data.c, links
with nxdk-link, then cxbe (XBE) and extract-xiso (an ISO holding only the
XBE; the game files are never shipped).

How the GNU-only link tricks were replaced (same scheme on the PC build, so
the PC tests check it):
- `objcopy --weaken-symbol` -> tools/pc_link_adapt.py writes a header per
  object with `#pragma weak NAME` lines (force-included when it is compiled
  again). On COFF that gives a weak external with a default.
- lld-link 18 rejects two weak definitions of one name met before a strong
  one ("duplicate symbol"); GNU ld takes the first strong, else the first
  weak. tools/coff_weak.py applies the GNU rule after compiling: in every
  object whose weak definition loses, the weak external is rewritten into a
  plain undefined reference (the aux record becomes an absolute static
  symbol so indices stay). That object's own calls then go to the winner,
  as on ELF. Checked on a 3-object test: all calls went to the strong copy.
  421 losing definitions are rewritten in the game link.
- clang names a COFF weak default `.weak.NAME.default.FIRST` after the
  object's first external definition; when that was a shared `__real@...`
  float constant, two objects collided. build_xbox.py force-includes a
  unique `__xtag_<object>` function first.
- `ld --defsym D_xxxx=table+off` -> `.set` aliases in the defining object's
  header (pc_link_adapt.py).
- `-rdynamic` + dlsym -> tools/gen_symtab.py table (name -> address).
- asm labels (`__asm__("game_w")` in pc_abs.py output and rt_ps2abs.h) now
  carry `__USER_LABEL_PREFIX__` ("_" on win32).
- Small libc gaps in nxdk's pdclib: atof (src/pc/xbox/xbox_libc.c). Paths:
  `fopen` is renamed to a wrapper that turns '/' into '\'
  (src/pc/xbox/xbox_compat.h, force-included in the host C).
- `num_tbl`: gcc drops an unused `strchr(num_tbl, c)` in hk_all.c, clang
  keeps the call; a dummy definition in xbox_libc.c.

Result (7 Oct 2026): 715 objects compile, link with no undefined symbols.
default.xbe 3.13 MB (null graphics) / 3.19 MB (nv2a); ISO 3.7 MB. Sections
of main.exe: .text 2780 KB, .rdata 298 KB, .data+.bss 4085 KB (7.2 MB loaded
before any heap). Main thread stack set to 1 MB (`-stack:0x100000`; nxdk's
default is 64 KB, the PC has 8 MB; not measured what the game needs).

On the Xbox (`#ifdef XBOX` in viewer.c) there is no command line: it mounts
E:, looks for AFS_DATA.AFS in `D:\data` (next to the XBE) then
`E:\Games\MH1\data`, and boots like `--boot` (title screen from power-on).
SDL2 (nxdk port) is used for the pad and the audio output, as on the PC.
The memory card is a null libmc (src/pc/xbox/mc_null.c: "no card", the game
plays without saving).

Not run anywhere yet (no xemu files): whether it boots, whether 64 MB is
enough with the PC-side waste still in (it is not: see the memory budget
below; the first boot may run out of memory before the title), stack depth,
SDL audio/pad on nxdk with this code.

### gfx_nv2a.c (started, untested)

src/pc/xbox/gfx_nv2a.c implements gfx.h on pbkit: one Cg vertex program
(src/pc/xbox/shaders/vs.vs.cg: one combined world*view*proj*viewport matrix,
pre-lit colour, texture matrix) and one pixel shader (texture x colour; a 1x1
white texture for untextured draws), compiled at build time with nxdk's cgc
+ vp20compiler/fp20compiler into build/xbox/shaders/*.inl. Vertices are
copied per draw into a 6 MB ring of contiguous memory (24 bytes each).
Power-of-two textures are swizzled A8B8G8R8 (repeat works), others linear
"rect" textures (clamp only, texel coordinates through the texture
matrix). Blend factors/equation, alpha test (GREATER ref), depth test/write,
filter and clamp follow gfx_gl.c; fade colour is multiplied on the CPU.
Second round: palettised textures (P8, see the memory section),
near-plane clipping on the CPU (only clays with a triangle behind w = 1;
the cut is done in object space with the shared gfx_clip_tri, checked on
200k random triangles), linear fog in the shaders (the vertex program
writes COLOR1 = fog colour + factor from the clip w, the pixel shader
lerps; NV2A's own fog unit is not used). Guesses to check on xemu: the
palette's DMA context bit, whether COLOR1 needs SPECULAR_ENABLE. Missing:
GPU skinning. It compiles without warnings; nothing about it has been
seen on a screen.

## How the platform layer maps to nxdk

The PC splits platform code behind small interfaces (src/pc/gfx/gfx.h,
src/pc/audio/audio.h, the pad in src/pc/pad/, files through fmt_afs, saves in
rt_mc.c). Each gets an Xbox implementation; the game C and src/pc/rt stay
shared.

| PC piece | PC today | Xbox plan |
|---|---|---|
| Rendering (gfx.h) | gfx_gl.c: OpenGL 1.x fixed function on SDL2; CPU skinning in fl_model | new gfx_nv2a.c on **pbkit** (nxdk's push-buffer library). nxdk has no OpenGL; the outside project pbgl (GL 1.x on pbkit) is a possible shortcut and a reference, but gfx.h is small (textures, a few render states, draw clays) so a direct pbkit backend is likely simpler and faster. NV2A does palettised (P8) textures natively, which fits the PS2's 4/8-bit textures. Later: move skinning to NV2A vertex programs (nxdk has a vertex-program compiler), as the PS2 did it on VU1. |
| Window / frame | SDL2 window, vsync | pbkit framebuffer, 640x480 (480i/480p), later 720p where the dashboard allows; aspect 4:3 like the PS2, widescreen optional |
| Pad (pad.h) | SDL game controller + keyboard | nxdk's SDL2 game controller (the `sdl_gamecontroller` sample) — same code as the PC; the Duke/S controller maps like the Xbox layout already used on PC. Analog buttons can stay digital. |
| Audio (audio.h) | portable mixer (audio_mix.c) + SDL2 output | keep audio_mix.c, output through nxdk's SDL2 audio (AC97). The Xbox's own APU (DirectSound, Xbox ADPCM) is not needed: the mixer does PS2 ADPCM and ADX in software. Budget: 48 voices + 2 ADX streams + reverb ~ a few % of the CPU (estimate). Decode VAG samples on the fly instead of caching PCM if memory is short. |
| Files | disc files read by path (AFS00/01, AFS_DATA, SLPM_654.95) | the same files copied from the player's own disc to the hard disk, e.g. `E:\Games\MH1\data\` (about 925 MB). The Xbox DVD drive is not expected to read a PS2 disc; reading the user's files from the HDD keeps "own disc required". A first-run check lists missing files. |
| Saves | rt_mc.c: libmc on a host directory, the PS2's own BISLPM-65495MH save files | same code, file calls on `E:\UDATA\<title id>\<save>\` with the dashboard's SaveMeta.xbx so it shows in the Xbox memory manager. Title id to be picked (homebrew range). |
| Timing | SDL ticks, 30 Hz game ticks | KeQueryPerformanceCounter / SDL ticks; 30 Hz game logic, render at 30 or 60 |
| Movies | skipped | see DECISIONS.md (Sofdec open question) |
| Network | none (offline first) | lwIP in nxdk, later, under the "keep the PS2 wire protocol" decision |

## Memory budget (64 MB, shared with the GPU)

### Measured on the PC build (round 25, 7 Oct 2026)

`RT_MEM=t1,t2,...` makes mhview print live heap bytes per category at those
host ticks (src/pc/rt/rt_memstat.c: the port's own malloc/free are counted
through a forced include; the decompiled game C does not allocate, it uses
fixed areas). `tools/pc_memstat.py` lists the static .data/.bss. Runs:
title = `--boot` tick 500; village = CONTINUE, tick 2500 (village after a
quest's files were loaded at start); Rathian = `--quest 10` with
RT_QUEST_STAGE=1, tick 300 (nest, Rathian awake). In KB:

| category | title | village | Rathian |
|---|---|---|---|
| program file copy (SLPM_654.95, for the data import) | 5516 | 5516 | 5516 |
| overlay binaries (game/lobby/select.bin as read) | 2657 | 2657 | 2657 |
| overlay data copies + relocation tables (rt_mem) | 4436 | 4436 | 4436 |
| main data tables imported from the ELF (rt_data) | 1235 | 1235 | 1235 |
| files for the game's loaders (load_file_mdl copies kept) | 2472 | 9540 | 2472 |
| files kept by the host renderer (models, stage) | 20108 | 21748 | 19547 |
| collision areas (rt_hit: 2 x 4 MB fixed) | 8192 | 8192 | 8192 |
| 2D / camera / font work areas (rt_2d 4096, rt_cam 1024, rt_font 976) | 6096 | 6096 | 6096 |
| host models (clays, skinning buffers) + skeletons/motions | 3623 | 5841 | 3510 |
| renderer CPU-side vertex arrays | 1523 | 2102 | 1108 |
| audio packs as on disc (PS2 ADPCM) | 1558 | 1737 | 1776 |
| audio decoded to 16-bit PCM (cache) | 86 | 599 (peak 7059) | 20079 |
| other runtime heap | ~255 | ~255 | ~255 |
| **CPU heap total** | **57745** | **69946** | **76873** |
| textures in GPU memory as RGBA8 | 19640 | 22415 (peak 25967) | 11716 |
| the same textures as on disc (4/8-bit + CLUT) | 4792 | 5419 | 2842 |
| static .data/.bss of the binary (`size`) | 4090 | 4090 | 4090 |
| code (.text) | 3099 | 3099 | 3099 |
| process RSS (incl. SDL, Mesa GL driver, libc) | 105476 | 113644 | 104724 |

(The title already holds quest-10 data: the viewer sets up a quest before the
boot. The village column is after the first quest's files.)

### What that means for 64 MB

Naively the PC needs ~75 MB of heap plus ~20 MB of RGBA textures: too much.
But most of it is PC-side waste with a clear fix:
- Program file copy 5.4 MB: copy only the tables used, then free -> ~0.5 MB.
- Overlay binaries + data copies (7 MB): the PS2 holds one overlay at a time
  (game.bin 1.4 MB or lobby.bin 1.3 MB) in place; one copy -> ~1.5 MB.
- Host renderer keeps whole model/stage files (~20 MB) after building its
  clays: keep only what drawing needs (vertex data and texture handles); the
  PS2 itself keeps these files in its 32 MB, so ~8-10 MB is the realistic
  floor here.
- Collision areas fixed at 2 x 4 MB: size them to the stage's files (the PS2
  area is much smaller [not measured]); est. 1-2 MB.
- Audio PCM cache (20 MB in the Rathian nest): decode PS2 ADPCM per voice
  while mixing (28 samples per 16-byte block, cheap) or keep a small LRU
  -> ~2 MB with the 1.8 MB of packed data.
- Textures: NV2A supports 8-bit palettised textures; 4-bit ones would be
  expanded to 8-bit: about 1.5x the disc size -> 4-8 MB instead of 12-26 MB.
- 2D/camera/font areas (6 MB fixed): check against the PS2 sizes.

Estimated Xbox total after those changes: code+static ~7.5 MB, game files
and work areas ~16-20 MB, host model data ~8-10 MB, textures 4-8 MB, audio
~4 MB, framebuffers ~3.7 MB, nxdk/kernel ~4 MB: about 47-57 MB of 64. It
fits, with little room; the trims above are required, not optional.

### After the trims (agent A, 7 Oct 2026, second round)

Done, on the PC and the Xbox alike (`tools/mem_report.sh` re-measures the
three points; RT_MEM_FILES=1 lists every file the viewer loads):
- Sound effects stay PS2 ADPCM and are decoded while mixing (the mixer keeps
  two 28-sample blocks per voice; audio_voice_play_vag). The PCM cache (up
  to 20 MB) is gone. A 60 s Rathian audio dump is byte-identical to before.
- The program file (5.5 MB) is freed after the import; only main's data
  part (1.2 MB) stays, in its own block, because imported tables point
  into it. The ELF symbol and relocation tables (3 MB) and the raw
  lobby/select.bin go too (rt_mem_trim); .bss shadows are per symbol
  (233 small blocks instead of 1.7 MB).
- Model and texture files are freed once fl_model_create has copied them
  (motion tables stay: the motion players read them in place).
- Meltw output buffers are shrunk to their size (they were allocated at 4x
  the packed size and kept so).
- Fixed areas sized from the data, with a size check on every load and a
  use report (RT_MEM): collision 2 x 512 KB (largest files 160 / 80 KB;
  were 4 MB each), data_load_ptr 2 MB (838 KB used; was 4 MB),
  cam_data_area 64 KB (largest camera file 3.4 KB; was 1 MB), glyph
  texture table per glyph (was 1 MB of pointers).
- Xbox textures: gfx_nv2a.c keeps power-of-two textures with <= 256
  colours (all the 4/8-bit APX ones) as NV2A P8 + palette.

Checked: the three PC tests pass; screenshots of the title, the Rathian
nest, the village, stage 4 and the quest-loop end are byte-identical to
before the trims.

Now (KB, same three points as above):

| | title | village | Rathian |
|---|---|---|---|
| CPU heap total (was) | 18433 (57745) | 23558 (69946) | 18421 (76873) |
| textures as the Xbox keeps them (P8/RGBA8) | 5097 | 5981 (peak 6890) | 2984 |
| (the same as RGBA8, the PC) | 19640 | 22415 | 11716 |

Estimated Xbox total in the village (the largest): heap 23.6 MB + textures
6.9 MB + vertex ring 6 MB (gfx_nv2a.c) + the XBE loaded 7.2 MB (code 2.8,
data/bss 4.1, of which rt_lb_mem 2.2) + framebuffers 3.7 MB + kernel and
nxdk ~4 MB [estimate] = about 51 MB of 64. Start-up peaks higher for a
moment (program file + relocation tables, ~9 MB, freed before any model is
loaded). Not measured on an Xbox: allocator overhead and fragmentation of
pdclib's malloc, and what SDL takes.

Left (not needed to fit, worth doing later): the host keeps model data
twice (fl_model's AMO arrays and the renderer's clay copies, ~4-6 MB);
lobby.bin is held three ways at run time (rt_lb_mem 2.2 MB static, a 1.2 MB
reload copy, game.bin 1.4 MB beside it) where the PS2 swaps one overlay;
the vertex ring could be smaller.

### Stack

RT_STACK=1 paints 2 MB below main and reports the deepest byte used: 35 KB
in the three PC tests, the title and the Rathian runs (gcc -O2, 32-bit;
the game C keeps its work in static areas). The XBE gets 256 KB
(`-stack:0x40000` in tools/build_xbox.py; nxdk's default is 64 KB).

## CPU budget (733 MHz Pentium III, 30 fps)

### Measured on the PC (agent A, 7 Oct 2026, round 4)

`RT_PROF=1` (src/pc/rt/rt_prof.c) splits the CPU time into subsystems.
Zones nest, and time goes to the innermost one. Logic zones are reported
per game tick and draw zones per drawn frame, every 300 ticks, with the
mean and the worst. On the PC it counts this thread's CPU time, so other
jobs on the machine don't distort it; on the Xbox it uses the performance
counter. `RT_STEP=1` with `--time S` runs one tick per drawn frame, as at
30 fps. It also counts vertices skinned, vertices drawn, triangles and
draw calls per frame. `tools/prof_scenes.sh` runs the four points below:
- village: CONTINUE, walking about;
- Rathian: quest 10 at her nest, hunter warped to her, attacking, GOD;
- Fatalis: quest 103, the same;
- movie: the opening movie.
All runs use `--audio-dump`, so the mixer runs in the tick and is measured.

Host: Core i5-10300H at ~4.0 GHz (turbo), gcc -O2 -m32 (x87 floats).
Numbers are ms of CPU, after this round's fixes (below):

| zone | village | Rathian | Fatalis | movie |
|---|---|---|---|---|
| game logic + host glue, per tick | 0.41 | 0.82 | 0.39 | 0.06 |
| set objects + effects move | 0.05 | 0.05 | 0.02 | 0 |
| sound tick + ADX stream decode | 0 * | 0.06 | 0.05 | 0.04 |
| mixer (48 kHz, ADPCM decoded while mixing, reverb) | 0 * | 0.54 | 0.39 | 0.33 |
| movie (MPEG-2 decode 1.5, colour conversion, texture) | | | | 2.7 |
| draw: host draw code (poses, game draw C, 2D) | 0.86 | 0.77 | 0.36 | 0.02 |
| draw: CPU skinning + lighting | 3.81 | 3.90 | 3.52 | |
| draw: GL backend (not representative of NV2A) | 2.94 | 3.11 | 2.81 | 0.47 |
| effects draw | 0.005 | 0.005 | 0.003 | |
| vertices skinned / drawn per frame | 20k / 36k | 16k / 34k | 12k / 30k | |
| triangles / draw calls per frame | 32k / 121 | 32k / 154 | 24k / 68 | |

\* The village runs no sound tick on the PC yet (no village sound path in
the viewer), so no mixer time was measured there; assume the Fatalis
figure.

### Projection to the Xbox

Assumption: the Xbox CPU is about **20x slower** than this host for this
code (range 15–30x). That is 5.4x for the clock (4.0 GHz vs 733 MHz) times
about 3.5x per clock: the modern core issues more per cycle, predicts
better, and has much larger caches. The Xbox's Pentium III has a 128 KB L2
cache and a 133 MHz front-side bus. Not measured: no hardware yet. The
first xemu or console run with RT_PROF replaces this guess; it would need
its report sent to the debug output instead of stderr.

At 30 fps one frame is 33.3 ms, with one game tick and one drawn frame.
Before this round, at 20x:
- skinning alone: 3.5–3.9 ms -> 70–80 ms, and 16 ms -> 320 ms in the
  village, where every villager body variant was skinned;
- mixer: 0.7 ms -> 14 ms;
- movie: 5.6 ms -> 112 ms.

After this round (fights, 20x):

| | Rathian | Fatalis | village |
|---|---|---|---|
| logic + sound | 18.6 | 9.2 | 9.2 |
| mixer | 10.8 | 7.8 | ~7.8 |
| host draw code | 15.4 | 7.2 | 17.2 |
| skinning | ~0 (GPU) | ~0 | ~0 |
| NV2A backend: pushbuffer, copies of unskinned geometry [estimate] | 3–5 | 3–5 | 3–5 |
| **total** | **~48–50** | **~27–29** | **~37–39** |

So at 20x: Fatalis holds 30 fps, while the village and the busiest Rathian
fight run at about 20–27 fps. At 15x all three fit in 33 ms except the
Rathian fight (~37 ms); at 30x none do. The movie projects to decode 30 +
conversion 12 + texture upload ~10 ms, about 50 ms per frame. That needs
libmpeg2's MMX code (it has it; not enabled in this build) and the YUV ->
RGB conversion done on the GPU (register combiners on three 8-bit planes,
or a YUY2 texture) to fit.

### What was done this round

- **Skinning moved to the GPU on the Xbox** (gfx.h "GPU skinning"):
  - `src/pc/xbox/shaders/skin.vs.cg`: 117 of 128 instructions. It blends
    the matrix rows of up to 24 palette bones per vertex, transforms the
    position and normal, does VU1-style lighting (3 directional lights +
    ambient, clamped), tint, fade and fog.
  - `gfx_skin.c` regroups each clay's triangles into batches of at most
    24 bones. Measured at quests 10 and 103: 75 batches from 69 material
    batches, and 4% more vertices from copies.
  - `gfx_nv2a.c` keeps skinned clays in a static vertex buffer (80 bytes
    per vertex, ~1.1 MB per scene) and uploads only bone matrices and
    lights per draw.
  - The PC/GL backend keeps the CPU path (`gfx_skin_capable() == 0`).
  - Checked on the PC: `RT_SKIN_CHECK=1` runs a C model of the vertex
    program (`gfx_skin_eval`) on every vertex copy and compares it with
    `fl_model_pose`'s CPU result. Quests 10, 103, 144, 173 and the village:
    39 M vertices, worst position error 2e-5 relative, colours within 1
    (truncation).
  - `build_xbox.py` checks that cgc's constant layout matches what
    `gfx_nv2a.c` uploads.
  - Not checked: the NV2A program itself, on xemu or hardware.
  - Skinned clays are not near-clipped: the clip happens on the CPU and
    needs the skinned positions.
- **Village:** the villagers' model holds every body variant (0x20 parts)
  and all were skinned per NPC; now only the parts that are drawn are
  skinned. The hidden Rathian model is no longer skinned either. Village
  skinning went from 16 to 4 ms on the PC. Screenshots are unchanged;
  `RT_POSE_ALL=1` gives the old behaviour.
- **Mixer**, about 2x cheaper:
  - The reverb tails had decayed into denormal floats, the slow path on
    x87 and on the Pentium III. A 1e-15 offset keeps them out: reverb went
    from 0.3 to 0.06 ms per tick.
  - Voices use 32.32 fixed-point positions and decode a block only when
    they leave it.
  - A 60 s Rathian audio dump differs from before by 1 LSB on 0.3% of
    samples.
- **Movie on the PC:** the Xbox texture-size estimate (a colour count per
  texture) now runs only with RT_MEM. It was half the movie time.

### Next for the frame rate (in order of the projected gain)

1. The host draw code ("draw (rest)", 0.4–0.9 ms -> 7–17 ms): profile
   inside it. It holds the hunter's per-part bone matrices, joint syncing,
   the game's draw C (`trans_stage`, prims) and the 2D layers.
2. Game logic ("logic (rest)"): 0.4–0.8 ms -> 8–16 ms. Profile by task;
   check that clang/SSE (`-mfpmath=sse` is not set by nxdk) helps the
   float-heavy monster code.
3. Mixer: about 8–11 ms. Options: mix at 24 kHz, use 16-bit integer
   arithmetic, or later the Xbox APU (its voices take ADPCM, but nxdk has
   no APU driver).
4. NV2A backend: keep the static stage geometry in GPU memory instead of
   copying it into the ring every frame.
5. Movies: libmpeg2 MMX, YUV -> RGB on the GPU, a streaming texture
   instead of creating one per frame.

## Why 32-bit x86 helps

The game C keeps pointers in u32 fields and depends on PS2 struct offsets. The
PC port already builds it as 32-bit x86 (gcc -m32) with static checks on the
struct sizes; the Xbox's Pentium III is the same pointer size and byte order,
and the win32 target keeps the same struct layout (checked above). So no
pointer-widening or byte-swapping work, and every PC fix carries over. The
PC's -ftrivial-auto-var-init=zero and -fno-strict-aliasing are needed on the
Xbox too (clang supports both).

## Risks

- CPU: 733 MHz Pentium III. Game logic is cheap (0.2-0.3 ms per tick on a
  modern x86, perhaps 10x that on the Xbox: fine), but the PC does CPU
  skinning and lighting of every model each frame. On the ARM box that
  already limits the frame rate (25-28 fps at 960x720). On the Xbox,
  skinning must move to the GPU (vertex programs) to hold 30 fps in fights.
- GPU backend from scratch: pbkit is low level (push buffers, register
  combiners for blending/texture modes, no driver). Most work and most risk
  of the port. xemu helps; real hardware checks catch what xemu gets wrong.
- Memory (above): no virtual memory; running out is a hard crash. The
  trims bring the estimate to ~51 of 64 MB; to be confirmed on xemu.
- Link step: solved (see "Linking for the Xbox"); watch for new GNU-only
  tricks in tools/build_pc.sh.
- Implicit declarations: clang treats them as errors in C99 by default; we
  pass -Wno-error, but a wrong implicit return type (pointer returned as int)
  is a real bug risk on any target. Worth adding prototypes over time.
- Data from the HDD: needs the player to copy ~925 MB from their disc with a
  PC tool or FTP; a small copy guide is needed.
- nxdk is a moving target; pin a known-good commit once the first build runs
  on hardware.

## What is needed from the owner

- For xemu: their own BIOS (kernel) image, MCPX boot ROM and a hard-disk
  image dumped from their console (never committed).
- About the console: how it is modded (softmod or modchip, which dashboard),
  hard disk size/partitions (is F: or G: there?), how files get onto it (FTP
  from the dashboard, IP address), and the video cable (composite /
  component: decides 480i vs 480p/720p).
- The capture card setup and, later, the controller emulator, as planned.
- A decision on where the game files live on the Xbox (suggested
  E:\Games\MH1\data) and a title id for saves.

## Next steps (when the files arrive)

1. xemu running the nxdk `hello` and `sdl` samples from this machine.
2. Check the memory estimate on the console (debug output of the free
   memory at the title, village and a hunt).
3. Boot build/xbox/default.xbe (null graphics) in xemu with the game files
   in D:\data or E:\Games\MH1\data; see where it stops (memory, stack).
4. gfx_nv2a.c: textured clays, then the HUD/2D; then pad and audio.
