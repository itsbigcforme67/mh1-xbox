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
The memory card is src/pc/xbox/mc_xbox.c (see "Saves on the Xbox" below).

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
lerps; NV2A's own fog unit is not used; COLOR1's alpha needs SPECULAR_ENABLE and LIGHT_CONTROL's
ALPHA_FROM_MATERIAL_SPECULAR, see "Fog" below). Guesses to check on xemu: the
palette's DMA context bit. Missing:
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

\* The village run measured no sound tick and no mixing: in the viewer, rt_snd_tick
and the --audio-dump mixing run from sim_tick (quests) and the boot screens
only, not in the village's game mode 6 (worth checking whether the village
is silent on the PC). The Fatalis mixer figure is assumed there.

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

### Round 5 (agent A, 7 Oct 2026): inside the host work, stage geometry, movies

**New zones.** RT_PROF now also splits out:
- motion evaluation (keys -> bone matrices);
- the host's joint matrices for the game C;
- the stage draw code, the game's prims and the 2D/HUD;
- a count of skeleton evaluations per frame.

Two runs of `tools/prof_scenes.sh` on this machine, while other agents were
working: the load average was ~7.5 on 8 hardware threads. Even thread CPU
time is inflated there, because a hyperthread shares its core, so these
numbers are upper bounds.

| ms per tick (CPU) | village | Rathian | Fatalis |
|---|---|---|---|
| game logic (game C + glue) | 0.20–0.35 | 0.38–0.44 | 0.23–0.28 |
| motion evaluation | 0.40–0.46 | 0.41–0.46 | 0.14–0.16 |
| joint matrices | 0.02 | 0.14 | 0.08 |
| sets + effects + sound tick | 0.07 | 0.11 | 0.09 |
| mixer | 0.26–0.37 | 0.56–0.64 | 0.42–0.48 |
| host draw code (stage, prims, 2D, rest) | 0.33 | 0.47 | 0.31 |
| skinning (GPU on the Xbox) | (3.8) | (3.3) | (2.9) |

**Projection at 20x**, per frame: CPU work + NV2A backend 3–5 ms
[estimate]:
- village: ~29–37 ms;
- Rathian fight: ~44–50 ms;
- Fatalis: ~28–33 ms.

On an idle host these would likely come out lower, so take them as the
pessimistic side. The Rathian fight is clearly above 33 ms; its largest
items are the mixer (11–13 ms at 20x), motion evaluation (8–9 ms) and the
game logic (8–9 ms).

**Done this round:**
- **Motion.** The same skeleton pose was evaluated up to three times per
  tick: game_core's joint sync, the end-of-tick sync, and the draw.
  fl_skel_pose_groups now skips a pose that is identical to its last one.
  Rathian scene: 19 -> 10 evaluations per frame, 0.45 -> 0.20 ms. The
  quest-loop end and 28 urgent-hunt screenshots are identical with the
  cache on and off (`RT_POSE_ALL=1`).
- **Stage geometry (Xbox).** Clays that never change, which is the stage
  and its set models, keep their vertices in a GPU-visible buffer. They are
  drawn from it without the per-frame copy into the ring. A bounding-sphere
  test skips the per-vertex near-plane check when the whole clay is in
  front of the camera. The fade colour is now a shader constant, so it no
  longer forces a copy.
- **Movies (Xbox):**
  - libmpeg2's MMX/MMXEXT IDCT and motion compensation are compiled in
    (`idct_mmx.c`, `motion_comp_mmx.c`, `mmx.h`, vendored from the same
    0.5.1 tree). On the PC, decoding dropped from 0.69 to 0.42 ms per
    frame, with 61.5 dB PSNR against the C decoder over 1200 frames.
  - The colour conversion moved to the GPU: one YUY2 texture
    (`LC_IMAGE_CR8YB8CB8YA8`) updated in place, so the CPU only moves
    bytes.
  - At 20x: decode ~8–14 ms, packing ~2 ms, so a movie should fit in a
    frame.
  - Not checked: the NV2A's YUV -> RGB matrix (assumed BT.601 studio
    range).
- **Sound in the village.** Snd_server (ADX streams) ran only inside
  game_core, so on the PC the village's music ring ran dry. It now runs
  every tick in every mode, as ACRMain does. Quest runs started without
  `--boot` also had silent audio (zero option volumes) and now set the
  option defaults.

**RT_PROF on the Xbox** is always on (the Xbox has no environment
variables). Its report goes to the kernel debug output (DbgPrint) and is
written to the game log (stderr = E:\Games\MH1\mh1log.txt, sent by the network loader; until 13 Oct 2026 to E:\mh1_prof.txt) every 300 ticks (10 s). The first xemu or
console run therefore gives real numbers for every table above.

Next for the frame rate:
- Mixer (~0.5 ms here): mix at 24 kHz, or in 16-bit integer arithmetic.
- Motion evaluation: the AAN key search per bone per frame. A cursor per
  curve would avoid searching from the start.
- Village NPCs: one skeleton per NPC kind, posed per instance; give each
  NPC instance its own pose cache.

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


## Saves on the Xbox (agent B, 9 Oct 2026; written, linked, not run)

src/pc/xbox/mc_xbox.c replaces mc_null.c: libmc (sceMc*) on the hard disk through nxdk's
winapi (CreateDirectoryA, FindFirstFileA, GetFileAttributesA, DeleteFileA) and pdclib's
fopen (xbox_fopen turns '/' into '\'). E: is mounted by viewer.c (nxMountDrive) before
the game's first sceMcInit. The game still sees one card in port 0 holding the PS2 save
directory `BISLPM-65495MH` (data file of 0x11450 bytes as encode_data writes it,
icon.sys, icon00.ico), the same bytes and names as rt_mc.c writes on the PC, so a PC
save file could be copied over (not tried). On disk:

    E:\UDATA\4D480001\TitleMeta.xbx                     "TitleName=Monster Hunter" (UTF-16LE + BOM)
    E:\UDATA\4D480001\4D48000100000001\SaveMeta.xbx    "Name=Monster Hunter save"  (made when the game creates the save dir)
    E:\UDATA\4D480001\4D48000100000001\BISLPM-65495MH, icon.sys, icon00.ico

The card path BISLPM-65495MH/... maps to the save id directory and back in listings;
the two .xbx files are hidden from the game's directory listings. Title id, names and
save id are constants in src/pc/xbox/xbox_title.h (placeholder id 0x4D480001 "MH1X"; the
XBE certificate's title id is set to the same value by tools/build_xbox.py (cxbe always
writes 0xFFFF0002, so the build patches the certificate in default.xbe from xbox_title.h
after cxbe; the title name "Monster Hunter" goes in through cxbe's -TITLE); the owner may supply a real id). Free space is reported as 8000 KB minus the
files, as on the PC.
Untested: everything (no hardware / xemu run). In particular the .xbx meta layout is from
memory of the dashboard format (UTF-16LE text with a BOM), the dashboard icon images
(SaveImage.xbx, TitleImage.xbx) are not written so it will show a default icon, and
whether the dashboard accepts a save directory name made only of hex digits that is
not a hash of anything is unverified.

## Xbox controller mapping (checked against nxdk's SDL, 9 Oct 2026; not run)

nxdk's SDL2 joystick driver (lib/sdl/SDL2/src/joystick/xbox/SDL_xboxjoystick.c) gives the
Duke/S controller 6 axes, 1 hat and 10 buttons and installs the game-controller mapping
"Original Xbox Controller": a:b0 b:b1 x:b2 y:b3 leftshoulder:b4 rightshoulder:b5 back:b6
start:b7 leftstick:b8 rightstick:b9 lefttrigger:a2 righttrigger:a5, sticks a0/a1 and
a3/a4 (Y already inverted), d-pad on the hat. The analog face buttons are digital here
(threshold 0x20). Crucially the driver reports WHITE as leftshoulder and BLACK as
rightshoulder (the XID report's analog buttons 8 and 9), and the analog triggers as
trigger axes. pad_sdl.c needs no change: it already maps leftshoulder/rightshoulder to
L1/R1 and trigger axes above 16000 to L2/R2. On the Duke/S:

| Xbox | PS2 role |
|---|---|
| A / B / X / Y | cross / circle / square / triangle |
| WHITE / BLACK | L1 / R1 (camera reset / guard) |
| left / right trigger | L2 / R2 |
| Back / Start | select / start |
| left / right stick click | L3 / R3 |
| d-pad | d-pad (camera turn / zoom in the hunt) |
| left / right stick | left / right stick |

White and black are small buttons and R1 (guard) is used a lot in a fight; if that feels
bad on hardware, swap to BLACK = camera and RT = guard in pad_sdl.c (not decided
without hardware). A Controller S (Xbox 360-style button order) report is covered by the
same mapping through SDL's own names.


## XBE identity and save icons (agent B, 9 Oct 2026)

build_xbox.py reads XBOX_TITLE_ID / XBOX_TITLE_NAME from src/pc/xbox/xbox_title.h, passes the name
to cxbe (-TITLE) and patches the title id into the certificate of default.xbe (header base address
at 0x104, certificate address at 0x118, id at certificate +8); it prints "XBE certificate: title
id 4D480001, title name ...". Checked by reading the XBE back. The XBE is unsigned (cxbe does not
sign); xemu and softmodded consoles do not check.

Save icons (SaveImage.xbx, TitleImage.xbx) are NOT done. What is needed: the dashboard shows a
64x64 (SaveImage) / 128x128 (TitleImage) picture stored as an XPR0 container holding one swizzled
DXT1 texture; the exact XPR0 header fields (resource table, data offset, D3D texture header for
that size/format), the swizzle and whether the dashboard wants a particular mip layout were not
verified without a console or the dashboard, and a wrong file may just show no icon (or worse,
confuse the memory manager), so it is skipped. Source of the picture would be the game's own PS2
save icon (icon00.ico, which mc_file_tbl / the game's save code builds from its data: a 128x128
texture), converted at run time and written next to SaveMeta.xbx / TitleMeta.xbx, never committed.
When someone can look at a real dashboard (or xemu with the owner's dashboard HDD), a reference
SaveImage.xbx from any homebrew save gives the exact header to copy.

## Running in xemu (prepared, not run; waiting for the owner's files)

xemu 0.8.136 (open source, github.com/xemu-project/xemu) is in ~/xboxdev/xemu (outside the repo;
x86_64 AppImage; it starts on this machine). It needs files only the owner can supply, none of
which we download or commit:

- `xbox_files/mcpx.bin`: MCPX boot ROM dump (512 bytes, "mcpx_1.0.bin"),
- `xbox_files/bios.bin`: BIOS (flash ROM) dump, 256 KB to 1 MB,
- `xbox_files/hdd.img`: raw HDD image. Optional: tools/run_xemu.sh makes a blank sparse 8 GB one;
  whether the kernel formats it on first boot is untested. A formatted image (E: with room for
  E:\UDATA) is what saving needs.

`xbox_files/` is gitignored. Then:

    tools/run_xemu.sh                          # boots build/xbox/mh1.iso (XBE only)
    tools/run_xemu.sh --data ~/path/to/disc/mh1  # ISO with the owner's files as D:\data (~925 MB, local)
    tools/run_xemu.sh --gfx nv2a               # build/xbox/nv2a/ instead of the null-graphics build
    tools/run_xemu.sh --shot out.png --after 90   # screenshot after 90 s, then quit

The script writes xemu's config (BIOS, MCPX, HDD, DVD paths) into xbox_files/xemu_home/ (it sets
XDG_DATA_HOME, so ~/.local/share/xemu is not touched), starts xemu, and for --shot drives it over
a QMP socket. xemu 0.8.136's QMP has no `screendump`, so the screenshot falls back to ffmpeg
x11grab of the whole display: xemu always opens a window (no headless mode, and Xvfb is not
installed here), so this needs the desktop and shows everything on screen. Checked here: the script
runs end to end with dummy 512-byte / 256 KB ROM files (xemu starts, rejects the dummy BIOS, the
screenshot fallback and the clean-up of xemu's processes work); nothing with real files.
Untested: real boot, the --data ISO build (extract-xiso on a 925 MB tree), HDD formatting, the
nv2a build. The first things to look for: the BIOS logo, then our XBE starting (title screen from
power-on, docs above), "free memory" lines, the E:\UDATA\4D480001 folder appearing after a save.

## Installed by hand, no loader: the testers' kit (agent A, 28 Oct 2026)

Volunteer testers copy a folder to their Xbox and start it from the dashboard; they have no network loader and no
laptop running tools/xbox_serve.py. `tools/make_release.sh G.iso [--mh1 MH1.iso] [--online]` makes that folder
(dist/release/xbox/MHG: default.xbe + data/) and the PC builds; docs/TESTERS.md is the testers' page.
How the game finds its things when started as E:\<folder>\default.xbe (code read, see the [checked] notes):
- Disc files: `D:\data` first, then `E:\Games\MHG\data` (viewer.c; D:\data counts only when it holds the game's own
  SLPM). nxdk's CRT mounts D: to the XBE's folder at start-up, **unless D: is already mounted** (nxdk
  lib/nxdk/automount_d.c) [checked]. Kernels / dashboards that launch an XBE normally map D: to its folder (the
  owner's loader relies on it, docs above) [known, not checked on other dashboards]; a dashboard that leaves D: on
  the DVD drive falls back to E:\Games\MHG\data, which is why TESTERS.md recommends E:\Games\MHG.
- Log: `mh1log.txt` in the XBE's folder (xbox_log.c, XeImageFileName), rewritten at each start. Reports: `reports\`
  next to it. Saves: E:\UDATA\<title id> as always.
- Settings: since this change `settings.ini` (and so `text\en.txt` next to it) is in the folder of a **default.xbe**
  started from the dashboard; an XBE a loader starts (mh1.xbe / mhg.xbe) keeps E:\Games\MH1 / MHG (gfx_opts.c).
- Leaving (Back + Start) and the watchdog / crash catcher: a default.xbe goes straight to the dashboard
  (xbox_leave: no loader to send the log), so testers copy mh1log.txt off by FTP.
Checked 28 Oct 2026, fresh clone (temporary directory, only the G .iso given): ./mh1 / make_release built the Xbox
folder and the Linux build; same file list as the owner's dist/xbox/MHG (default.xbe + data/AFS_DATA.AFS, AFS00.AFS,
AFS01.AFS, SLPM_658.69, data byte-identical) plus VERSION.txt; every Xbox object's sections equal a developer build
of the same source except rt_log.obj (the version hash and build time; the XBE differs only through those strings'
places). The kit's XBE booted hidden in xemu with the kit's data as D:\data (run_xemu.sh --data, no xbox_serve.py,
RT_NOMOVIE=1): "disc directory: D:\data", logos, title screen at 150 s. MH1 from the same fresh clone: Xbox folder
and Linux build made. Not checked: a dashboard launch from E: (xemu has no dashboard here), real hardware.

## Real Xbox bench setup (11 Oct 2026)

- Console: modded, Evolution X V4.3935, HDD C: free ~328 MB, MAC 00:0D:3A:31:08:AB.
- Video: MacroSilicon USB capture card on the laptop (/dev/video2). Use raw mode:
  `ffmpeg -f v4l2 -input_format yuyv422 -video_size 1280x720 -i /dev/video2 -frames:v 30 -update 1 shot.jpg`
  (MJPEG mode and the first frames give colour bars; only one program may hold the device).
- Input: RetroKM pad player (MULTI KM/xboxpad, Atom S3R as a Controller S). Drive it via the hub:
  `printf 'goto xbox\n' | nc -q0 127.0.0.1 24851`, then `printf 'pad <hex>\n'` and `pad 0`
  (A=1 B=2 X=4 Y=8 back=40 start=80 up=800 down=1000 left=2000 right=4000). Use one short tap
  (~50 ms) per step; longer holds auto-repeat in Evolution X menus.
- Evolution X main menu (one scrolling list): Play Games from HD, Trainers, Emulators, DVD,
  Launch Applications, Microsoft Dashboard, Skin, System Configuration (-> System Settings,
  Lock/Unlock Harddisk, Power Cycle, Flash BIOS: never select the lock/flash items), Reboot, Power Off.
- Network: works since 11 Oct after a port swap and a power-strip change (a tripped breaker had
  been cutting power). DHCP gives it an address (192.168.1.115, then .116 after a router restart on 11 Oct; MAC 00:0D:3A:31:08:AB; XBOX_IP=... for xbox_upload.sh); FTP (21), telnet (23)
  and 731 answer. It sits behind a mesh node (ping TTL 63), so a MAC sweep from the laptop never
  shows its MAC; find it on the System Settings page (main menu: 7 downs, A, A) or ping .115.
- FTP (Evolution X server): passive mode fails (EPSV and PASV both answer 502), active mode
  works: `tools/xbox_upload.sh --data` (active mode is the default). NLST/listing also answers 502, so the
  script cannot see the drives and copies to E:\Games\MH1 (where the game looks). About
  0.4 MB/s over the laptop's Wi-Fi via the mesh node: ~40 min for the 925 MB of disc files.
  The owner types the login; agents never enter it.
- The HDMI adapter in the capture chain is powered by the tall monitor: do
  not blank or switch off the tall monitor (e.g. `tie 1 1`) while capturing,
  or the capture card shows only colour bars.
- Pad presses only reach the Xbox while the KVM has it active (`goto xbox`); the owner's mouse
  pulls focus back to the laptop.
- Menu presses: take a capture after every press before pressing A. Main menu order is Play Games
  from Hard Drive, Trainers, Emulators, DVD, Launch Applications, MS Dashboard, Skin, System
  Configuration, Reboot, Power Off.

## Network loader (11 Oct 2026; one per game since 16 Oct 2026)

So builds reach the real Xbox without the FTP login (agents never type it), a
small loader (src/pc/xbox/loader/main.c, built by `make -C src/pc/xbox/loader`
after `. ~/xboxdev/env.sh`) is installed once per game by `tools/xbox_upload.sh --loader`
(the owner runs it and types the login): `E:\Games\MH1\default.xbe` (Evolution X lists it as
"Monster Hunter") and `E:\Games\MHG\default.xbe` ("Monster Hunter G"), each with a
`loader.txt` ("<laptop address>:8765" and "game=mh1" / "game=mhg"). It is one program built
twice with a different XBE title. Which game it serves: the "game=" line of loader.txt in the
folder it was started from (XeImageFileName, src/pc/xbox/xbox_path.h), else the folder's name
(MHG: G, anything else MH1). On each start the loader:

1. gets an address by DHCP (nxdk lwIP), reads the laptop address from its folder's
   `loader.txt`;
2. POSTs the last game log (`mh1log.txt` in its folder, the game's stderr) to the laptop
   (`/log?game=mhg`) and deletes it; with it the game's save (`E:\UDATA\<title id>\<save id>\`, POST
   `/save?game=G`, since 21 Oct 2026), which xbox_serve.py keeps in `build/xbox/saves/<time>-<game>/`;
   `python3 tools/xbox_save_to_pc.py build/xbox/saves/<that folder>` makes the PC card folder from it
   (`<folder>/pc_card/BISLPM-...`) and prints the `MH1_SAVE_DIR=... mhview ...` line that plays it on the PC;
3. compares `GET /version?game=G` with `mh1.ver` / `mhg.ver`; when different downloads
   `GET /mh1.xbe` / `/mhg.xbe` to `mh1.xbe` / `mhg.xbe` in its folder;
4. starts that game (D: is then the loader's folder, so the game finds D:\data).

When the game exits (or Back + Start is held for 2 s: the box has no in-game
reset), src/pc/xbox/xbox_log.c shows the log tail for 10 s and starts the
loader in its own folder (`<folder>\default.xbe`, launch data "MH1X"), which only
sends the log and goes back to the dashboard. A game XBE that is itself
default.xbe (installed by hand) goes straight to the dashboard. The next start
(pad: A on "Play Games from Hard Drive", A on "Monster Hunter" or "Monster Hunter G")
fetches the newest build. A first version (11 Oct) let the game start the loader
again, which then polled the laptop every 3 s: the Xbox's network froze after 5 such
connections (ping dead too, cause unknown; nxdk's lwIP debug output is off by
default, so not that). Each start now makes at most 4 connections (log, version,
build, test script), and the loader started after a game makes one (the log). From
11 to 16 Oct the game went straight to the dashboard and the log waited for the next
start; the return to the loader is back since 16 Oct (tested in xemu, not yet on hardware).
If the laptop does not answer, the loader runs the copy already on the disk; with no
copy it says so and goes back to the dashboard after 30 s.

Laptop side: `tools/xbox_serve.py` (port 8765) serves both games at once: MH1's build
dist/xbox/MH1/default.xbe (`--xbe` for another, e.g. build/xbox/nv2a/default.xbe) and G's
dist/xbox/MHG/default.xbe (`--xbe-g`), saves each log to build/xbox/logs/<time>-<game>.txt and
prints it. Requests without `?game=` (the loader installed before 16 Oct) count as MH1, so an
old MH1 loader keeps working. A new loader talking to an older xbox_serve.py gets 404s and
starts the copy on its disk: restart the server after updating. Loop for testing on hardware:
build_xbox.py, start from the pad, watch the server output and the capture card. A hang (no
exit) still needs the owner at the power button. First results (11 Oct): the null-graphics build
boots, finds D:\data and the controller, sets up quest 10 and runs (about 13,000 loops a second
with nothing drawn); Back + Start exits cleanly.

**Tested in xemu (agent D, 16 Oct 2026):** `tools/xemu_loader_test.sh` (hidden, a scratch disk
image in build/xbox/loader_xemu, never xbox_files/hdd.img; a test DVD, src/pc/xbox/testlaunch,
starts the loader from E:\Games\<game> as the dashboard would). Results under "Two loaders" below.

### First frame on hardware (11 Oct 2026)

The nv2a build (served by the network loader) drew the memory card check
screen on the real Xbox: Japanese text with its outline, correct (owner's photo
of the TV). The next frame halted the GPU on SET_TEXTURE_CONTROL1 pitch 4
(fixed: pitches padded to 64 bytes). To check: the background looked grey in
the photo (camera exposure or the clear colour).

### Boot sequence on hardware (11 Oct 2026, nv2a build)

Seen through the capture card, in order: memory card check, violence warning,
Sofdec/ADX logos, ACCESS AVE-TCP, CAPCOM logo, page turn, opening movie
(CAPCOM Presents, sky, Aptonoth and Apceros herds) with correct colours after
the movie moved to RGBA textures (docs/REVIEW.md). Open:
- CAPCOM logo: was a small copy plus a magnified, wrapped one. Fixed: the
  logo is two 2D quads from a 256x256 P8 texture (capcom_logo, demo.c), and
  the NV2A vertex cache kept the previous screen's full-screen quad because
  the vertex ring restarts at offset 0 every frame; gfx_nv2a.c now sends
  NV097_BREAK_VERTEX_BUFFER_CACHE before each draw's arrays. (An earlier note
  here said "a clay": wrong, the trace had stopped before the logo. Use
  `RT_STEP=1 ... --boot --frames 470 --time 100` for a repeatable logo frame.)
  The light frame after the page turn went away with the same fix.
- The violence warning's parchment background is much darker than on the PC:
  not the renderer, see "Violence warning darker on hardware" below.
- One frame after the page turn shows a plain light screen; check against PC.
- Movie smoothness not measured yet (captures were 1 per second).

### Dumping the BIOS for xemu (11 Oct 2026)

src/pc/xbox/dumper (`make -C src/pc/xbox/dumper`) reads the top 1 MB of the
flash window (physical 0xFFF00000, read-only mapping) and writes it to
E:\Games\MH1\mh1log.txt after a "MH1DUMP flash.bin" line. Serve it with
`tools/xbox_serve.py --xbe build/xbox/dumper/default.xbe`, start Monster Hunter
twice from the pad (once to run the dumper, once so the loader sends the file);
the server saves build/xbox/dump/flash.bin (git-ignored, Microsoft's file).
It does not dump the EEPROM (this console's keys; xemu makes its own) and
cannot read the MCPX boot ROM (hidden after start-up).
Why not over the network: a program started right after one that used the
NIC hangs in nxNetInit, because nothing called nxNetShutdown() before
XLaunchXBE. The loader source now does; the copy installed on 11 Oct does not.

### xemu works (11 Oct 2026)

The owner supplied an MCPX 1.0 boot ROM (md5 d49c52a4102f6df7bcf8d0617ac475ed)
and a Complex 4627 BIOS (1 MB, md5 ec00e31e746de2473acfe7903c5a4cb7) as
xbox_files/mcpx.bin and bios.bin (git-ignored; the BIOS can be swapped for this
Xbox's own dump later: the dumper read 768 KB of the top MB then froze in the
MCPX overlay; it now reads the mirror at 0xFFE00000, untested). xemu 0.8.136
runs hidden on its own Xvfb display (tools/run_xemu.sh, Xvfb unpacked without
sudo into ~/xboxdev/xvfb/root, Mesa llvmpipe): no window on the owner's
screen. `tools/run_xemu.sh --gfx nv2a --data disc/mh1 --shot out.png --after 32`
shows the ACCESS screen; the Xbox boot animation at ~10 s, Sofdec/ADX at ~30 s.
Open: the HDD image is blank (E: saves and the log file need a formatted disk).

### Black screen after the CAPCOM logo in xemu (11 Oct 2026, fixed)

The game's log in xemu: the HDD has no E:, so xbox_log.c also writes the log
lines (rt_log, notes, "alive: frame N, S s" every 5 s) to COM1. xemu emulates
the debug kit's LPC47M157 Super I/O when started with `-device lpc47m157
-serial file:...`, which tools/run_xemu.sh now does: the log is
xbox_files/xemu_debug.txt. The game checks the UART's line status register
first (a read only; a retail Xbox has nothing there and reads 0xFF), so
nothing is written on hardware. `--after "34 36 40"` takes a series of
screenshots (out_34s.png ...).

What happened: CAPCOM logo at ~33 s, then black for the whole opening movie
(the demo's own timer, 5625 ticks, then the title). The game was running all
along (60 frames a second). The movie stayed on its first frame (black)
because its clock is the audio: nxdk's AC97 driver gets one buffer callback
in xemu 0.8.136 and then no interrupt (also with `-audio none` or
SDL_AUDIO_DRIVER=dummy), so the mixer never consumed a sample. That part is
an xemu limitation (all game audio is silent there). Fix in rt_movie.c: when
the audio device has an open stream queued but has played nothing of it for
a second, the movie is timed by the game tick instead (silent), with a log
warning. That then showed a second bug: with nobody listening, audio_feed
threw away all queued audio every tick, so sfd_audio_fill read about a
second of the stream per tick and the unread video piled up (13 MB after 2 s;
malloc failed and the Xbox stopped with a fatal error 0x1E, a memcpy to
NULL). The audio is now thrown away only as fast as the movie plays (also
the PC's headless runs). With both, xemu shows the whole opening movie
(CAPCOM Presents at ~38 s, the herds, Rathalos, the hunters) and then goes
on to the title and the logo loop. xemu decodes it at about 20 fps, so it runs
somewhat slower than real time.

Also seen in that log: nxdk's printf has no floating point (%f is skipped
without taking its argument, so everything after it in the line is garbage),
e.g. the summary line and the prof: lines. Fixed the same day (next section).

### Floats in Xbox logs; violence warning brightness (12 Oct 2026)

printf with floating point: nxdk's pdclib skips %f/%e/%g/%a without taking
the argument (_PDCLIB_print.c), so everything after a float in a line was
garbage ("fps (2225186295 frames)", empty prof: lines). xbox_libc.c now has
the whole printf family (printf, fprintf, sprintf, snprintf and the v*
forms): float conversions are formatted there (digits from a scaled 64-bit
integer: 15-17 significant digits, ties to even), every other conversion is
handed, one at a time, to pdclib's snprintf. xbox_compat.h renames the
names before <stdio.h> declares them; it is force-included in every C file
of the Xbox build (front end and game C; not xbox_libc.c / xbox_log.c). The
game C itself prints no floats (its sprintf/vsprintf are for on-screen text
with %d/%s), but goes through the same functions. rt_log.h's format attribute
says __printf__ so the rename does not disable the format checks. Checked
against glibc on 200,000 random values (differences only in the 17th digit
and in values just next to a rounding tie) and in xemu: the prof: lines and
the summary show correct numbers.

Violence warning darker on hardware: not the renderer. How it is drawn
(violence_logo, demo.c): one flps0008 sprite, the 256x256 parchment texture
(P8 on the Xbox) stretched over the 512x448 frame, vertex colour 0xFF606060
(fl's 0xFF = 1.0 scale, so 0.38 of the texture, the same on PS2, PC and
Xbox), then the two lines of text. Measured (mean RGB of the 4:3 picture):
PC (RT_SHOTS=240 ... --boot --time 14) 60,28,17; xemu nv2a build (at about
42 s of xemu time this run; boot timing in xemu varies by 10 s between runs,
so take a shot every second) 59,27,17: the same, and the picture matches
the PC's (emblem clearly visible). The capture-card picture of the real Xbox
(seq_tile.jpg, 11 Oct) has 30,9,4. Comparing percentiles of the hardware
capture with xemu, the hardware picture is the xemu picture minus about
25-30 levels at every brightness: parchment median red 60 -> 31, green
23 -> 3, blue 13 -> 0; the ACCESS screen's white 255 -> 225-239. A uniform
level drop over every screen (ACCESS is a different texture path) points at
the video output chain, not at our drawing: the framebuffer is right (xemu
reads the same surfaces), pbkit and XVideoSetMode load an identity gamma
ramp. The Evolution X dashboard capture (xbox2.jpg) also has its dark
background text almost invisible. Candidates for the owner to check: the
console's video standard (NTSC-J has no 7.5 IRE black-level setup; an
adapter expecting NTSC-M's setup would crush the darkest ~20 levels), and the
HDMI adapter / capture card brightness. Dark game screens (this one, night
areas, caves) will look too dark through this chain; a check with a retail
game's dark scene through the same chain would settle it.

### Scripted play in xemu: the first quest loop runs (agent F, 12 Oct 2026)

**How far it gets.** In xemu (nv2a build, hidden) the scripted run goes
power-on -> logos -> opening movie (skipped with start at tick 700) -> GAME
MENU -> NEW GAME -> character creation (name TEST) -> save to E: -> village
(first-visit event, Elder's talk, the house) -> quest 131 accepted at the
Elder -> camp -> area 1 (Aptonoth herd) -> two kills, carves -> delivery at
the camp box -> CLEAR -> reward screen -> village -> house bed save; a second
boot with CONTINUE loads that save (money 1550). Times in the serial log of
one run: village at 53 s (tick 1787), quest start 108 s (t4028), area 1
110 s, clear 190 s (t7448), reward 197 s, back in the village 209 s (t8301).
Every screen was compared with the PC build at the same game tick (RT_HOLD
in xemu, RT_SHOTS on the PC; 12 ticks from character screen to the village
after the quest): the pictures match apart from the 4:3 frame. xemu draws
20-25 fps in the field and ~37 in the village on this laptop (Mesa llvmpipe,
software GL, while other agents load the CPU): not a measure of the Xbox.
`tools/xemu_quest_loop.sh` runs the whole thing (about 10 minutes) and
checks the CONTINUE money.

**Scripted input and test aids on the Xbox.** No command line, so:
- `D:\data\input.txt`: a pad script in the PC's `--input` format, played
  from power-on instead of the controller when the file exists (viewer.c,
  the same reader as `--input @file`).
- `D:\data\env.txt` (or `E:\Games\MH1\data\env.txt`): NAME=VALUE lines read
  by the Xbox build's getenv (src/pc/xbox/xbox_env.c; pdclib's always
  returned NULL), loaded first thing in main, so every RT_* test aid and
  trace of docs/pc.md works on the Xbox.
- `tools/run_xemu.sh --input FILE|SCRIPT --env FILE` puts both on the data ISO.
- RT_HOLD=t1,t2,... (RT_HOLD_SECS, default 20): the game clock stops at
  those ticks while drawing goes on; xemu screenshots then show an exact
  tick (find the held frames as identical consecutive screenshots: xemu's
  clock drifts against the host's, so the screenshot time of a tick varies).
- The PC tests' scripts work unchanged after the movie skip: the movie
  starts at tick 412 without a save (558 with one) on both builds; replacing
  the scripts' `idle*600,start*2,idle*60` by `idle*700,start*2,idle*200`
  shifts everything after it by 240 ticks; village/player-tick test aids
  (RT_LB_WARP, RT_PL_WARP) count their own ticks and need no shift.

**E: in xemu.** `tools/xbox_fatx.py format xbox_files/hdd.img` writes an
empty FATX file system on E: (Partition1 at 0xABE80000, 0x1312D6000 bytes;
the layout of nxdk's nxFormatVolume); the 4627 kernel mounts it.
`tools/xbox_fatx.py ls|get IMG PATH` read files back: the game's stderr log
(E:\Games\MH1\mh1log.txt, now with the RT_* traces), saves in
E:\UDATA\4D480001. xbox_log.c makes E:\Games\MH1 itself. Only with xemu
stopped (it caches the disk).

**Bugs found and fixed on the way** (all would also have hit the real Xbox):
- nxdk's pdclib: fopen "w+" fails always (CREATE_ALWAYS|TRUNCATE_EXISTING
  is an invalid disposition): no new save could be made. mc_xbox.c creates
  with "wb" and reopens "r+b".
- pdclib's "r+" stream refuses writes until a seek (C's rule for switching
  read/write): every save file had 0 bytes. mc_xbox.c seeks before each
  read/write. The save written in xemu equals the PC's but for 7 bytes
  (time stamps). (A general fix in xbox_libc.c's fopen wrapper would cover
  future users; not done here, that file was another agent's.)
- pdclib's scanf converts no %f/%e/%g: every sscanf with a float failed on
  the Xbox (the village/player warp aids, --cam, --follow). xbox_scanf.c
  replaces sscanf (floats here, the rest through pdclib's vsscanf).
- The hunter floated ~1 m above the ground everywhere on the nv2a build
  (very visible on the bed): the ground offset read the CPU skinning
  buffers, which nv2a (GPU skinning) does not have. fl_model_min_y skins on
  the CPU for that measurement (docs/REVIEW.md).
- Viewer catch-up: in real-time runs every lost tick was run before the next
  frame; with ticks dearer than 33 ms (the opening movie decodes a frame per
  tick, ~25 ms in xemu on a busy host) xemu showed one frame a second and
  never recovered (also in agent E's log). Now at most 4 ticks per frame, the
  game slows down instead, with a "slow:" log line (mostly at loads now).
- Motion sets leaked: create_plcom/pl/em_motion write new handles over the
  table entries of the last load each quest and village visit, and the PS2's
  releases (all_reset, stage_free, em_motion_free) are stand-ins here; about
  0.5-0.7 MB a loop. rt_motion_gc (at each st_model_load) frees every set no
  table entry and no player/monster motion player refers to; slots are
  reused. PC soak (RT_MEM, village before / after loop 1 / after loop 2):
motion sets 1344 / 1231 / 1223 KB (were 1788 / 2311 / 2836), tracked heap
20.5 / 23.1 / 23.1 MB (were 21.0 / 24.2 / 24.7). (A first try
  that made flReleaseMotionSetHandle real broke the quest loop's carving:
  all_motion_free on the character screen then also freed the player's
  weapon motions, which the host loads once.)

**Memory (the main open problem).** Free physical memory (MmQueryStatistics,
now in RT_MEM reports and on every "stage a -> b" log line of the Xbox
build), measured with RT_MEM on (its table costs 768 KB on the Xbox; it was
12 MB and made the game run out at the village, now 16-bit):
character screen 14.9 MB free, village 8.7 MB, field 6.0 MB, reward 4.8 MB.
With RT_MEM on, the game ran out of memory (fatal error 0x1E) on the return
to the village after the quest; without it the loop and a second quest start
went through, so the margin is about 1 MB at that point (measured before the
motion-set fix; it saves ~0.7 MB a loop). Per loop the PC's tracked heap
still grows (village before vs. after: textures as the Xbox keeps them
+0.8 MB, loaders' kept files +1 MB, host models +0.9 MB, mostly in the first
loop; in loop 2 only the textures still grow, +0.84 MB, 6.7 -> 7.55 MB
in the village; the CPU heap stays flat after the motion fix): monster models are cached per kind (em_kind_model) and never freed,
release_texture / all_model_free / stage_free are stand-ins. Next: free the
host models and textures of monster kinds and stages no longer in use
(like the motion GC), then re-measure free memory over several quests in
xemu.

After the motion fix, without RT_MEM (tools/xemu_quest_loop.sh, free KB on
each "stage a -> b" line, i.e. what the stage being left still had): village
9.9 / 10.0 / 9.5 MB, quest start 7.6 MB, area 1 7.4 MB, camp 6.6 MB, back in
the village 6.1 MB, the next quest start 5.9 MB; CONTINUE boot: 10.6 MB in the
village. About 4 MB less in the village after one quest than before it:
that is the cached monster models/textures and kept files above.

**Still open.**
- Audio: silent in xemu (its AC97 stops after one buffer); not testable there.
- Memory as above; a soak test over several quests (the second loop needs
  its own village warps).
- The slow-motion fallback only hides a CPU problem: on hardware, check the
  movie decode time (prof lines) and the field frame rate.

### Test runs on the real Xbox through the loader (12 Oct 2026)

`tools/xbox_serve.py --xbe build/xbox/nv2a/default.xbe --input FILE --env FILE` serves a
test run's scripted input and RT_* switches as GET /test ("MH1TEST <n_input> <n_env>\n" + both).
The loader (source; needs one owner install with tools/xbox_upload.sh) writes them to
E:\Games\MH1\data\input.txt and env.txt, which the game reads as D:\data\... (viewer.c,
xbox_env.c), and removes them when the server has none, so a normal start never plays a stale
script. One extra connection per start (4 in all). With this, the xemu quest loop's input
(tools/xemu_quest_loop.sh) can run on hardware with nobody at the pad.

### Memory stays flat over quest loops (agent F, 12 Oct 2026)

**Soak test.** `tools/test_soak.sh [LOOPS]` (PC, RT_MEM, ~25 s a loop) and
`tools/xemu_soak.sh [LOOPS]` (xemu, hidden, ~4 + 2.5 min a loop) play NEW GAME
and then LOOPS times village -> Elder (quest 131 accepted again) -> gate ->
camp -> area 1 -> two Aptonoth carved -> delivered -> reward -> village, in one
run. tools/mk_soak.py writes the pad script and env: later loops repeat loop
1's aids with `RT_AID_PER_QUEST=1` (player-tick aids count from each quest's
start) and the pad script waits for the game with `@villageN` / `@questN`
(pad_sdl.c), because a quest's clear comes up to 10 ticks earlier or later
from loop to loop. On the PC every "stage a -> b" log line now ends with the
tracked heap and the textures as the Xbox keeps them (with RT_MEM); on the
Xbox with the free physical memory, as do the serial "alive" lines (every 5 s).

A bug the soak found first: the second delivery quest of a session was
cleared 4 ticks after it started. init_pl_work clears each player work before
a quest (memset 0xA00); the host's rt_player_game_init did not, so the
delivery stock (PLW+0x8F4) still held the first quest's two raw meat. It is
cleared now (only that field; the host keeps its own fields in the work).

**What leaked and what was done** (docs/REVIEW.md has the entry):
- 2D textures: every load_pit / load_texlist / mkTexture wrote new handles
  over mem_tex slots (release_texture is a stand-in): 10 textures, ~0.8 MB a
  loop, and the 1024-entry table would have filled after ~100 loops.
  rt_2d_gc frees what no mem_tex slot holds (24 title textures at the first
  stage load, 5 per stage change after that).
- Motion files: plcom (1 MB), lbcom (1 MB), selcom (250 KB) and the NPC motion
  files (930 KB) stayed loaded; also a second plcom copy in the hunter
  (hunter_load, 1 MB). rt_village_files_gc frees a file once no live motion
  set reads it; the motion table's part for a model is emptied before
  create_*_motion fills it again (lbcom has more motions than plcom: its tail
  kept the old sets), and rt_motion_gc no longer counts the motion players of
  unused works, mot1 outside a blend or an unmixed bset.
- lbcom's motions run past the common part of the table into the hunter's
  (entries 500-521), replacing some weapon motions after the first village
  visit; the weapon motions are now made again at every quest start
  (weapon_motions in viewer.c, as the PS2's pl_motion_load).
- Monster models per kind (em_kind_model, with the free-play Rathian loaded at
  start: ~1.6 MB with its 1 MB motion file) and the NPC models stayed loaded;
  host_models_gc frees them at stage loads when nothing uses them, the game
  loads them again (em_create_model / npc_create_model).
- Static/heap trims: collision areas 96 KB + 192 KB (were 2 x 512 KB; the
  largest lw / lg files on the disc are 81204 / 164352 bytes, checked over all
  AFS entries); the RT_POSE_CHECK tables (375 KB of .bss) are allocated only
  when that test aid is on.

**PC numbers** (tools/test_soak.sh 3, KB when the village is left for the
quest, tracked heap + textures as the Xbox keeps them): before 29175 / 30747 /
31588 (+841 a loop); after 20672 / 20671 / 20671. Heap alone when leaving the
village: 22926 / 23441 / 23440 before, 18140 / 18139 / 18139 after; textures
6249 / 7306 / 8148 before, 2532 after.

**xemu numbers** (free physical KB on the "stage a -> b" lines, nv2a build):

| | before (12 Oct, after the motion fix) | after |
|---|---|---|
| village (first visit) | 9.9-10.0 MB | 16.5 MB |
| leaving for quest 1 | 7.6 MB | 17.1 MB |
| area 1 | 7.4 MB | 19.2 MB |
| back in the village after quest 1 | 6.1 MB | 16.2 MB |
| leaving for quest 2 | 5.9 MB | 16.3 MB |
| back in the village after quest 2 | - | 15.7 MB |
| leaving for quest 3 / area 1 / village after 3 | - | 16.2 / 18.2 / 15.8 MB |

tools/xemu_quest_loop.sh passes (CONTINUE 1550z). The PC heap is flat to the
KB from loop 2 on; in xemu loop 2 -> 3 still lost ~130 KB at every point
(16320 -> 16188 leaving the village, 18364 -> 18220 in area 1). Not found:
nothing the PC tracks grows, so it is something the Xbox build has alone
(pdclib's malloc keeping freed pages fragmented, nxdk/SDL, contiguous GPU
allocations, which are page-rounded: textures cost ~10% more than counted).
At 130 KB a loop that is ~100 loops of headroom; a longer xemu soak would
show whether it levels off.

**Biggest remaining users** (PC tracked heap 18.1 MB leaving the village;
.bss 4.8 MB on the Xbox): main data image 1.2 MB, rt_data tables 1.2 MB,
overlay binaries 1.4 MB + data copies 0.5 MB, rt_lb_mem 2.2 MB (.bss),
data_load_ptr 2 MB (838 KB used; what can be loaded there disc-wide is not
known, so not trimmed), audio packs 1.7 MB, host models 3-4 MB, audio streams
576 KB (.bss), mission_buf 256 KB (also takes downloaded event quests of
unknown size, kept), the 6 MB vertex ring (gfx_nv2a.c). Load cost to watch:
monster, NPC and motion files are now read again on area changes and village
visits.

### First quest loop on the real Xbox (12 Oct 2026)

With the /test loader installed (owner, Xbox at 192.168.1.118 that day), tools/xbox_serve.py
--input/--env with tools/xemu_quest_loop.sh's input1/env1 ran the whole loop on hardware with
nobody at the pad: character creation, the house, quest 131, hunt and carve, reward, village
(money 1500 -> 1550, carved items in the pouch), bed save (owner saw it save). Recording:
build/show/hw/run1.mp4 (local). Hardware numbers from the game's log (sent by the loader):
about 30 drawn frames a second in the field and village (890-950 frames per 30 s), no warnings.
After the save the hunter ended up standing at the Forest and Hills camp; the script had ended,
so probably one of the run's later RT_* warp aids firing (to check on the PC with the same env).
Not yet measured on hardware: free memory per stage (the KB-free lines go to the serial port,
which the retail Xbox lacks), movie decode time, audio.

### Sound on the real Xbox (12 Oct 2026)

The game was silent on hardware, like in xemu. Cause on hardware: the waits for the GPU in
gfx_nv2a.c (`while (pb_busy());`, `while (pb_finished());`) spun without giving the CPU away,
and the SDL audio thread never ran. A build that also wrote every log line to the disk played
sound (the disk writes paused the main thread), which pointed at it. Fix: SwitchToThread() in
those waits (and once per frame in the null backend). Confirmed by the owner on hardware with
a build without the log-to-disk mirror: music and effects sound right. The same starvation
made the Back + Start quit hang (closing the audio device waited on the audio thread); the quit
now also goes straight to the log screen and the dashboard (xbox_log_quit), and works.
xemu stays silent with the fix (its AC97 still stops after one buffer): that part really is
xemu's, so the movie's audio_driving fallback stays for xemu.
The log mirror (rt_log.c: every log line also to stderr = E:\Games\MH1\mh1log.txt, which the
loader sends) stays: it is how hardware logs reach the laptop.

### Owner's hardware report: spit, carcasses, fades, NPCs (agent E, 12 Oct 2026)

Four things the owner saw playing the nv2a build on the real Xbox. Each was first
looked for on the PC and in xemu at the same game tick (frames in agent E's scratch
folder, not committed).

**New Xbox test aid: `RT_ARGS`** in env.txt is the command line the Xbox lacks, e.g.
`RT_ARGS=--quest 131 --stage 39 --follow 250,150,-0.35 --cam 0,0,0,0,0`: the run then
starts like the PC test runs (straight into the quest, no power-on boot), so the
PC's tests/aids (tools/test_activities.py scenarios) run unchanged in xemu with
`RT_HOLD`. `REC_FPS=30 tools/run_xemu.sh ... --record` records faster than 10 fps.

1. **The BBQ spit did not show - on the PC either.** Not the renderer: no build drew
   the hunter's hand items at all (pl_item_trans was never called). Now the host
   calls it after the hunter in the field (rt_player_item_draw; weapon3_nm.c, with
   pl_item_trans_sub, PICK_X). Its near-match had `default: return` where the asm
   (0x164FEC) draws: type 0 items (the spit's stand and fire ring, the stool) were
   never drawn. The roast's colour by doneness (meat_col / bone_col: raw 255,210,192
   -> rare 216,180,141 -> well-done 177,150,90 -> burnt 99,79,65) is drawn from the
   material states the game sets (flSetRenderState 0x3A+k: ef_00 got a material
   table, rt_clay_mat_draw multiplies each batch by colour / file diffuse, only for
   the hand item). Before: camp, hunter crouched at an empty fire (PC and xemu).
   After: stool, stand with fire ring, the roast turning, darkening as it cooks; the
   xemu nv2a frames at ticks 330 and 400 (`--quest 131`, square at 70 and 250,
   `RT_PL_ITEMS=129:1,18:2`, `RT_PL_POKE=240:888:1`) match the PC's. Also draws
   pickaxe, bug net, whetstone, drinks, ... (whatever pl_item_trans places). The
   colour step is a guess about how the PS2's shader uses the material colour on
   this model (the lit family 0001 multiplies the light by it); the doneness change
   is subtle on the red meat texture.

2. **Carcasses flicker - not reproduced in xemu; cause found by reasoning, fixed.**
   PC and xemu frames of a carve (quest 131 stage 39, the carve_small aids, RT_HOLD at
   ticks 600-1150) match. skin.vs.cg divides by w and NV2A does not clip, so a
   GPU-skinned triangle with a corner at or behind the eye is drawn wrapped across the
   screen; the CPU clays had near clipping (execute_cpu), skinned monsters/hunters
   not. Carving puts the camera at the body: a PC trace (since removed) found the
   Aptonoth body with vertices behind w = 1 in ~1960 draws of a 40 s carve run. xemu
   draws through OpenGL, which clips, so it never showed there. Now each skin batch is
   tested with bind-space spheres per palette slot moved by their bones; a batch that
   may reach w < 1 is skinned on the CPU (skin.vs.cg's math, read from its vertex
   buffer) and cut at w = 1 like execute_cpu. Held xemu frames are the same with and
   without it (`RT_SKIN_NOCLIP=1` brings the old path back). Carve scenario: ~17% of
   skinned batches (~2100 vertices a frame) take the CPU path; in xemu that added
   15-25 ms a frame (TCG-emulated float code), on a 733 MHz P3 it should be 1-3 ms
   (reads of write-combined memory are slow; to measure: `RT_SKIN_CLIP_TRACE=1`,
   RT_PROF). **Owner to check:** carving a carcass and standing inside a big
   monster's body with the camera close: no more triangles across the screen.

3. **Screens flash as they fade in - not reproduced (PC and xemu fades match).**
   `RT_FADE_TRACE=1` prints the fade (req, alpha) of every drawn frame. Boot fades
   held in xemu at ticks 134-322 have the PC's brightness at the same tick (ACCESS
   screen 13/41/68/123 vs PC 14/41/68/123), and a 30 fps xemu recording of the boot
   fades has no bright frame. Things seen on both builds, faithful to the game code:
   the GAME MENU appears at full brightness one tick after the title's fade-out (no
   fade-in of its own); pressing start during a logo's fade-in restarts a fade-out at
   alpha 16 (a jump from dark to almost full). Hardware-only suspect fixed blind:
   textures were freed at once and the next screen's textures, created in the same
   tick, got the same addresses; NV2A caches texels and the palette by address (no
   cache-flush method is known here; xemu re-reads memory). Freed GPU memory is now
   kept 3 frames (docs/REVIEW.md). **Owner to check:** which screens flash (boot
   logos, title, menus, area changes?), whether the flash is the whole new screen at
   full brightness for a frame or wrong colours/blocks; with that the next step is
   clear (whole-screen flash: a frame drawn before its fade starts; wrong colours:
   the texture/palette cache).

4. **Village NPCs walk too fast and jump back - also on the PC; fixed in the host.**
   NPC models were posed without root_lock (the monsters got it on 9 Oct): the
   walkers' root motion was drawn on top of the game's movement (root bone up to 300
   units from the actor, ~200-unit jumps at each loop). npc_model_load sets it
   (docs/pc.md; test `test_activities.py npc_walk`).

Checks: tools/build_pc.sh, build_xbox.py (null and nv2a) 0 failed; PC
test_quest_loop and test_activities (bbq, carve_small, potion, whetstone, gather_*,
fishing, herbivore_rewind, npc_walk) pass; tools/xemu_quest_loop.sh passes with all
of it (CONTINUE 1550z). One earlier run failed only because run 2 ran out of its 90 s
under host load average 12 before CONTINUE; run 2 now gets 120 s.

### Near clipping of skinned models: removed for now, kept as a candidate fix (13 Oct 2026)

Owner's decision: undo agent E's CPU near clipping (b85749b0 + the clipping half of 8ba1612c)
until it is clear it fixes a real problem. The carcass flicker was never seen in xemu, and the
owner did not see it again either, while the frame rate felt worse. Removed from gfx_nv2a.c:
per-batch bounding spheres (c->bsph), batch_in_front, draw_batch_cpu, RT_SKIN_NOCLIP,
RT_SKIN_CLIP_TRACE. The deferred GPU free from 8ba1612c stays.

The problem it addressed (by reasoning, not seen on hardware): skin.vs.cg divides by w, so a
GPU-skinned triangle with a corner behind the eye can be drawn smeared across the screen;
the CPU clays have near clipping (execute_cpu), the skinned ones not. It would show when the
camera is inside or at a monster's body (carving, a hunter against a big monster).
Candidate fixes if it shows up:
1. CPU (the removed code; restore from b85749b0/8ba1612c): batches whose bounds reach the near
   plane are skinned on the CPU with the shader's math and cut at w = 1. Cost estimated 1-3 ms a
   frame on the real CPU in the worst case (17% of batches while carving), not measured.
2. GPU, no CPU cost: in skin.vs.cg, a vertex with w below the near limit is moved off-screen
   (or its triangle collapsed), so a triangle reaching behind the camera is not drawn at all
   instead of smeared. Loses those triangles (a hole very close to the camera), keeps 100% GPU.
3. GPU, exact: have the vertex program output clip-space positions and let the NV2A's own
   clipper cut at the near plane, if the hardware clips in programmable mode (not checked;
   xemu cannot tell, it clips through OpenGL). Needs a hardware test.
Check first on hardware: carve and stand inside big monsters, look for triangles across the screen.
Result (owner, 13 Oct 2026): with the CPU clipping removed the frame rate is "back to where it was",
so the CPU path was what slowed the game down on hardware. If the smear ever shows, prefer the
GPU-only candidates 2 or 3.

### Loader download froze again (13 Oct 2026)

Second time: after a long play session, Back + Start, then Monster Hunter from Evolution X: the
loader posted the log and the server sent the whole XBE, then the Xbox dropped off the network
(no ping) and hung until a power cycle. Both freezes came right after Evolution X (which runs its
own network stack and FTP server) launched the loader. Theory, untested: the NIC is still active
from the dashboard when nxNetInit starts it again (like the dumper's hang after the loader before
nxNetShutdown). To try at the next loader install: reset the NIC first (nvnetdrv stop/reset
before nxNetInit), and keep the 10 s receive timeout (it cannot help when the whole stack hangs).

## Keyboard and mouse on the Xbox (research, agent E, 14 Oct 2026; HID part built 16 Oct 2026)

Why: the PC's F8 bug reporter, F9 debug menu typing, the name entry and a free camera want a keyboard and mouse; on
the Xbox the debug menu works with the pad alone (Back + R3, docs/pc.md "Debug menu"). Read from nxdk's sources
(~/xboxdev/nxdk) and the RetroKM docs; no firmware, nxdk or game change was made. Statements marked [checked] were
read in the files named; [memory] is general knowledge not checked here; [design] is a proposal.

**What nxdk has today**
- USB host stack (lib/usb, libusbohci): the core (hub.c, ohci.c, usb_core.c) and the Xbox XID driver
  (libusbohci_xbox/xid_driver.c, usbh_xbox.c) are always built. HID, CDC, audio, video and mass storage are built only
  with NXDK_USB_ENABLE_HID / _CDC / ... set (lib/usb/Makefile) [checked]. Our link uses nxdk_usb.lib, which today has no
  HID driver (src_hid has no objects) [checked: Makefile; the object list from the research pass].
- The HID driver (libusbohci/src_hid/hid_driver.c, hid_core.c): hid_probe takes any interface of class 3 with an
  interrupt endpoint and records subclass / protocol (boot keyboard 1/1, boot mouse 1/2). API in inc/usbh_lib.h:
  usbh_hid_init, usbh_install_hid_conn_callback, usbh_hid_set_protocol(hdev, 0 = boot), usbh_hid_set_idle,
  usbh_hid_start_int_read(hdev, ep, callback). The callback gets the raw report and the transfer is resubmitted; there is
  no report parser and no key or mouse layer: we decode the 8-byte boot keyboard report and the mouse report ourselves.
  The callback runs in the OHCI interrupt's DPC (usbh_xbox.c): it may only copy into a buffer.
- inc/usbh_hid.h has `#define HID_DBGMSG sysprintf` switched on [checked], which ends in debugPrint: every HID probe or
  error would print on the screen. A HID-enabled nxdk_usb.lib has to be built with that line commented out.
- SDL2 on nxdk: SDL_config_xbox.h has the XID joystick driver and the video driver only, and
  src/video/xbox/SDL_xbevents.c XBOX_PumpEvents does nothing [checked]: SDL never produces keyboard or mouse events on
  the Xbox. SDL's joystick driver (src/joystick/xbox/SDL_xboxjoystick.c) is what starts the USB stack (usbh_core_init,
  usbh_xid_init) and polls the hubs each update.

**Route 1 (the owner's preference): the RetroKM Xbox pad dongle as one composite USB device**
The Atom S3R dongle (MULTI KM/xboxpad, TinyUSB on the ESP32-S3; descriptors in xboxpad/src/xidpad.c, one XID interface
today, a custom TinyUSB class driver in src/xid_usb.cpp) would offer three interfaces on one device:
- IF0: XID 0x58 / 0x42, EP1 IN + EP2 OUT, 32 bytes, 4 ms: as now, first [design];
- IF1: HID 3 / 1 / 1 (boot keyboard), EP3 IN, 8 bytes, 10 ms, the standard boot report descriptor;
- IF2: HID 3 / 1 / 2 (boot mouse), EP4 IN, 4 bytes (buttons, dx, dy, wheel), 10 ms.
Our stack can take that: usb_core.c parses every interface of the configuration and asks each registered driver's probe
per interface (first taker wins), xid_probe matches the interface class 0x58 / subclass 0x42 only (xid_driver.c line 97)
and not the VID/PID [checked]; limits MAX_UDEV_DRIVER 8, MAX_HUB_DEVICE 8, CONFIG_HID_MAX_DEV 8 are enough [research pass]. The device
class stays 0 (per interface), the VID/PID can stay the Controller S's. RetroKM already forwards the laptop's keyboard
and mouse to its agents over TCP 24850 (common/rkm_proto.h: RKM_KEY with HID usage codes, RKM_MOVEREL, RKM_BUTTON,
RKM_WHEEL; the hub grabs the USB keyboard and trackball through evdev); the dongle announces RKM_CAP_PAD only today and
would add RKM_CAP_KEYS / RKM_CAP_REL and turn those messages into the two HID reports. The usage codes are already the
boot keyboard's key codes.
Unknowns, to test on the real console: what the dashboard / Evolution X (XAPI's own USB stack, linked into each XBE
[memory]) does with a pad that has two extra HID interfaces: ignore them (expected [memory]: XAPI drivers match by
interface class; the Communicator is a multi-interface device too), or refuse the device. The game itself uses nxdk's
stack, not XAPI, so inside the game it should work once HID is built in.

**Route 2: a hub with separate devices.** hub.c is a full hub class driver (registered in usbh_core_init; the config
comments count four Controller S pads with their internal hub), so a keyboard and a mouse behind a hub on a pad port
(through a pad-port-to-USB adapter) should enumerate in nxdk the same way [checked: driver present; not run]. On the
dongle side a hub means extra hardware; the composite device is simpler.

**Route 3: keyboard and mouse over the network from the laptop.** nxdk's lwIP (lib/net/nforceif/include/lwipopts.h)
has NO_SYS 0, LWIP_UDP, LWIP_SOCKET and LWIP_NETCONN on [research pass]; src/pc/xbox/xnet_http.c already uses lwIP
sockets, but only the network loader links the network stack (NXDK_NET), not the game. Sketch [design]: the game opens a
UDP port (e.g. 24855) and reads it without blocking once per tick; the laptop (a RetroKM "xbox-net" agent) sends one
datagram per change or every 8 ms: magic u16, sequence u16, a 256-bit key bitmap of HID usages, mouse dx / dy since the
last datagram, buttons, wheel. Each datagram carries the whole key state, so a lost one heals itself; older sequences are
dropped; no datagram for 500 ms releases every key. Latency: under 1 ms on a cable, 2-20 ms with jitter over the laptop's
Wi-Fi: fine for menus and typing, jittery for mouse look. Costs: the network stack's memory and thread in the game, and
two known problems in docs/xbox.md (nxNetInit hanging when the NIC is still live from the loader, the stack freezing
after a few connections).

**How the game would read them [design].** One small module (src/pc/xbox/kbm_xbox.c) behind kbm_init / kbm_poll(keys,
dx, dy, buttons, wheel), filled by the HID callbacks (route 1 / 2) or the UDP reader (route 3). Simplest hook: turn its
changes into SDL_PushEvent (SDL_KEYDOWN / KEYUP with the scancode = HID usage, SDL_TEXTINPUT for printable keys,
SDL_MOUSEMOTION / BUTTON / WHEEL): viewer.c's event loop, the F8 / F9 / F10 menus and pad_event (name entry) then work
unchanged; pad_read's keyboard part reads SDL_GetKeyboardState, which pushed events do not fill, so pad_sdl.c would take
the key state from kbm under #ifdef XBOX (its key table is already in SDL scancodes = HID usages). The mouse camera
code needs relative motion only (no SDL relative mode, which asserts on nxdk). For route 1 / 2: build nxdk_usb.lib with
NXDK_USB_ENABLE_HID=y and HID_DBGMSG off, call usbh_hid_init after SDL's joystick init, and in the connect callback
set_protocol(0), set_idle(0), start_int_read.

**Risks to test on hardware:** the dashboard with a three-interface pad; TinyUSB on the S3 with the XID app driver next
to two HID interfaces (3 IN + 1 OUT endpoints: check them against the S3's endpoint limit) and OTA updates still working; the nxdk HID driver on
connect / disconnect and its DPC callback racing the game thread; the USB memory pool (MEM_POOL_UNIT_NUM 96, 512 KB) with
the extra pipes; power from a pad port with the dongle's Wi-Fi on; for route 3 the nxNetInit hang and the game's memory.

**Built (16 Oct 2026, agent E): route 1 / 2 in the game, for the bug reporter only** (next section): src/pc/xbox/kbm_xbox.c.
Differences from the design above: nxdk_usb.lib is left as it is; tools/build_xbox.py compiles nxdk's hid_core.c /
hid_driver.c into the game from a patched copy (build/xbox/usbhid/, HID_DBGMSG off). The HID driver must be registered
before SDL's joystick start (usbh_core_init is a no-op the second time; SDL's 500 ms enumeration then probes keyboards
with both drivers present), so pad_init calls kbm_init first. Boot-protocol interfaces only (subclass 1, protocol 1 / 2:
fixed 8-byte keyboard and 3-4-byte mouse reports, no descriptor parser); the connect callback only records the device, the
game thread sends set_protocol(0) / set_idle(0) / start_int_read; the DPC callback copies keyboard reports into a ring
and adds up mouse motion. No SDL events are pushed: pick.c reads kbm_key / kbm_mouse itself (the menus, name entry and
free camera do not use it yet). **Untested**: xemu attaches no USB keyboard to the guest here and no hardware was tried.

## Bug reporter on the Xbox (agent E, 16 Oct 2026)

Owner request: the PC's F8 bug reporter (docs/pc.md "Bug reporter") on the Xbox. How the owner uses it: docs/PLAYING.md
"Reporting a bug". src/pc/pick.c is now built for the Xbox too (it was compiled out: pick.h had empty stand-ins).

- **Combo: Back + Black** (select + R1), read from every game tick's pad in viewer.c read_pad -> pick_tick_pad (so a
  --input script can press it; the combo is taken out of the game's pad state). Free on the Xbox: Back + Start (held 2 s:
  quit), Back + R3 (debug menu), Back + L3 (settings) are the others. Back + White stays free; LT + RT combos were avoided
  (both are held in fights). Pressing Back first lets one or two ticks of select reach the game, as with the other combos.
  A USB keyboard's F8 freezes too.
- **Controller UI** (pick_pad, once per drawn frame while frozen; the viewer hands it the pad or the --input script, as
  for the debug menu): right / left stick move a drawn pointer (dead zone 20, up to 12 px a frame, squared), the d-pad
  1 px (3 px a frame when held); A down / up = the PC's mouse button (a click under 5 px, else a box from where A went
  down); X undo; Y the on-screen keyboard (4 rows of characters + Shift / Space / Del / Done; A types, X deletes, Y space,
  B closes); White hides the panel; Start saves; B or the combo again cancels. A USB keyboard types into the note (Enter
  saves, Esc cancels), a USB mouse moves the pointer (left = A, right = undo) (kbm_xbox.c, above). The panel uses the 5x7
  font at scale 2 with short lines (a TV at 640 x 480).
- **Id pass on NV2A** (gfx_nv2a.c): as the GL backend, each clay / 2D draw asks gfx_pick_cb for an id. The shaders are
  unchanged: the fog path paints the id (fog colour = id colour, fog factor (k, b) = (0, 1) so rgb = COLOR1.rgb, for the
  plain and the skinning program), the fade constant is 1, alpha stays texture x vertex alpha so the alpha test still cuts
  out; blending is held off for the pass (gfx_set_render_state does not switch it on while gfx_pick_pass; gfx_begin_frame
  switches it off and restores the game's state on the frame after). The render states the GL backend reports (blend,
  alpha / z functions, filter, clamp, fog, fade, texture size; the texture's address instead of a GL name) are tracked for
  the report. **No depth read** (pbkit keeps its depth buffer's address; it may be tiled): no hit position / nearest bone
  in Xbox reports. **No clip.gif** (the 14 MB frame ring does not fit the 64 MB console); everything else as on the PC.
  The id frame is the viewer's `goto redraw` (gfx_begin_frame twice without a flip; gfx_read_pixels waits for the GPU
  first). gfx_null.c (the headless build): stand-ins, a report there has a black picture and no objects.
- **Saving**: E:\Games\MH1\reports\report_<date>_<time>\ (G: E:\Games\MHG), from rt_log_dir's parent like the PC;
  GetLocalTime for the name. A "Bug report saved" line shows over the game for 4 s.
- **To the laptop**: src/pc/xbox/xbox_report.c. The network loader (loader/main.c) sends every finished report folder
  without a sent.txt in ONE POST /report?game=mh1|mhg (body "MH1REPORTS n", per report "R name", per file "F size name"
  + bytes, "E"; at most 24 MB, the rest next time) right after the log, in both its paths (normal start and after the game
  exited), then writes sent.txt into each folder (they stay on E:). tools/xbox_serve.py saves each to
  build/xbox/reports/<time>-<game>[-k]/ (+ xbox_folder.txt, the folder's name on E:). The online build (MH1_ONLINE) also
  sends at once when its network is already up (net_cpinet.c xbox_net_ready; laptop from the game folder's loader.txt);
  it never starts the network for this. Connections: the loader's start makes at most one more (5 with a log, the
  reports, the version, a new build and the test files; the old freeze was after 5 in a polling loop, docs above);
  the usual way is the after-game path (log + reports = 2).
- **Owner**: the loaders must be rebuilt and installed again (tools/xbox_upload.sh --loader) to send reports.

**Tested** (16 Oct 2026): tools/test_pick.sh part 5 (PC, the same pad code: RT_PICK_PAD=1 turns on the Back + Black combo
there; a script freezes, clicks, drags a box, types "hi" on the on-screen keyboard and saves; marks at the expected
pixels); tools/xemu_pick_test.sh in xemu, MH1 and G nv2a (all checks pass for both): Back + Black at tick ~400 of
quest 10, click + box + "hi" by script; the report on E: has 14 (MH1) / 15 (G) objects of the kinds stage, sky, hud,
monster, hunter, weapon, effect, the id buffer is flat (the first try showed texture noise: the id override was set after
the fog constants were already copied into the push buffer); the loader POSTed it, the laptop's copy is byte-identical,
E: got sent.txt. Frozen-frame screenshots with the panel and the on-screen keyboard: build/xbox/pick_xemu/run1_*.png
(not committed). Not tested: a real Xbox, a USB keyboard / mouse, the online build's direct send.

## Later idea: 5.1 surround (owner, 13 Oct 2026; not started)

- PC: SDL can open a 5.1 device directly. Xbox: the console's own Dolby Digital path is the APU's
  DSP running Microsoft/Dolby's encoder program (not shippable, and nxdk doesn't drive the APU);
  instead encode AC-3 on the CPU (an open encoder; the AC-3 patents have expired, the Dolby name
  and logo are trademarks) and send IEC 61937 frames on the AC97 S/PDIF DMA channel, which
  nxdk's hal/audio.c already feeds (it mirrors the stereo PCM there today). CPU cost unmeasured.
- Source of the positions: our mixer takes vol + stereo pan (audio_voice_play); the game's sound C
  (snd_nm.c) folds each 3D sound's position into the pan. 5.1 needs the direction passed through
  there and a direction-aware mixer. Shared code: every game version gets it.
- Reference: Wii Monster Hunter G mixes for Dolby Pro Logic II (the Wii has only analog stereo
  out, so matrix surround), so its code places sounds front/back: Capcom's own surround design.
  Read it for the placement rules, write the same in our C.
- Second reference (owner, 13 Oct): Monster Hunter Portable / Freedom (PSP, the owner's disc in
  disc/mhp) also has Dolby Pro Logic II output (owner's memory; not yet checked in the files). Its
  code is MIPS (Allegrex), the same instruction family as the PS2 (easier for us to read than the Wii's
  PowerPC), but nothing matches byte for byte (docs/survey/mhg_vs_mh1.md). Compare both: if Wii G and
  MHP place sounds the same way, that is Capcom's design rather than one port's.
- Belongs to the G modernization timeline; PC and Xbox; owner tests on a receiver.

### Invisible save prompt at the bed (agent E, 13 Oct 2026, fixed)

Owner's report (real Xbox): going to bed in the village, the game waited for "yes" on the
save prompt but no box was on screen; pressing the button blind saved. **Not a hardware
problem: the PC and xemu missed the box too.** PC (tools/test_quest_loop.sh run 1 with
`RT_STEP=1` and `RT_SHOTS` every tick around the bed) showed the hunter lying down, then
the memory-card "checking" / "saved" boxes, but never the prompt in between.

Cause: the bed (lb_pl_mv052, src/lobby/f/lb_village_nm.c) asks with SetDialogData(0x30) +
SetDialogYesNo and sets pNet+0xC each tick; the box is drawn by DispDialogData, called
only from text_lobby_trans_ot3, the function of the lobby's fourth prim (lb_cli.c:
Lbc_init_network_work stores `text_lobby_trans_ot3_o` in it; Local_main queues it on ot2).
`text_lobby_trans_ot3_o` is an lbtu3 alias name (config/lobby_aliases.txt) that
gen_rt_auto.py resolved only for ONLINE=1 builds; offline it was a generated no-op
stand-in (`nm`: `W text_lobby_trans_ot3_o` at its own address), so the prim drew nothing.
`pNet_c7`, which text_lobby_trans_ot3 reads, was likewise a stand-in function. Fix:
tools/build_pc.sh runs gen_rt_auto.py with RT_GEN_LB_ALIASES=1 in every build (the Xbox
build takes build/pc/rt_gen.c), docs/REVIEW.md has the entry.

Before / after: new test `tools/test_activities.py bed_prompt` (village start, warp to the
bed, square; font trace) FAILS on the old build ("save prompt drawn 0 times") and PASSES
with the fix (107 frames of "これまでのゲームデータを / メモリーカード(PS2)に保存しますか？"
with はい / いいえ). xemu nv2a build, same scenario via RT_ARGS=--quest 131 +
RT_VILLAGE_START/RT_LB_WARP and RT_HOLD=440: the prompt is on screen and matches the PC frame
(position, frame border, cursor). The same layer also draws the bed's later dialogs
("ゲームを終了しますか？", 0x37/0x39), now visible too.

Other windows checked on the PC (item box menu and lists, item shop list and its
"よろしいですか？ はい / いいえ", the Elder's quest list, quest card and "やってみるかの？
【はい】 いいえ"): all drawn before and after; they use other prims (Lb_shop_trans,
the talk window), not the dialog layer. Earlier xemu comparisons (12 Oct) matched those.

Also: RT_SHOTS was dropped at once when RT_STEP=1 was set (the past---time check used the
stepped clock); viewer.c now compares with --time.

Found on the way, **not fixed** (host flow, viewer.c): tools/pc_scripts/bed_save.txt ends
with dleft + circle on "ゲームを終了しますか？" (quit the game: yes). Local_main then returns
-1 and the host's quest_back() restarts the last quest, so the hunter appears at the Forest
and Hills camp. That is the "after the save the hunter ended up at the camp" seen on
hardware on 12 Oct. The PS2 goes back to the title there (to check); the host should do
the same.

**Owner to check on hardware:** bed in the house -> the save prompt box with はい/いいえ shows;
after saving, the "quit game?" box shows; answer いいえ to keep playing (はい currently drops
you at the quest camp, see above).

### Area change loads: timed, made faster, loading screens back (agent F, 13 Oct 2026)

**Problem (owner, hardware):** walking between areas, village <-> quest: a long pause with a
frozen or black picture, no loading screen.

**Timing in the log.** Every Xbox (and PC) log now has `load:` lines (rt_prof.c, wall clock):
- `load: area 39 448 ms: read 31/7/891K melt 161/6/1386K texdec 25/17 texup 89/17 model 72/2 ...`
  for named loads: `area N` (st_model_load), `quest start` (quest_from_village), `village enter`.
  Each zone is `ms/calls[/KB]`: read = disc/HDD reads, melt = Meltw, texdec = APX -> RGBA,
  texup = gfx_create_texture (Xbox: palette, swizzle, GPU memory), model = AMO/AHI set-up,
  motion, hit = collision, gc = the stage-change clean-ups, snd = sound packs, flip = loading
  screen frames; other = the rest.
- `load: pause 1544 ms, 1 frames, t210-214, st 21>21: ...` for any gap between two drawn frames
  of RT_LOAD_MS (default 150) or more, summed over consecutive slow frames. In xemu the field
  draws at 5-8 fps on this laptop, so use RT_LOAD_MS=1000 there.
The log comes back from the real Xbox through the loader as before (mh1log.txt).

**Where the time went (xemu, nv2a build, camp <-> area 1 of quest 131 four times:**
`RT_ARGS=--quest 131 --play`, `RT_PL_WARP=10,12250,10000;200,6750,11900;400,12250,10000;600,6750,11900`):

| | before | after |
|---|---|---|
| st_model_load (area span) | 3.0-4.7 s | 0.36-0.45 s (one 1.1 s while the host was busy) |
| of which reads | 2.2-3.7 s (14 files, 2.5-2.8 MB) | 0.01-0.06 s (7-8 files, 0.9-1.2 MB) |
| texture decode | 0.34-0.45 s | 0.014-0.075 s |
| texture upload | 0.16-0.29 s | 0.06-0.24 s |
| Meltw | 0.13-0.35 s | 0.12-0.18 s (quest loop, after the memcpy change) |
| whole frame gap at an area change | 3.7-7.5 s | ~1.1-1.5 s |
| boot: SLPM_654.95 load | 2.3 s | 0.1-0.2 s |
| first quest set-up pause (boot) | 2.8 s | 0.4 s |

Full xemu quest loop afterwards (tools/xemu_quest_loop.sh, passes; under the same host load the
run before the changes did not even reach the quest clear in its 480 s): village enter 28 ms +
area 87 349 ms, village stage changes 140-335 ms, quest start 1.6 s (melt 0.4, texup 0.24,
model 0.16, flip 0.17), area changes 0.33-0.46 s. PC (same scenario): area changes 18-35 ms.
xemu is not the Xbox: its CPU speed and IDE timing differ, so the owner's hardware log is the real
measure.

**What was slow and what changed:**
1. pdclib's fread copies byte by byte through a 1 KB buffer, one kernel read per KB. xbox_fread
   (xbox_libc.c, via xbox_compat.h) does reads of 8 KB+ on read-only streams with one ReadFile
   straight into the caller's buffer. The biggest part by far.
2. Sound packs: every area change re-read common00/01, the weapon, voice, map and monster packs
   (rt_snd_stage). Cleared packs are now kept aside and taken back when the same file is loaded
   again (2.5 MB -> 0.9 MB read per area change).
3. Textures (gfx_nv2a.c): the swizzle used a bit-deposit loop per texel and wrote scattered
   texels into write-combined GPU memory; now an incremental mask step into cached memory and
   one memcpy. APX 4/8-bit decode: one 32-bit store per texel.
4. Meltw: back-references that do not overlap are one memcpy (identical output on all 2312
   AFS_DATA entries).
Not done (ideas if hardware still shows long loads): decode APX straight to P8 + palette for the
Xbox (skips RGBA and the re-palettising hash), keep the previous area's stage models and textures
when memory allows (camp <-> area 1 back and forth), read the stage files in the background.

**Loading screens (what the PS2 showed).** The game draws them itself and calls flFlip, which was
a no-op on the port (as were Disp_load_start, Disp_NowLoading2, Disp_NowLoading):
- area change (f_game.c game2 step 2): flFlip x7, then DispWholeMap + flFlip twice, then
  st_model_load: the whole hunting-ground map (texture 0x156 frame, hunter marker) stays on
  screen while the area loads;
- quest start (game11): Disp_load_start: "NOW LOADING" on the parchment with the quest title;
- village stage changes (lb_village_nm.c): Disp_NowLoading before Lb_stage_load;
- Disp_NowLoading2 modes (game0/10/12/13, the online lobby): the same screen with load_msg /
  connect_msg text.
flFlip now shows what the tick drew since the previous flip as a frame at once (viewer.c
game_flip; the draws are recorded with gfx_rec as the boot screens are), empty segments skipped;
the game's own drawing code runs (disp1_nm.c, dsp02.c now built). Seen on the PC (RT_FLIP_SHOT
frames) and in xemu (screenshot during an area change shows the map, same brightness as the PC:
mean RGB 55,49,28 vs 55,48,25). Headless --shot runs draw no extra frames unless
RT_FLIP_SHOT=prefix is set (then each flip frame is written to prefix_<tick>_<n>.png).
The picture is held while the tick blocks; after the tick the game fades the new area in as before.

**For the owner to check on the real Xbox:**
- Walk camp <-> area 1 and village <-> quest: the map / NOW LOADING screen should show for the
  whole load, never a frozen game picture. Send the log (start Monster Hunter once more): the
  `load: area`, `load: quest start`, `load: pause` lines give the hardware times.
- If an area change still takes more than ~1 s on hardware, the zones in those lines say which
  idea above to do next.

### Loader download "freeze" (agent F, 13 Oct 2026)

Tested in xemu with a test copy of the loader (make ... XH_HOST='\"10.0.2.2\"' XH_PORT=8799,
`tools/xbox_serve.py --port 8799`, `tools/run_xemu.sh --xbe build/xbox/loader_test/default.xbe
--net`): DHCP, log POST, /version, download, /test and the game launch all work in xemu
(NIC shut down before XLaunchXBE; the game then starts). One real problem found: after the server
had sent the XBE, the loader wrote it with pdclib's fwrite (1 KB per kernel call) and the screen
said "Downloading build ..." for ~10 s in xemu, longer on a real disk possibly. That matches what
the owner saw ("the server sent the whole XBE, then the Xbox hung"). Now one WriteFile (2 s for
the whole loader start in xemu), and the screen says "Got N bytes in X ms. Writing it ..." and
"Written in Y ms".
Not testable in xemu (no Evolution X there): the theory that the dashboard leaves the NIC running.
Needs the owner: install the new loader (tools/xbox_upload.sh --loader, the owner types the FTP
login), start Monster Hunter from Evolution X a few times, and note the last line on screen if it
hangs again (no ping, before or after "Writing it"). If it still hangs before "Got N bytes", the
NIC-reset idea above is next. Building: never `make clean` in src/pc/xbox/loader (it deletes the
shared nxdk libraries; happened once on 13 Oct, rebuilt the same day).

### Audio after a quit and relaunch; xemu silence (agent F, 13 Oct 2026)
Owner: after Back + Start and a relaunch from the loader without a power cycle the game was silent
("movie 0: the audio device stopped playing it (0 frames played)"), the same as xemu always was.
nxdk's lib/hal/audio.c only calls SDL back when the PCM-out AND the S/PDIF-out DMA have both finished
a buffer, and drops completions after a "last valid buffer" interrupt; if S/PDIF does not run (xemu)
or a channel halted once, the callback never comes and the SDL audio thread waits forever. Our quit
also left the DMA running into the next program. src/pc/xbox/xbox_ac97.c replaces nxdk's driver
(same hal/audio.h API, linked before nxdk's library so its audio.obj is not pulled in): PCM-out alone
drives the callbacks (counted), S/PDIF mirrored but not waited on, halted channels restarted, init
stops the old DMA/interrupt, cold-resets the AC-link and resets both bus masters; xbox_log_quit
stops the DMA. Verified: xemu (nv2a build, --after 70) no longer logs "stopped playing" for the
opening movie (it did before, e.g. logs/20261013-015719). Quit + relaunch: not testable in xemu here;
owner to test on hardware (Back + Start, relaunch from the loader without a power cycle: sound?).
- Owner's hardware test of that driver (13 Oct, logs/20261013-163416 in the main tree): quit + relaunch still
  silent, 0 frames. Next build: after the cold reset the codec is set up (power-down bits cleared, waits for
  the ready bits, VRA off, master / headphone / mono / PCM out unmuted at 0 dB), and the log gets
  "ac97 open:" (GLOB_CNT/GLOB_STA, both bus masters' CIV/LVI/SR/PICB/CR, interrupt and callback counts, codec
  regs 00/02/18/26/2A/2C) and the same as "ac97 stalled:" when the movie finds the device not playing.
  xemu: GLOB_CNT reads 0 and a codec register access hangs, so the codec is only touched when GLOB_CNT kept
  the cold-reset bit (real hardware). xemu's S/PDIF DMA never moves (PICB stays 2048): the reason nxdk's
  driver, which waited on it, was silent there. Untested guess for hardware: the dashboard's DirectSound
  (XLaunchXBE(NULL) on quit) leaves the APU driving the AC-link; a power cycle resets it.

## Monster Hunter G on the Xbox (agent B, 14 Oct 2026)

**Build.** `tools/build_pc_mhg.sh` (G's PC build in build/mhg/pctree), then
`python3 tools/build_xbox.py --version mhg --gfx nv2a` (or `--gfx null`): the same steps as MH1 run inside
build/mhg/pctree (its build/pc holds G's objects and recorded commands), the front end gets -DVERSION_MHG,
output build/mhg/xbox[/nv2a]/default.xbe + mhg.iso. 803 objects, links with no undefined symbols (first try).
`./mh1 xbox DISC` with the G disc does all of it and makes dist/xbox/MHG (XBE + data/: AFS_DATA.AFS,
AFS00.AFS, AFS01.AFS, SLPM_658.69, ~1.1 GB); tools/disc_files.py knows G's files (sizes + CRC32, checked
on the folder and on the .iso; `disc_files.py game DISC` prints mh1 / mhg); `./mh1 linux|windows` build G for
the PC too since 15 Oct (docs/PLAYING.md "Monster Hunter G"). tools/xbox_install.sh sends a folder holding SLPM_658.69 to E:\Games\MHG.
xemu: `tools/run_xemu.sh --version mhg --gfx nv2a --data disc/mhg ...` (or VERSION=mhg).

**Identity, so MH1 and G install side by side** (src/pc/xbox/xbox_title.h, VERSION_MHG block): title id
4D480002 "Monster Hunter G" (MH1: 4D480001), saves in E:\UDATA\4D480002\4D48000200000001 holding G's PS2
save folder BISLPM-65869MHG (78032-byte save, checked in xemu after a new game), game folder E:\Games\MHG
(data in E:\Games\MHG\data). The XBE looks for its disc files in D:\data first, but only when that folder
holds its own program file (SLPM_654.95 / SLPM_658.69), so a G XBE started by the MH1 network loader
(D: = E:\Games\MH1, MH1's files) skips to E:\Games\MHG\data. The log is mh1log.txt in the folder of the
running XBE when that is on E: (XeImageFileName; the loader starts E:\Games\MH1\mh1.xbe and sends that
file), else in the game folder; the loader's test script/env (E:\Games\MH1\data\input.txt, env.txt) are
read from D:\data as before, also by G.

**How far G gets in xemu** (hidden, nv2a, scripted; build/mhg/xb/x3 and x4):
- power-on -> ACCESS / logos -> title -> NEW GAME -> character creation (name TEST) -> save to E: ->
  village intro (stage 87 -> house 86) -> out to Kokoto village (87), walking (tools/pc_scripts/newgame.txt
  + `@village1,idle*1500,down*100,idle*10,square*3,...`, env RT_NOMOVIE=1 RT_NAME=TEST). Pictures match
  the G PC build.
- A quest from the village is not possible yet on G (the quest counter's Lb_make_quest_tbl_local is a G
  stand-in, agent D). With the test aid `RT_ARGS=--quest 10` (+ RT_QUEST_STAGE=1 RT_EM_HP=30 RT_DMG_MUL=40
  RT_PL_GOD=1 RT_PL_WARP_EM=90-9000 RT_PL_TARGET=0:0): Rathian nest -> kill -> CLEAR (t2062) -> reward screen
  (t2406) -> village (t2916) -> house: the same ticks as the G PC build.

**Memory (free physical KB, "alive" / "stage a -> b" lines).** First G run: village 7.2 MB free, 3.4 MB at
the village's first load. Two G-only costs found and fixed (shared code, MH1 unchanged or better):
- fmt_mwo_unpack (melt.c) kept its worst-case output buffer (8x the packed size): G's overlays are packed
  (MH1's are not), and game.bin / sub_main.bin stay loaded, so ~6.5 MB were never used. Shrunk to the
  unpacked size (realloc).
- rt_bss_shadow: a pointer into .bss with no sized symbol got a block for the whole .bss of its image.
  G's symbols are generated and leave many small .sbss objects unnamed, so G made the whole main .bss
  (1.7 MB) plus sub_main / game.bin blocks (~2.2 MB in all). Now a block covers only the gap between the
  named symbols around the address (14 gaps, a few KB to 27 KB), unless a whole block already exists.
- G collision areas back to MH1's 96 / 192 KB (G's largest lwg parts are the same files: 81204 / 164352).
After: title ~19.8 MB, character screen/save 14-16 MB, first village load low point 10.6 MB, village
15.5-16.5 MB, quest 10 (Rathian nest) 20.8 MB, village after the quest 17.7 MB. (MH1 in the same xemu,
quest 10: 22.9 MB; MH1's village 16.5 MB, docs above.) PC tracked heap in the village: G 17.7 MB, MH1 17.6 MB.
Still G-only: rt_meta.c's 1.5 MB of .rdata (G's 31,500 symbols with names for the data import 1.0 MB,
MH1->G address maps 0.35 MB, pointer words 0.15 MB); the loaded image is 9.3 MB (MH1 7.9 MB). Only data
symbols and the names the import asks for are needed at run time: the next trim if G runs short.

**Frame time.** xemu (software GL on this loaded laptop, not a measure of the Xbox): quest 10 Rathian
nest G 8.1 fps, logic 22-25 ms/tick, draw ~115 ms/frame; MH1 in the same scene and session 7.6 fps, logic
24-28 ms, draw 115-157 ms: G costs the same as MH1 there. G village 24 fps in xemu.

**For the owner on hardware.** Copy G's disc files to E:\Games\MHG\data (FTP, ~1.1 GB; `./mh1 xbox` with
the G disc + tools/xbox_install.sh, or any FTP program). Then either install dist/xbox/MHG as its own game
(shows as "Monster Hunter G"), or serve the G XBE through the existing loader
(`tools/xbox_serve.py --xbe build/mhg/xbox/nv2a/default.xbe`): it runs from E:\Games\MH1, finds G's files
in E:\Games\MHG\data and its log reaches the server as usual. Check: boot to the title, NEW GAME, character
creation, save (E:\UDATA\4D480002 next to MH1's 4D480001), the village; the free-memory numbers in the
log ("alive" lines); frame rate in the village. Not testable yet: a quest from the village (G quest counter).

### Installing game data through the loader (agent B, 14 Oct 2026)

**Why.** Copying G's folder (dist/xbox/MHG: default.xbe 4.8 MB, data/SLPM_658.69 2 MB, AFS01.AFS 24 MB,
AFS_DATA.AFS 144 MB, AFS00.AFS 884 MB) to the owner's Xbox over Evolution X's FTP kept failing: transfers
stall or abort at random points (curl 55 / 426), no passive mode, no SIZE (so no resume), and a half-written
file stays locked until the Xbox restarts. Small files always get through. So the big files now go through
our own network loader over HTTP, and FTP is only needed once for the small loader.

**How it works.**
- Laptop: `tools/xbox_serve.py --data dist/xbox/MHG` (or `dist/xbox/MH1`, or a bare folder of the four disc
  files). At start it reads the folder once and computes a CRC32 for each file and for each 4 MB chunk
  (about 10 s for G). `/version` then ends in a `DATA` line; `GET /manifest` gives
  `MH1DATA <folder> <chunk> <files>` and per file `F <size> <crc32> <path>` + `C <chunk crc32s>`;
  `GET /data/<path>` answers `Range: bytes=N-` with 206 and streams the file in 1 MB pieces (never the
  whole file in memory). Only the listed files are reachable. The target folder is MHG when the folder holds
  SLPM_658.69, else MH1. For G the folder's default.xbe is installed too (E:\Games\MHG\default.xbe, so G
  shows as its own game) and no build is served (`/version` "none"); for MH1 the folder's default.xbe is
  served as the build (mh1.xbe), never written over the loader (the loader refuses a manifest that tries).
- Xbox (src/pc/xbox/loader/install.c, xh_get_range in ../xnet_http.c): on `DATA` the loader fetches the
  manifest before the usual build check. Per file: if `E:\Games\<folder>\mh1data.txt` already lists it with
  the same size and CRC it is skipped without reading. Otherwise the file on the disk (FTP leftovers, a cut
  copy, an older file) is read chunk by chunk and kept up to the first chunk whose CRC is wrong; the download
  starts there with a Range request. Each 4 MB chunk is checked in memory and then written with one
  WriteFile (the pdclib 1 KB fwrite lesson). A dropped connection or 10 s without data: "connection lost at
  N MB, resuming in 3 s", again from the last good chunk; it gives up after 20 tries in a row without
  progress (about 2 minutes). A finished file is read back and checked once more, then added to
  mh1data.txt. The screen shows one line per file (Checking / Copying N / M MB and the speed / OK).
- After that: for G, with no build served, "Starting E:\Games\MHG\default.xbe in 5 s" and G runs. For MH1
  the loader carries on as usual (downloads the build, starts mh1.xbe). If something could not be finished:
  "Install not finished. Start Monster Hunter again to go on from here." and the dashboard after 60 s; the
  next start goes on from the last good chunk.
- The trigger is the server: started with --data, every loader start checks the data (a few seconds when all
  is recorded in mh1data.txt); without --data nothing changes. No pad input needed.

**Tested in xemu** (hidden, test loader `make -C src/pc/xbox/loader OUTPUT_DIR=../../../../build/xbox/loader_test
XH_HOST='\"10.0.2.2\"' XH_PORT=8792`, server on port 8792, `tools/run_xemu.sh --xbe
build/xbox/loader_test/default.xbe --net`):
- a 47 MB folder of random bytes: all files arrive, byte-identical (read back with tools/xbox_fatx.py get);
- the real dist/xbox/MHG over the fake one (wrong files already there, same names): server killed during
  AFS_DATA.AFS at 56 MB and restarted 15 s later: the loader retried and went on at 56 MB; xemu then killed
  (power off) during AFS00.AFS at 88 MB on screen; the next start skipped the four recorded files, found
  84 MB of AFS00.AFS good on the disk (the last chunks had not reached the disk image) and went on from
  there (resumed at 592 MB after a second power-off); the last start read AFS00.AFS back (884 MB, CRC
  434fa486 as on the laptop), recorded it and started E:\Games\MHG\default.xbe: G booted from
  D:\data = E:\Games\MHG\data (AFS_DATA.AFS opened, SLPM_658.69 loaded). default.xbe, SLPM_658.69,
  AFS01.AFS and AFS_DATA.AFS read back from the disk image byte-identical (tools/xbox_fatx.py get; don't
  do that for AFS00.AFS: xbox_fatx.py holds the whole file in memory and that run ran the laptop out of
  memory). mh1data.txt is rewritten from the current manifest each start (stale lines drop out; changed
  after the test run, built, not rerun);
- xemu needs more than the 1.5 GB `ulimit -v` cap (it fails at "cannot set up guest memory" even at 3 GB;
  ran at 6 GB);
- xemu's NAT network gives 1.0-2.8 MB/s (so ~10 minutes for G there); the real Xbox on a wired LAN should be
  faster, not measured.
Not tested: the real Xbox (Evolution X launching the loader; the network freezes noted above were never
reproduced in xemu).

**Owner, to install G** (superseded on 16 Oct 2026 by one loader per game: see "Two network loaders" below):
1. Install the new loader once over FTP (two small files): `tools/xbox_upload.sh --loader` (asks for the
   FTP login; it rebuilds the loader first).
2. `./mh1 xbox disc/mhg` if dist/xbox/MHG is not there yet, then `tools/xbox_serve.py --data dist/xbox/MHG`
   and leave it running.
3. On the Xbox start "Monster Hunter" (E:\Games\MH1). It installs G into E:\Games\MHG and starts it. If it
   stops ("Install not finished"), just start Monster Hunter again: it goes on where it stopped. The leftover
   FTP files in E:\Games\MHG do not need deleting (good chunks are kept, the rest is overwritten; the
   FTP server's lock ends when the dashboard starts the loader).
4. Afterwards serve builds as before (`tools/xbox_serve.py`, without --data); G is also in the games list as
   "Monster Hunter G".

**Owner's first try (14 Oct 2026): stayed on the Evolution X screen, the server got no request.**
Rechecked in xemu with exactly what `xbox_upload.sh --loader` builds (default address; the built XBE differs
from the one in the main checkout only in 6 timestamp bytes), E:\Games\MH1\loader.txt pointing at a test
server, and E:\Games\MHG holding a half install like the owner's (default.xbe, SLPM_658.69, AFS01.AFS complete,
AFS_DATA.AFS 60 MB, AFS00.AFS 50 MB, empty mh1data.txt; set up with the new `tools/xbox_fatx.py put`): the
loader screen comes up, /version and /manifest arrive 1 s after DHCP, the first three files are checked OK and
AFS_DATA.AFS resumes. Nothing before the loader's screen touches the network or E:\Games\MHG (it sets the
video mode first, then mounts E:), so "stays on the Evolution X screen" means the XBE did not start at all.
Candidates, not confirmed: the FTP upload of the loader ended early without an error (xbox_upload.sh now reads
the size back from the listing and fails if it differs), or the half-copied G XBE (E:\Games\MHG\default.xbe,
listed as "Monster Hunter G") was started instead of "Monster Hunter", or a file still locked by a stalled FTP
transfer (restart the Xbox before trying).

**G on the owner's Xbox: no pad; G Xbox link (agent B, 14 Oct 2026).** G installed through the loader ran but
ignored the controller (even Back + Start): viewer.c's "G runs are always hidden" rule (for agents' PC runs)
also applied on the Xbox, and hidden means pad_set_headless (no real controller). Now PC only
(`#if defined(VERSION_MHG) && !defined(XBOX)`, the coordinator's fix). Checked in xemu (G nv2a, no script):
the log says `boot: window 640x480` (no "hidden") and `controller: "Original Xbox Controller"`; before, no
controller line. No other G-only switch touches the pad (pad_sdl.c is shared; the other MHG hide rule at the
command line is already PC only).
The G Xbox link failed after agent D's village work with "undefined symbol: lb_sys": lb_guild_g.c named it
through an asm label (`extern u8 g_lb_base[] __asm__("lb_sys")`), which skips win32's "_" prefix; the PC
(ELF) has no prefix so it linked there. Now a plain `extern u8 lb_sys[]` + #define. No other asm labels of that
kind in src/ (grep `__asm__("`); the link has no other undefined names.
tools/xbox_serve.py --data now makes the manifest again when a file in the folder changes size or time (checked
at each /version and /manifest), so a new G XBE copied into dist/xbox/MHG goes out without a server restart
(a server started before this change needs one restart).

### Two network loaders: one per game (agent D, 16 Oct 2026)

Owner's request: starting G updates G and starts it; starting Monster Hunter does the same for MH1.
What changed (details in "Network loader" above):
- The loader serves the game of its own folder (loader.txt "game=", else the folder name) and names it in
  every request (`?game=mh1|mhg`); its files are `<folder>\mh1.xbe|mhg.xbe`, `.ver`, `mh1log.txt`,
  `data\input.txt|env.txt`. tools/xbox_upload.sh --loader builds the program twice (titles "Monster
  Hunter" / "Monster Hunter G") and installs it into both folders with their loader.txt (the laptop's
  address found with `ip route get <xbox>`).
- E:\Games\MHG\default.xbe is now G's loader; G itself is mhg.xbe. A data install (`--data`) never
  lists default.xbe for either game (the loader also refuses such a list), writes into the loader's own
  folder, and is only answered to the loader of that game (`/manifest?game=mhg`; the list's MH1/MHG name
  must match the loader's game). After the install the usual build check runs: the folder's default.xbe
  is served as the build (/mhg.xbe), downloaded and started. `--data` may be given twice (both games).
- Leaving a game returns to that game's loader (xbox_log.c xbox_leave: `<folder>\default.xbe`, launch
  data "MH1X"), which sends the log and goes to the dashboard. With no network after a game exit the
  loader now goes to the dashboard instead of starting the game again (an old loop).
- G's logs: mh1log.txt was already written next to the running XBE (now E:\Games\MHG\mh1log.txt, sent
  by G's loader); rt_log's session logs went to E:\Games\MH1\logs for G too, now
  `XBOX_GAME_DIR\logs` (E:\Games\MHG\logs).

**Tested in xemu** (`tools/xemu_loader_test.sh`, 23 checks, all pass; hidden, scratch disk image
build/xbox/loader_xemu/hdd.img, server on port 8799, the loaders exactly as xbox_upload.sh builds them with
loader.txt "10.0.2.2:8799" + "game=mh1" for MH1 and no game line for MHG, so the folder name picks G):
1. G's loader, laptop up: GET /version?game=mhg, /mhg.xbe?game=mhg, /test?game=mhg; mhg.xbe on the disk has the
   served size; G started from E:\Games\MHG (no data on the scratch disk: "no SLPM_658.69 in D:\data or
   E:\Games\MHG\data"), its exit screen read "Last lines of E:\Games\MHG\mh1log.txt", it started G's loader,
   which POSTed G's log (/log?game=mhg) and went to the dashboard. default.xbe stayed the loader; nothing in MH1's folder.
2. MH1's loader, laptop up: the same with ?game=mh1 and mh1.xbe (the MH1 nv2a build).
3. G's loader, no server: "The laptop does not answer", the mhg.xbe on the disk ran (once; after its exit the
   loader did not start it again), its log stayed on the disk.
4. G's loader with `--data` (a small fake G folder holding a default.xbe too): the log left from run 3 was sent,
   then /manifest?game=mhg and the four files (byte-identical on the disk afterwards); default.xbe stayed the
   loader (the fake default.xbe never left the laptop); G then started from mhg.xbe and complained about the
   (fake) data sizes. MH1's /version and an old loader's plain /version had no DATA line meanwhile.
Each run ends at the kernel's "error 21" screen: "back to the dashboard" finds no dashboard on the scratch disk
(expected). The test DVD src/pc/xbox/testlaunch stands in for the dashboard (starts E:\launch.txt's XBE once).
Not tested: the real Xbox (Evolution X starting each loader from its folder; XeImageFileName is how nxdk and the
kernel name the running XBE, the same field xbox_log.c already used for the log path), and the loader with no
network at all after a game exit (now goes to the dashboard instead of starting the game again; a code path, not run).

**Owner, once:** `XBOX_IP=<the Xbox's address> tools/xbox_upload.sh --loader` (asks for the FTP login; builds both
loaders, installs E:\Games\MH1\default.xbe + loader.txt and E:\Games\MHG\default.xbe + loader.txt with this
laptop's current address). Then restart tools/xbox_serve.py (an older server does not know `?game=`; the loaders
then start the copy on the disk). G's game was E:\Games\MHG\default.xbe before; that is now the loader, and the
first start of Monster Hunter G with the server running downloads G as mhg.xbe (the G data already on the disk is
kept). If the laptop's address changes (DHCP), run the upload again.

## Settings menu on the Xbox (agent F, 15 Oct 2026)

The PC's settings menu (docs/pc.md "Settings menu") now runs on the Xbox, MH1 and G. **Back + L3** (hold Back, click
the left stick) opens it at any time; the title screen shows "Back + L3: SETTINGS" in the bottom-left corner. The game
does not tick while it is open and the music pauses. D-pad or left stick up/down selects, left/right changes (A also
steps to the next value), B, Start or Back closes (on release). Back + R3 is still the testers' debug menu.

| row | what | when |
|---|---|---|
| Text language | Japanese / English; English only when a translation table is found: `text_table` in the ini, else `E:\Games\MH1\text\en.txt` (MHG: `E:\Games\MHG\text\en.txt`), `...\data\text\en.txt`, `D:\data\text\en.txt` | next start |
| Aspect ratio | 4:3 (original) / 16:9 anamorphic | at once |
| HUD | original / prototype gunner (docs/pc.md "Prototype gunner HUD"; bowgun users only) | at once |
| Button layout | B confirms (Japan) / A confirms (West): circle and cross swapped for the whole game | at once |
| Music / Effects volume | the game's own OPTION values (kept in the save, as on the PC) | at once |
| Rumble | on / off | at once |
| Debug menu (testers) | on / off (Back + R3) | at once |

Code: src/pc/menu.c (the PC file; `rows[]` picks the Xbox rows, a 320 x 240 frame so the 5x7 font is 2x on 640 x 480,
panel 32 px in from the screen sides for TV overscan), src/pc/gfx/gfx_opts.c (settings file, now built for the Xbox;
pdclib has no strings.h, so its own case-insensitive compare), src/pc/font5x7.c (the font, out of pick.c),
xbox_env.c `xbox_env_set` (language = en hands the table to rt_text.c as RT_TEXT_TABLE).

**Settings file.** `settings.ini` in the game folder: `E:\Games\MH1\settings.ini`, `E:\Games\MHG\settings.ini`. Same
`key = value` format as mh1pc.ini, keys widescreen, language, text_table, confirm, rumble, hud, debug_menu (only once
changed). Read at boot before anything else (after env.txt; RT_ARGS flags such as `--widescreen`, `--hud gunner` still
override it), written when the menu closes. Without a `widescreen` line the dashboard's 16:9 setting decides
(XVideoGetEncoderSettings & VIDEO_WIDESCREEN; logged as "settings: dashboard widescreen on/off").

**16:9.** The video mode stays 640 x 480 (no 720p); the TV is expected to stretch it to 16:9 (set the dashboard and the
TV to widescreen). gfx_nv2a.c then: the 3D projection is 16:9 (gfx_scene_aspect: the vertical field of view stays, the
horizontal one grows: Hor+, as on the PC), the game's 4:3 2D is drawn on the middle 3/4 of the frame (x 80..560, so it is
4:3 again on the TV), HUD widgets anchored left / right by rt_2d.c (render state 0x105, no longer ignored) slide to the
frame edges, fades and frame-sized draws cover the whole frame, gfx_project maps onto the 4:3 part (name tags, sights,
sound pan). No culling depends on the aspect (only the near-plane test). Not done: WSS / widescreen signalling of the
video encoder for SCART (the kernel sets it from the dashboard flag at the mode set, untested), 720p.

**Checked in xemu** (MH1 nv2a, hidden, own HDD image, scripted pad, env RT_ARGS=--quest 10 --play, RT_WEAPON=g2,
RT_PL_ITEMS, RT_PL_GOD; shots in build/show/F/xset/, not committed):
- run 1, blank E:: "dashboard widescreen off" -> 4:3 with the original HUD (mh1_43_original.png); at tick 503 the script
  presses select+l3: "settings menu: opened" (mh1_menu.png, the menu on nv2a, mid-navigation), down/right/down/right/circle:
  "settings: E:\Games\MH1\settings.ini written", "closed (aspect 16:9, ... hud 1 ...)"; the quest goes on in 16:9 with the
  gunner HUD.
- run 2, same disk (a reboot): "settings: E:\Games\MH1\settings.ini read", the quest starts in 16:9 with the gunner HUD
  (mh1_169_gunner_after_reboot.png); the menu again: HUD original (mh1_169_original.png: timer and vitals at the left
  edge, item bar at the right edge, scene wider than in 4:3), then 4:3; the file read back with tools/xbox_fatx.py get.
- Frame time (xemu on the loaded laptop, software GL: only relative): 2D/HUD prof 5.2-5.4 ms/frame with the gunner HUD,
  5.6-5.9 original 16:9, 5.6-6.8 original 4:3; draw 52-59 ms/frame and 14-16 drawn frames a second in all three, within
  the run-to-run noise. The menu costs nothing while closed (one test per frame); 16:9 draws a little more scene.
- G (nv2a, power-on boot, RT_NOMOVIE): the "Back + L3: SETTINGS" hint on the logos/title (mhg_hint_43.png), the script opens
  the menu at tick 1000, 16:9 on, "settings: E:\Games\MHG\settings.ini written"; the title then shows 4:3 in the middle of
  the 16:9 frame (mhg_title_169.png).
- Builds: MH1 PC, test_menu.sh and test_quest_loop.sh pass; G PC (the first build_pc_mhg.sh run failed to link with a
  duplicate `D_3F360A` between rt_gen.c's stand-in and rt_game's alias header, a second run linked: the adapt headers are
  made during the first link; not caused by this change); MH1 Xbox null and nv2a, G nv2a link.

## Online on the Xbox (agent C, 15 Oct 2026)

MH1's online town on the Xbox: `ONLINE=1 tools/build_pc.sh`, then `python3 tools/build_xbox.py --online --gfx nv2a`
-> `build/xbox/online/nv2a/default.xbe` (own objects in build/xbox/online; `./mh1 xbox DISC --online` copies it to
dist/xbox/MH1-online/). How the owner runs it: docs/PLAYING.md, "Online on the Xbox". Decision: docs/DECISIONS.md.

What the build does differently from the plain Xbox build:
- the ONLINE=1 PC build's objects (build/pc/objs.txt after an ONLINE=1 build; build_xbox.py checks build/pc/.variant)
  plus src/pc/net (net_cpinet, net_dnas, net_netcnf, net_peer) and `-DMH1_ONLINE=1` for the front-end;
- nxdk's lwIP (lib/net, libnxdk_net.lib). lwIP has its own global `tcp_init` (its TCP module start) that clashed with
  the game's `tcp_init` (lb_tcp01.c): the link uses a copy of the library with lwIP's renamed (`llvm-objcopy
  --redefine-sym _tcp_init=_lwip_tcp_init`, build/xbox/online/libnxdk_net_mh1.lib);
- net_cpinet.c / net_peer.c `#if defined(NXDK)`: lwIP sockets. nxdk's lwIP has no LWIP_SO_RCVBUF, so FIONREAD
  (bytes waiting, TcpGetStatus st[3]) is a non-blocking MSG_PEEK of up to 0x3CA bytes. The first CpInetInitialize
  calls nxNetInit(NULL) (dashboard settings; DHCP waits up to 10 s; -2 = no answer yet, the interface stays up). The NIC
  is shut down before the game goes back to the dashboard (xbox_quit_hook in xbox_log.c), as the loader does;
- the server comes from settings.ini (`online_server`, `online_login`, `online_password`, the PC's mh1pc.ini keys), which
  the settings menu now keeps when it rewrites the file; without online_server, loader.txt's host, port 10200.
- rt_np.c: no Win32 co-op dialog or _putenv_s on the Xbox (xbox_env_set instead).

**Checked in xemu** (tools/xemu_online_test.sh: hidden xemu with NAT networking, its own blank disk image with
E:\Games\MH1\settings.ini put on it by xbox_fatx.py, `mh1_server.py serve --open` on 127.0.0.1, the Xbox reaches it as
10.0.2.2): "net: Xbox address 10.0.2.15" (xemu's DHCP) 8 s after the start, "online: connecting to 10.0.2.2 port N"
(the port from settings.ini), connected, the Capcom ID screens by script, the plaza, lobby entry ("quest download: none")
and "town step 4" at tick 578. A PC client (mhview_online, hidden) then logged in to the same lobby: the server logged
both handles, the PC log has `slot 0 "XBOX" ... shown`, the Xbox log `slot 1 "PC" ... shown`; screenshots
build/show/xemu_online/xbox_sees_pc.png and pc_sees_xbox.png (not committed) show both hunters by the fountain, named.
Not yet on a real Xbox; rooms and a co-op hunt from the Xbox are not tested.

### Black screen at "ac97 open" after a Back + Start relaunch (agent A, 21 Oct 2026)
Owner (G, real Xbox, logs/20261021-190054-mhg): after a quit through Back + Start and a relaunch from the loader the
game stopped at boot; the log ends after the first "ac97 open" line with GLOB_STA 00308000 (bit 8, codec ready, not
set) and before the codec registers line: the codec register read stalled. A power cycle cleared it. Now:
- leaving (xbox_log_show, every way out, and xbox_log_quit): both DMA channels run bit off, wait for the halt (DCH,
  50 ms max), bus master reset, CR 0 (interrupts masked), status cleared, the interrupt disconnected;
- XAudioInit: every wait has a time-out (codec-access semaphore 20 ms, codec ready 250 ms, power-down bits 200 ms,
  halt / bus master reset 50 ms each). No codec ready after the cold reset: a second, longer (20 ms) cold reset; still
  none: a warning, no codec register is touched, no DMA runs, the game goes on silent.
Built (both games); not run on hardware or in xemu (a warm relaunch can't be made there anyway).

## Freeze watchdog and crash catcher (agent E, 21 Oct 2026)

src/pc/xbox/xbox_watch.c, started from main once the log is open (both games, every Xbox build).

- **Freeze watchdog.** A second thread checks every 0.5 s whether the game made progress: a drawn frame
  (xbox_log_frame) or a finished load step (rt_lt_end: a file read, an unpack, a texture). With none for 5 s it
  writes "FREEZE: ..." and the crash report into the log (mode / step / stage / quest, the last checkpoint passed,
  the last 300 log lines), commits the log and the save files to the disk, stops the AC97, shows the reason and
  the log's last lines for 4 s and starts the network loader, which sends the log and the save to the laptop.
  Known long operations (rt_lt_span_*: a quest start, a gallery scene, ...) get 60 s; in xemu (a UART answers)
  the limit is always 60 s, because xemu can run far below real time; `RT_WATCHDOG_MS=n` in env.txt sets it
  (0 = off). Checkpoints (xbox_watch_mark, a string): the boot-screen tick, the game tick, the sound tick, the
  drawing, the frame end, the card's open / read / write, the AC97 start. The kernel has no call that reads another
  thread's registers, so a freeze report has no backtrace of the frozen code: the checkpoint and the log tell
  where.
- **Crash catcher.** An SEH record at the bottom of the main thread's stack (fs:[0]; the kernel's dispatcher calls
  it before it would stop the console): the exception code and address (with the symbol from the build's own
  table), the registers, 16 stack words and a backtrace (stack words that point behind a call), then the same
  report, flush and screen ("crashed - sending the report") and the loader. Exceptions in other threads (audio,
  the watchdog) are not caught.
- Not run on hardware or in xemu yet (21 Oct 2026: no memory for an emulator); both nv2a builds link.

## Fog: the camp's fog in every area; COLOR1 alpha (agent D, 25 Oct 2026)

Owner: Forest and Hills area 1 fog too thick on the Xbox. Two causes, both fixed (details docs/RELEASE_G.md section 17):
- A stage's fog (stage_fog_set, fl states 0x0F-0x11) is set during the stage load, and a load tick's draws are recorded from its
  first flip and dropped (viewer.c game_flip / flip_tick_end): the fog states went with them and every area kept the quest start's
  fog (area 1: the camp's end 9000 instead of 90000, far cliffs flat grey). gfx_rec.c now applies the fog states at once too.
  Shared code: MH1 and G, and the PC in a window.
- gfx_nv2a.c passes the fog factor in COLOR1.a. The NV2A replaces the specular alpha with 1 unless NV097_SET_LIGHT_CONTROL has
  ALPHA_FROM_MATERIAL_SPECULAR (bit 17), also with vertex programs (xemu: vsh.c ignore_specular_alpha, commit 7a34eedd6f, written
  from hardware tests). fixed_state now sets LIGHT_CONTROL = ALPHA_FROM_MATERIAL_SPECULAR | SEPARATE_SPECULAR; RT_NV2A_LC=hex
  overrides it (RT_NV2A_LC=0 in xemu: the whole screen in the fog colour). The real console's power-on value is not known.
- Test aids: RT_FIXED_CAM=1 with --view/--cam in RT_ARGS keeps that camera (the Xbox build always plays, so the host camera
  followed the hunter); RT_FOG_TRACE now reaches the Xbox log, with a "nv2a fog: colour .. start .. end .." line per change.

## Second start, saves and the watchdog in xemu (agent F, 28 Oct 2026)

Open hardware issues: the black screen when the game is started a second time without a power cycle (GB5), saves across a
quit, and whether the freeze watchdog (21 Oct, never run on hardware) fires where it should not. Notes for agents and the
list of questions only the real Xbox can answer: docs/agents/agent-F.md "Xbox: second start, saves, watchdog".

**New test: tools/xemu_relaunch_test.sh** (`--version mh1|mhg`, `--online`, `--save`, `--powercut`; hidden, scratch disk,
its own loader server on port 8796; run it under the emulator lock). It plays the owner's chain: loader -> game -> Back +
Start -> loader (sends the log) -> dashboard -> loader -> game -> Back + Start. The "dashboard" is the test program
src/pc/xbox/testlaunch installed as C:\xboxdash.xbe (tools/xbox_fatx.py `FATX_PART=c`); without a dashboard on C: xemu
stops at the kernel's error 21 screen. The pad script quits with select+start (counts as Back + Start in scripted runs).
The game runs with RT_WATCHDOG_MS=5000, the hardware's limit (xemu's default is 60 s).

Results (xemu, nv2a builds of 28 Oct):

| run | result |
|-----|--------|
| MH1, plain | both starts reach the opening / title and quit; start 2 drew 1167 frames, sound device opened, no FREEZE |
| G, plain | the same; start 2 drew 1377 frames |
| G online build, plain | the same (the loader and the game's quit both stop the NIC); start 2 drew 1185 frames |
| MH1 online build, plain | the same; start 2 drew 1207 frames |
| G, --save | start 1: blade 2 equipped, bed save (78032 bytes, check BA2D4597), quit; start 2 from power-on, CONTINUE: loaded 78032 bytes, check BA2D4597, village reached; the loader sent the save |
| MH1, --save | start 1: bed save on an empty card (70736 bytes, check 65E017FC); start 2 CONTINUE: the same bytes, village reached |
| G, --powercut | xemu killed 3 s after the save: the file on the disk has the logged size and check sum (78032, 0089AE77) |

Every run: no FREEZE or CRASH line with the hardware's 5 s limit, also on the 10 s exit screen (which the old code would
have reported as a freeze: from reading the code, not run); the longest wait was the boot's, 1.5-3.1 s. A last G run with
the split boot / play report: boot 2.0 s, nothing of 1 s or more after the first frame (title, opening movie). The quit in the scripted runs needs select+start
held for 60 drawn frames: in the house xemu draws ~15 a second, so the scripts hold it for 400 ticks.

**Fixed:**
- The watchdog called every Back + Start quit a freeze: the exit screen waits 10 s on the main thread and the watchdog's
  5 s limit ran out (xemu hid it with its 60 s limit). The exit screen now stands the watchdog down.
- The boot up to the first frame, DHCP (nxNetInit, up to 10 s), DNS look-ups and the bug report's save + upload are long
  operations (60 s); the online town's co-op and game-server waits count as progress (they have their own time-outs).
- "watchdog: longest wait so far N ms, at <checkpoint> (limit there S s)" in the log at each new longest wait of 1 s or more
  after the first frame, and "watchdog: longest wait during the boot N ms" for the boot.
- AC97: an interrupt that none of the known status bits explains no longer returns "not mine" (a level interrupt would
  then repeat for ever: a black screen with nothing logged); all AC97 interrupt enables go off, the game runs silent and
  "ac97 open: N interrupt(s) with no known cause" is logged. A guard, not a proven cause.

**Not reproducible in xemu:** xemu's AC97 has no codec handshake (GLOB_CNT reads 0, the codec is never touched) and its
stand-in dashboard is not an XDK program (no D3D / DirectSound set-up between the starts). Every failing hardware log so
far stops between the controller line and "audio: driver", or at the first codec read with GLOB_STA 00308000 (codec not
ready, read time-out). Since 21 Oct each step there is written through to the disk ("boot step:", "ac97: init:"), so the
owner's next failing start will name the step; the questions are in docs/agents/agent-F.md.

**The testers' kit (round 2, 28 Oct):** `tools/make_release.sh G_DISC --xemu-test` runs each kit Xbox folder through
`tools/xemu_relaunch_test.sh --kit DIR --save` (installed as testers do, the game as default.xbe, no loader). The kit of main
842bf3cc passed: two starts, bed save, CONTINUE loads the same bytes, no freeze; after the first frame no watchdog wait
of 1 s or more with agent E's frame overlap.

**Saves:** a corrupt or cut-short save file (PC check, both games, the same game code as the Xbox) gets the game's own
"data may be damaged, start anyway?" after its retries, as on the PS2. Memory units are not supported (E:\UDATA only).

## G online frame rate: where the time went (agent E, 27-28 Oct 2026)

The G online XBE ran at 5-7.5 fps in the guild hall and the town in xemu. Measured with RT_PROF (always on on the Xbox;
its report is in the game log every 300 ticks, and in xemu's serial log). New in the report: the zones "GPU wait"
(waiting for the GPU idle, a vblank, a free flip slot) and "net (sockets)" (the CpInet calls), the counts per frame
"2D draws", "pushbuffer kicks", "textures made" + "texture KB", "GPU waits", "net calls", and one line with the frames
per second of wall clock and the game thread's own share of the CPU (its kernel tick count over the wall time: what is
left went to other threads or to idle waits). On the PC (RT_PROF=1 RT_STEP=1) the same counts come out of gfx_gl.c.

How measured: a hidden xemu (tools/run_xemu.sh, the shared xemu.lock, an empty /dev/input) booting the G online nv2a
XBE against `mh1_server.py serve --open` on 127.0.0.1 (the Xbox reaches it as 10.0.2.2), RT_LB_WARP to the hall's door
and square, about 8 minutes; the same with no square (the town) and the plain G XBE in Kokoto village. The laptop's
load average was 10-13 (other agents' jobs) through all of it, so xemu's milliseconds are only good for comparing runs
made the same hour; the counts do not depend on it.

What the first run showed (hall, before): 5.7-5.9 fps, 4 game ticks per drawn frame, "logic (rest)" **31 ms per tick**
(the PC: 0.2 ms), the game thread at **21% of the CPU** (offline village: ~50%). Net: 16 calls a frame, 0.1 ms: no
busy waiting on sockets. 2D: 10 draws a frame (the 2D path is not a cost). Textures: 6 made every frame. GPU: 1300
pushbuffer kicks a frame, 19 ms GPU wait, 10 ms "stage draw code".

Fixes, each measured in the hall in xemu (fps, laptop load 10-12):

| step | fps | logic / tick | game thread CPU | kicks / frame | textures / frame | GPU wait / frame |
|---|---|---|---|---|---|---|
| before | 5.6-5.9 | 31-33 ms | 21% | 1308 | 6 | 17-19 ms |
| 1. font palettes kept | 5.6-5.8 | 31 ms | 21% | 1308 | 0 | 17-20 ms |
| 2. no per-tick sleep | 15.3-17.9 | 3.9-4.5 ms | 52% | 1295 | 0 | 21-23 ms |
| 3. one kick per draw | 19.2-19.6 | 3.6-3.7 ms | 54% | 88 | 0 | 21 ms |
| 4. vblank wait only when needed | 23.0-23.7 | 3.5-3.6 ms | 62% | 88 | 0 | 13.5-14.6 ms |

Town (online, same build as step 4): 21.3-22.1 fps; with `RT_ONLINE_PACE=1` (the old per-tick sleep, everything else
the same) 5.5-5.7 fps, logic 32-33 ms/tick, game thread 21%. Offline G, Kokoto village: 8.8-10.4 fps before (1651
kicks a frame) -> 13.4-14.0 fps after (119 kicks), at a higher load (13.6 vs 10-12).

1. **Font palettes** (rt_font.c flfntSetPalData): G's lobby sets its text palettes every frame; each call threw away
   every glyph texture made with that palette, so the hall's labels were rebuilt each frame (6-8 textures: on the Xbox
   each a GPU allocation, a colour count, a copy). Now the glyphs stay when the palette's colours did not change.
   Real on hardware too, but small (a few allocations a frame). PC: 8 -> 0 textures a frame in the hall.
2. **The per-tick sleep** (rt_online.c pace, rt_np.c's quest tick): online, every tick slept until its own 33 ms slot
   so headless test runs keep the PS2's pace. In a real-time run the viewer already ticks by the clock, running a
   frame's ticks back to back to catch up and up to two ahead; with pace() each of those ticks waited too. Once a frame
   took longer than one tick, every frame slept ~2 ticks: 4 ticks a frame at 33 ms each, the game thread asleep 80% of
   the time. Now pace() only runs in fixed-time (--time, headless) runs; viewer.c tells rt_online.c
   (rt_online_host_paced). **Not an emulator effect:** the same arithmetic holds on the console (any frame over 33 ms
   fell into it) and on a PC window (MH1 and G online). RT_ONLINE_PACE=1 brings the old pacing back (A/B aid).
3. **Pushbuffer kicks** (gfx_nv2a.c cmd_begin / cmd_end / cmd_kick): pbkit's pb_end sends at once (an sfence, a
   write-combine flush that polls a GPU register, the PUT register write); the backend wrote each state change as its
   own block. Now blocks only move a write pointer and one kick sends them after each draw (or past 4096 dwords, or
   before any pbkit call that pushes or waits: the macros at the top of gfx_nv2a.c). Same commands, same order. In
   xemu each register access is an exit to the emulator, so most of the gain there is emulator cost; on hardware it
   saves ~1200 flush-and-poll rounds a frame [estimate: a few microseconds each, i.e. a few ms a frame; not measured].
4. **The vblank wait** (gfx_begin_frame): every frame waited for a vblank before drawing, which also held the CPU until
   the previous frame was on screen (8 ms a frame on average below 60 fps). pbkit triple-buffers (flips at the vblank
   to finished frames, three back buffers), so the next buffer is free while one finished frame still waits; the
   wait now happens only when two do (PCRTC_START, the scanned-out buffer, is neither of the last two finished
   frames). **Real on hardware.** Not yet seen on a real Xbox: worth a look for tearing or a flicker in fast menus.

Rendering checked: the hall screenshots before / after match (a walking NPC aside). render_score (tools/render_score.sh
--reps 1, against agent D's 27 Oct PS2 captures) after all four: MH1 m_camp / m_village on the Xbox (xemu) and the PC,
G g_village / g_scene on the Xbox, G g_camp / g_village / g_scene on the PC are pixel-identical to the 27 Oct
captures; G g_camp / g_menu on the Xbox differ only where the Elder's-lesson banner is now drawn (agent C's
static-function fix, merged after the capture; SSIM 49.1 -> 52.2 and 54.3 -> 57.4); G g_menu on the PC differs in its
top two pixel rows (max 12 levels), the same with the font change taken out, so from other merges.

**What is left** (hall, after, xemu): GPU wait 13-15 ms a frame (mostly xemu: its NV2A runs on Mesa llvmpipe, software
GL), "stage draw code" 10 ms (village 22 ms): the backend's work for the stage, which is the per-vertex near-plane test
(the camera is inside the hall, so the bounding-sphere shortcut fails), the copy of the whole stage into the ring when
a triangle crosses the near plane, and the indices written into the pushbuffer (24k triangles: ~36k dwords a frame).
Those are real CPU costs on hardware as well, and the next candidates: keep the near-clip result per stage part
(only the parts near the camera), or put static index lists in GPU memory as pushbuffer subroutines (NV2A CALL)
instead of copying them every frame. "draw (rest)" 5.5 ms and the skinning set-up are next after that. The game thread
still gets only ~60% of the CPU: the rest is idle waits (GPU) and the audio thread.

### Round 2: the stage's near-plane cost and the end-of-frame GPU wait (agent E, 28 Oct 2026)

**What it was.** gfx_nv2a.c draws static clays (the stage, set models) from their own GPU buffer, but when the camera's
near plane cut through one (always in the hall, the town, often in the village and the field), the whole clay went
down the copy path: every vertex tested against the plane every frame (a malloc'd w array), every vertex copied into
the vertex ring, all its triangles re-indexed. Real CPU work on the console, in offline play too.

**Fix 1: only the crossing triangles are cut** (static_clip). Each batch's triangles are grouped in runs of 32 with a
bounding sphere (made on the clay's first crossing draw, dropped by gfx_update_clay). A run wholly in front of the
plane or wholly behind needs no test; the rest is tested per triangle, each vertex once a draw (a stamped w cache).
Triangles in front are drawn from the clay's GPU buffer, crossing ones are cut into the ring, wholly-behind ones are
dropped (as before), and each batch is drawn as alternating runs in the original triangle order, so blending and
depth ties are unchanged. Same w values, the same cuts (gfx_clip_tri, vtx_lerp on the same values) and the same
fall-back rule (more than 65535 vertices: unclipped) as the copy path. Dynamic clays keep the copy path.
`RT_NV2A_OLDCLIP=1` = the copy path (A/B aid). RT_PROF counts it: "near-plane vertex tests", "ring vertices (3D)".

**Fix 2: the GPU finishes a frame while the next game ticks run.** gfx_end_frame waited for the GPU to go idle before
queueing the flip; gfx_begin_frame waited again. Now only the next frame's start waits (the vertex ring and the deferred
frees need it), so the ticks between two frames overlap the GPU's drawing. The triple-buffer check (round 1) runs after
that wait and loops up to 4 vblanks. `RT_NV2A_SYNC=1` = the old wait. Real on hardware: the GPU's time per frame no
longer adds to the CPU's.

Also: RT_PROF puts the backend's work for a clay (gfx_execute_clay) in "gfx backend", as the PC does; before, the
stage's part of it showed as "stage draw code" and the actors' as "draw (rest)".

**Counts per frame** (they do not depend on the laptop's load), the copy path -> fix 1:

| scene | near-plane vertex tests | vertices copied to the ring |
|---|---|---|
| G guild hall (online) | 7949 -> 2780 | 7909 -> 261 |
| G town (online) | 9765 -> 6416 | 20175 -> 660 |
| G Kokoto village (offline) | 14283 -> 2810 | 14915 -> 588 |
| MH1 Forest and Hills camp (quest 10) | 16325 -> ~8820 | 19770 -> ~1190 |

**Time** (xemu, load average 11-17 during all of it, so milliseconds swing by 30% between runs; each scene's runs
were made back to back). "base" = both old ways, "fix 1" = RT_NV2A_SYNC=1, "both" = this round:

| scene | fps base / fix 1 / both | gfx backend ms base -> both | GPU wait ms base -> both |
|---|---|---|---|
| G guild hall | 15.7-16.9 / 32.2-32.4 / 33.4-34.4 | 16.9-17.4 -> 7.3-7.5 | 18.2-20.1 -> 6.7 |
| G village | 15.3-15.4 / 17.0-17.2 / 21.8-22.1 | 25.4-27.0 -> 13.0-13.1 | 10.5-12.0 -> 5.3-5.9 |
| G town | 15.5-18.0 / - / 20.2-20.9 | 19.0-20.7 -> 14.1-15.0 | 11.1-12.4 -> 3.4-4.4 |
| MH1 field camp | 10.4-12.8 / - / 25.1-27.7 | 30.5-36.2 -> 13.4-15.0 | 19.9-23.4 -> 7.3-7.7 |

The hall's runs came at load 9-14; fix 1 alone doubled it there (the hall's crossing stage was the whole frame's
largest item). xemu shows more than 30 fps here because it draws more frames than the game's 30 ticks.

**What is left** (hall, both): GPU wait 6.7 ms (mostly xemu's software GL), the backend 7.3 ms (indices written into the
pushbuffer every frame, bone constants), logic 3.4-3.7 ms a tick, draw (rest) 2.8 ms (posing, joints). The village and
the field still spend 13-15 ms in the backend: the larger stages' index lists. On hardware those index writes are a
plain copy into write-combined memory [estimate: ~0.5 ms a frame], so they are mostly an xemu cost; NV2A pushbuffer
subroutines (CALL) for static index lists would remove them, but were left for now (untested on hardware).

**Pictures.** render_score's Xbox runs with both old ways (RS_XBOX_ENV="RT_NV2A_OLDCLIP=1 RT_NV2A_SYNC=1", a new
render_score.py aid) against this round: g_camp, g_village, g_forest1, g_cave, m_camp, m_forest1, each at the tick and
one tick either side: 18 pictures, all pixel-identical. (One old-way capture, g_camp one tick late, came out torn the
first time; run again it matched: an xemu capture glitch.)
