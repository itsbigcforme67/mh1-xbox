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
- Not tried yet: linking. The PC link relies on GNU tools the Xbox link does
  not have in the same form: `objcopy --weaken-symbol` on game objects (to let
  matched copies win over near-match copies), `--defsym` aliases for D_xxxx
  data, `-rdynamic` + dlsym. With lld-link these become: /alternatename or
  weak externals, a generated alias .c/.def file, and the generated symbol
  table above. Also `audio_sdl.c`, `gfx_gl.c` and the viewer front-end were not
  compiled (they are the parts that get Xbox backends).

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
- Memory (above): no virtual memory; running out is a hard crash. Needs a
  memory report from the PC build first.
- Link step: the PC build relies on GNU objcopy/ld features (weakening
  symbols, --defsym, dlsym); these need a different scheme with lld-link.
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
2. A memory report from the PC build (per category), then trim.
3. Link the game C for the Xbox with stub platform backends (no graphics):
   boot to the village logic headless, print to the debug output.
4. gfx_nv2a.c: textured clays, then the HUD/2D; then pad and audio.
