# PS2 Monster Hunter G vs MH1: how much carries over (10 Oct 2026)

Measured with tools/mhg_match.py. It counts MH1 functions whose instructions show up
unchanged in G, ignoring addresses, jump targets and 16-bit immediates.

| MH1 module | functions | found in G | share of code bytes |
|---|---|---|---|
| main   | 6978 | 5040 | 65.5% |
| game   | 2640 | 1797 | 55.0% |
| lobby  | 3486 |  915 | 22.5% |
| select |   44 |   11 | 23.4% |
| yn     |  141 |   96 | 57.4% |

- About 52% of all MH1 code, and about 61% outside the lobby, is in G unchanged in shape.
  Most of the rest is probably edited versions of the same functions, not new code; this
  count does not measure that.
- G moved part of main into a new overlay, sub_main.bin. 295 MH1 main functions turned
  up there.
- AFS archives: 2003 of the 2318 MH1 AFS_DATA names are also in G (G has 3347). Every
  AFS00 and AFS01 name from MH1 is in G as well.
- G is stripped, so the names have to come from MH1. Matched functions give those names
  directly.
- G keeps MH1's compiler and overlay layout, so the same matching setup should work.
- The Wii G (PowerPC, a different compiler) cannot reuse the matched C byte for byte. Only
  the C's meaning carries over.

## MH2 (Dos), SLPM_662.80 VER 1.04, English patch v1.03 image

Run: `tools/mhg_match.py mh2`. The overlays are stored uncompressed in DATA.BIN; carve
them at each 'MWo3' magic, 0x40 + text + data bytes long.

- Same compiler ("MW MIPS C Compiler (2.4.1.01)"), same MWo3 overlay system, same
  overlay names, plus new ones: gm_sub, plsel, lbguild, and stubs for test.
  The ELF is stripped.
- The game is much bigger. game.bin's text is 1.9 MB (MH1: 1.08 MB) and sub_main's is
  1.1 MB. The main ELF shrank to 0.8 MB because code moved into sub_main.
- Unchanged MH1 functions, by share of code bytes: main 27%, game 10%, lobby 6%, yn 26%.
  Overall about 17%.
- Movies are CRI Sofdec (MWSFD/PS2EE 3.33). MH1 uses plain MPEG-2.
- Verdict: still the same engine family, and the tools, compiler setup, file formats and
  PC/Xbox platform layer carry over. Most of the game code is new or rewritten, so MH2
  would be close to a new decompilation that has MH1 to lean on.
