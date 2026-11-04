# MH Dos (MH2, JP 1.04) on the original Xbox: plan (paper only, 4 Nov 2026)

- **Written:** 4 Nov 2026, by a low-CPU research agent. Nothing was built, run or flashed: no nxdk build, no xemu, no Xbox.
  The Xbox stays off.
- **Sources:** tools/build_xbox.py, tools/build_pc_mh2.sh, tools/build_pc.sh, src/pc/viewer.c, src/pc/xbox/*, src/pc/install.c,
  src/pc/rt/rt_snd.c, rt_mem.c, rt_data.c, ./mh1, tools/run_xemu.sh, xbox_serve.py, xbox_upload.sh, xbox_install.sh,
  disc_files.py, make_release.sh; docs/xbox.md ("Monster Hunter G on the Xbox", "Memory budget", loader sections),
  docs/RELEASE_G.md G8/G9, docs/mh2_port.md, docs/mh2/platform.md, host.md, shared5.md.
- **Numbers:** `size` / `llvm-readobj` on the existing builds (build/mh2/pc/mhview_2, build/mhg/pc/mhview_g,
  build/mhg/xbox/nv2a/main.exe of 1 Nov), the ELF program headers of SLPM_662.80 and SLPM_658.69, the sizes of
  build/mh2/*.bin and build/mhg/*.bin, and the AFS tables of DATA.BIN entry 0 and G's AFS_DATA.AFS (a throwaway stdlib script in
  the scratch folder). **Guess** marks every estimate; there is no MH2 memory measurement on any platform yet.
- Community mh2-decomp: not opened.

## Plain summary

- MH2's PC build already uses the same pipeline G's Xbox build starts from (its own symlink tree, recorded compile commands,
  link-adapt headers). What is missing is the Xbox half: build_xbox.py, the title identity, the boot-time data check, the
  data install and the loader all know only MH1 and G. In several places MH2 would silently be **treated as G** (it compiles
  with `-DVERSION_MHG` too) or **as MH1** (anything that is not "MHG" by name). Those must be fixed before anything is
  installed on a console.
- Memory: MH2 is bigger than G where the port keeps PS2 images (overlays about 3 MB more, a larger lobby block), but its
  per-scene files are only about 10-20% bigger than G's. **Guess:** 2.5-3.5 MB less free than G at the same point, so a quest
  around 15-18 MB free in xemu (G: 20.8 MB). The 64 MB box should hold the title and quest 1014. The village is the unknown.
- Data: about 2.7 GB to install (G 1.1 GB), and the three streams exist only on the owner's English v1.03 image.
  The title screen needs only DATA.BIN and SLPM_662.80 (408 MB).
- First target: the title screen in xemu, from a `--version mh2` build of the existing code, with DATA.BIN + SLPM as D:\data.
  Then `--quest 1014` through env.txt's RT_ARGS, as G's scripted xemu runs did.

## 1. What the G Xbox build does that the MH2 PC build does not yet (per step)

| step | G (works on hardware) | MH2 today | what MH2 needs |
|---|---|---|---|
| PC build in its own tree | tools/build_pc_mhg.sh, build/mhg/pctree | tools/build_pc_mh2.sh, build/mh2/pctree; 786 linked objects (G 785), all with recorded commands in build/pc/cmd | nothing: build_xbox.py can replay these as it does for G |
| choose the tree | build_xbox.py `--version mhg` chdirs into build/mhg/pctree, output build/mhg/xbox | `--version mh2` exits ("--version mh1 or mhg") | a mh2 branch: tree build/mh2/pctree, OUT build/mh2/xbox, ISO name mh2.iso, error text naming build_pc_mh2.sh |
| front-end flags | the FRONT files (viewer.c, fmt/*, gfx_nv2a.c, mc_xbox.c, ...) get `-DVERSION_MHG` | not compiled for the Xbox | `-DVERSION_MH2 -DVERSION_MHG` both (docs/mh2_port.md section 8: they always go together). viewer.c has 15 VERSION_MH2 blocks (DATA.BIN, mh2_boot, --model, the overlay setters); without the flag the Xbox viewer would open AFS_DATA.AFS. The game objects already carry the flags in their recorded commands |
| title identity | xbox_title.h `#ifdef VERSION_MHG` block; build_xbox.py's title_identity() cuts the header at `#ifdef VERSION_MHG` / `#else` and patches the id into the XBE | none | an `#if defined(VERSION_MH2)` block placed **before** the MHG one (MH2 defines VERSION_MHG, so today it would take G's id 4D480002, G's save folder and E:\Games\MHG); title_identity() must parse three blocks |
| data found and checked at boot | viewer.c (XBOX): D:\data then XBOX_GAME_DIR\data, only if it holds SLPM_658.69; sizes of AFS_DATA.AFS, AFS00, AFS01, SLPM_658.69 must match or it exits with "DATA PROBLEM" | the VERSION_MHG list would be used: it asks for AFS_DATA.AFS and SLPM_658.69 | an MH2 list: DATA.BIN 407,248,896 and SLPM_662.80 822,368 required; AFS00 1,186,904,064, AFS01 47,835,136, AFS02 1,077,581,824 checked only when present (the title does not read them, below) |
| boot | `boot = 1` unless RT_ARGS; G's boot tasks to the title and on | on the PC `--boot` jumps to `mh2_boot` and runs src/pc/title_mh2.c to the title; Start does nothing | the same jump works on the Xbox once viewer.c gets the flag (the XBOX block sets `boot = 1`, which is what `--boot` does) [read from the code, not run] |
| sound | rt_snd_init opens `<disc>/AFS00.AFS` and AFS01 | the PC logs "snd: no disc/mh2/AFS00.AFS: no sound": disc/mh2 has no streams, only the ISO does. The `--boot` path skips rt_snd_init altogether (the mh2_boot label is after it); `--quest` calls it | on the Xbox the streams would be next to DATA.BIN after an install, so `--quest` would **start sound with G's tables on MH2's packs**, a path the PC has never run. Use `--mute` in RT_ARGS until MH2's sound table work (platform.md "Not done") is in |
| movies | AFS00 SFD through rt_movie | AFS02 not read; title_mh2.c leaves movies out | nothing for the title; RT_NOMOVIE=1 in test runs as for G |
| saves | mc_xbox.c: E:\UDATA\4D480002\4D48000200000001, PS2 folder BISLPM-65869MHG, UD_WEAR 0xC75 (G's User_data) for the log line | MH2's title skips the card; MemcardWork not laid out | own title id and save id; PS2_SAVE_DIR is **unknown** (no "BISLPM" string in SLPM_662.80: it is probably in an overlay or built at run time, not looked for); UD_WEAR must come from UDMO (MH2's worn equipment is at 0x2090, docs/mh2_port.md) or the log line reads the wrong bytes. Harmless until a save exists |
| settings, log, English | settings.ini, logs\, data\text\en.txt under XBOX_GAME_DIR; ini name mhgpc.ini on the PC | follows xbox_title.h | comes right with the identity block; no English for MH2 |
| ./mh1 xbox | disc_files.py says mh1 / mhg from the boot file; G branch builds, copies 4 files (1.1 GB) to dist/xbox/MHG | disc_files.py knows no MH2; ./mh1 would refuse the disc | an MH2 entry (boot file SLPM_662.80; extract DATA.BIN, AFS00-02, SLPM_662.80 from the image), an mh2 branch to dist/xbox/MH2 |
| data install over the network | xbox_serve.py `--data`: game = mhg if SLPM_658.69 is in the folder, **else mh1**; FOLDER {'mh1','mhg'} | none | **must fix first:** an MH2 folder would be announced as MH1 and the MH1 loader would write 2.7 GB into E:\Games\MH1\data. Add mh2 (SLPM_662.80), FOLDER 'MH2' |
| network loader | loader/main.c: game from loader.txt `game=mh1|mhg`, else folder name (MHG = G, **anything else MH1**); save POST uses "4D480001"/"4D480002" literals; title "Monster Hunter" / "Monster Hunter G" | none | a third game id `mh2`, its title id literal, folder rule, title; xbox_upload.sh --loader builds a third copy; xbox_report.c has the same "mhg else mh1" folder rule |
| xemu | run_xemu.sh `--version mh1|mhg` with a FILES list per game | none | `--version mh2`: XB=build/mh2/xbox, FILES "DATA.BIN SLPM_662.80" (streams optional) |
| testers' kit | make_release.sh (G first, MH1 optional) | none | later; not needed for xemu |
| online | --online, lwIP | not started for MH2 | out of scope |

Smaller things found on the way:
- gfx_nv2a.c's `--ps2-output` mode draws a 512 x 448 frame. MH2's 2D is 640 wide (rt_2d.c SCR_W); whether MH2's PS2 frame is
  512 or 640 wide was not checked. Keep that option off for MH2.
- The Xbox link uses lld-link, which fails on any undefined symbol. The MH2 PC link's first pass had 1,147 undefined names
  (G 1,138), filled by rt_gen as for G, so this should behave the same [guess from the counts].
- No MH2 C uses an `__asm__("name")` label (the lb_sys link failure G had on 14 Oct) and include/mh2_layouts.h has no bit-fields
  (MSVC packs those differently). Checked by grep.

## 2. Size and memory against G

### Code and PS2 images

The PS2 layout (ELF program headers, end addresses include .bss):

| | MH2 | G |
|---|---|---|
| main | 0x100000-0x2E3900, 1.89 MB (file 0.78 MB) | 0x100000-0x4A0C80, 3.63 MB (file 1.93 MB) |
| sub_main (resident) | to 0x6AF200, 3.80 MB (file 1.80 MB: **2.0 MB of .bss**) | to 0x542600, 0.63 MB (almost no .bss) |
| game.bin slot | 0x6AF200, 2.28 MB | 0x542600, 1.62 MB |
| gm_sub (beside game) | 0x8FAE00, 0.24 MB | none |
| resident + game (+ gm_sub) | 8.2 MB | 5.9 MB |

The twelve overlays (build/mh2/*.bin, as rebuilt byte-identical; G's in brackets):

| overlay | bytes | the PC loads it? |
|---|---|---|
| sub_main.bin | 1,892,352 (659,840) | yes, kept |
| game.bin | 2,383,872 (1,695,232) | yes, kept |
| lobby.bin | 1,682,816 (1,067,392) | yes: static rt_lb_mem plus a reload copy |
| select.bin | 143,232 (44,544) | yes |
| gm_sub.bin | 230,784 (none) | yes, extra image, kept |
| plsel.bin | 113,536 (none) | yes, extra image, kept |
| nethttp.bin | 392,192 (392,448) | no |
| lbguild.bin | 169,088 | no |
| yn.bin | 47,744 (48,384) | no |
| dnas_ins.bin, dnas_net.bin | 204,160, 592,640 | no (DNAS stays out) |
| test.bin | 256 | no |

What the port keeps of them: G keeps game.bin and sub_main.bin after the import and frees raw lobby/select (docs/xbox.md).
On that rule MH2 keeps about 2.3 MB more (game +0.69, sub_main +1.23, gm_sub + plsel +0.34). main's kept data part should be
smaller on MH2 (main.bin 0.82 MB against 1.93 MB) [guess: 0.3-0.8 MB less].

The x86 binaries (`size`, gcc -O2 -g, 32-bit):

| | text | data | bss |
|---|---|---|---|
| build/mh2/pc/mhview_2 | 4,903,657 | 469,512 | 7,090,344 |
| build/mhg/pc/mhview_g | 5,177,137 | 600,264 | 5,984,868 |

The 1.1 MB more .bss is mostly rt_lb_mem (2,883,584 against 2,228,224: LB_SPAN 0x2C0000 on MH2), rt_gen's zeroed stand-in data
(510 KB against 211 KB) and rt_sel_mem (196 KB against 64 KB). rt_meta (symbol names, address maps, pointer words) is smaller on
MH2: 1.14 MB against 1.56 MB.

G's Xbox image for comparison (build/mhg/xbox/nv2a/main.exe, 1 Nov): .text 3.21 MB, .rdata 1.77 MB, .data+.bss 4.75 MB, about
9.7 MB loaded (docs/xbox.md said 9.3 MB on 14 Oct). **Guess** for MH2's XBE: 10-10.7 MB loaded (G's plus the .bss difference,
minus the smaller text and rt_meta).

### Data files

| | MH2 | G |
|---|---|---|
| data archive | DATA.BIN 407,248,896 (entry 0: 6,650 files, 390.7 MB stored; entry 1: the 12 overlays) | AFS_DATA.AFS 150,759,424 (3,347 files, 146.6 MB) |
| AFS00 (ADX streams) | 1,186,904,064, **only in the English v1.03 image** | 926,939,136 |
| AFS01 (sound packs) | 47,835,136, only in the image | 25,186,304 |
| AFS02 (SFD movies) | 1,077,581,824, only in the image | none |
| boot file | SLPM_662.80 822,368 | SLPM_658.69 2,029,336 |
| **install total** | **2,720,392,288** (1,642,810,464 without AFS02) | 1,104,914,200 |

The archive is 2.7 times G's, but most of that is more of everything (191 stages against 85, 235 body parts against 94,
mib.afs 23.7 MB, endroll.tex 8.4 MB, gallery.tex 4.2 MB), not bigger pieces. What one scene loads is close to G (stored sizes;
which files are Meltw-packed is not known on either side, so this compares stored bytes, not memory):

| family | MH2 largest / average | G largest / average |
|---|---|---|
| stN_tex.bin (stage textures) | 848,958 / 518 KB | 831,088 / 457 KB |
| stN_amh.bin (stage model) | 528,986 / 298 KB | 513,480 / 267 KB |
| stN_N_tex.bin (area textures) | 504,868 / 138 KB | 294,116 / 86 KB |
| emN_tbl.bin (monster motions) | 871,496 | 837,682 |
| emN_tex.bin / emN_amh.bin | 664,116 / 327,924 | 663,784 / 270,124 |
| lwgN.bin (collision) | 124,358 | 99,518 |

### Expected headroom on a 64 MB Xbox (all guesses)

G in xemu, free physical memory ("alive" lines, docs/xbox.md): title about 19.8 MB, quest 10 Rathian nest 20.8 MB, village
15.5-16.5 MB, lowest point 10.6 MB at the first village load. On hardware G runs at 30 fps (RELEASE_G G8).

MH2 against G, before any trim:
- XBE: +0.5 to +1 MB.
- Kept overlay images: +2.3 MB, minus main's smaller data part (0.3-0.8 MB).
- lobby: the static block is 0.66 MB bigger (counted in the XBE line), the reload copy 0.6 MB bigger.
- DATA.BIN's table and names held open: about +0.2 MB (6,650 entries of 48-byte names against 3,347).
- Per scene: +0-20% of G's stage and monster data.

So about 2.5-3.5 MB less free than G at the same point: **title 16-20 MB free** (MH2's boot path loads less host scaffolding than
G's title, which already held quest 10), **quest 1014 about 15-18 MB free**. A village at G's lowest point minus 3 MB would still
have about 7 MB free. All of this needs the measurement in step A2 below.

What might not fit or needs a look:
- **32-bit TIM2 textures.** gfx_nv2a.c keeps 4/8-bit pictures as P8 + palette; 32-bit ones stay RGBA8. How many MH2 scene
  textures are 32-bit is not counted. A stage of them would cost 4x.
- **endroll.tex (8.4 MB), gallery.tex (4.2 MB), editpl_f.amo (2.7 MB, character creation).** If read whole and expanded, each is a
  large one-off peak. Not reached by the title or quest 1014.
- **Collision areas** are 96 / 192 KB (G's). MH2's largest lwg file is 124 KB stored; its part sizes were not read. The load
  path checks sizes and says so, so a misfit shows in the log rather than as corruption [docs/xbox.md "After the trims"].
- **Sound packs.** AFS01 is twice G's; the average pack is about 116 KB. The packs a stage holds at once were not counted.
  Sound effects stay ADPCM and are decoded while mixing (no PCM cache), as on G.
- Trims available if it runs short, cheapest first: rt_meta's names (only the data symbols the import asks for are needed
  at run time; 1.14 MB); lobby held twice (2.88 MB static + 1.68 MB reload copy) while the quest never uses it; gm_sub / plsel
  kept outside the modes that use them; the vertex ring.

Not a concern: the stack (35 KB deepest on the PC; the XBE gets 256 KB), file sizes (all streams are below 2 GB, so `long`
offsets work; FATX allows 4 GB per file).

## 3. Xbox code that hard-codes MH1 or G

| place | what | effect on an MH2 build |
|---|---|---|
| src/pc/xbox/xbox_title.h | `#ifdef VERSION_MHG` ... `#else` (MH1) | MH2 gets G's id, name, save and folder |
| tools/build_xbox.py | `--version mh1 or mhg`; title_identity() two blocks; FRONT gets -DVERSION_MHG only for mhg; iso name | no MH2 build |
| src/pc/viewer.c (XBOX block) | need[] lists for MH1 and VERSION_MHG; disc_name | MH2 asks for G's files and exits |
| src/pc/xbox/mc_xbox.c | UD_WEAR 0xC75 under VERSION_MHG | log line reads wrong User_data bytes on MH2 |
| src/pc/gfx/gfx_opts.c | INI_NAME mhgpc.ini under VERSION_MHG; settings / en.txt under XBOX_GAME_DIR | follows the identity block; on the PC MH2 names its ini mhgpc.ini (in its own app folder) |
| src/pc/rt/rt_log.c, rt_online.c | XBOX_GAME_DIR for logs, loader.txt | follows the identity block |
| src/pc/xbox/loader/main.c | game ids mh1/mhg; folder MHG = G, else MH1; "4D480001"/"4D480002" literals; titles | an MH2 loader would serve MH1 |
| src/pc/xbox/loader/install.c | "mhg" -> "MHG" by upper-casing, folder must equal the loader's game | works for "mh2" as is [read] |
| src/pc/xbox/xbox_report.c | folder "mhg" -> mhg, else mh1 | MH2's reports filed as MH1 |
| src/pc/xbox/xbox_log.c | comments name E:\Games\MH1 / MHG; code uses XeImageFileName and XBOX_GAME_DIR | fine with the identity block |
| tools/xbox_serve.py | FOLDER {'mh1','mhg'}; game by SLPM_658.69 else mh1; --xbe / --xbe-g | MH2 data would go to E:\Games\MH1 |
| tools/xbox_upload.sh, xbox_install.sh | two loaders; ELF SLPM_654.95 / SLPM_658.69 | no MH2 |
| tools/run_xemu.sh | `--version mh1|mhg` | no MH2 |
| tools/disc_files.py, ./mh1, make_release.sh | mh1 / mhg only | refuses the MH2 disc |
| src/pc/install.c | already has the MH2 list (PC install) | the CRC table to reuse for disc_files.py |

## 4. Data install path and title id

**Proposal (owner's choice; ids are placeholders like MH1's and G's):**
- Title id **4D480003** ("MH" 0003), name "Monster Hunter Dos" (the PC window says "Monster Hunter 2"; either fits the XBE's
  40-character field). Saves E:\UDATA\4D480003\4D48000300000001. No other id of that form is used in the repo (grep).
- Game folder **E:\Games\MH2**, data in E:\Games\MH2\data, short name MH2, loader game id `mh2`, XBE mh2.xbe beside the loader
  as with G.
- PS2 save folder name inside the Xbox save: unknown until MemcardWork / the card code is read.

**Data:** DATA.BIN, SLPM_662.80, AFS00.AFS, AFS01.AFS, AFS02.AFS into E:\Games\MH2\data (SYSTEM.CNF not needed). 2.72 GB, or
1.64 GB while movies are not played (AFS02 can be left out at first). The owner's E: free space is not in the docs: check before
the first install (the C: figure of 328 MB in docs/xbox.md is the wrong partition). Over xemu's network the loader install ran at
1.0-2.8 MB/s, which would be 16-45 minutes for 2.7 GB [estimate from G's numbers]. Source: the streams exist only in the image
named English v1.03 (its DATA.BIN and SLPM are byte-identical to disc/mh2, JP 1.04). install.c checks CRCs of that image, so an
owner with an unpatched JP image would be refused; whether its streams differ is unknown. Don't use tools/xbox_fatx.py `get` on
AFS00 or AFS02 (it holds the whole file in memory; it ran the laptop out of memory on G's 884 MB AFS00).

## 5. Ordered plan: title in xemu, then quest 1014

Every step is for an agent with the laptop's rules (one heavy job at a time, nice, -j4, hidden runs, xemu only with > 2.5 GB
free; xemu needs `ulimit -v` of about 6 GB). No hardware until step E, and then only the owner.

**A. On the PC, before any Xbox build**
1. ~~Owner: confirm the identity~~ Confirmed by the owner on 4 Nov 2026 (docs/DECISIONS.md).
2. Measure MH2 on the PC with the tools G used: `RT_MEM=150` on `--boot` (title), `RT_MEM=300` on `--quest 1014`
   (stage 110) and with `RT_QUEST_STAGE=1 RT_MH2_EM=1` (stage 120, Rathian), plus `RT_MEM_FILES=1` and `RT_STACK=1`. This replaces
   the guesses in section 2, and the texture line says how much stays RGBA8 on the Xbox.
3. Code, MH1 and G unchanged:
   - xbox_title.h: an `#if defined(VERSION_MH2)` block before the MHG one.
   - build_xbox.py: `--version mh2` (tree build/mh2/pctree, OUT build/mh2/xbox, FRONT `-DVERSION_MH2 -DVERSION_MHG`, mh2.iso,
     title_identity for three blocks).
   - viewer.c XBOX block: MH2's need list (DATA.BIN + SLPM_662.80 required, streams checked only when present), disc name.
   - mc_xbox.c: UD_WEAR for MH2 (from UDMO), or leave the log line out on MH2.
   - run_xemu.sh: `--version mh2` with FILES "DATA.BIN SLPM_662.80".
4. Build: tools/build_pc_mh2.sh, then `python3 tools/build_xbox.py --version mh2 --gfx null`, fix what clang / lld-link reject,
   then `--gfx nv2a`. Check that the MH1 and G Xbox objects did not change (the testers' kit check compared sections the same
   way). Expected trouble [guess]: MH2-only host files (rt_mh2*.c, title_mh2.c) compiled by clang for the first time, and stand-in
   names lld-link treats more strictly than GNU ld.

**B. Title screen in xemu**
1. `tools/run_xemu.sh --version mh2 --gfx nv2a --data <folder with DATA.BIN and SLPM_662.80> --env <RT_NOMOVIE=1> --shot ... --after N`,
   hidden. The data ISO is about 408 MB.
2. Look for, in the log: "disc directory: D:\data", DATA.BIN opened, the overlays set, the title art, PRESS START, and the
   "alive ... KB free" lines. Compare the picture with the PC's `--boot` shot at tick 150.
3. Expected, as on the PC: no sound, Start does nothing, the (C) line as the PC draws it.

**C. Quest 1014 in xemu**
1. env.txt `RT_ARGS=--quest 1014 --mute` and an input.txt (`idle*30,up*120`, as host.md's runs). `--mute` keeps the streams
   closed: MH2 sound has not run in a game on the PC either. On the Xbox `play` is always on, so the game camera should start
   without the PC's `--input` rule [read from the code, not run].
2. Stage 110: the hunter walks (the PC's round 3 result). Then `RT_QUEST_STAGE=1 RT_MH2_EM=1`: stage 120 with the Rathian. On
   the PC that still hangs in the em_cmd stage-number opcodes (docs/mh2/shared5.md); expect the Xbox's freeze watchdog to catch
   the same hang until the PC fix lands. Fix on the PC, not on the Xbox.
3. Record the "alive" free memory at the stage load and in the hunt against G's 20.8 MB. xemu's frame rate is not the Xbox's.

**D. Installing like G (still xemu)**
1. disc_files.py: an MH2 entry (boot file SLPM_662.80; sizes and CRCs from src/pc/install.c), extract from the image.
2. ./mh1 xbox: an mh2 branch to dist/xbox/MH2.
3. xbox_serve.py: game mh2 by SLPM_662.80, FOLDER 'MH2', `--xbe-2`; loader main.c and xbox_report.c: game id mh2; xbox_upload.sh
   --loader: a third loader. Extend tools/xemu_loader_test.sh with an MH2 run before any real install.

**E. Hardware (owner only)**
After D passes in xemu: check E: free space, install the MH2 loader once (owner types the FTP login), let it install the data,
then the title, then quest 1014. Power the Xbox off afterwards and check that it is off.

## 6. Unknown, plainly

- Every memory figure for MH2 above is an estimate; no MH2 run has measured memory.
- Whether title_mh2.c and the MH2 host C compile and behave the same under clang for win32 (no MH2 file has been compiled for the
  Xbox).
- The PS2 save folder name, MemcardWork, and so whether saving can work.
- How many MH2 textures are 32-bit; which data files are packed; what one stage's sound packs add up to.
- Whether an unpatched JP image carries the same AFS00-02.
- The owner's free space on E:.
