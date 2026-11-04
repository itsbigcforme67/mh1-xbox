# Status log

A dated log, oldest entry first and the latest at the end. Each entry says what was done, how it was checked, and what
came next. A short summary of where things stand comes first.

## Where things stand (3 Nov 2026)

Gathered from the entries below during the readability pass of 3 Nov. Numbers are the last ones recorded; see the entry
named for each.

- **MH1 (Japanese, the test bench).** All five modules rebuild byte-identical. 59.668% of the code matched (7,204 of
  12,585 functions; 12 Oct). The PC build plays the whole single-player game and the online town against our own server.
  The real Xbox runs a full quest loop at ~30 fps with sound and saves (13 Oct). (CLAUDE.md said "about 8% as of 5 Oct"
  until 3 Nov; it now points here.)
- **MH G (the main release).** Rebuilds byte-identical (7 modules) with `VERSION=mhg`. 19.8% matched (15 Oct, last number in
  this log). Village and all 98 offline quests on the PC; installed on the real Xbox and runs the village at 30 game frames a
  second (29 Oct). English: 47% official text on 15 Oct; the source order (Freedom first) was decided on 28 Oct
  (docs/DECISIONS.md).
- **MH2 / Dos.** Main and all 12 overlays rebuild byte-identical (13 of 13, tools/mh2_check.py). 2,319 functions, 6.99%
  matched (4 Nov, round 5 below). On the PC, `--quest 1014` runs MH2's own quest flow: the hunter walks, the Rathian
  animates and flies with RT_MH2_EM=1, and both quest end paths finish (the result screen and village are stand-ins).
  Stage 120 starts. Xbox not tried.
- **Gap in this log:** there are no entries for 16-28 Oct 2026. Decisions of those days are in docs/DECISIONS.md; agent
  hand-overs of 23-29 Oct are in docs/agents/COORDINATOR.md ("Current assignments").
- **Next:** MH2: the "Next" lists of docs/mh2/host.md and docs/mh2/shared.md. G and MH1: docs/agents/COORDINATOR.md.

## 4 Oct 2026, session 1: PS2 Monster Hunter G survey

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

## 4 Oct 2026, session 1 (cont.): Japanese MH1 survey

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

## 4 Oct 2026, session 1 (cont.): identifying the compiler build

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

## 4 Oct 2026, session 1 (cont.): Phase 1, matching build of main

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
- Solved (Oct 2026): normal_char_set and to_normal match with `case 0: default:`
  on the switch's default case (a lone `default:` reuses the constant 1 of the
  `case 1` compare; the original reloads it). Both are in
  src/main/pl/normal_char_set.c; pl_normal_nm.c is gone.
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
- Parked near-matches (src/**/*_nm.c, not built): adx_nm.c,
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
- `#pragma opt_common_subs off` scoped to normal_char_set makes it match
  (the old "constant reuse" quirk). Found by sweeping the compiler's opt_*
  pragmas over the near-matches. It does not help to_normal (its last call
  needs CSE), shell00_i or set05_m, so it is a workaround, not a claim
  about Capcom's settings.
- Effects started (include/eft.h, EFTW): eft00, eft07, eft09, eft19,
  eft21 match (eft10 too, see below); eft05, eft24 match apart from one
  function each
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

### Update: effects, set14 and shell22 (date not recorded)

These notes used to sit under "Not verified / guesses" of the 4 Oct G survey, where they did not belong. They are
matching notes from the Phase 1 work, each checked with tools/check.py. The day they were written was not recorded. The
effects update above says "eft10 too, see below" and the eft15 notes here explain how eft10_m came to match, so they
probably belong to the same day (a guess).

- eft22 (the fishing float, include/uki.h) matches except eft22_end_init
  (4 instructions: the 35.0f/27.0f flight-time constant lands in v1 where
  ours uses v0; an 8k-candidate permuter run found nothing). Split into
  eft22.c / eft22b.c, near-match in eft22_nm.c. The 2956-byte eft22_m
  matched. Findings, each checked with tools/check.py:
  - MWCC tests switch cases in reverse source order: the original's
    0x60, 0x63, 0x62 sequence comes from `case 0x62: case 0x63: case 0x60:`.
  - A two-way pick written `if (c == 0) lim = 50; else lim = 20;` matched;
    every ternary spelling gave movz instead of movn.
  - A `switch` with one case plus default compiles differently from the
    same `if`/`else` (eft22_t, eft22_line_sub).
  - eft22_se_req ignores its first argument and passes 1 to se_req2.
  - PLW +0x8E8 holds the bait kind here (0x60/0x62/0x63 make the float drift
    gently), so the "fish_time" name from eft23 is unverified.

- eft15 (sprite bursts, nine types) and eft14 (sparks and flashes, eleven
  types) both match as single files. Findings, each checked with
  tools/check.py:
  - eft14_m00's two UV-frame searches keep a loop counter `j` that is
    never read. Our build dropped it until the same variable was also used
    for the UV nibble in the type 3/6 branch (`j = p->uv & 0xF; ...`): a
    variable with other uses keeps its increments.
  - Keyframe reads: `d = eft15_data[idx++]; eft_vec_linear(p->lag, d, ...)`
    matches; passing `eft15_data[idx++]` straight in loads lag first and
    shifts registers (eft08 already used the local; the permuter found it
    again).
  - Loops that spawn sprites index the work directly (`w[i].lag = ...`).
    Stepping a pointer (`p++`) emits its increment after the other
    induction variables instead of first (eft14_i00, eft15_i).
  - eft15_m's joint switch has no default: the original leaves `joint`
    unset there and the register still holds the 4 from an earlier
    compare. Writing `default: joint = 4;` adds code.
  - `if ((ew = pull_eft_work(1)) != 0)` tests v0 directly; a separate
    assignment tests the saved copy (Eft15_set, eft14_set, Eft14_set3/4).
  - `&mw->clay[p->lag] + 27` and `&mw->clay[p->lag + 27]` differ.
  - A plain int passed to make_mat_srt's u16 argument gives the `andi
    0xFFFF` at the call; a u16 local does not.
  - `ew->arg <= 1U` gives the original's `sltiu at`; `< 2` and `<= 1` give
    slti.
  - Argument evaluation order shows parameter order: Eft14_set3 is
    (pos, arg, scale, pl) and eft_trans_sub is (clay, mat, flag, alpha,
    mats), since the float is set up before the pointer after it.
  - Some float constants are one ulp off the obvious decimal (0x3C75C290 =
    0.015000001f, 0x3C23D70B = 0.010000001f); write them that way.
  - Local declaration order sets saved-register order. For eft15_t a
    brute-force over the 120 orders of five declaration groups found the
    matching one (`ew, p, mw` first, then `mats, cl, flag, order`).
  - A drot field read with `lhu` in `rot += drot` is still s16 (eft14 and
    eft15 both match only with s16).
  - Those two fixes plus the eft15_m layout made eft10_m match (it had
    been 191 instructions off). Its last 5 instructions were the order of
    the six table reads at the top: a brute force over the 720 orders found
    num, all, idx, step, time, tt. In eft10 `drot` is u16 (the original
    loads rot first there).

- eft12 (cooking: the barbecue spit, smoke cloud, meat on the spit with
  its smell, the thrown egg and steam) matches in full, 28 functions. New
  GAME_W fields: x1E (smoke UV scroll), x80[4] (per player, copied into the
  smell) and meat_num (0x211). Findings, each checked with tools/check.py:
  - Loop increments are emitted in the order of the for-expression.
    eft12_m01 matched only as `for (i = 0, r = rotz77; i < 4; i++, p++,
    r++)` with `p->rot += *r`; indexing `rotz77[i]` let the compiler add
    its own pointer after `p`.
  - Argument types show in the caller: a parameter declared s16 is passed
    on unextended (Eft12_set3, eft12_set_sh), an int is sign-extended
    before the call.
  - Colour words are built `((g << 8) | ((a << 24) | (r << 16))) | b`;
    the plain left-to-right OR chain swaps one operand pair.
  - A store placed before a call lands in its delay slot: eft12_t01 clears
    enmaku_flag before clay_attr_reset(), not after.
  - `game_w.x1E` (u8) converted with a signed cvt needed `(f32)(int)`.
  - Six u8 colour locals had to be declared bone r,g,b then meat r,g,b to
    get the saved-register order.

- eft18 (blasts and debris, twelve types; type 4 throws rocks that fall
  to the ground) matches except eft18_m00 (2 instructions: the owner and
  joint loads for get_joint_wmat come out in the other order) and
  eft18_set_com (7: the original leaves the delay slot after `beqz ew`
  empty). Split into eft18.c / eft18b.c / eft18c.c, near-match in
  eft18_nm.c. Findings, each checked with tools/check.py:
  - A `p++` at the end of the loop body is emitted before the compiler's
    own induction variables; in the for-expression it comes after them
    (eft18_i00 matched only with `p++;` as the last statement).
  - Reading a table entry through a pointer (`sp = &tbl[k]; a = *sp;
    b = *sp;`) reloads it after each store, as the original does;
    `tbl[k]` three times is loaded once.
  - Ternary conditions test the variable itself: `(i ? -0x4000 : 0x4000)`
    gave the original's movz on i; `i == 0 ? ... : ...` did not.
  - eft18_i02 adds the rock's sideways offset to its velocity, not its
    position (checked: the stores go to +0x08..0x10, the velocity).
  - New SHLW field pos0 (0x3C), used by Eft18_set3 to size the effect by
    the distance from pos2.

- set14 (a stage overlay with scrolling masked textures; on stages 51-53
  two of five layers fade at random intervals) matches except set14_trans,
  its draw function (still assembly, last in the file so no split).
  set14_m repeats the same mask-timer code for stages 51, 52 and 53; it
  matched written out three times.

- shell22 (cannon rounds, stage cannons on stages 12/25, debris, and rocks
  dropped on stages 11/28/30) matches except shell22_i and shell22_h.
  Split into shell22.c / shell22b.c / shell22c.c, near-match in
  shell22_nm.c. Findings, each checked with tools/check.py:
  - shell22_trans matched only with `goto end;` for its model checks: the
    original branches straight to the function end there, while its
    switch defaults (`return`) go through their own jump blocks.
  - Confirmed again across several switches: compare chains test the cases
    in reverse source order, and the bodies are laid out in source order.
  - A shell spawner's `u16` angle argument shows up as lhu at the call
    (Eft18_set5 is declared with u16 angles here).
  - The landing test `g = 50 + GetGroundShellHit(); if (y <= g) { y = g;
    ...}` keeps the store inside the branch.
  - Small gp tables need their real sizes declared (shell22_tbl is 4
    bytes).
  - check.py reports one difference in shell22_trans only because the
    original names its draw helper disp_sub; the built bytes match.

### Update: shell06b and shell06 (bowgun shots), 4 Oct 2026

src/game/shell/shell06b.c, 0x62C4D0-0x62D4C4 (19 functions, all byte-match,
checked with tools/check.py and `tools/rebuild.sh game` = game OK). Types in
include/shell06.h: the shot keeps a work block at sh+0x18 (SH06W), and damage
falls off in steps (shell06_time_ck/atck_data_calc) scaled by the gun's growth
row plus silencer/long-barrel rows. Matching notes:
- `x > 0xFF` gives `slti at`; `x >= 0x100` gives `slti v1` (shell06_m,
  shell06_change_atck_data).
- Constant-index table reads `tbl[k][0..2]` inside a loop get their addresses
  hoisted and spilled (shell06_eft_t); a pointer `p = tbl[k]` does not.
- Shell06 first half (set, init_sub, move_sub, hit, trans_sub) still to do.
- shell06.c: shell06_set .. shell06_init_sub (0x62A6C0-0x62B154) match too
  (check.py + game OK). Lessons: when every call re-extends an s16 argument
  (no CSE of dsll32/dsra32), the source passed `(s16)joint` of an int
  parameter to an s16 prototype (shell06_set). Load order of three struct
  fields picked the float registers in shell06_init_sub (spd, spread, drop).
  Still to do: shell06_move_sub, shell06_hit, shell06_trans_sub.
- shell06_hit and shell06_trans_sub match and now head shell06b.c
  (0x62BDA0-0x62D4C4; its rodata slot 0x68A060-0x68A0D0 spans a 4-byte gap
  between jump tables and still builds identically). shell06_move_sub is
  one branch off (a compare-chain `beq` to the next instruction that my
  switch drops); full near-match in shell06_nm.c, permuter running.
  Lessons: `x > 0xFF` / `x > 2` / `a > b` (not `>=`/`<`) give the `slti at`
  forms; Shell09_set_pl2's stg is `u8` (fixes argument load order).
- get_joint_wmat's joint parameter is `s16`: with that prototype the
  owner/joint load order comes out right. This fixed eft18_m00 (was 2 off),
  which is now in eft18.c (0x551830-0x552CF8, rodata to 0x6857EC).
  Verified: check.py OK and `tools/rebuild.sh game` = game OK.

### Next

1. Remaining shells (00, 03, 06, 08, 09, 10, 12, 14, 22: bigger, with
   draw code) and set*/eft* in game.bin.
2. Larger player and monster files as PLW/EMW fill in. Good first targets:
   small leaf functions in main's player/monster code, building up shared
   headers (PLW, EMW...) as offsets are confirmed.
3. normal_char_set: a 30-minute permuter run (~608k candidates) found
   nothing better than the one-instruction difference; its "best" outputs
   only change behaviour (wrong constants). Further evidence that the
   quirk is the compiler build, not the source. Left parked. (Solved
   later: `case 0: default:`, see "Update: player file 0x14F030".)
4. Infer Capcom's file boundaries in the big text blocks (needed before
   whole files can be compiled as C).
5. Data typing per object (strings in .data vs .rodata) as C files appear.

## 5 Oct 2026: PS2Recomp feasibility test, Wii MH G, first model off the disc

### Verified
- Agent A's tools/clay_dump.py exports em01_amh.bin (Rathalos) from
  disc/mh1/AFS_DATA.AFS to .obj: 5 parts, 4184 vertices, 4583 triangles; a
  flat-shaded render is clearly Rathalos in bind pose (no textures yet). Format
  write-up: docs/formats/graphics.md.
- PS2Recomp (ran-j/PS2Recomp, commit c5a9d02, cloned to tools/ps2recomp,
  gitignored; built recompiler + analyzer only with g++ 13 and pip cmake/ninja,
  runtime skipped because it needs FFmpeg dev libraries):
  - main (SLPM_654.95, has symbols): analyzer 8 s, recompiler 7 s,
    7223 C++ files. 26800 "unhandled instruction" errors, all at addresses past
    the last main function (0x293B68), i.e. data taken for code. Zero errors in
    real code.
  - game overlay (our build/game.elf, linked with the full symbol list):
    2766 files, 16779 errors, all past the last game function (0x63BB50).
  - A sample output file (shell06_d) compiles with g++ -std=c++20 against the
    runtime headers. Output is literal: one C++ statement per instruction.
  - All four overlays (game, lobby, select, yn) link at 0x533980, so a hybrid
    build needs per-overlay function tables switched when the game loads one.
- The owner's Wii "Monster Hunter G" (Japan, RVL SDK GX build of Sep 2008) is
  in disc/mhg_wii (extracted with the Dolphin flatpak's dolphin-tool).
  sys/main.dol (7.2 MB) is stripped (only source file names like
  slib_ftask.c), but keeps the same asset paths as the PS2 game
  (emmodel/em01/em01_amh.bin, motion/em01_tbl.bin) and the clay system
  ("CREATE CLAY FAIL !!"). Not yet checked whether the data format is the same.

### Not verified
- That recompiled code runs: the runtime was not built or linked yet.

## 5 Oct 2026 (later): pause point, handover

- 1095/12585 functions match (10.3%); all five modules rebuild byte-identical
  (verified with tools/rebuild.sh after the last merges). Everything merged is pushed.
- PC viewer (src/pc/, tools/build_pc.sh, docs/pc.md) renders stage 4 + Rathian
  + hunter in real time from disc/mh1; verified by offscreen screenshots only.
- Paused by the owner. Agents were asked to commit what builds and record where
  they stopped in docs/agents/agent-X.md. Their branches (agent-A..F in
  ../mh1-wt/) may hold commits not yet merged into main: merge them first
  (procedure in docs/agents/COORDINATOR.md, which also lists assignments).
- The agent running as "A" was replaced by a fresh agent on the port-runtime
  skeleton task (run decompiled game C natively on top of src/pc/gfx).
- PS2Recomp runner build (no longer needed: the owner dropped PS2Recomp on 3 Nov 2026, docs/DECISIONS.md): last attempt was
  stopped by the app restart; rerun
  `ninja -C tools/ps2recomp/out/build ps2EntryRunner` (the isnan fix is applied).
- Usage pacing (hourly check, pause at ~88%) was a session-only cron; recreate it
  when resuming agents.

## 5 Oct 2026 (evening): agents resumed, merges

- Owner: no scheduled usage checks; keep going until told to stop; 32-bit PC host confirmed.
- Merged all branches (A twice more, B, C twice, D, E, F); all five modules byte-identical
  after each merge. tools/build.py now skips src/pc/.
- Progress (tools/progress.py): 1,467 of 12,585 functions; 456,520 of 3,481,696 bytes (13.112%).
- PC runtime (agent A): all decompiled set/eft/shell C runs natively; trans_stage written
  (near-match C) fixes the floor holes; all 88 stages render (contact sheets in
  ../mh1-wt/A/build/show/A/stages/). Plan for player+monster with input: docs/pc.md "Plan".
- Running: A motion system + pad backend; B f_em_*; C f_menu display code; D weapon/rail cam/hit;
  E f_quest/f_stage; F f_pl.

## 5 or 6 Oct 2026: 35% matched, PC hunt works

(This heading used to say "2026-10-05 (afternoon)". It comes after the 5 Oct evening entry, which had 13.1%, so the date or
the time of day is probably wrong: likely 6 Oct. Kept in its place in the order of progress. Not checkable from git: the
repo's history starts on 29 Oct 2026.)

```
main           6525    1516544   304460 20.076%
select           44      19404     9736 50.175%
game           2640    1065496   877480 82.354%
yn              104      30372     3492 11.497%
lobby          3272     849880    23524  2.768%
total         12585    3481696  1218692 35.003%
4318 of 12585 functions decompiled
```
All five modules byte-identical after every merge; main pushed.
- PC runtime (agent A): hunter runs the game's player code (move, roll, draw, attack combos,
  guard; sword-and-shield and great sword) with sounds; collision, camera, audio (ADX music,
  ambience, SEs). Rathian runs her own AI (em01 + em_core + em_cmd): notices, roars, charges,
  bites (hunter 100 -> 51 HP), takes hits (2500 HP from em01_init). `mhview --quest 10`.
  Not yet: carving, quest clear, death/restart, HUD (agent A working on it).
- Network outage ~09:50-10:30 killed all agents; restarted, nothing lost.
- Merge rule now: agents merge main before reporting; a conflicting merge on main is
  aborted and handed back to the agent (docs/agents/COORDINATOR.md).
- Running: A full quest loop on PC; B lobby 0x533980-0x5C4E60; C main leftovers + menu03/04
  range overlap; D push game overlay to 100%; E (background permuter on mc_sel_ck; restart
  E when it finishes: memory-card run, IME near-matches); F lobby 0x5C4E60-end.
- Owner said (5 Oct): keep going until told to stop; no scheduled usage checks.

## 7 Oct 2026: checkpoint (coordinator)

- 55.3% matched overall (6902 / 12585 functions): game 93.2%, select 90.8%, yn 68.9%, main ~39%, lobby ~35.5%.
- PC build: power-on -> title -> character creation -> Kokoto -> quests -> reward -> save/continue
  works; every monster kind runs; gathering, fishing, flash/sound bombs, intro cutscenes work.
  tools/play.sh (boot) / play.sh quest. ARM box (RK3518): ~25 fps at 960x720 in fights, 48 at 640x480.
- Owner decisions today: village (single player) before online; in-game web browser paused
  (website/subscriptions only). Open question to the owner: when to start the original Xbox (nxdk) build.
- Agent ranges: see docs/agents/COORDINATOR.md. Lessons for agents: end of docs/agents/BRIEF.md
  (one TU per original file; literal addresses -> symbol fields; never pkill -f).
- Coordinator: check-then-push only (rebuild all five OK + build_pc builds), one check at a time.

## 7 Oct 2026: checkpoint, paused at the weekly usage limit

All agents stopped cleanly by the coordinator. main = origin/main, verified:
rebuild.sh OK x5, build_pc.sh builds, test_quest_loop / test_progression /
test_urgent pass.

Progress: 57.5% (7049 / 12585 functions). game 95.3%, select 90.8%,
yn 68.9% (rest is Sony netcnf library, not worth a round), main 41.2%
(about half the remainder is CRI Sofdec/ADX and Sony libs, parked),
lobby 38.0%.

Unmerged agent work (on branches, NOT verified; each agent was stopped
mid-round). Merge each with the usual procedure and check_push only after a
full rebuild + PC build:
- agent-A: 3 commits + 3 uncommitted files (Xbox: memory report RT_MEM, link
  scheme, first null-backend Xbox link in progress). nxdk in ~/xboxdev.
- agent-B: 2 commits + 3 uncommitted (PC: real soft keyboard from sk/; then
  movies via libmpeg2, see DECISIONS.md "Movies (Sofdec): libmpeg2").
- agent-C: 1 commit (unify tools/lbtu3.py + lbtu3c.py; plaza *Trans rewrites).
- agent-D: 2 commits + 3 uncommitted (game near-matches; em_fly10 found score 0).
- agent-E: nothing new.
- agent-F: 4 commits + 9 uncommitted (village whole-file TUs: shops/forge).

Next when resuming:
1. Merge the branches above (verify each).
2. Xbox port (owner brings a modded Xbox + capture card): A needs from the
   owner the xemu BIOS / MCPX / HDD dumps, how the Xbox is modded and its
   dashboard, HDD size, FTP/IP, video cable. Plan in docs/xbox.md.
3. B: soft keyboard, then libmpeg2 movies, then rt_gen stand-ins, frog fishing.
4. Decomp agents: whole-file TUs (lbtu3, split_rodata_objects) are what still
   gains; near-match grinding mostly doesn't.
5. Owner may play-test the PC build and report bugs (tools/play.sh).

## 8 Oct 2026: checkpoint

main = origin/main (a963743f+): rebuild OK x5; PC tests all pass (quest_loop, progression,
urgent, name_entry, movie, frog, all_quests 38/38, audio, log, activities 35, pick), online and
co-op (2, hunt2, handover), Xbox links. 57.995% matched.

Done since 7 Oct: owner play-testing loop (debug logs, F8 bug reporter with exact replay,
level select tools/play.sh select, desktop launcher); Windows builds (test releases 1-3 on GitHub,
public, pre-release); libmpeg2 movies; real soft keyboard; co-op over direct connect (2-4 players,
--coop, ONLINE=1, docs/pc.md "How to play co-op"); online lobby groundwork vs a local test server
(docs/network.md, draft message to MH Oldschool, never connect without permission); Xbox: memory
trims (~51/64 MB est.), GPU skinning, saves, pad, xemu prep (needs owner's BIOS/MCPX/HDD);
many owner-reported bugs fixed (Aptonoth float, forest crash, map item, cutscene input, raptor
crests via em_material_sub for all kinds, tail tip + tail cutting, fog veil order, scroll rule,
effects order, body_hit on, x87 -> SSE single precision float); targeted decomp: PC now runs
matched C for ~160 main files (agent B), semdiff review of near-match copies (agent E).

Open: vine climbing (needs location), Monoblos tail cut visual, em_ride_sub (Lao back) untested in
play, Xbox hardware runs, PS2 reference comparison (PCSX2/BIOS or ESP32 pad + debug ELF plan).

## 10 Oct 2026: checkpoint

main = origin/main (1ff526d7): rebuild OK x5; all 13 PC tests, test_online, test_online_town,
test_coop (incl relay, hostleave), tools/server/test_server.py pass; Xbox links; Windows builds.
57.995% matched.

Since 8 Oct: Windows test build 3 published; PS2 save import/export (all formats); English text
layer (RT_TEXT_TABLE, tools/text_dump.py, proportional ASCII; no translation shipped - waiting on
owner: US disc? Grender permission? title art?); display options + F10 settings menu + widescreen;
x87->SSE float fix (Cephadrome hang, PC/Win/Xbox); semantic review: ~30 real behaviour fixes
(monster hit reactions, bowgun aim, ground rounding, HUD/map, collision push); monster draw
root_lock (owner F8 "rewind moving"); online town on screen (login, plaza, town, guild hall,
rooms, quest, back to town, friends, mail) vs local test server; mh1-server design
(docs/server.md) + session relay + SQLite accounts; Xbox online research (docs/xbox_online.md).

Owner decisions: no robot playtesting (human F8 reports); keep code tidy (remove superseded
workarounds); OpenAL on hold; PS2 online via a patched game is acceptable.
Waiting on owner: Xbox setup/files, PS2 setup, English questions, server hosting/name/
invite-only/DNAS note, MH Oldschool message.
### Later, 10-11 Oct 2026 (65f0c343, all checks pass incl. test_online_guest/test_online_event)

F tidy-up (one monster draw
path, 123 dead weak stand-ins, ~30 near-match copies, docs/pc.md handover) and 26 host overrides
replaced by matched C (faint camera, bowgun upgrades, chat phrases, item pool); B online round 3 (guest
rooms solo per game rule, town weapons, event quest download+start, patch refusal, personal data
not sent). Survey of other games: docs/survey/mhg_vs_mh1.md (G ~52% reuse, MH2 ~17%, MHP ~0%; no
US MH1 disc on this machine).
### Then (4d6c683b, all 21 checks pass)

B rounds 4-6 (event quest to reward + saved; Windows/Wine vs
mh1-server; invite-only server with SQLite accounts/hunters, stored mail, crash recovery, config file,
"Run your own server" + public-launch gap list in docs/server.md; room rules = players/password/
message, enforced by server; game's HTML message renderer ported; password join tested with
clients via RT_SK_TEXT). F: Monoblos tail cut verified; Lao-Shan riding fixed (riders carried);
get_joint_pos family now game C (host fills joint arrays); village entry frees effects/shells;
scripted hunt luck-proof (RT_PL_ITEMS_MIN); monster-slot model-work leak fixed; carving checked
(no lost items). Left: Material_set_sub, get_mdlw_ptr, init_eft/shell_work in char editor.
Agents idle; online next steps wait on owner decisions (hosting, name, PS2 cross-play).

## 11 Oct 2026: real Xbox and xemu

Hardware bench (docs/xbox.md "Real Xbox bench setup" and later sections): Evolution X Xbox on
DHCP (192.168.1.116 now; was .115), capture card, RetroKM pad Atom. Laptop has 192.168.1.166 as
a second address saved in the HomePage_EXT Wi-Fi profile (the router moved it to .100 after its
5 am restart; owner ran the nmcli commands).
- Network loader (src/pc/xbox/loader, tools/xbox_serve.py, tools/xbox_upload.sh): builds and logs
  over HTTP, no FTP login for agents. Installed twice by the owner; current one stops the NIC
  before launching (nxNetShutdown).
- On hardware: boot through memory-card check, warning, logos, CAPCOM logo (vertex-cache fix),
  opening movie in correct colours (CPU YUV->RGBA, docs/REVIEW.md). Back + Start exits.
- xemu: owner's MCPX 1.0 + Complex 4627 BIOS in xbox_files/ (git-ignored). tools/run_xemu.sh
  runs xemu on a hidden Xvfb display (no window on the owner's screen). Whole opening movie plays
  (movie times itself by the tick when xemu's AC97 stalls; audio over-read fix). Serial log from
  xemu: xbox_files/xemu_debug.txt.
- Dumper (src/pc/xbox/dumper): own BIOS dump not done yet (top-MB read froze; reads the mirror at
  0xFFE00000 now, untested). EEPROM deliberately not dumped (console keys).
- docs/REVIEW.md: changes that drop old code, for the owner to review (owner's request).
Running (owner away, coordinator's judgement): helper in worktree F = scripted input in xemu and
going as far into the game as possible; helper in worktree E = float printf on Xbox + the dark
violence-warning screen. Waiting on owner: a remote power button for the Xbox (idea: servo or
optocoupler on a hub Atom), Tab5 not reconnecting after the router restart.
Done (worktree E, merged): printf with floats on the Xbox (xbox_libc.c). The violence warning
is NOT darker in our renderer (xemu = PC); the hardware capture is ~25-30 levels darker on every
screen: the video chain (NTSC-J black level vs the HDMI adapter, or adapter/capture brightness).
Owner check when convenient: capture a retail game's dark scene through the same chain.

### Later, 11-12 Oct 2026 (f49c80b4, all 21 PC tests + online, rebuild OK x5, both Xbox builds)

Worker F merged: the first player loop runs in xemu (new game, character, save, village, quest 131, hunt,
carve, deliver, reward, village, bed save; CONTINUE loads it): tools/xemu_quest_loop.sh (~10 min,
passes). Scripted input/env on the Xbox (D:\data\input.txt, env.txt), FATX formatter for the
xemu HDD (tools/xbox_fatx.py), pdclib fixes (fopen w+/r+, sscanf floats), hunter no longer floats
(nv2a), viewer catches up at most 4 ticks per frame, motion-set GC. Open: memory falls per loop
(village 10 MB free -> 6 MB after one loop); a helper is on it in worktree F. Audio untestable in
xemu (AC97 stalls). Next on hardware: run the same loop on the real Xbox (needs input.txt in
E:\Games\MH1\data, i.e. one FTP copy or a loader that fetches it).

### 12 Oct 2026 (b494a2ff, rebuild OK x5, all 22 PC tests incl. the new test_soak + online, both Xbox builds)

Worker F memory round merged: free memory flat over quest loops (xemu: village 16.5 MB, after
quest 3 15.8 MB; was 10 -> 6 MB after one loop). 2D texture GC, motion/NPC file GC, host model
GC, collision areas 1 MB -> 288 KB. Gameplay bugs fixed on the way: village motions overwrote
the hunter's weapon motions (rebuilt per quest now), second delivery quest cleared at once
(delivery stock reset at quest start). Soak tests: tools/test_soak.sh, tools/xemu_soak.sh.
Open: ~130 KB/loop still lost in xemu only (allocator?). Recording: run_xemu.sh --record.

### 12 Oct 2026, later (c1128dc3, rebuild OK x5, all 22 PC tests + online, both Xbox builds)

59.668% matched
(main 45.989%, 7204 of 12585 functions). Workers B (gameplay side of main: player, menu, emw,
`case 0: default:` compiler quirk solved) and C (engine side: flps0, omake whole file, usbkbdm,
fltex02, flswap, tx03; host pad driver and system_w_set replaced by matched f_ioread) merged.
Agent E merged: hand items drawn (BBQ spit, pickaxe, net; pl_item_trans was never called),
village NPCs root_lock (owner: slide-then-snap on PC and Xbox), nv2a near clipping for skinned
clays (carcass flicker suspect), freed GPU memory kept 3 frames (fade flash suspect).
Real Xbox: sound works (GPU waits yield; owner confirmed), Back + Start quits cleanly, first
scripted quest loop ran on hardware at ~30 fps; one loader download stalled mid-transfer (loader
needs a receive timeout + fallback to the disk copy; next owner install).
Merge note: the first agent-C merge broke the rebuild (stale omakeb-e and npcb/npcc lines from
both sides' bases); kept on branch c-merge-attempt-1, C fixed it on its branch. Untracked
generated stubs (src/main/npc/npcb.c, npcc.c) left by a failed rebuild break the next one:
after a failed merge check `git status --untracked-files=all src`.

## 13 Oct 2026: checkpoint (hand-over to a fresh session)

Start here: CLAUDE.md, then docs/agents/COORDINATOR.md (owner's standing decisions, rules,
current assignments, Xbox bench), then this section, then docs/xbox.md from "Real Xbox bench
setup" on, and docs/REVIEW.md (changes waiting for the owner's review).

State: main = origin/main, 59.668% matched (main module 45.989%), rebuild OK x5, 22 PC tests +
online pass, PC/Windows/Xbox build. PC plays the whole single-player game and the online town
against our own server. Xbox: full quest loop on the real console at ~30 fps with sound, saves,
clean quit; builds and logs travel through the network loader; xemu runs hidden for daily tests.
Owner's Xbox play-test (12-13 Oct): fixed and confirmed — cooking spit, NPC walking, sound, quit,
frame rate (the CPU near clipping was the slowdown and was removed; candidates noted).
Open, in the owner's order: invisible "Save?" box at the bed (agent E on it); forest ambience
during the boot logos; quest-clear music again in the village when saving; long pause on area
changes, no loading screen (owner idea: option to skip it if loads are fast); loader download
freezes right after Evolution X launches it (theory: NIC still active; next loader install).
Paused by the owner: decomp workers B (lobby) and C (main), work on their branches unmerged.
Waiting on the owner: decisions on online hosting/name/PS2 cross-play, English (US disc,
translation permission, title art), a remote power button for the Xbox (idea), a DHCP
reservation for the Xbox and laptop, own BIOS dump (optional; dumper reads the mirror now).

## 13 Oct 2026 (evening): MH1 polish round merged, G is the main target, then paused

Owner decisions (docs/DECISIONS.md): Monster Hunter G is the main release; one codebase for MH1
JP / MH1 US / G with a version switch and a start-up launcher; priorities G > MH1 JP > MH1 US.
US disc added (disc/mh1_us; docs/mh1_us.md). Later idea: 5.1 surround (docs/xbox.md).
Merged and checked (tools/full_check.sh all ok incl. online; VERSION=mhg rebuild 7 OK):
- E: bed "Save?" box visible (lobby dialog prim was a stand-in), silent boot logos, no
  quest-clear music in the village after saving.
- F: Xbox loads ~8-10x faster in xemu (fread in big blocks, sound packs, swizzle/APX, Meltw);
  the game's own loading screens back (area map, NOW LOADING); loader write fix.
- C: monster rumble (vib_set_pl), flmatInit33, two menu aliases; headless runs never open real
  controllers (an earlier test rumbled the owner's pad).
- D: ./mh1 linux|windows|xbox DISC (disc check, build, data copy), Xbox install script and
  start-up data check, docs/PLAYING.md.
- A: docs/RELEASE.md (MH1 release criteria and gap list), US script survey.
- B: G bring-up: carry-over 95% of MH1 code has a G counterpart (56% identical), G rebuild
  byte-identical (VERSION=mhg), 7.5% matched, all G data loads in our readers; docs/mhg.md.
Owner Xbox tests pending: see COORDINATOR.md "Current assignments". Paused after this push.

## 13-14 Oct 2026: Monster Hunter G comes up on the PC (owner asleep from 14 Oct ~2 am)

G on the PC engine (tools/build_pc_mhg.sh, hidden by default): title -> menu -> NEW GAME ->
character creation -> save -> CONTINUE -> Kokoto village (walk the village, item box); G quest 10
(Rathian) hunt runs to the reward screen with test aids; quest 1 base camp loads; village NPCs run.
G matched 7.5% -> 14.3% (VERSION=mhg tools/progress.py). English: tools/text_map.py builds
English text tables locally from the owner's US disc: MH1 JP 75%, G 37% official English
(docs/text_coverage.md). MH1 Xbox (owner-confirmed): Save? box, silent logos, loading map, rumble;
area loads ~0.25-0.5 s on hardware.
Crashes: the coordinator's session was OOM-killed twice (a 5.6 GB python during a G rebuild;
not reproduced by agent D). Rule since then: builds under `ulimit -v 3000000`, one at a time,
G registrations proven from a clean build (docs/agents/BRIEF.md).
Open for the owner: agent F's Xbox audio fix (silent after a warm relaunch) is unmerged on
agent-F and needs a hardware test (docs/agents/COORDINATOR.md); bed "quit game? yes" drops to
the Forest camp.
### Overnight, 14 Oct 2026 (merged, clean rebuild 5+7 OK, MH1 PC + quest loop, G PC build)

G 15.5%.
A: G monster level / hit-zone state / size set as G does; subspecies kinds 0x24/0x2D/0x32; reward
list without supply-box items (stray "1:0" first entry left); armour tables at G sizes; 21 timer
tables. Choice for owner review (docs/REVIEW.md): the PC keeps MH1's monster-work (EMW) layout,
G's shifted layout only on the PS2 build. G quest 10 without test aids: Rathian behaves, 1-5
damage per hit, not won by the script in 900 s. Still zero in G: Shell_data, St_data, Equip_Bonus,
em_hp_vital_tbl, monster_data. C: f_pl registered (40 of 41 parts; pl_mv004 jump-table order).
Next: G camera layout (C's notes), those zero tables, quest failure and a G-only monster quest.

## 14-15 Oct 2026: G playable on the PC, installed on the Xbox

See docs/agents/COORDINATOR.md "State 15 Oct" for the summary. Highlights: G village and all 98 offline
quests on the PC; G 19.8% matched; G 47% official English; G installed on the real Xbox through the
loader's HTTP data install (FTP to Evolution X stalls on big files); Xbox sound after warm relaunch fixed
(own AC97 driver, owner confirmed); debug menu, Xbox settings menu, prototype gunner HUD from cockpit2.apx;
prototypes/demo surveyed (docs/FINDS.md). OOM crashes: wiping build/raw before a G rebuild made wibo
loop (BRIEF.md rules).

## 29 Oct 2026: G on the real Xbox, driven by the coordinator

The coordinator now runs the real Xbox alone (power servo, capture card, scripted pad; docs/agents/COORDINATOR.md
"Xbox bench"). Build 2aa6993a / 57e1ab1f, G nv2a through the loader. Verified on hardware:
- Frame rate: village 30 game frames a second (capture card: a new picture every 2nd 60 Hz frame, no repeat
  longer than one frame while walking past villagers); Forest and Hills field and a fight with Velociprey the
  same. The game log: 59 drawn fps, 30 ticks a second in the village. No tearing (owner's eye on the CRT).
- Second start (Back + Start or the watchdog, then G again from the dashboard, no power cycle): 3 of 3 fine.
- Villagers present on a normal village visit (GB5 bug 3 confirmed fixed on hardware).
- Fixed: the debug menu's quest list froze the GPU every time (watchdog "no frame for 5 s", with or without
  RT_NV2A_SYNC): a frame wrote past pbkit's 512 KB push buffer, which never wraps. gfx_nv2a.c now waits for the
  GPU and starts the buffer again from its head when a frame nears the end (2aa6993a). Quest list drawn after.
- English on the Xbox: ./mh1 xbox copies build/text/en_mhg.txt to dist/xbox/MHG/data/text/en.txt and
  xbox_serve.py --data installs it through the loader; the language setting is "auto" by default (English when a
  table is found). The owner's Xbox had "language = ja" saved by the old menu (it always wrote ja), so it needed one
  switch in Back + L3. Character select, mode select: English (57e1ab1f).
- The "Back + L3: SETTINGS" hint covered the character / mode select help line: now the first 10 s only.
Open: a ~200 ms frame every ~90 s in the village (log "load: pause ... other", very regular; the owner suspects the
capture card, not checked); no audio through the capture card yet (RetroTINK HDMI gives none); Kut-Ku not fought.

## 29 Oct 2026 (late): two Xbox fixes checked on hardware

- Village pause gone: the log files reserve 4 MB, so the FATX disk no longer stops the game every ~90 s to grow them
  (4.5 min idle in the village, no pause).
- Cold-start black screen (rare, 1 in ~25): the audio setup no longer reads the sound chip's registers (the reads never
  worked: always 0000) and only writes them. 15 of 15 cold starts fine afterwards; likely fixed, not proven.

## 2 Nov 2026: Monster Hunter 2 Phase 1, first matching C (docs/mh2.md)

Owner's change of 1 Nov: MH2 is split and decompiled by us, from the owner's disc. Branch of an isolated worktree; the
community mh2-decomp repo was not opened.
- Split: main and all 12 overlays rebuild byte-identical (`tools/mh2_rebuild.sh`, VERSION=mh2 in tools/build.py). Verified by
  `tools/mh2_check.py`: SHA-256 of each file against disc/mh2/overlays and against its bytes inside DATA.BIN, and of the
  whole SLPM_662.80 with the rebuilt main.bin put in (ELF headers from the disc). 13 of 13 identical.
- Names: 7,285 of 17,034 functions, 39.7% of code bytes, plus 659 data names, from MH1's symbol table directly or through G
  (`tools/mh2_map.py`, docs/survey/mh2_map.csv). 3,034 are the same code bar addressing, 1,282 same shape, 2,927 edited
  guesses. `tools/mh2_namecheck.py` agreement of callees: 98% identical, 96% good edited, 84% weak edited: an estimate.
- Compiler: mwcps2-3.0b52-030722 -O4,p, as MH1/G, plus `-sdatathreshold 0`: MH2 has no small data, no code uses gp.
  Survey of all 14 local compilers in docs/mh2.md: b50 and b52 tie on top, 3.0.1 far behind.
- Matched: 201 functions, 34,280 bytes, 0.62% of code (`tools/mh2_progress.py`): 192 unchanged MH1 files
  (`tools/mh2_carry.py`) and 8 new small functions, src/mh2/plsel. All in the build that passed the check above.
- Not done: no data slots yet, so C with strings, literals or jump tables is not carried; nothing has been run, only rebuilt.
Next: data slots for carried C, then the 300 files that compile but differ (docs/mh2.md section 6).
- Regression from this worktree: `tools/rebuild.sh` (MH1) all five OK. `VERSION=mhg tools/rebuild.sh`: six modules OK, but G `main`
  fails to link: "multiple definition of Lb_menu_win_size_mv" (menu09_p1 and menu09_p2 both get it: in src/main/menu/menu09.c the `#ifdef VERSION_MHG` block at line 161 sits outside the G_PART guards).
  No file under src/main, src/mhg, src/lobby, src/game, config/mhg or include changed on this branch, so this is
  not from the MH2 work; src/main/menu/menu09.c last changed in the G commits of 1 Nov. Left alone for the G side.

## 2 Nov 2026: MH2 data slots, carrying C at 3.74% (docs/mh2.md section 5)

Decided alone, the owner being away. Community mh2-decomp not opened.

- tools/mh2_setup.py and the build cut `MODULE:data` slots. 25 slots in.
- Found a bug: tools/mh2_carry.py failed every multi-function file, because MWCC gives each function its own .text section. At least 79
  of the "300 that differ" did not differ. Only 22 candidates held data, so slots alone were not the big gain docs/mh2.md had expected.
- Rewrote the carry: every file MH1 or G links is searched for in MH2's code; relocations are checked against the original
  instructions. New: files tried in G's form (VERSION_MHG through a wrapper), per-object renames (c_symfix.txt), and parts: the
  functions of a file that match go in without a source edit (src/mh2/parts, c_parts.txt, keep_sections in tools/build.py).
- Result: 1,532 functions, 208,104 bytes, 3.74% (was 201, 0.62%): 471 whole files, 349 parts, 8 hand. Remaining equal functions outside
  this: 19, 2,884 bytes.
- Report for later: docs/survey/mh2_differ.csv, 977 files by instructions that differ. A sample of the small ones shows real edits
  (struct offsets, message numbers), so no flag or tool fixes them.
- Verified: tools/mh2_rebuild.sh OK for all 13, tools/mh2_check.py 13 of 13 identical. tools/rebuild.sh 5 OK and VERSION=mhg
  tools/rebuild.sh 7 OK; neither changed any tracked file. I did not compare their sha1s with a build from before this branch; both compare against the original bytes, so OK is the proof.
- tools/mh2_rebuild.sh now limits itself to `ulimit -v 3000000`. Not run in any emulator: proof is bytes only.

Next: the small files in mh2_differ.csv by family (docs/mh2.md section 6).

## 2 Nov 2026: MH2 near-misses, four branches merged into one design, 6.37% (docs/mh2.md section 5)

Decided alone, the owner being away. Community mh2-decomp not opened.

- Merged A (CnetSys_w, MH2V sources), B (compiler probe for PLW and EMW, base of the merge), C (lobby and network), D (near-misses, SKO, mh2_diff.py). One set of generated
  headers remains: mh2_plw.h, mh2_emw.h, mh2_ofs.h, cnet_mh2.h. A's pl_mh2.h, em_mh2.h, game_mh2.h, gwx_mh2.h and D's game_mh2.h, game_mh2_gwx.h are gone, STG_T became stg_t, PLW_MH2 became PLW_AT.
- Result: 2,268 functions, 353,996 bytes, 6.37% (branches: 2,038, 2,037, 1,640, 1,598; union 2,274).
- Against each branch: A 6 functions, B 2, C 2, D 0. Five of the six are one C function that a branch put at its other twin site; the merge has it at the first site, so one function counts
  in each. The one real loss is 20 bytes, sub_main_func_00368E80, with all_sprite_free's twin at 0x368ED0: mhg/main/sprite/sysw_p2 no longer goes in whole, main/sprite/sysw parts took its place. Why it no longer goes in whole was not looked into.
- Bug found in B's game_w: a pad after area_mdlw left the pointer at 0xB2, which aligned to 0xB4 and put every later field 2 too high. Taru_ok_ck showed it.
- Double mappings fixed: MH2V inside SKO (6) once D's SKO arrived, and inside M2C_FIELD, EM_FIELD, PU8 (one each). Rule in docs/mh2.md.
- MH2_OFS added to one-line macros in 60 more files, outside em and pl. Its gain was not measured on its own (it ran together with the game_w fix). 11 files went back: `void *` or integer arguments do not compile with it.
- Tools: mh2_structs.py gained `cnet`; mh2_own.py works again (p00D61710.c: OK, 0 differ); the object cache in mh2_carry.py notices include/; MH2_JOBS sets register's jobs.
- Verified: build/mh2/carry emptied, build/mh2/src/mh2/parts deleted, register, full tools/mh2_rebuild.sh, tools/mh2_check.py 13 of 13 identical. MH1 by the steps of tools/rebuild.sh at -j 2: 5 OK. G the same way: 7 OK.
  tools/build_pc.sh builds build/pc/mhview. MH1, G and PC were run on the commit before the last header change, which touched only mh2_emw.h and mh2_ofs.h (MH2 only). PC build only: not run, full_check.sh is the owner's.
- Not run in any emulator: proof is bytes only. What is a guess is listed in docs/mh2.md.
- Later the same day (this was a separate short entry, "MH Dos near-misses, four agents merged", now folded in here):
  carried code 3.74% to 6.37% (2,268 of 17,034 functions). Verified on the merged tree: full check 47 of 47, mh2_check
  13 of 13 identical. The first full check failed 6 runtime steps (`MH2V` hid addresses from `tools/pc_abs.py`); fixed
  (docs/mh2.md 5.3). Next: remaining struct layouts in docs/mh2.md.
  Order, from the commit times: "full_check.sh not run" was written at 11:17 (commit 25adb3b); the 47 of 47 came with the
  `MH2V`/pc_abs fix at 13:19 (commit 67debca). So the full check was run after the first write-up. Who ran it is not
  recorded.

Next: docs/mh2.md section 6.

## 2 Nov 2026: MH Dos on the PC, first step: the title screen (docs/mh2_port.md)

Owner's new goal: PC and Xbox ports of MH Dos. `tools/build_pc_mh2.sh` builds build/mh2/pc/mhview_2 from G's form of the C. A hidden `--boot --shot` shows MH2's title art, PRESS START BUTTON and the copyright, read from DATA.BIN and its overlays. The boot itself is about 150 lines from MH2's asm (src/pc/title_mh2.c); no game logic beyond fade, font and sprites has run, and Start does nothing.
New: DATA.BIN and TIM2 readers, tools/mh2_pcmeta.py (symbols, pointer scan: unchecked), tools/mh2_datamap.py (1,499 more data names, 640 of 641 agree on names already placed, 930 on one vote).
Formats: models, skeletons, motion and collision load with our readers (not drawn or played); quests (.mib 0x74 header in mib.afs) and MOMO textures are new. The font is byte-identical to G's.
Gap: 933 of 1,340 tables have no address, 344 function stand-ins; about 23% of the linked functions are identical to MH2 or the same shape. Xbox not tried.
Next: docs/mh2_port.md section 6.

## 3 Nov 2026: MH Dos, the four port agents merged (QUEST, STRUCTS, PLATFORM, DATA)

Decided alone, the owner being away. Community mh2-decomp not opened. Area notes stay in docs/mh2/data.md, structs.md, quest.md, platform.md;
docs/mh2.md and docs/mh2_port.md section 6 now say what of the plan is done.

- Result: 2,290 functions, 370,408 bytes, 6.66% (tools/mh2_progress.py). Branches: QUEST 2,272 (6.63%), STRUCTS 2,280 (6.39%), PLATFORM and DATA
  2,268. 2,290 is the union of the four sets (a scratch script compared each worktree's config/mh2/c_files.txt by function): nothing lost.
- On the way: STRUCTS' PLW had moved wpn_ammo to 0x30C (two probe votes), which dropped two pl_ammo parts; MH2 reads it at 0x35E, now pinned in
  config/mh2/struct_map.txt. QUEST's `union EMT` in include/em_core_g_mh2.h is gone (STRUCTS' EMW has those u16 fields; bytes identical); the one in
  em_dmg_sys_nm.c stays (EMW + 0x3D0 is still padding). em_cmd_ck.c calls its handlers by DATA's opcode names (em_cmd_op1F, 37, 73, 76 declared).
  The PC host's User_data reads (rt_dbg, rt_player, rt_flow) go through UDMO. data_map.txt: DATA's, which already pins STRUCTS' bases through
  config/mh2/pc_names.txt (quest_w 0x5D8260, User_data 0x5D4A60, net_common_w 0x5D46A0, MemcardWork 0x6AF100); pc_names.txt is the union.
- DATA's pipeline (mh2_map.py, datamap --fresh, pairmore, datamap, pairmore, datamap) rerun on the merged C, three times: identical md5s each time,
  so kept. It moved 8 data names (c names from the new matched C: PitMenu, User_data, quest_w now `c:9.0`) and added one function pair. Symbols unchanged.
- Verified: build/mh2/carry emptied, build/mh2/src/mh2/parts deleted, register, full tools/mh2_rebuild.sh, tools/mh2_check.py 13 of 13 identical.
  MH1 by the steps of tools/rebuild.sh at -j 2: 5 OK; G the same: 7 OK. tools/build_pc.sh, build_pc_mhg.sh, build_pc_mh2.sh build.
  `python3 tools/test_all_quests.py 138 10`: both PASS. Hidden `--boot --shot` of mhview_2 at 5 s: MH2 title, PRESS START BUTTON and the copyright, looked at.
  Hidden `--quest 1014` with RT_QUEST_STAGE=1: stage 120 with Rathian (kind 1), four kind-66 monsters and the hunter beside her, looked at.
  full_check.sh not run (the coordinator's).
- Noticed: the first register after emptying the cache said 17 files "do not compile" (4 on the later runs of the same tree); cause not found, so check
  that count. func_fix.txt names game 0x72B240 twice (opcode 0x2C em_cmd_myemtype_sel, and by file order game_func_005885E0); the later wins.
- Nothing of MH2's game code has run: proof is bytes, plus drawing checks on the PC.

Next: docs/mh2_port.md section 6, the part after "What is left".

## 3 Nov 2026 (later): MH Dos, CODE and HUNT merged

Written during the readability pass of 3 Nov, from the commit messages and the two area notes. Nothing was re-run for this
entry. Details: docs/mh2/code.md and docs/mh2/hunt.md.

- CODE agent, byte-matched: stage_set_set (sub_main 0x378A30), game 0x80D270 and 0x75FBF0 (two monster act_sets, with their
  jump tables), em01_senkai_sub (game 0x70A290, 3,136 bytes). Near-match 0x3A2E30 went from 117 to 96 instructions off.
  func_fix: em_cmd_myemtype_sel is 0x72B3A0 (opcode 0x70, as in G).
- mh2_carry.py retries a failed compile once with a longer limit (the first register after an empty cache had lost 12
  files).
- HUNT agent: `--quest 1014` on the PC runs G's quest path on MH2's own Quest_start set-up. It draws stage 110, the hunter
  in the game camera and the HUD, then stops at the blockers listed in docs/mh2/hunt.md. Monsters only with `RT_MH2_EM=1`.
- Progress after the merge: 6.88%, per the message of commit 0a67047 (docs/survey/mh2_differ.csv regenerated). The
  function count was not recorded.

## 3 Nov 2026 (evening): docs follow-up, three open questions settled by the owner

- Owner decisions (docs/DECISIONS.md, "Settled 3 Nov 2026"): PS2Recomp / a hybrid build is off the table; the community
  MH1j English patch is a very-low-priority option for later; the community mh2-decomp repo is an allowed source of facts
  (not of code, names, comments or text).
- CLAUDE.md "Where things stand" updated (it still said 8% matched and that PS2Recomp was being tested) and now points at
  the summary at the top of this file.
- Doc conflicts from the readability pass settled from git history and the code: game_w 0x2BC; the em_cmd near-match
  counts and the opcode 0x2C/0x70 move (code.md is newer); TIM2 alpha 0-255 (src/pc/fmt/apx.c:43-45, commit c2671f0);
  118 vs 109 tie files and 14/44 vs 17/51 two-address files are two runs, not a disagreement; c_symfix.txt was 145, 213,
  and is 198 lines now; the 2 Nov full check ran after the first write-up (commits 25adb3b, 67debca).
- Still open, need a run with the disc: the MH2 sleep table names (docs/mh2/hunt.md vs data_map.txt) and quest_w byte 0x9D
  (x168 or x169). The date of the "5 or 6 Oct" entry cannot be checked: git history starts on 29 Oct.
- Docs only; no C, header, tool or config file changed.

## 3 Nov 2026 (night): MH Dos rounds 3 and 4 (WALK + QUEST, then SHARED + HOST)

Two agents at a time (the laptop runs hot). Each round merged into main, then the full check (47 of 47), carry register,
mh2_rebuild and mh2_check (13 of 13 identical), plus hidden PC runs of quest 1014.
- Round 3, 6.88% to 6.92%. WALK (docs/mh2/walk.md): FRW, HENT, PLLC, EFT01/EFT13 MH2 views in shared C; PLW part pointers
  at 0xD0; 866 func_pairs names folded into the PC's symbol view; data_load_ptr / pl_area_top swapped back. QUEST
  (docs/mh2/quest_flow.md): MH2's quest state machine 0x3A2E30 and script 0x3A9230 as host C in place of G's
  Quest_condition_judging; quest 1014 no longer clears at tick 7. Result: the hunter walks off the beach into the sea.
  The first full check failed kr_returns / kr_returns_g: five host declarations said int where the game function returns
  u8 / s16. Fixed before the push.
- Round 4, 6.95%. SHARED (docs/mh2/shared.md): Game_clear_ck, Quest_time_get / Quest_timer_reset (MISSION2), EMT.prog
  0x3D0, em_init's part flags at EMW 0x4C8, HCHR / HSHL / PLX MH2 views, PLW_STRIDE 0x970, 9 hit functions matched,
  info_str0..3 placed. HOST (docs/mh2/host.md): MH2's em_move 0x904CF0 and the quest monster set-up as host C; em_prog_tbl
  is at 0x90DE10 (data_map's 0x90DE70 is its kind-24 entry). With RT_MH2_EM=1 the quest runs 900 ticks; the Rathian sits
  in em_fly00 because its motion layer is never set up. Both quest end paths reach mode 5 and crash in records_update
  (hunt_group_tbl NULL on MH2).
- At the merge: dropped the host Game_clear_ck, Quest_time_get, em_effect_move and the timer workaround (shared C reads
  MH2's places now); restored config/mhg/pc_ptrs.txt, which an MH2 branch had regenerated from an empty build/mhg.
- The cloud readability pass (docs only, plus docs/READABILITY_REPORT.md, suggestions for the C) merged the same night.

Next: the monster's motion set-up on MH2's EMW (em_char_set, em_frame_check's speed at 0x884); hunt_group_tbl or MH2's
result code; stage 120's camera; the per-stage tables at 199 entries; MH2's CameraWork. Reports: docs/mh2/host.md,
docs/mh2/shared.md.

## 4 Nov 2026: MH Dos round 5 (SHARED + HOST)

Same flow as rounds 3 and 4: two agents, merged into main, full check 47 of 47, carry register, mh2_rebuild, mh2_check
13 of 13, hidden PC runs (walk, monsters, monster next to the hunter, time-up, clear, stage 120), all exit 0. 6.95% to 6.99%.
- SHARED (docs/mh2/shared5.md): why the monsters stood still. f_frameb.c's frame_init used MH1's motion handle bases
  (1700 / 500); MH2's are 1910 / 710. Fixed under VERSION_MH2, plus em_frame_check's speed at 0x884. The Rathian now takes
  off, flies, lands. Stage 120's default_area_data copies cam_cnf_chs (camarea03.c), so stage 120 starts. QEM MH2 view
  (quest.h), the model slots at game_w+0x2A, 4 more quest functions matched. tools/mh2_funcfix.py takes `split` lines;
  0x90A990 is em_effect_move and emt02.c carries whole.
- HOST (docs/mh2/host.md, "Round 5"): hunt_group_tbl at 0x44C800 and em_base_size at 0x44C930 (found by their readers);
  MH2's result screen is plsel 0xD683C0 (26 steps), a host stand-in for now that writes no rewards; the village is dark on
  MH2. Per-stage fog, sun, area_move_high_y_tbl and fishcam_ofs_tbl from MH2's own code and tables (199 stages). Host files
  use PLW_STRIDE / EMW_STRIDE / ITEM_ROW from version.h.
- At the merge: both agents fixed frame_init; kept SHARED's byte-matched one and dropped HOST's host copy and its rename.

Next: the monster command scripts (em_cmd) on stage 120 hang: MH2's opcodes 0x0E / 0x15 read the stage as 2 bytes through
stage maps (tables 0x440E30 / 0x440FC0 to import); get the Rathian on screen and test its hits; MH2's result screen and
village; MH2's CameraWork; kind 10 stuck in act 0; the remaining host em_work offsets. Reports: docs/mh2/shared5.md,
docs/mh2/host.md.
