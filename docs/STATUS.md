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

### Update: inferred source-file boundaries (same session)

tools/file_bounds.py rebuilds the original object-file layout from the
order of LOCAL symbols in the ELF symtab. Results in
docs/survey/mh1_<module>_files.csv (start, end, kind file/gap, prefix).

How it works (each step was checked, not assumed):
- The linker writes each object's locals as one contiguous run; inside a
  run the order is scrambled (hashed), between runs it follows link order.
  A cut is accepted where everything before is below everything after, per
  output-section bucket, within a 200-symbol window.
- Buckets must be real output-section regions. main: text, data
  (0x2E5EA0-0x357980), string literals in .data (-0x35C250), named rodata
  (-0x3671C0), rodata literal pool (-0x386B80). Mixing data with the string
  area collapsed all of Capcom's code into one group. sdata and bss are
  excluded: their symbols are not in link order.
- Outliers: crt0's `_root` (0x100220) is listed after the overlays'
  symbols and blocked every cut in the fl* engine library. Groups whose
  functions form several address runs have the smaller runs removed and
  the partition is redone (1 outlier in main, 0 in overlays).
- Validation against the ground truth that exists: none of main's 342 or
  lobby.bin's 588 library .text SECTION boundaries falls inside a detected
  file, and no two detected files overlap.
- Result: main 242 code files, game.bin 145, lobby.bin 95, select 2,
  yn 5, plus "gaps" that hold only global functions (no locals to place
  them). game.bin has one file per monster (em01, em04, em09, em15...),
  per effect (eft*), set piece (set*) and projectile (shell*).
- Known weaknesses: it over-splits where a file's locals happen to be in
  address order (set01 comes out as 3 pieces), and globals in gaps are not
  assigned to a file. Padding between functions is not a usable signal
  (functions themselves are padded to 8/16 bytes).
- The asm split now cuts at these starts (files f_<prefix>, gaps
  g_<first function>): 1,694 text files. All five modules still OK.

### Update: workflow tools and first whole source file (same session)

- tools/check.py compiles a C file and compares every function against the
  original in any module, masking only fields covered by the object's own
  relocations; -v prints a side-by-side diff; --add appends the range to
  c_files.txt once every function in it matches.
- tools/draft.py runs m2c (cloned into tools/m2c, gitignored) on single
  functions pulled out of the split asm, with registers renamed from $31
  to $ra (m2c needs names).
- **src/main/set/set12.c: the whole set12 object file (6 functions,
  0x1567C0-0x1569D8) matches** and is built as one C file. Shared struct in
  include/set.h (SETW, offsets taken from matched code). Took one fix
  round: the timer test is `(u16)timer != 0xFFFF`, the counter is
  incremented and compared in separate statements, and se_req2 takes six
  arguments (MWCC EABI passes up to 8 in a0-a3, t0-t3).
- func_001567B4 in the split is alignment padding spimdisasm took for a
  function. The set12 file really starts at set12_set.
- setup_split.py now drops inferred cuts that fall inside a C range
  (otherwise splat emits asm for the same code and it links twice).
- Progress: 11 of 12,585 functions.

### Update: player file 0x14F030 (same session)

- 11 of its 13 functions match and are built: src/main/pl/pl_normal.c
  (0x14F030-0x14F0C4) and pl_normal2.c (0x14F330-0x14F850, includes the
  pad reader sw_set_sub). include/pl.h holds PLW (0xA00 bytes, ~55 fields
  placed from matched code, most still named by offset) and PLSW (pad
  state at +0x364). include/game.h holds GAME_W. Progress: 22 functions.
- **Open problem (compiler):** normal_char_set and to_normal are one
  instruction from matching (src/main/pl/pl_normal_nm.c, not built). In a
  switch's default case our builds reuse the constant 1 left by the
  `case 1` compare; the original reloads it. Same in every 3.0-family
  build on decomp.me (3.0b22-020926/3.0b38/3.0b50/3.0b52), unaffected by
  -opt sub-options, statement order or switch/if forms. Suspect a build
  we do not have. Revisit if more cases turn up.
- That function did narrow the compiler: 3.0.3-020716 and 3.0b22
  011126/020123/020716 are 44 instructions off; -O4,s/-O3/-O4 are 44 off.
  -O4,p confirmed. Remaining candidates: 3.0b22-020926, 3.0b38, 3.0b50,
  3.0b52.
- MWCC conventions learnt (each confirmed by a match):
  - `switch` cases are tested from the highest value down and `default`
    is laid out first when it is written first; Capcom writes `default:`
    first.
  - Calls without a prototype (`void pl_chr_set();`) re-mask u16 args
    (andi 0xFFFF); with a prototype they do not.
  - `static` changes the caller: MWCC knows a static callee in the same
    file leaves a0 alone and skips reloading it. Keep the original's
    statics or callers stop matching.
  - u16/s16 locals load constants with daddiu, ints with addiu.
  - Up to 8 args in a0-a3, t0-t3 (EABI).
- A C file must cover a contiguous range, so a non-matching function in
  the middle splits the file (pl_normal.c / pl_normal2.c). Callers of a
  static in the other half are fine as long as the a0-knowledge above
  does not cross the split.

### Update: set06, set01 and relocation-refined file boundaries

- src/main/set/set06.c (7 functions) and set01.c (14 functions, the info
  banner: queue, slide, text drawing) match. 43 functions total.
- More conventions, each from a match: flSetRenderState is (int, u32) and
  Capcom passes pointers cast to u32; a `none = -1` local assigned per
  branch; string tables must be declared with their real sizes (MWCC
  uses gp-relative addressing for objects of 8 bytes or less only when it
  knows the size); strlen returns unsigned; `-O4,p` unrolls fixed loops
  (a 30-byte copy became 5 x 6 bytes); `++x >= 20` vs `x++; if (x >= 20)`
  compile differently.
- Literal strings already in the data asm (e.g. "%s" = lit_355_0035B7C0)
  are referenced as externs, not redefined, until data is split per file.
- The link caught Info_control (a global in a "gap") calling set01's
  statics: it belongs to set01's file. Turned into a general rule in
  file_bounds.py: **code that references a LOCAL symbol (by name, or a
  per-object .text SECTION symbol) is in that symbol's file.** Using the
  ELF's relocation tables: main 339 gap functions placed and 26 over-split
  pieces merged; game.bin 103 placed, 48 merged (gaps 95 functions, was
  ~1,000); lobby 34/9. Still 0 known library boundaries crossed, 0
  overlaps. set01's file now starts at 0x1554E0 automatically. (em08's
  file contains em_act21/em_fly21: those are action numbers, not em21.)

### Update: shells, per-file data, permuter (same session)

- **Per-file data**: a C file's own data (MWCC puts switch jump tables in
  .rodata) is placed back into the data region with a line like
  `game:rodata 0x0068A3F0 0x0068A410 shell/shell18` in c_files.txt.
  setup_split cuts the data asm there; build.py renames the object's
  .rodata to .data when the slot is in a region typed data
  (config/c_renames.txt, generated). Adjacent slots work.
- **check.py now verifies call targets** (R_MIPS_26 relocs resolved to
  names and compared with the original's target). Before, a wrong case
  order in shell18_move passed the check and only the full build caught it.
- **decomp-permuter** (tools/perm.py) solved get_sw (`return sw & 0xFFFF`)
  and swset (copy/clear loop + chained zeroing). Runs in the background.
- Shells (game.bin projectiles/attack objects): include/shell.h (SHLW) and
  include/em.h (EMW, monster work 0xA10 bytes; shares its first fields
  with PLW). shell18 by hand; tools/gen_shell.py generates the template
  family from the original asm (move switch order from the jump table,
  type number, _m case list, second-animation-channel and arg-dependent
  flag variants) and registers only verified matches: shell13/15/16/18/
  20/21/23. shell19 by hand (its _m has two switches).
- Conventions learnt: MWCC tests case labels in **reverse source order**
  (descending values only because Capcom usually writes them ascending);
  a vector copied through lw/sw is a word-wise copy macro (VEC3_COPY), not
  a struct assignment; jump tables live in .rodata.
- Parked near-matches (src/**/*_nm.c, not built): pl_normal_nm.c
  (normal_char_set, to_normal: constant-reuse quirk), adx_nm.c,
  release_texture_nm.c.

- Later in the session: shell01/02/04/05/11/17 finished by hand on top of
  the generator output (extra spawners set2-set4, draw callback at
  SHLW+0x14, flash "senko" object, owner-alive checks). tools/
  register_shell.py registers a hand-finished shell with its jump-table
  slot. Matching shells: 01 02 04 05 11 13 15 16 17 18 19 20 21 23.
  Progress: 147 functions.

### Update: more shells and set files; a pattern in the compiler quirk

- Matching now also: shell09/10/14, set03 (split), set07, set10.
  198 functions incl. everything earlier.
- EMW+0xA0 / SHLW+0x24 are integer rotation angles (shell14_trans converts
  them with 2*pi*v/65536), not positions; the "VEC3_COPY" guess is gone.
  PLW/EMW world position is at +0xAC.
- tools/declperm.py tries every order of a function's local declarations
  (MWCC assigns saved registers partly by declaration order); fixed
  shell14_trans, set07, set03_trans.
- **Second sample of the compiler quirk**: set03_m keeps a divide-by-zero
  trap on `% num` where num is always 3, and converts a constant 100 from
  a register in set03_trans; our builds (3.0b22-020926/b38/b50/b52) fold
  the first and drop the trap. Same family as normal_char_set: Capcom's
  compiler propagates constants less than any build we have. Every other
  build is far worse on this function too. Handled by splitting the file
  around the one function (set03.c / set03b.c, set03_nm.c parked).
- Parked near-matches: shell00 (shell00_i 2 off, permuter no help),
  set17 (set17_trans: 10 off; a full search of case 1's statement order
  found nothing better), set22_m (trap), Set20_set (original leaves the
  delay slots of its quest-number compare chain empty; cause unknown).
- set00, set11, set15, shell03 match; set05 matches apart from set05_m
  (2 instructions, split into set05.c / set05b.c, near-match in
  set05_nm.c). Findings, checked with tools/check.py:
  - A table read earlier than a call that precedes it in the source means
    the source read it into a local first (set00_trans, stage 41).
  - `(u16)(s32)(f)` converts float to a 16-bit angle with a plain cvt.w;
    `(u16)f` goes through the unsigned conversion.
  - Pl_stg_ck returns u8 (callers mask with 0xFF).
  - set05_m: u16 kind + u8 n moves every temporary; u8 kind + int n is
    2 off. Type changes to small locals reshuffle temporaries, not just
    saved registers.
  - shell12 (player traps) matches. `x >= N` versus `x > N-1` keeps
    mattering: the original's comparisons that use the `at` register are
    the `> N-1` form (5 cases in shell12_m alone).
  - A C file with several jump tables needs ONE rodata slot in
    config/c_files.txt covering all of them (MWCC emits a data section per
    function and each slot line pulls in the whole object's data).
- **The "divide trap quirk" was a compiler setting.** Capcom built with
  `#pragma divbyzerocheck on` (bne/break after every division by a
  non-constant). Found by searching the compiler binary for pragma names
  after eft11_m showed our build skipping the check even where the divisor
  was unknown. tools/check.py and tools/build.py now pass
  `-pragma "divbyzerocheck on"`; every registered file still matches, and
  set03_m and set22_m now match, so set03 and set22 are whole files again.
- Effects started (include/eft.h, EFTW): eft00, eft07, eft09, eft19,
  eft21 match; eft05, eft10, eft24 match apart from one function each
  (split as before; *_nm.c holds the near-match). Findings:
  - `if (flag != 0)` on an s16 local re-sign-extends before the test, as
    the original does; a bare `if (flag)` does not (eft21 i and m).
  - A clamp written `(t > 15) ? 15 : t` matched where `if (t > 15) t = 15;`
    left an extra nop (eft19).
  - eft24_m: another divide trap kept by the original after an explicit
    `n == 0` check (compiler quirk, same family as set03/set22).
  - Early `return`s and nested ifs compile differently: eft07_t needed the
    nested form.
  - Some functions get called with fewer arguments than elsewhere (eft09
    calls Ext_pick_point_clr() bare), so those are declared without a
    prototype.
  - CLAY: material count at +4 and 32 material indices at +8 (shell03_trans
    sets each material's colour to white before drawing).
- set20 (stage 25 gate) matches apart from Set20_set. `++t > 180` and
  `++t >= 181` compile differently (slti into at vs v0); the original used
  the first.
- set04 and set08 (game.bin) match. set08 is the floor/water tiles on
  stages 0/26 (8x10 grid, model per tile from st00_obj_type0/1, culled with
  flCheckMeshFOV) and a 3x3 grid elsewhere, with scrolling UVs. Its
  set08_set writes type 6, not 8 (checked against the bytes). Lessons from
  set08_trans, all checked with tools/check.py:
  - An index `k * 2` shows up as a separate +2 counter on the stack; MWCC
    strength-reduces it, so write `k * 2`, not a second variable.
  - A stack aggregate of 16 bytes or more is 16-aligned. The culling sphere
    sits at sp+0xD4, which only fits a 28-byte struct with x,y,z,r at +4.
    Unused locals are dropped, so they cannot fill a gap.
  - Float registers follow declaration order. The texture-scroll values
    and the tile position needed separate variables (u,v,w vs x,z,y) even
    though the original reuses the same registers for both.
- set16, set18, set19 match; set22 matches except set22_m (split into
  set22.c / set22b.c like set03, near-match in set22_nm.c). Findings, each
  checked with tools/check.py:
  - set16_trans: pointers held in saved registers for one branch come from
    a one-pass loop (`for (i = 0; i < 1; i++)`) that the compiler unrolls.
  - `(int)(u8)x` converts a byte to float with a plain signed convert; a
    bare `(u8)x` gives the unsigned-conversion sequence instead.
  - ran_suu returns a 32-bit value. Capcom writes `(u16)ran_suu(1)` at most
    call sites, but set22_m's rotation uses it uncast (no andi 0xFFFF,
    unsigned float convert). Earlier files that declared it `u16` still
    match because they mask anyway.
  - set22_m: `switch { default: case 7: n = 5; ... case 0x19: n = 5; }`
    reproduces the original's leftover `li 7` with no compare; the only
    remaining difference is the `% n` divide trap (the set03 quirk again,
    checked against every compiler build in tools/compilers).
  - set19: three `*pos++` reads compile to two combined increments and one
    separate, which is how the original reads its position tables.

### Next

1. Remaining shells (00, 03, 06, 08, 09, 10, 12, 14, 22: bigger, with
   draw code) and set*/eft* in game.bin.
2. Larger player and monster files as PLW/EMW fill in.
3. normal_char_set: a 30-minute permuter run (~608k candidates) found
   nothing better than the one-instruction difference; its "best" outputs
   only change behaviour (wrong constants). Further evidence that the
   quirk is the compiler build, not the source. Left parked. Good first targets: small leaf functions in
   main's player/monster code, building up shared headers (PLW, EMW...)
   as offsets are confirmed.
2. Infer Capcom's file boundaries in the big text blocks (needed before
   whole files can be compiled as C).
3. Data typing per object (strings in .data vs .rodata) as C files appear.
