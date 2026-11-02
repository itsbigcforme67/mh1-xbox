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
- 201 functions (34,280 bytes, 0.62% of the code) are already matching C: 193 functions in 192
  unchanged MH1 files, plus 8 small MH2-only functions written fresh from our disassembly.

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
| game | 4580 | 3 | 1963064 | 780 | 0.04% |
| gm_sub | 128 | 0 | 64152 | 0 | 0.00% |
| lbguild | 1 | 0 | 4 | 0 | 0.00% |
| lobby | 3819 | 90 | 1205876 | 11216 | 0.93% |
| main | 3188 | 16 | 602132 | 6232 | 1.03% |
| nethttp | 1329 | 0 | 345532 | 0 | 0.00% |
| plsel | 179 | 8 | 88364 | 336 | 0.38% |
| select | 251 | 1 | 99636 | 384 | 0.39% |
| sub_main | 3437 | 70 | 1154632 | 12488 | 1.08% |
| yn | 122 | 13 | 36468 | 2844 | 7.80% |
| **all** | **17034** | **201** | **5559860** | **34280** | **0.62%** |

The totals include Sony's libraries and middleware (CRI, OpenSSL), which have no marker yet, so the share of Capcom's
game code is a little different.

- **Carried (192 files):** `tools/mh2_carry.py register` takes each matched MH1 C file whose functions all have twins
  in one module, compiles it with the MH2 flags and keeps it only if it equals MH2's bytes, sits in MH2 in the same order,
  has no data of its own, and every name it uses agrees with the symbol files. Out of 1,719 MH1 text files: 500 got that
  far, 300 came out different (MH2 changed them, or MH1's struct layouts in include/ are out of date for MH2), the rest
  were dropped by those checks. The files are shared: src/lobby/f/lb_z43.c is the same file in all three games.
- **Written for MH2 (8):** src/mh2/plsel/p00D*.c, plsel_func_00D61710 and friends, from our own disassembly
  (`tools/mh2_own.py -v` shows a side-by-side). Registered by hand at the end of config/mh2/c_files.txt, globals in
  config/mh2/hand_names.txt. Struct and global names there are placeholders; what these functions do in the game is not known.
- Every one is checked three ways: per function (relocated fields masked), by the full link (OK per module), and by
  tools/mh2_check.py.

## 6. Next

1. Data of its own: 22 files have data slots, and most of the 500 candidates that have strings, float literals or jump
   tables need `MODULE:SECTION` slots like G's (tools/mh2_setup.py does not cut data yet). That is the biggest cheap gain.
2. The 300 "not equal" files: sort by how few instructions differ. Struct offsets moved, so most need one
   header edit, and `#ifdef VERSION_MH2` must stay out of G's and MH1's paths.
3. Files with one function lacking a twin (1,143 of them): name those functions by hand or split the file for MH2.
4. Type main's data block and name the Sony and CRI code properly.
5. A PC port is not started. Whether to port MH2 onto the G engine is the owner's call later.

## 7. Not done or not tested

- Nothing here has been run: no PC build, no PCSX2 boot. Proof is byte identity with the disc only.
- The English v1.03 ISO in disc/mh2 was not used.
- dnas_net and dnas_ins only rebuild as bytes. No work was done on what is in them, by design.
- No names or conclusions were cross-checked with the community repo; the allowance to do so was not needed.
