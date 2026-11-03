# Monster Hunter 2 (Dos, PS2, SLPM_662.80, JP 1.04) in this repo

"MH2", "MH Dos" and "Dos" all mean this game in these docs.

- **Set up:** 1-2 Nov 2026, after the owner's decision of 1 Nov (docs/DECISIONS.md): we split and decompile MH2 ourselves,
  from the owner's disc, with our tools.
- **Community repo:** mh2-decomp was not opened during this work. Nothing here comes from it.
- **Numbers:** each comes from the tool named next to it.
- **PC port:** see docs/mh2_port.md (the plan) and docs/mh2/hunt.md (where `--quest` stands).

## Plain summary

- Every piece of MH2's code rebuilds byte for byte from disassembly: main and all 12 overlays.
- 7,285 of 17,034 functions (39.7% of the code bytes) have a real name carried over from MH1's symbol table, directly or
  through G. The rest stay `func_ADDRESS`.
- MH2 uses the same compiler as MH1 and G. One difference: it was built with small data off, so it never uses the gp
  register. Add `-sdatathreshold 0` and MH1's C compiles to MH2's bytes.
- Matched C, merge of 3 Nov 2026 (`tools/mh2_progress.py`): 2,290 functions, 370,408 bytes, 6.66% of the code. That is
  1,230 C objects built from MH1's and G's sources (which now hold MH2's differences behind macros), plus 11 files written
  fresh from our disassembly, among them the monster command interpreter em_cmd_ck and next_cmd_search.
- Later on 3 Nov the CODE agent byte-matched four more functions (docs/mh2/code.md), and the commit after the CODE+HUNT
  merge gives 6.88%. That figure is from the commit message (0a67047); the tables in section 5 are still the 6.66% run.
- Area notes:
  - docs/mh2/data.md: data names, function pairs, call graph (DATA agent, 2 Nov).
  - docs/mh2/structs.md: struct layouts (STRUCTS agent, 2 Nov).
  - docs/mh2/quest.md: quest core (QUEST agent, 2 Nov).
  - docs/mh2/platform.md: PC host, file formats (PLATFORM agent, 2 Nov).
  - docs/mh2/code.md: byte-matched code for the hunt (CODE agent, 3 Nov).
  - docs/mh2/hunt.md: `--quest` on the game's own code, blocker list (HUNT agent, 3 Nov).
  - docs/mh2_port.md: the port plan.

## 1. Layout

| | |
|---|---|
| Program | SLPM_662.80, 822,368 bytes, stripped, `.comment` "MW MIPS C Compiler (2.4.1.01)" (a linker string; it identifies nothing) |
| main | file offset 0x280, loads at 0x100000, 0xC8600 bytes in the file, 0x1E3900 in memory; ends at 0x2E3900, where sub_main loads |
| main text | 0x100000-0x195C00 (read from the code: the data starts with the "PsIIlibcdvd" version tag) |
| main data | 0x195C00-0x1C8600. Not typed further. The block 0x198000-0x19A000 looks like code (`jr ra`s, no calls in or out) but is kept as data; the bytes rebuild either way |
| gp | 0x1D05F0 (.reginfo). The file ends exactly at gp - 0x7FF0: there is no .sdata in the file, and no code uses gp |
| Overlays | MWo3, stored uncompressed in DATA.BIN (an AFS; G's overlays are compressed). Header as in tools/mwo_unpack.py |

| overlay | loads at | shares its address with | text | data | bss |
|---|---|---|---|---|---|
| sub_main | 0x2E3900 | resident | 0x11E880 | 0xAF740 | 0x1FD900 |
| select | 0x6AF200 | game, lobby | 0x18B40 | 0xA400 | 0x1100 |
| game | 0x6AF200 | select, lobby | 0x1E60C0 | 0x5FF00 | 0xB00 |
| lobby | 0x6AF200 | select, game | 0x12C640 | 0x6E700 | 0xB0E80 |
| gm_sub | 0x8FAE00 | lbguild, nethttp | 0xFE40 | 0x28700 | 0x6280 |
| lbguild | 0x8FAE00 | gm_sub, nethttp | 0xC0 | 0x29380 | 0 |
| nethttp | 0x8FAE00 | gm_sub, lbguild | 0x55040 | 0xAB80 | 0x1480 |
| yn | 0xAC8000 | dnas_net, dnas_ins | 0x9140 | 0x2900 | 0x46700 |
| plsel | 0xD60200 | | 0x15D40 | 0x5E00 | 0x100 |
| test | 0x939600 | | 0xC0 | 0 | 0 |
| dnas_net | 0xAC8000 | yn, dnas_ins | 0x70440 | 0x20680 | 0x81300 |
| dnas_ins | 0xAC8000 | yn, dnas_net | 0x299C0 | 0x8380 | 0x6FB00 |

**Guess:** which overlays are loaded together (the `LOADED_WITH` table in tools/mh2_setup.py) is assumed from the shared
addresses and from G. It only decides which names a module may see. It is not tested in play. docs/mh2/platform.md has the
same slots as a table; docs/mh2/data.md has cross-reference counts that support game + gm_sub and yn + lobby.

## 2. Build

Needs disc/mh2 (SLPM_662.80, DATA.BIN, overlays/*.bin carved from DATA.BIN), splat in .venv, and tools/binutils and
tools/compilers/ as in README Phase 1.

    tools/mh2_rebuild.sh                 # split and build all 13 modules; prints OK per module
    python3 tools/mh2_check.py           # the check against the disc itself
    MH2_NOSPLIT=1 tools/mh2_rebuild.sh   # after editing C only
    python3 tools/mh2_carry.py register  # rebuilds the list of carried C, wrappers and slots; then a full rebuild
    python3 tools/mh2_carry.py fix       # after a failed compare: which C files sit on the wrong bytes

- `MH2_PY=/path/python` picks the python with splat when .venv is elsewhere (git worktrees).
- Output goes to asm/mh2, build/mh2 and assets/mh2. All are gitignored.
- config/mh2 holds only names and addresses. `config/mh2/*_aliases.txt` are generated (and large), so they are not committed.
- In tools/build.py this is `VERSION=mh2`. The MH1 and G branches of build.py only gained exclusions of the mh2 folders.

**How it was verified.** tools/mh2_check.py reads disc/mh2, not our copies. It takes the SHA-256 of:
- each built overlay, against disc/mh2/overlays;
- the same overlay as found inside DATA.BIN;
- the whole SLPM_662.80 with its code and data swapped for the rebuilt main.bin (the ELF headers come from the disc: they
  are not rebuilt).

Result on the last full build, with names and matched C in: 13 of 13 files identical.

## 3. Names (tools/mh2_map.py, docs/survey/mh2_map.csv)

Sources: 13,289 functions in MH1's symbol table, plus 2,032 function bodies that exist only in G's form (G's names are
MH1's, moved by tools/mhg_map.py). Nothing is guessed from behaviour.

| module | funcs | identical | absolute | shape | edited | rewritten | ambiguous | mh2-only | named bytes |
|---|---|---|---|---|---|---|---|---|---|
| main | 3188 | 633 | 258 | 112 | 511 | 11 | 146 | 1517 | 57.0% |
| sub_main | 3437 | 709 | 34 | 183 | 858 | 13 | 90 | 1550 | 46.3% |
| select | 251 | 6 | 0 | 4 | 21 | 0 | 0 | 220 | 8.8% |
| game | 4580 | 195 | 5 | 748 | 637 | 10 | 468 | 2517 | 31.2% |
| lobby | 3819 | 231 | 35 | 199 | 634 | 5 | 249 | 2466 | 27.8% |
| yn | 122 | 46 | 6 | 5 | 23 | 0 | 3 | 39 | 55.9% |
| plsel | 179 | 1 | 0 | 2 | 11 | 0 | 0 | 165 | 7.9% |
| gm_sub | 128 | 3 | 0 | 1 | 25 | 0 | 1 | 98 | 28.6% |
| lbguild | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 1 | 0.0% |
| nethttp | 1329 | 542 | 330 | 28 | 207 | 3 | 75 | 144 | 95.5% |
| **all** | **17034** | **2366** | **668** | **1282** | **2927** | **42** | **1032** | **8717** | **39.7%** |

- 6,271 names came from MH1 directly and 1,014 through G's form of a function, plus 659 data names.
- What the classes mean (full text at the top of tools/mh2_map.py):
  - identical, absolute, shape: the same code.
  - edited: a good guess, with a score.
  - rewritten: a hint only.
  - ambiguous: no name given, because several names have that body.
- dnas_net, dnas_ins and test have no function list. The two DNAS overlays decode to almost no functions and were left alone.
- **Guess:** the function list itself is a heuristic (call targets, ends of `jr ra` blocks, starts of twins, as in
  tools/mhg_map.py). The 17,034 can include a few wrong cuts, and tiny stubs fool it. Wrong cuts cost no bytes.

**How far to trust the names (tools/mh2_namecheck.py).** Take named pairs with the same number of calls. Does each MH2 callee
turn out to be the twin of the reference's callee, with neither side's name taken from a call site?

| class | callee pairs that agree |
|---|---|
| identical | 5260 of 5344 (98.4%) |
| absolute | 1832 of 1850 (99.0%) |
| shape | 2016 of 2152 (93.7%) |
| edited, score >= 0.9 | 2814 of 2934 (95.9%) |
| edited, score < 0.9 | 691 of 826 (83.7%) |

So treat `edited` below 0.9 as roughly one in six wrong. Some disagreements are real game changes, not wrong names. This is
an estimate, not a proof of any single name.

**Where code went.** Of the 3,440 or so named functions from MH1's main: 1,635 are in sub_main, 1,518 stayed in main, 183
went to game, 69 to lobby. 1,062 of MH1 lobby's functions are now in nethttp (OpenSSL/HTTP), as in G.

## 4. Compiler setup (tools/mh2_carry.py survey)

    mwcps2-3.0b52-030722  -O4,p  -sdatathreshold 0  -nostdinc  -pragma "divbyzerocheck on"

The same compiler choice as MH1 and G, plus `-sdatathreshold 0`.

- **How it was found:** the first 8 MH2-only functions came out with gp access where MH2 has `lui $at` + `lw x, lo($at)`, and
  a grep of all of asm/mh2 found no gp-relative instruction at all.
- The compiler's help says this option also implies `-model absolute`. No other flag was needed.

Every local Metrowerks compiler was run at -O4,p with that flag on the same 500 candidate MH1 C files (861 functions). Many
candidates hold real MH2 edits, so no compiler reaches 861. What counts is which ones reach the top:

| compiler | functions equal to MH2's bytes |
|---|---|
| 3.0b50-030527, 3.0b52-030722 | 290 |
| 3.0b38-030307 | 281 |
| 3.0b22-020926 | 276 |
| 3.0.3-020716, 3.0b22-020716 | 98 |
| 3.0b22-020123 / 011126 | 86 / 83 |
| 3.0.1 builds (b44 to b95) | 57 falling to 42 |

- b50 and b52 tie, so the data cannot separate them. b52 stays, as for MH1 and G.
- MH2 shipped in 2006, but the code fits a mid-2003 compiler, not the newer 3.0.1 line.
- `-O3` was only tried before the flag was found: 109 against 233 of 233 for -O4,p, on a smaller set of 151 files.

## 5. Matched C so far (tools/mh2_progress.py)

Numbers from the merge of 3 Nov 2026 (before the CODE+HUNT merge; see the plain summary).

| module | functions | matched | code bytes | matched bytes | % |
|---|---|---|---|---|---|
| game | 4580 | 523 | 1963064 | 125364 | 6.39% |
| gm_sub | 128 | 4 | 64152 | 524 | 0.82% |
| lbguild | 1 | 0 | 4 | 0 | 0.00% |
| lobby | 3819 | 543 | 1205876 | 67484 | 5.60% |
| main | 3188 | 131 | 602132 | 24812 | 4.12% |
| nethttp | 1329 | 8 | 345532 | 1332 | 0.39% |
| plsel | 179 | 11 | 88364 | 428 | 0.48% |
| select | 251 | 5 | 99636 | 812 | 0.81% |
| sub_main | 3437 | 1017 | 1154632 | 142624 | 12.35% |
| yn | 122 | 48 | 36468 | 7028 | 19.27% |
| **all** | **17034** | **2290** | **5559860** | **370408** | **6.66%** |

The totals include Sony's libraries and middleware (CRI, OpenSSL), which have no marker yet. So the share of Capcom's own
game code is a little different.

### 5.1 How C gets in (tools/mh2_carry.py register, rewritten 2 Nov 2026)

About 2,160 C files that MH1 or G link are compiled with MH2's flags. Each object is laid out as ld would lay it out and
searched for in MH2's code:
- a run without relocations is found directly;
- the rest are compared with relocated fields masked;
- the name map only breaks ties. It decides nothing;
- then every relocation is checked against what the original instruction holds.

Results of the last run:

| what | count |
|---|---|
| whole files that match | 710 |
| parts of other files | 520 |
| data slots | 38 (with the hand ones) |
| code not found anywhere | 1,358 files |
| several sites and no name to choose | 118 files (**Conflict:** section 7 says 109 files) |
| a name needs two addresses | 14 files, 44 single functions: one global reached at two offsets that do not fit (**Conflict:** section 6 item 7 says 17 files, 51 functions) |
| overlaps a bigger match | 12 |
| do not compile | 4 |

- **The old count was wrong.** Earlier versions of this file said 300 of 500 files "compile but differ". At least 79 of those
  did not differ at all (every function equal). MWCC gives every function its own .text section, and the old compare took
  each function's offset inside its own section as its offset in the file, so any file with two functions failed. Only 22
  candidates held data of their own, so data slots were not the big unlock this file's next-steps list said they were. The
  bug and the wider search were.
- **Data slots.** `MODULE:data START END NAME` in config/mh2/c_files.txt, cut by tools/mh2_setup.py.
  - MH2's module data is one typed block, so every slot is a `.data` linker entry, and the object's .rodata is renamed to
    .data, as for MH1 and G (build.py's existing k-th section handling).
  - The slot address comes from the code: the original `lui`/`addiu` pair says where the data is. The section's bytes must
    then equal MH2's; words with relocations are checked as addresses (jump tables).
  - Slot ends are rounded up to a word, because the asm after a slot must start aligned or all its labels shift (zeros
    only, else refused).
- **Parts.** When a file does not match whole, the functions of it that do match still go in, with no source edit.
  - src/mh2/parts/NAME.c includes the file. config/mh2/c_parts.txt lists the object sections that stay, and tools/build.py
    keep_sections drops the rest at link time.
  - What a kept function needs from a dropped one is its real address: a global name through c_aliases.txt, a static as an
    absolute symbol.
  - Data a kept function uses gets a slot if it matches whole and no other part took it. Otherwise it stays in the asm and
    is reached by address, so a part may carry code but not its strings.
  - A jump table cannot stay behind (its labels live in the C), so such a function is refused.
  - Section numbers are the compiler's: **rerun `mh2_carry.py register` after editing any carried C or header**, or
    keep_sections drops the wrong sections and the link says so.
  - This is a different way from G's `G_PART` guards, chosen because it leaves the shared sources alone.
- **G's form.** A file that does not match as MH1 sees it is tried with VERSION_MHG on (G's headers). Those go through a
  wrapper that defines it, so nothing changes for MH1 or G.
- **Renames.** A name that the symbol files or another file place elsewhere is renamed in that object only
  (config/mh2/c_symfix.txt, `objcopy --redefine-sym` through c_renames.txt). Mostly these are map names that are wrong or
  that exist twice, so treat that list as a to-do for the name map. **Conflict:** this paragraph said 145 lines; the
  "Name map errors" paragraph below says it has grown to 213.
- Files with `.inc` raw functions are not carried: those hold original bytes, not C.
- **Written for MH2 (11 files at the 3 Nov merge):** src/mh2/plsel/p00D*.c, plsel_func_00D61710 and friends; src/mh2/game:
  next_cmd_search, cmd_target (cmd_target_kind_set and target_kind_set, C in include/em_core_g_mh2.h, with a jump-table
  slot) and em_cmd_ck (docs/mh2/quest.md). Four more came later on 3 Nov: stage_set_set, game 0x80D270, em01_senkai_sub,
  game 0x75FBF0 (docs/mh2/code.md).
  - Written from our own disassembly. `tools/mh2_own.py -v` shows a side-by-side; it takes map names too.
  - Registered by hand at the end of config/mh2/c_files.txt, with globals in config/mh2/hand_names.txt. Both blocks
    survive `register`.
  - config/mh2/hand_parts.txt keeps only the named functions of a hand file whose same-file static must be compiled for
    the right registers (next_cmd_search).
  - Near-matches, not in the MH2 build: src/mh2/game/em_dmg_sys_nm.c (Em_Dmg_Sys, linked by the PC through em_taisei.c),
    src/mh2/sub_main/p003A2E30_nm.c and p003A9230_nm.c.
- Every one is checked by the search above, by the full link (OK per module), and by tools/mh2_check.py.

### 5.2 Files that differ (docs/survey/mh2_differ.csv, counts only)

750 files with at least one named twin, sorted by how many instructions differ from MH2 (relocated fields masked): 37 with
none, 90 with 1-7, 235 with 8-63, 388 with 64 or more.
- `functions_same` says how many of a file's functions are equal. Those are mostly already in as parts.
- The MH1 and G views of one source can both be listed.
- This list ranks only the MH1 form of many files, so it needs a G-form column before it can be trusted as an order of work
  (section 6).

### 5.3 How MH2's differences live in the shared sources (merge of 2 Nov 2026)

All of these are plain values for MH1, G and the PC. Only an MH2 build changes them.

| thing | where | what it does |
|---|---|---|
| `MH2V(mh1, mh2)` | include/types.h | a constant MH2 changed |
| `stg_t` | include/types.h | a stage number: u8, 16 bits in MH2 |
| `MH2_OFS(p, o)` | include/types.h, include/mh2_ofs.h (generated) | MH1 offset o into what p points at; picks PLW's or EMW's map by `sizeof(*p)`, any other size keeps o |
| `MH2_EM(o)` | include/types.h | the EMW map, for pads of local copies of EMW |
| PLW, EMW | include/mh2_plw.h, mh2_emw.h (generated) | `tools/mh2_structs.py probe`, then `gen`; hand entries in config/mh2/struct_map.txt |
| game_w | include/game.h | G's struct with room added, one definition for G and MH2 |
| `GWX(o)` | include/gwx.h | G's table, then `MH2_GW` on the result |
| CNET_SYS, `CNO(o)` | include/cnet_mh2.h (generated) | `tools/mh2_structs.py cnet` from include/lbnet.h and config/mh2/cnet_map.txt |
| `SKO(o)` | include/sk_g.h | soft keyboard work offsets |
| `PLW_AT(i)` | include/version.h | one player by index |
| browser block | include/mh2_lb.h | see "Facts read from MH2's code" below |
| QUEST_W and four more | include/mh2_layouts.h (generated) | `tools/mh2_structs.py layouts` from config/mh2/layouts.txt |
| `QWO`, `UDMO`, `NCO`, `LBM`, `PMO` | include/mh2_lay_ofs.h (generated), types.h | MH1 offset (UDMO: also G's) into quest_w, User_data, net_common_w, lb_sys, PitMenu |
| `qno_t` | include/types.h | quest number in copies of quest_w's head: u16 in MH2 |

**Rule: write the MH1 offset once.** The merge broke this rule three times.
- `CNO`, `SKO`, `GWX` and `MH2_OFS` already map an MH1 offset to MH2's. Inside them, write the MH1 offset alone.
- `CNO(MH2V(a, b))` or `SKO(MH2V(a, b))` applies the move twice.
- Macros that call `MH2_OFS` on a parameter (`EM_FIELD`, `M2C_FIELD`, `EMF`, `PU8` and others) follow the same rule.
- `MH2_OFS` needs a pointer to a complete type. A macro used with `void *` or an integer does not compile. That is why the
  macros of src/main/mc/netfile2*, src/main/chat, src/main/tu/chat_all.c and src/lobby/f/lb_shp.c stayed plain.

**`MH2V` around an absolute address, and the PC build.** This is safe in shared C only because `tools/pc_abs.py` drops the
`MH2V` before rewriting addresses for the PC (and the `grep` in `tools/build_pc.sh` lets such files through). Without that,
the PC build read address 0x3F3404 directly and crashed. It was found by the full check on the four-branch merge: 6 of 47
steps failed (monster death, `Em_Dmg_Sys`), and all 47 passed after the fix. MH1/G byte checks cannot catch this kind of
bug. So after edits to shared C run `tools/full_check.sh`, and also `tools/mh2_rebuild.sh` + `tools/mh2_check.py`, since
the full check does not cover MH2.

### 5.4 Facts

**Read from MH2's code.**
- PLW is 0x970 bytes, EMW 0x9D0.
- game_w is at 0x6ADCB0, CnetSys_w at 0x84A440.
- There are 10 players and lobby members, not 8.
- Stage numbers are 16 bits.
- LBCOMMER and CWPLAYER are 0x11C.
- The browser's globals sit in one block, reached through the pointer at 0x895270, the slot that is bsUrl in MH1
  (include/mh2_lb.h).
- The HTTP task is 0xE4, BsLbsInfo 0x114 x 40, PIT_CHAT 0x4F, EQ_ARMOR 0x28, EQ_GUN 0x18, an Item_data row 24 bytes.
- `#pragma divbyzerocheck off` is needed around UpdateEndpoint.
- The extra nops in MH2 are 8-byte aligned branch targets.

**Proven by memsets (merge of 3 Nov; details in docs/mh2/structs.md).** quest_w 0x5D8260 (0x560), User_data 0x5D4A60
(0x37F8), net_common_w 0x5D46A0 (0xE0), PitMenu 0x5BB080, game_w 0x6ADCB0 (0x2BC), system_w 0x6ADF90 (0xF4), MemcardWork
0x6AF100 (0xF4), lb_sys 0x8BDC10 (0xF68).

**About the tools.**
- `register` deletes src/mh2/parts, so a hand copy cannot live there.
- mh2_carry.py's object cache now notices changes in include/ (not a .h next to a source; empty build/mh2/carry then).
- mh2_diff.py always compiles fresh. It is the quick way to see why one function differs.
- `MH2_JOBS=N` sets register's jobs.
- A C function goes in at one site only. MH2 has shape twins (same code, other relocations), so the map's `_mh2_ADDRESS`
  twin can stay in the asm while its first site is C.
- `mh2_carry.py register` once reported 17 files "do not compile" right after the carry cache was emptied (4 on the reruns,
  same tree). The cause was not found (heat at 95 C, other agents compiling). Check that count after a register and rerun
  if it jumps. (docs/mh2/code.md, 3 Nov: each compile now gets a second try with a 600 s limit; a timeout is the guess.)

**Name map errors found.** get_prim and get_prim2 are swapped (0x376EF0 is the 0x200 pool). config/mh2/c_symfix.txt has
grown to 213 lines; each is a place to check.

### 5.5 Guessed, not proven

- GAME_W's size 0x2C0. **Conflict:** the memsets above (and docs/mh2/structs.md) prove 0x2BC; the merge of 3 Nov already
  called 0x2C0 wrong. Kept here because it was the earlier claim.
- CNET_SYS's size 0x4B000.
- reward_item[48] (only the room between its neighbours says so).
- area_mdlw at 0xF0 (no code seen; the pad has to sit before it because of alignment).
- Offsets in runs where no field sits on either side of a boundary.
- The end of SHLW. The G-only tail of EMW.
- Overlaid fields: 11 of EMW and 4 of PLW sit in the unions at the end of the struct, because they land on a better-placed
  field. They compile at the given offset, but their real place or type is not settled. MH1 offsets: EMW 50C, 616, 617,
  74C, 762, 765, 766, 824, 9ED, 9EE, A00; PLW 4E3, 5FC, 73C, 8C3.

**Where the two field generators disagreed** (kept value, then the other method's value as "A"). Fields the probe has votes
on stayed with the votes.
- PLW: 35F 35B (A 31F), 360 35C (A 368), 362 35E (A 36A), 4D6 4BA (A 4CA), 4DD 4C1 (A 4D1), 4E0 4C4 (A 4D4; no votes),
  615 417 (A 599), 934 890 (A 88C), 936 892 (A 88E).
- EMW: 73A 684 (A 686; two votes), 881 997 (A 7D5), 88B 437 (A 7DF; two votes), 88D 9A8 (A 7E1).
- With no votes, the run-read value was tried in config/mh2/struct_map.txt and kept. The count rose by 5 with these, the
  game_w fix and the macros in the same run, so the gain is not proven for each line: PLW 601, 603, 714, 739, 73A, 800-818;
  EMW 600, 6E0, 6E2, 7F0-7F2, 824, 828-82B, 9C8. Only x824 and x828 onward are shown by code (em_cmd_sub_contents,
  `sb v1,1910(a0)`).
- EMW 734 and 73C stayed with the probe's fill, because the run-read 0x680 lands on the stage number.

### 5.6 Merge of 3 Nov 2026 (QUEST, STRUCTS, PLATFORM, DATA)

- Result: 2,290 functions, the union of the four branches' sets (QUEST 2,272, STRUCTS 2,280, PLATFORM and DATA 2,268).
  Nothing lost.
- Two parts of pl_ammo (Pl_scope_ck at sub_main 0x3B54C0, 0x3B5590, 48 bytes each) were lost on STRUCTS' branch, because the
  probe's votes moved PLW wpn_ammo to 0x30C. MH2 reads it at 0x35E (lhu 862), so config/mh2/struct_map.txt pins it there.
- QUEST's temporary `union EMT` in include/em_core_g_mh2.h is gone. STRUCTS' EMW has x828 0x77A, x829 0x77C, x882 0x998,
  x883 0x99A, x92F 0x986, x95A 0x98C (u16) and cmd_idx 0x774 (byte), and the bytes stay identical.
- The EMT in src/mh2/game/em_dmg_sys_nm.c stayed at the merge: it reaches EMW + 0x3D0, a pointer to a table with a function
  at +0x18, which EMW still had as padding. (Later on 3 Nov, docs/mh2/code.md: EMW + 0x3D0 is now a real field, `EMW.x3CC`,
  type `EM_X3CC`, include/em.h.)
- em_cmd_ck.c calls its handlers by DATA's opcode names.

## 6. Next

Remaining work, by reason. Status notes in brackets were added on 3 Nov from docs/mh2/code.md and hunt.md.

1. Layouts not done: CameraWork (restructured, docs/mh2/structs.md), ConnWork, MemcardWork, system_w. quest_w, User_data,
   net_common_w, lb_sys and PitMenu are done (3 Nov).
2. The third CnetSys_w send layout (cnlbs*).
3. Array scale changes: an index multiplied by a new element size.
4. False twins from the name map, and tiny sites with the wrong name.
5. Real rewrites: MH2 changed what the function does.
6. A few register-allocation stragglers: flGetMotionMatrix, SaveNetFileBr, dic_learn.
7. Two-address files: a global reached at two offsets (17 files, 51 functions), mostly a struct still wrong. (**Conflict**
   with the table in 5.1: 14 files, 44 functions.)
8. mh2_differ.csv needs a G-form column.
9. Parts that left strings in the asm: when the rest of such a file matches, its data gets a slot by itself on the next
   register.
10. Type main's data block and name the Sony and CRI code properly.
11. PC port: started 2 Nov 2026, reaches the title, on G's form of the C; `--quest` draws a quest's stage, monsters and
    hunter (no game code runs). docs/mh2_port.md. [Since 3 Nov `--quest` runs G's quest path on MH2's Quest_start and stops
    at the blockers in docs/mh2/hunt.md.]
12. Quest core (docs/mh2/quest.md): the near-matches Em_Dmg_Sys, 0x3A2E30, 0x3A9230; em01_senkai_sub (rewritten); game
    0x80D270, 0x75FBF0. [em01_senkai_sub, 0x80D270 and 0x75FBF0 byte-matched on 3 Nov; the three near-matches remain,
    docs/mh2/code.md.]
13. Names: the 584 tables the PC cannot place yet, 114 of them on the one-monster quest path (docs/mh2/data.md).
    func_fix.txt has two lines for game 0x72B240 (opcode 0x2C em_cmd_myemtype_sel, and file order game_func_005885E0); the
    later wins, so em_cmd_ck.c calls game_func_005885E0. One of them is wrong. [Settled on 3 Nov, docs/mh2/code.md:
    0x72B240 is game_func_005885E0 and em_cmd_myemtype_sel is 0x72B3A0, opcode 0x70, as in G.]

## 7. Not done or not tested

- Nothing here has been run in PCSX2. The matching proof is byte identity with the disc only. The PC build
  (docs/mh2_port.md) does not prove the matched C either: it runs only a small boot (and, since 3 Nov, the start of a quest
  up to the blockers in docs/mh2/hunt.md).
- The English v1.03 ISO in disc/mh2 was not used here. (docs/mh2/platform.md uses it for the install and the streams.)
- dnas_net and dnas_ins only rebuild as bytes. No work was done on what is in them, by design.
- No names or conclusions were cross-checked with the community repo; the allowance to do so was not needed.
- Ties: when a file has several sites and no name chooses, it is left out (109 files; **Conflict:** the table in 5.1 says
  118). When the map names a site, its word is taken, so a tiny function can sit under the wrong name although the bytes are
  right. Bytes prove the code, not the name.
- A part's data left in the asm means that part cannot move in memory yet (it holds absolute addresses). That is fine for a
  byte match, but it must be fixed before any relink or port.
