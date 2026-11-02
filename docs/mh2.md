# Monster Hunter 2 (Dos, PS2, SLPM_662.80, JP 1.04) in this repo

Set up 1-2 Nov 2026 (owner's decision of 1 Nov, docs/DECISIONS.md: we split and decompile MH2
ourselves, from the owner's disc, with our tools). The community mh2-decomp repo was not opened
during this work: nothing here comes from it. No PC port yet. Numbers are from the tools named.

## Plain summary

- Every piece of MH2's code now rebuilds byte for byte from disassembly: main and all 12 overlays.
- 7,285 of 17,034 functions (39.7% of the code bytes) have a real name carried over from MH1's symbol
  table, directly or through G. The rest stay `func_ADDRESS`.
- MH2 uses the same compiler as MH1 and G. The one difference: it was built with small data off, so it
  never uses the gp register. Add `-sdatathreshold 0` and MH1's C compiles to MH2's bytes.
- 2,268 functions (353,996 bytes, 6.37% of the code) are matching C: 1,223 C objects built from MH1's and G's
  sources, which now hold MH2's differences behind macros, plus 8 small MH2-only functions written fresh from our
  disassembly (`tools/mh2_progress.py`).

## 1. Layout

| | |
|---|---|
| Program | SLPM_662.80, 822,368 bytes, stripped, `.comment` "MW MIPS C Compiler (2.4.1.01)" (linker string, identifies nothing) |
| main | file offset 0x280, loads at 0x100000, 0xC8600 bytes in the file, 0x1E3900 in memory; ends at 0x2E3900 where sub_main loads |
| main text | 0x100000-0x195C00 (read from the code: the data starts with the "PsIIlibcdvd" version tag) |
| main data | 0x195C00-0x1C8600. Not typed further: the block 0x198000-0x19A000 looks like code (`jr ra`s, no calls in or out) but is kept as data; bytes rebuild either way |
| gp | 0x1D05F0 (.reginfo). The file ends exactly at gp - 0x7FF0: there is no .sdata in the file, and no code uses gp |
| Overlays | MWo3, stored uncompressed in DATA.BIN (AFS: not compressed like G's). Header as in tools/mwo_unpack.py |

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

Which overlays are loaded together (the `LOADED_WITH` table in tools/mh2_setup.py) is an assumption from
the shared addresses and from G. It only decides which names a module may see; it is not tested in play.

## 2. Build

Needs disc/mh2 (SLPM_662.80, DATA.BIN, overlays/*.bin carved from DATA.BIN), splat in .venv,
tools/binutils and tools/compilers/ as in README Phase 1.

    tools/mh2_rebuild.sh              # split and build all 13 modules; prints OK per module
    python3 tools/mh2_check.py        # the check against the disc itself
    MH2_NOSPLIT=1 tools/mh2_rebuild.sh # after editing C only
    python3 tools/mh2_carry.py register # rebuilds the list of carried C, wrappers and slots; then a full rebuild
    python3 tools/mh2_carry.py fix      # after a failed compare: which C files sit on the wrong bytes

`MH2_PY=/path/python` picks the python with splat when .venv is elsewhere (git worktrees). Everything goes to
asm/mh2, build/mh2, assets/mh2, which are gitignored. config/mh2 holds only names and addresses;
`config/mh2/*_aliases.txt` are generated (and large), so they are not committed.
In tools/build.py this is `VERSION=mh2`; the MH1 and G branches only gained exclusions of the mh2 folders.

**How it was verified.** tools/mh2_check.py reads disc/mh2, not our copies. It takes SHA-256 of each
built overlay against disc/mh2/overlays, of the same overlay found inside DATA.BIN, and of the whole
SLPM_662.80 with its code and data swapped for the rebuilt main.bin (the ELF headers come from the disc: they are
not rebuilt). Result on the last full build, with names and matched C in: 13 of 13 files identical.

## 3. Names (tools/mh2_map.py, docs/survey/mh2_map.csv)

Sources: 13,289 functions in MH1's symbol table, plus 2,032 function bodies that exist only in G's form
(G's names are MH1's, moved by tools/mhg_map.py). Nothing is guessed from behaviour.

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

6,271 names came from MH1 directly, 1,014 through G's form of a function, plus 659 data names. Class meanings are at
the top of tools/mh2_map.py; the short version: identical/absolute/shape are the same code, edited is a good guess
with a score, rewritten is a hint, ambiguous got no name because several names have that body. dnas_net, dnas_ins
and test have no function list (the two DNAS overlays decode to almost no functions and were left alone).

The function list itself is a heuristic (call targets, ends of `jr ra` blocks, starts of twins, as in tools/mhg_map.py).
17,034 can include a few wrong cuts, and tiny stubs fool it. Wrong cuts cost no bytes.

**How far to trust the names (tools/mh2_namecheck.py).** For named pairs with the same number of calls, does each MH2
callee turn out to be the twin of the reference's callee, with neither side's name taken from a call site?

| class | callee pairs that agree |
|---|---|
| identical | 5260 of 5344 (98.4%) |
| absolute | 1832 of 1850 (99.0%) |
| shape | 2016 of 2152 (93.7%) |
| edited, score >= 0.9 | 2814 of 2934 (95.9%) |
| edited, score < 0.9 | 691 of 826 (83.7%) |

So treat `edited` below 0.9 as roughly one in six wrong. Some disagreements are real game changes, not wrong names.
This is an estimate, not a proof of any single name.

Where code went: of the 3,440 or so named functions from MH1's main, 1,635 are in sub_main, 1,518 stayed in main,
183 went to game, 69 to lobby. 1,062 of MH1 lobby's functions are now in nethttp (OpenSSL/HTTP), as in G.

## 4. Compiler setup (tools/mh2_carry.py survey)

    mwcps2-3.0b52-030722  -O4,p  -sdatathreshold 0  -nostdinc  -pragma "divbyzerocheck on"

The same compiler choice as MH1 and G, plus `-sdatathreshold 0`. Found because the 8 first MH2-only functions came out
with gp access where MH2 has `lui $at` + `lw x, lo($at)`, and a grep of all of asm/mh2 found no gp-relative
instruction at all. The compiler's help says this option also implies `-model absolute`. No other flag was needed.

Every local Metrowerks compiler at -O4,p with that flag, on the same 500 candidate MH1 C files (861 functions).
Many candidates hold real MH2 edits, so no compiler reaches 861; what counts is which ones reach the top:

| compiler | functions equal to MH2's bytes |
|---|---|
| 3.0b50-030527, 3.0b52-030722 | 290 |
| 3.0b38-030307 | 281 |
| 3.0b22-020926 | 276 |
| 3.0.3-020716, 3.0b22-020716 | 98 |
| 3.0b22-020123 / 011126 | 86 / 83 |
| 3.0.1 builds (b44 to b95) | 57 falling to 42 |

b50 and b52 tie, so the data cannot separate those two; b52 stays, as for MH1 and G. MH2 shipped in 2006 but
the code fits a mid-2003 compiler, not the newer 3.0.1 line. `-O3` was only tried before the flag was found
(109 against 233 of 233 for -O4,p on a smaller set of 151 files).

## 5. Matched C so far (tools/mh2_progress.py)

| module | functions | matched | code bytes | matched bytes | % |
|---|---|---|---|---|---|
| game | 4580 | 514 | 1963064 | 110452 | 5.63% |
| gm_sub | 128 | 4 | 64152 | 524 | 0.82% |
| lbguild | 1 | 0 | 4 | 0 | 0.00% |
| lobby | 3819 | 541 | 1205876 | 67388 | 5.59% |
| main | 3188 | 131 | 602132 | 24812 | 4.12% |
| nethttp | 1329 | 8 | 345532 | 1332 | 0.39% |
| plsel | 179 | 11 | 88364 | 428 | 0.48% |
| select | 251 | 5 | 99636 | 812 | 0.81% |
| sub_main | 3437 | 1006 | 1154632 | 141220 | 12.23% |
| yn | 122 | 48 | 36468 | 7028 | 19.27% |
| **all** | **17034** | **2268** | **5559860** | **353996** | **6.37%** |

The totals include Sony's libraries and middleware (CRI, OpenSSL), which have no marker yet, so the share of Capcom's
game code is a little different.

**How C gets in (tools/mh2_carry.py register, rewritten 2 Nov 2026).** About 2,160 C files that MH1 or G link are compiled
with MH2's flags, each object laid out as ld would, and searched for in MH2's code: a run without relocations is found,
the rest compared with relocated fields masked. The name map only breaks ties, it decides nothing. Then every relocation
is checked against what the original instruction holds. Results of the last run:

| what | count |
|---|---|
| whole files that match | 704 |
| parts of other files | 519 |
| data slots | 36 |
| code not found anywhere | 1,361 files |
| several sites and no name to choose | 118 files |
| a name needs two addresses | 17 files, 51 single functions: one global reached at two offsets that do not fit |
| overlaps a bigger match | 12 |
| do not compile | 4 |

- **The old count was wrong.** Earlier versions of this file said 300 of 500 files "compile but differ". At least 79 of
  those did not differ at all (every function equal): MWCC gives every function its own .text section, and the old compare took each function's
  offset inside its own section as its offset in the file, so any file with two functions failed. Only 22 candidates
  held data of their own, so data slots were not the big unlock this file's next-steps list said they were. The bug
  and the wider search were.
- **Data slots.** `MODULE:data START END NAME` in config/mh2/c_files.txt, cut by tools/mh2_setup.py. MH2's module data is
  one typed block, so every slot is a `.data` linker entry and the object's .rodata is renamed to .data, as for MH1 and G
  (build.py's existing k-th section handling). The slot address comes from the code: the original `lui`/`addiu` pair says where
  the data is; the section's bytes must then equal MH2's, words with relocations checked as addresses (jump tables). Slot ends
  are rounded up to a word because the asm after a slot must start aligned or all its labels shift (zeros only, else refused).
- **Parts.** When a file does not match whole, the functions of it that do still go in, with no source edit. src/mh2/parts/NAME.c
  includes the file, config/mh2/c_parts.txt lists the object sections that stay, and tools/build.py keep_sections drops the rest
  at link time. What a kept function needs from a dropped one is its real address: a global name through c_aliases.txt, a static
  as an absolute symbol. Data a kept function uses gets a slot if it matches whole and no other part took it; otherwise it
  stays in the asm and is reached by address, so a part may carry code but not its strings. A jump table cannot stay behind
  (its labels live in the C), so such a function is refused. Section numbers are the compiler's: **rerun
  `mh2_carry.py register` after editing any carried C or header**, or keep_sections drops the wrong sections and the link says so.
  This is a different way from G's `G_PART` guards, chosen because it leaves the shared sources alone.
- **G's form.** A file that does not match as MH1 sees it is tried with VERSION_MHG on (G's headers). Those go through a wrapper
  that defines it, so nothing changes for MH1 or G.
- **Renames.** A name that the symbol files or another file place elsewhere is renamed in that object only
  (config/mh2/c_symfix.txt, 145 lines, `objcopy --redefine-sym` through c_renames.txt). Mostly these are map names that
  are wrong or that exist twice, so treat that list as a to-do for the name map.
- Files with `.inc` raw functions are not carried: those hold original bytes, not C.
- **Written for MH2 (8):** src/mh2/plsel/p00D*.c, plsel_func_00D61710 and friends, from our own disassembly
  (`tools/mh2_own.py -v` shows a side-by-side). Registered by hand at the end of config/mh2/c_files.txt, globals in
  config/mh2/hand_names.txt. Struct and global names there are placeholders; what these functions do in the game is not known.
- Every one is checked by the search above, by the full link (OK per module), and by tools/mh2_check.py.

**Files that differ (docs/survey/mh2_differ.csv, counts only).** 750 files with at least one named twin, sorted by how many
instructions differ from MH2 (relocated fields masked): 37 with none, 90 with 1-7, 235 with 8-63, 388 with 64 or more. `functions_same`
says how many of a file's functions are equal, and those are mostly already in as parts. The MH1 and G views of one source can both be
listed. This list ranks only the MH1 form of many files, so it needs a G-form column before it can be trusted as an order of work (section 6).

**How MH2's differences live in the shared sources (the merge of 2 Nov 2026).** All of these are plain values for MH1, G and the PC.

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
| browser block | include/mh2_lb.h | see the facts below |

**One rule follows, and the merge hit it three times:** `CNO`, `SKO`, `GWX` and `MH2_OFS` already map an MH1 offset for MH2. Inside them write the MH1
offset alone. `CNO(MH2V(a, b))` or `SKO(MH2V(a, b))` applies the move twice. Macros that call `MH2_OFS` on a parameter (`EM_FIELD`, `M2C_FIELD`, `EMF`,
`PU8` and others) follow the same rule. `MH2_OFS` needs a pointer to a complete type: a macro used with `void *` or an integer does not compile, which is why
the macros of src/main/mc/netfile2*, src/main/chat, src/main/tu/chat_all.c and src/lobby/f/lb_shp.c stayed plain.

**Facts, read from MH2's code.** PLW is 0x970 bytes, EMW 0x9D0. game_w is at 0x6ADCB0, CnetSys_w at 0x84A440. There are 10 players and lobby members, not 8.
Stage numbers are 16 bits. LBCOMMER and CWPLAYER are 0x11C. The browser's globals sit in one block, reached through the pointer at 0x895270, the slot that is bsUrl
in MH1 (include/mh2_lb.h). The HTTP task is 0xE4, BsLbsInfo 0x114 x 40, PIT_CHAT 0x4F, EQ_ARMOR 0x28, EQ_GUN 0x18, an Item_data row 24 bytes.
`#pragma divbyzerocheck off` is needed around UpdateEndpoint. The extra nops in MH2 are 8-byte aligned branch targets.

**Facts about the tools.** `register` deletes src/mh2/parts, so a hand copy cannot live there. mh2_carry.py's object cache now notices include/ (not a .h next to a source;
empty build/mh2/carry then). mh2_diff.py always compiles fresh; it is the quick way to see why one function differs. `MH2_JOBS=N` sets register's jobs.
A C function goes in at one site only. MH2 has shape twins (same code, other relocations), so the map's `_mh2_ADDRESS` twin can stay in the asm while its first site is C.

**Name map errors found.** get_prim and get_prim2 are swapped (0x376EF0 is the 0x200 pool). config/mh2/c_symfix.txt has grown to 213 lines; each is a place to check.

**Guessed, not proven.** GAME_W's size 0x2C0. CNET_SYS's size 0x4B000. reward_item[48] (only the room between its neighbours says so). area_mdlw at 0xF0 (no code seen; the
pad has to sit before it because of alignment). Offsets in runs where no field sits on either side of a boundary. The end of SHLW. The G-only tail of EMW. Overlaid fields: 11 of EMW and
4 of PLW sit in the unions at the end of the struct, because they land on a better placed field; they compile at the given offset, but their real place or type is
not settled (MH1 offsets: EMW 50C, 616, 617, 74C, 762, 765, 766, 824, 9ED, 9EE, A00; PLW 4E3, 5FC, 73C, 8C3).

**Where the two field generators disagreed (kept value, with the method's value).** Fields the probe has votes on stayed with the votes. PLW: 35F 35B (A 31F), 360 35C (A 368), 362 35E (A 36A),
4D6 4BA (A 4CA), 4DD 4C1 (A 4D1), 4E0 4C4 (A 4D4; no votes), 615 417 (A 599), 934 890 (A 88C), 936 892 (A 88E). EMW: 73A 684 (A 686; two votes), 881 997 (A 7D5), 88B 437 (A 7DF; two votes), 88D 9A8 (A 7E1).
With no votes the run-read value was tried in config/mh2/struct_map.txt and kept: the count rose by 5 with these, the game_w fix and the macros in the same run, so the gain is not proven for each line. PLW 601, 603, 714, 739, 73A, 800-818; EMW 600, 6E0, 6E2,
7F0-7F2, 824, 828-82B, 9C8. Only x824 and x828 on are shown by code (em_cmd_sub_contents, `sb v1,1910(a0)`). EMW 734 and 73C stayed with the probe's fill, because the run-read 0x680 lands on the stage number.

## 6. Next

Remaining work, by reason:

1. Layouts not done: net_common_w, PitMenu, CameraWork, User_data, ConnWork, MemcardWork, quest_w.
2. The third CnetSys_w send layout (cnlbs*).
3. Array scale changes: an index multiplied by a new element size.
4. False twins from the name map, and tiny sites with the wrong name.
5. Real rewrites: MH2 changed what the function does.
6. A few register-allocation stragglers: flGetMotionMatrix, SaveNetFileBr, dic_learn.
7. Two-address files: a global reached at two offsets (17 files, 51 functions), mostly a struct still wrong.
8. mh2_differ.csv needs a G-form column.
9. Parts that left strings in the asm: when the rest of such a file matches, its data gets a slot by itself on the next register.
10. Type main's data block and name the Sony and CRI code properly.
11. A PC port is not started. Whether to port MH2 onto the G engine is the owner's call later.

## 7. Not done or not tested

- Nothing here has been run: no PC build, no PCSX2 boot. Proof is byte identity with the disc only.
- The English v1.03 ISO in disc/mh2 was not used.
- dnas_net and dnas_ins only rebuild as bytes. No work was done on what is in them, by design.
- No names or conclusions were cross-checked with the community repo; the allowance to do so was not needed.
- Ties: when a file has several sites and no name chooses, it is left out (109 files). When the map names a site, its word
  is taken, so a tiny function can sit under the wrong name although the bytes are right. Bytes prove the code, not the name.
- A part's data left in the asm means that part cannot move in memory yet (it holds absolute addresses): fine for a byte match,
  to be fixed before any relink or port.
