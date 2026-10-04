# Status log

## 2026-10-04, session 1: PS2 Monster Hunter G survey

Source: owner's image "Monster Hunter G (Japan).iso" (4,206,362,624 bytes),
extracted to disc/mhg/ (gitignored). MH1 not yet supplied.

### Verified (how)

- Disc root: SYSTEM.CNF boots `cdrom0:\SLPM_658.69;1`, VER 1.01. Data in
  AFS_DATA.AFS (3347 entries), AFS00.AFS (124, .adx music/streams),
  AFS01.AFS (205, .snd sound banks), all with names. 0FLIST.DIR lists the
  three AFS files. DUMMY.DAT is padding. Also IOP/MODULES/MHDRIVE.DAT
  (magic "MUU-", not yet looked at) and an NTGUIDVD/ network-setup tool.
  (7z listing; afs_extract.py --list, outputs in docs/survey/.)
- **SLPM_658.69 is stripped.** .symtab header exists with size 0; no
  .mdebug, DWARF or STABS. One unnamed alloc section, 2,028,032 bytes at
  0x00100000 holding code and data together. (elf_survey.py, docs/survey/mhg.txt)
- **Compiler: Metrowerks CodeWarrior, "MW MIPS C Compiler (2.4.1.01)"**,
  not GCC. (.comment section)
- Most game code lives in overlays inside AFS_DATA.AFS entries 0-7, each a
  Metrowerks "MWo3" module, all but dnas_ins.bin compressed with 16-bit
  word LZSS. Decompressor written: tools/mwo_unpack.py. Checked: for every
  compressed overlay, output size == 0x40 + text + data from the header,
  and the end marker is the last word of the file. Header table in
  docs/survey/mhg_overlays.txt. Text sizes: game.bin 1.23 MB,
  lobby.bin 0.68 MB, sub_main.bin 0.38 MB, select.bin 32 KB, yn.bin 35 KB.
  dnas_net/dnas_ins/nethttp are DNAS + OpenSSL + HTTP (source paths like
  ./crypto/asn1/x_x509.c in the strings): replace, do not port.
- No function names in any overlay either. Only two source file names seen
  in the main ELF strings: flps2etc.c, plAMO.c.
- select.bin contains a name-entry profanity word list; lobby.bin contains
  an HTML renderer (tag/attribute names).
- Unused Xbox HUD textures confirmed present: cpit1xb.apx, cpit2xb.apx
  (AFS_DATA entries 11, 31) next to the ps versions.

### Fixed

- elf_survey.py reported ".symtab present" for an empty .symtab header and
  printed a zero symbol table. Now treats size-0 sections as absent.

### Not verified / guesses

- MWo3 header fields after data size (bss, entry) are guesses.
- The LZSS decoder matches sizes but has not been compared with the game's
  own decompression routine.

### Next

1. Owner supplies Japanese MH1. Survey it the same way. The plan's base
   choice hinges on whether MH1's executable really has symbols.
2. Look into the compiler question (see DECISIONS.md, Open).

## 2026-10-04, session 1 (cont.): Japanese MH1 survey

Source: owner's image "Monster Hunter (Japan).iso" (4,000,579,584 bytes),
extracted to disc/mh1/. SYSTEM.CNF boots `SLPM_654.95`, VER 1.02.

### Verified (how)

- **SLPM_654.95 is unstripped: 50,151 symbols** (6,978 FUNC in main plus
  the overlays' functions, 21,180 OBJECT). Every FUNC symbol has a real
  size. No FILE symbols, so no source file names; no .mdebug/DWARF/STABS,
  so no types or line numbers. (elf_survey.py, docs/survey/mh1.txt,
  docs/survey/mh1_symbols.csv - the CSV is derived from Capcom's binary;
  decide before publishing whether symbol lists belong in the public repo.)
- Same compiler as G: MW MIPS C Compiler 2.4.1.01.
- The ELF names every overlay as its own (empty) section, with symbols
  for each overlay's functions and a **relocation section per overlay**
  (.relmain, .relgame.bin, .rellobby.bin, ...). Relocations mean every
  pointer in the code is known, which makes a clean split much easier.
- Overlays in AFS_DATA.AFS are **uncompressed** on MH1 (G compresses them).
  game.bin, lobby.bin, select.bin, yn.bin, dnas_net.bin, dnas_ins.bin. No
  sub_main.bin or nethttp.bin (those are new in G).
- MWo3 header fields confirmed against linker symbols (_game_text_size
  == header text size, etc.). Documented in tools/mwo_unpack.py.
- Function bytes by section: main 1,627,172; game.bin 1,065,496;
  lobby.bin 887,732; dnas_net 347,464; dnas_ins 118,760; yn 35,928;
  select 19,404.
  Excluding DNAS: 3,635,732 bytes (~909k instructions, ~13.2k functions).
  About 463k of that matches library name patterns (OpenSSL in main and
  lobby, CRI ADX audio, Sony libs) - a rough regex estimate, not an audit.
  So **game code to decompile is roughly 3.2 MB / 790k instructions.**
- VU1 microcode is labelled: 250 distinct Vu1Code_XXXX_YYYY program names,
  127 .vutext sections. Answers part of README's "how many VU1 programs".
- Many static functions share names across files (sound_call x25,
  ef_move_sub x24, em_act00 x20): monster/effect code is one file per
  monster with the same function names. Splitting must disambiguate.

### Fixed

- elf_survey.py counted untyped linker labels (_game_bss_end, _gp, VU1
  labels) as functions and "estimated" multi-GB sizes. It now counts typed
  FUNC symbols only (NOTYPE fallback only if a file has no FUNC symbols)
  and prints per-section totals.

### Next

1. Decide the compiler question (DECISIONS.md, Open). Blocks Phase 1.
2. Build the symbol_addrs list for splitting from mh1_symbols.csv,
   disambiguating duplicate static names by address.
3. Inventory data formats (.apx, .amo, .ahi, *_amh.bin, *_tex.bin).

## 2026-10-04, session 1 (cont.): identifying the compiler build

Downloaded (owner approved): wibo 1.2.0 (decompals/wibo, Win32 loader for
Linux) and mwcps2 3.0b52-030722, 3.0.1b74-030811, 3.0.1b75-030916,
3.0.1b87-031208, 3.0.1b95-040309 from decompme/compilers releases, into
tools/compilers/ (gitignored). New tools: tools/mips_dis.py (small
stdlib disassembler), tools/matchtest/ (test C + byte compare).

### Verified

- The .comment string "MW MIPS C Compiler (2.4.1.01)" is written by the
  **linker** (mwldps2), and every linker build tested writes the same
  string. It does not identify the compiler. (Linked a test object, and
  `strings mwldps2.exe` in all five builds.)
- Five small MH1 functions reconstructed in tools/matchtest/first5.c and
  compared with cmp.py (relocated fields masked):
  - Pl_stg_ck_tw, flmatSetTrans, fmsGetFrame: byte match with all five
    builds at -O3 and above. Not at -O2.
  - Pl_master_ck: matches **only 3.0b52-030722**. Newer builds load the
    global's high half into v0 instead of at.
  - get_clay_ptr: no match with any build/flag/spelling tried (6 C
    variants). The original loads the gp global first into a1; ours
    schedules it third into v0. Consistent with an older compiler build,
    not proven.
- Conclusion so far: optimisation is -O3 or higher; compiler is 3.0b52 or
  older. Untested older builds: 3.0b50-030527, 3.0b38-030307,
  3.0.1b51-030512, 3.0.1b44-030325, 3.0.3-020716, 3.0b22-020926.

### Update: compiler family identified (same session)

Owner approved further downloads; added 3.0b50-030527, 3.0b38-030307,
3.0.1b51-030512, 3.0.1b44-030325, 3.0.3-020716 and ps2_compilers.tar.xz
(3.0b22 x4 dates, 2.3, ee-gcc builds) to tools/compilers/.

- get_clay_ptr matched once the global was declared as an int address and
  the offset written `n * sizeof(CLAY)` (unsigned multiply). `n * 140`
  (signed) does not match. A `CLAY *volatile` pointer also matches; the
  int form was kept as the likelier source style. Either is a guess about
  the source, the bytes are what is verified.
- **All 5 test functions byte-match with every "3.0" family build**
  (3.0b22 x4, 3.0.3-020716, 3.0b38, 3.0b50, 3.0b52) at -O3, -O4, -O4,p,
  -O4,s. Every "3.0.1" family build (b44 to b95) fails Pl_master_ck (4/5).
  2.3-991202 fails fmsGetFrame.
- Working default: **mwcps2-3.0b52-030722, -O4,p** (newest 3.0-family build
  before the ELF date). The 3.0 builds are not yet told apart; bigger
  functions are needed for that. Recheck once ~50 functions match.

### Next

1. Phase 1 setup: splitter config seeded from mh1_symbols.csv, per-overlay
   relocations, byte-identical rebuild of main.
2. Keep a growing match set in tools/matchtest/ to narrow the 3.0 build.

## 2026-10-04, session 1 (cont.): Phase 1 - matching build of main

### Verified

- **The 'main' section of SLPM_654.95 (2,662,528 bytes, sha1
  8c79109f0b95eebe73a37f5870f8786f4bb32f72) rebuilds byte-identical** from
  splat disassembly: `python3 tools/build.py` prints OK after a clean
  build. Negative test: changing one immediate in asm/text/abort.s gave
  MISMATCH at exactly that address (0x199781); restoring it gave OK.
- **One function decompiled into the build**: src/pl/pl_stg_ck_tw.c
  (Pl_stg_ck_tw, 0x152030, 0x14 bytes) compiled by mwcps2 3.0b52 -O4,p
  through wibo, linked in place of its asm; still OK. The linker map shows
  the C object's .text at 0x152030 and three `jal Pl_stg_ck_tw` callers
  resolving to it.

### How it is set up

- Tools: splat 0.50.0 / spimdisasm 1.42.4 in .venv; GNU binutils 2.47 for
  mips64r5900el-ps2-elf built from source into
  ~/.local/opt/ps2-binutils-2.47 (symlinked as tools/binutils, the prefix
  must not contain spaces).
- tools/setup_split.py writes disc/mh1/main.bin, config/symbol_addrs.txt
  (18,570 symbols; duplicate static names get an _ADDRESS suffix) and
  config/mh1_main.yaml. Text is cut at the 342 .text SECTION symbols and
  around each range in config/c_files.txt.
- Layout of main (vram): text 0x100000-0x293B80; VU1 microcode
  0x293B80-0x306640 (kept as bin); data/rodata/sdata 0x306640-0x38A080
  (one bin for now); bss to 0x533980; _gp 0x38EB70.
- Gotchas found: GNU as with -mabi=eabi rejects o32 register names
  ($t4..$t7), and -mabi=32 expands 64-bit ops into macros. Fixed with
  splat `mips_abi_gpr: numeric` and `as -mabi=eabi`. ld needs
  `--no-warn-mismatch` (objects are eabi64, ld's emulation is not).
  splat's default SUBALIGN(16) is turned off (objects are not 16-aligned).
- Game code 0x100000-0x16ACC8 is a single .text block in the symbol table:
  Capcom's own file boundaries are not recorded. They will have to be
  inferred (static-name clusters, padding) as decompilation proceeds.

### Limits of the current build

- Data is one opaque bin. A C file that defines strings, floats or
  initialised data would duplicate bytes the bin already holds. Before
  decompiling such functions, split .data/.rodata/.sdata per symbol.
- Only main is built. Overlays (game.bin, lobby.bin, select.bin, yn.bin)
  are next; their symbols and relocations are in the same ELF.
- The rebuild is a flat binary of the section, not a full ELF file.

### Update: data split (same session)

- **Data now disassembled with symbols and still byte-identical.**
  Layout (vram): VU1 microcode 0x293B80-0x2E5EA0 (bin; `__data_start` is
  0x2E5EA0), game data 0x2E5EA0-0x306640 (one file, no SECTION symbols),
  library .data 0x306640-0x35C250 (cut at 109 SECTION symbols), .rodata
  0x35C250-0x386B80 (cut at 135), .sdata 0x386B80-0x38A080 (starts at
  _gp - 0x7FF0, adx_cnfvol_tbl). 246 data files.
- 19 jump tables come out as `.word .Lxxxxxxxx` tables tied to text labels.
  Unresolved data references dropped from 12,655 to 1,152 (offsets into
  objects, bss).
- Bug found and fixed: with Shift-JIS string guessing, spimdisasm wrote
  float data (0x3DCCCCCD) as half-width kana in UTF-8, growing game_data by
  600 bytes. Now ASCII-only string guessing; Japanese text stays as bytes.
  Found by comparing built symbol addresses (nm) against symbol_addrs.txt;
  first moved symbol was light_specular01.
- The library data/rodata split follows SECTION symbols. Whether every
  Capcom object's strings sit in .data or .rodata is not checked yet.

### Update: overlays (same session)

- **All five modules rebuild byte-identical**: main (2,662,528 bytes),
  select.bin (32,768), game.bin (1,423,232), yn.bin (56,576), lobby.bin
  (1,265,152). Targets are the MWo3 files from AFS_DATA.AFS, header
  included. sha1s are printed by tools/build.py.
- Only raw bins left: the four 64-byte MWo3 headers (splat `textbin`, so
  they stay in front of the code) and main's VU1 microcode. 1,382 asm files.
- Overlay calls into main resolve by name (e.g. game.bin `jal
  get_joint_pos_em`); 0 jal targets left unnamed in game.bin.
- setup_split.py now makes one splat config per module
  (config/<module>.yaml) and symbol files in config/symbols/. Names are made
  unique across all modules. c_files.txt has a module column; C lives in
  src/<module>/.
- splat prints "Unable to determine a segment" for main's symbols when
  splitting an overlay. Harmless here (the build matches); could be
  silenced with `absolute:True` on those symbols.
- tools/progress.py: 1 of 12,585 game functions (20 of 3,481,696 bytes).

### Next

1. Start decompiling for real. Good first targets: small leaf functions in
   main's player/monster code, building up shared headers (PLW, EMW...)
   as offsets are confirmed.
2. Infer Capcom's file boundaries in the big text blocks (needed before
   whole files can be compiled as C).
3. Data typing per object (strings in .data vs .rodata) as C files appear.
