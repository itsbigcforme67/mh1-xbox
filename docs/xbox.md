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
XBE certificate's title id must be set to the same value, which the build does not do
yet; the owner may supply a real one). Free space is reported as 8000 KB minus the
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
