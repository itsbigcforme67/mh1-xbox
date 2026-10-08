# PS2 save containers (importing a real Monster Hunter save)

Code: `src/pc/fmt/ps2save.c` (+ `lzari.c`), host side `src/pc/rt/rt_save.c`, tests `tools/test_save.sh`.
Written by agent C, 8 Oct 2026. No Python or tool is needed at run time; all containers are read and
written in C.

## What the game itself saves

The game's save is one directory on the card, `BISLPM-65495MH`, with three files (mc_file_tbl[0],
src/main/mc/mccomb.c, src/pc/rt/rt_mc.c keeps them byte for byte in the PC card folder):

| file | size | content |
|---|---|---|
| `BISLPM-65495MH` | 0x11450 (70736) | the save data (hunter slots, items, money, options), scrambled by `encode_data` |
| `icon.sys` | 964 | PS2 icon header (title lines, icon file names, lights) |
| `icon00.ico` | 35776 | the 3D memory card icon |

### Is the data bound to a console or card?

No. `encode_data` / `decode_data` (mccomb.c) write and check: u16 version `0x100`, u16 key seed
(random, `ran_suu`), u16 checksum, u16 `0x5963`, then 0x8A20 u16 words XORed with a key stream
(`key = key * 0xB0 % 65363`, a key of 0 restarts at 1). The checksum is the 16-bit sum of the plain
words. Load fails ("corrupt") if the version word is not 0x100 or the sum differs. There is no
memory card serial, console id or DNAS id in it (grepped src/ and include/ for sceCdReadModelID,
sceCdRI, serial, ConsoleID: nothing), so a save from one console works on any other, and on the PC.
`ps2s_check_mh1_data` repeats exactly this test; the importer refuses a file that fails it and
the exporter will not write one. `check_sum_ck` (the load screen's summary compare) only compares
the time stamp the game itself wrote into the data, so it holds for an imported save too.

The icon files are not read when loading; only the data file is. They are kept as imported so a
save that goes back to a PS2 still shows its icon. (On the PC the icon files in the card folder
are written by the game from the disc data at the first save.)

## Directory entry and time (shared by every format except MAX)

A card directory entry is 512 bytes (mymc `ps2mc_dir.py`, PCSX2 docs):

```
0x00 u16 mode      0x8427 directory, 0x8497 file (bits: 1 R, 2 W, 4 X, 0x10 file, 0x20 dir,
                   0x80, 0x400, 0x2000 hidden, 0x8000 exists)
0x02 u16 unused
0x04 u32 length    file: bytes; directory: number of entries (includes . and ..)
0x08 u8[8] created
0x10 u32 first cluster of the data / directory chain (card only; 0 in PSU)
0x14 u32 entry number in the parent (card only)
0x18 u8[8] modified
0x20 u32 attributes (unused)
0x24 28 bytes zero
0x40 char name[448]  zero padded (names are at most 31 characters here)
```
Time: `[0]` unused, `[1]` seconds, `[2]` minutes, `[3]` hours, `[4]` day, `[5]` month, `[6..7]`
year (u16). The console's local time (JST on a Japanese console).

## .psu (EMS / uLaunchELF "export save")

No magic. Three directory entries (the save directory with its entry count, `.`, `..`), then per
file one 512-byte entry followed by the data padded to a multiple of 1024 bytes (cluster size).
Detection: the first three entries are directories, the second is named `.`, the third `..`.
Source: mymc `ps2save.py` (`load_ems`, `save_ems`).

## .max (Action Replay MAX)

```
0x00 "Ps2PowerSave" (12)
0x0C u32 crc32 of the whole file with this field zero (standard zlib/IEEE CRC-32)
0x10 char dirname[32]
0x30 char title[32]      ascii title taken from icon.sys (ours: "Monster Hunter")
0x50 u32 compressed length + 4   (some files hold the uncompressed length here; then everything after the header is used)
0x54 u32 number of files
0x58 u32 uncompressed length
0x5C LZARI stream
```
Uncompressed payload: for each file `u32 length, char name[32], data`, then zero padding so that
`(offset + 8)` is a multiple of 16. Time stamps are not stored (the importer uses the current
time). The reader checks the CRC but only reports a mismatch as a note, as the data checksum of
the game catches real damage.

LZARI is Haruhiko Okumura's 1988 compressor: 4096-byte window, match lengths 3..60, literals and
lengths share one adaptive frequency model of 314 symbols, match distances use a second model whose
cumulative frequencies start from `10000/(200+i)`, all coded with a 17-bit arithmetic coder
(`lzari.c`, written from mymc's public domain `lzari.py`). The initial history is 4036 spaces.
No length prefix in the stream. Source: mymc `ps2save.py`, `lzari.py`.

## .cbs (CodeBreaker)

```
0x00 "CFU\0"
0x04 u32 unknown (we write 0x10)
0x08 u32 header length (we write 0x128; at least 124 is accepted)
0x0C u32 uncompressed body length
0x10 u32 file length (the whole file or just the body; both occur, the reader accepts either)
0x14 char dirname[32]
0x34 created[8]   0x3C modified[8]
0x44 u32, 0x48 u32 unknown
0x4C u32 directory mode (0x8427; the reader falls back to it when the bits are wrong)
0x50..0x5B unknown
0x5C char title[hlen-92]
hlen: body
```
The body is RC4-encrypted with a fixed permutation as the starting state (no key schedule; the
256-byte table is in ps2save.c; first output index is 1) and holds a zlib stream. Decrypted body:
per file `created[8] modified[8] u32 size u16 mode u16 u32 u32 char name[32]` (64 bytes), then data.
The writer stores the zlib stream as uncompressed blocks (valid for every inflater, bigger file);
our own inflater (RFC 1951) handles real compressed CBS files. Source: mymc `ps2save.py`
(`load_codebreaker`, `rc4_crypt`, `PS2SAVE_CBS_RC4S`). The unknown fields and the file-length
convention are guesses; the reader does not depend on them.

## .sps / .xps (SharkPort / X-Port)

mymc loads both with one function (its message reads "SharkPort/X-Port"), so they share a layout:

```
"\x0d\0\0\0SharkPortSave" (17)
u32 save type (we write 2 = PS2)
3 strings, each u32 length + bytes: directory name, date stamp (YYYYMMDDHHMMSS), comment
u32 length of the rest
directory header (98 bytes), then per file a 98-byte header + data
4 byte checksum (ignored by readers; we write the sum from mymc's commented sps_check, unverified)
header: u16 length (98, extra bytes skipped), char name[64], u32 length (directory: entries incl . ..),
        8 zero, u16 mode BYTE SWAPPED (0x2784 for 0x8427), 2 zero, created[8], modified[8]
```
`.xps` is accepted by extension and by the same magic. Real X-Port files with a different magic
would be refused ("unrecognised save format"). Unverified for X-Port: we have no sample of one.

## Raw memory card image (.ps2, .mcd, .mc2, .bin, as PCSX2 and mymc/uLaunchELF dumps)

Superblock in page 0 (mymc `ps2mc.py`, PCSX2 docs):
```
0x00 "Sony PS2 Memory Card Format " (28)   0x1C version "1.2.0.0"
0x28 u16 page size 512     0x2A u16 pages per cluster 2   0x2C u16 pages per erase block 16
0x2E u16 0xFF00            0x30 u32 clusters per card 8192
0x34 u32 first allocatable cluster (41)    0x38 u32 allocatable clusters (8135)
0x3C u32 root directory cluster (0)        0x40 u32 good block 1 (1023)   0x44 u32 good block 2 (1022)
0x50 u32[32] indirect FAT cluster list     0xD0 u32[32] bad block list (0xFFFFFFFF)
0x150 u8 card type (2)     0x151 u8 flags (0x2B)
```
Sizes: 8 388 608 bytes without ECC; 8 650 752 (16384 pages x 528) with ECC: each 512-byte page
is followed by 16 spare bytes, 3 ECC bytes per 128 bytes of data (a Hamming code, the algorithm of
mymc `ps2mc_ecc.py`) and zero fill. Clusters are 1024 bytes (2 pages). Cluster numbers in
directory entries and the FAT are relative to the first allocatable cluster; the FAT itself is
reached through the indirect FAT cluster list (superblock) -> indirect cluster of u32 FAT cluster
numbers (absolute) -> FAT cluster of u32 entries. A FAT entry is `next | 0x80000000` for a used
cluster, `0xFFFFFFFF` for the last cluster of a chain, `0x7FFFFFFF` for a free cluster. Directories
are cluster chains of 512-byte entries (`.` first, then `..`, whose first entry holds the number
of entries in its length field). The root is at cluster 0.

The reader follows these chains for the wanted directory only (no ECC check, so an image with
bad ECC still loads); the writer builds a fresh, standard 8 MB card with ECC and the one save in
it (the unused spare erase block 1022 is 0xFF). Mymc+ (`mymcplus`) opened our image, `check()` passed
and it exported the files byte for byte (see Verification). Other card sizes are read when the
superblock is consistent. Exporting into an existing card is not done: export makes a new image; use
mymc / uLaunchELF to copy the .psu into a real card.

## Verification

- `tools/test_save.sh` (sanitizers on): LZARI round trips on random, repetitive and structured data
  of 1..70000 bytes; every container written, detected, read back with identical files and time
  stamps; read-then-write is stable; truncated and bit-flipped containers are survived; import/export
  through the card folder with backups, refusal of damaged, foreign and garbage files.
- Cross-check against an independent implementation (8 Oct 2026, one-off, not in the repo because
  it needs mymc+): files written by this code were loaded by mymc+ 3.0.5 (PSU, MAX, CBS, SPS) and the
  card image was opened and `check()`ed by it, all contents identical; a card, a PSU and a MAX
  (compressed by mymc+'s own LZARI) written by mymc+ were imported by this code, files identical.
  CBS and SPS written by other tools could not be tested (mymc+ cannot write them).
- No real PS2 save has been available. Everything real-world depends on the format notes above;
  the owner's save is the real test.
