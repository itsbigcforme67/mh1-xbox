# MH2 platform notes (PLATFORM agent, 2 Nov 2026)

Host side of MH Dos: format readers, overlays, stage loading, sound, install. Everything here was read from the owner's files
(disc/mh2, the English v1.03 image) with throwaway scripts or seen in hidden runs of build/mh2/pc/mhview_2. "Guess" marks what was
not checked against the game's code.

## Test aids (hidden runs only)

    build/mh2/pc/mhview_2 disc/mh2 --model st01_amh.bin --shot x.png        # one model with its _tex.bin (RT_MODEL_AZ turns)
    build/mh2/pc/mhview_2 disc/mh2 --model q_clear.tex --shot x.png         # the pictures of a .tex pack (RT_TEX_FIRST)
    build/mh2/pc/mhview_2 disc/mh2 --quest 1014 --shot x.png                # start stage + its monsters + a hunter
        RT_QUEST_STAGE=1  the stage of the quest's own monster instead     RT_MIB=n0  another .mib variant (default d0)
        RT_QUEST_CAM=1300 camera distance   RT_QUEST_LOOK_HUNTER=1 look at the hunter   RT_EM_BIND=1 bind pose
    build/mh2/pc/mhview_2 --install ISO --install-dir DIR --shot x.png      # copy DATA.BIN, AFS00-02, SLPM from the image
    RT_STREAM_DIR=DIR build/mh2/pc/mhview_2 disc/mh2 --model adx:N --shot x.wav            # AFS00 entry N as wav
    RT_STREAM_DIR=DIR build/mh2/pc/mhview_2 disc/mh2 --model snd:PACK:PROG:NOTE --shot x.wav # one note of an AFS01 pack

Seen: quest 1 and 12001 on stage 110 (beach, hunter and small monsters), quest 1014 with RT_QUEST_STAGE=1 on stage 120 (Rathian,
kind 1, wings open, hunter beside her). Nothing of the game's code runs in these: they are drawing and format checks.

## Overlays (MWo3 headers of DATA.BIN entry 1)

| slot | address | modules (end incl. .bss) |
|---|---|---|
| A | 0x6AF200 | game.bin (0x8F5D00), lobby.bin (0x8FAE00), select.bin (0x6D3280) |
| B | 0x8FAE00 | gm_sub.bin (0x939600), lbguild.bin (0x924280), nethttp.bin (0x95BE80) |
| | 0x939600 | test.bin (right after gm_sub's end) |
| C | 0xAC8000 | yn.bin, dnas_ins.bin, dnas_net.bin |
| D | 0xD60200 | plsel.bin |
| resident | 0x2E3900 | sub_main.bin (to 0x6AF200) |

Modules in one slot cannot be loaded together; that is read from the addresses, not from the loader (guess for which pairs the game
uses). slot B starts where lobby.bin ends. The PC loads sub_main, game, lobby, select by their setters and gm_sub, plsel as extra
images (rt_add_overlay). The import relocates pointer words inside them in place to host pointers, and rt_is_pointer does not cover
the extra images: rt_va_of_host(host) turns such a word back into its PS2 address (viewer's mh2_word_va uses it).

## Mission files (mib.afs in DATA.BIN, m<no><d|n><0-2>.mib, stored raw)

Header 0x74 bytes of offsets from the file start. Known so far:
- +0x00 info block (0x74); info+0x20 indexes gm_sub's start-stage lists at 0x9316B0.
- +0x04 player start table, 16 bytes a player, first word the stage (quest 1: 110). Positions there are zero in the files looked at.
- File byte 0x5D: non-zero = random start (78 of the d0 files): gm_sub list 0x9316B0[info+0x20], entry 0x5D - 1, is a list of u16
  pairs {weight, stage} ended by 0xFFFF, weights summing to 100 (0x931400-0x931540). The PC takes the first pair.
- +0x14 three pointers (the 0-2 choice, guess) to per-stage entries {stage, ?, kind list, monster list}, ended by stage 0: the small
  monsters.
- +0x18 {count, ?, kind list, monster list}: the quest's own (large) monsters.
- Monster records (0x3C bytes): +0 kind (u8; the u16 0xFFFF ends a list), +4 1 seen, +8 stage (large ones; 0 in per-stage lists),
  +0x1C angle (u16, 65536 = turn), +0x20 x, y, z floats. Kind -> emNN files by number is a guess (MH2's per-kind tables not read).
- +0x24 / +0x40: lists with stage numbers and floats (gathering or area points, guess). The rest not looked at.

## Stage files

st<N>_amh.bin is a 3-entry link file (st<N>_1_amh.bin, the set model, has 2). Entries: AMO, AHI, and a 170-byte block, the same size
in all 194 stage files that have one. Read from the values (guess for each name):

| offset | what |
|---|---|
| +0 | u16 2 |
| +2 | RGBA, fog colour (stage-like: dark blue-grey on st01, white on st06) |
| +6, +0xA | floats: fog near 0, far (60000 st01, 8000 st11) |
| +0xE | two direction vectors (lights 0 and 1) |
| +0x26 | three floats, 0 |
| +0x32 | colours as floats: light 0, light 1, ambient (RGB each), then a fourth RGB (0 on st01, 0.3 0.3 0.4 on st11) |
| +0x62.. | zeros, then at +0x7A a position (-90000, 55000, 90000 on st01: sun, guess), then packed RGBA bytes |

Not used by the PC yet: stages are drawn unlit and without fog. The models in the 0x040C1B00-header AMOs draw with the existing
reader (the word does not change the layout).

## Textures

TIM2 (one picture, version 4): alpha is 0-255 in the files, not doubled. .tex packs: MOMO-style (Meltw, count, offset/size pairs)
read as link files of TIM2 (loading, load_stg, psp_con, q_clear); endroll.tex and gallery.tex are AFS archives of TIM2 files.
Hunter part textures are one .tm2 per part (viewer: PL_TEX_EXT).

## Install and streams

The streams are only on the disc image. Sizes and CRC32 (install.c's table): DATA.BIN 407,248,896 0xD92FBA8C; AFS00 1,186,904,064
0xFF78F33B; AFS01 47,835,136 0x2B8B934C; AFS02 1,077,581,824 0x002E3C25; SLPM_662.80 822,368 0xF276750D; SYSTEM.CNF 77 0x8D41D694.
These are the image named English v1.03; DATA.BIN and SLPM there are byte-identical to disc/mh2 (JP 1.04). Whether an unpatched
JP image has the same three streams is unknown: an owner with that image would be refused by the CRC check.

- AFS00, 309 entries: plain CRI ADX, 48 kHz, stereo, 4-bit (the existing reader). Entry 0 decodes to a steady 1 kHz tone.
- AFS01, 414 entries: 413 MOMO v3 sound packs + 1 "SPUM". v3: MOMO header count 3 (HD, BD, SE table), no TSBD. HD chunks have plain
  4-byte tags (not SCEI's): tag, size, max index, 0xFFFFFFFF, entries.
  - Head (0x20): +8 1, +0xC HD size, +0x10 BD size, +0x14 Prog, +0x18 Smpl, +0x1C Vagi offsets.
  - Prog: max + 1 u32 offsets from the chunk start. Program: 8-byte head {u8 splits, ?, u8 volume, u8 pan, ...} then 14-byte splits
    {?, ?, low note, high note, u16 bend down, u16 bend up (200/400: cents, guess), volume, pan, ?, ?, u16 Smpl index}.
  - Smpl: 12-byte entries {5 bytes ?, base note, volume, pan, 2 bytes ?, u16 Vagi index}.
  - Vagi: 16-byte entries {BD offset, size, flags, rate}; offsets + sizes run on without gaps; rates 12000, 16000, 24000, 44100.
  - SE table "dbsT": 16-byte head {tag, 0x10, size, count}, 32-byte entries (G's TSBD has 16). Not read.
- AFS02, 13 entries: MPEG program streams (0x000001BA), the SFD movies. Not read yet.

## Not done / next

- Sound in a running game: the SE table (dbsT) layout, MH2's sound pack table (which AFS01 entry a stage or monster loads) and
  the BGM table. rt_snd.c still opens AFS00/AFS01 in the disc folder: point it at the install folder.
- Stage light and fog from the third entry; monsters posed by motion ids the game uses; ground height for the hunter.
- Movies (AFS02) through the SFD player.
- User_data uses in src/pc/rt (rt_dbg.c, rt_player.c, rt_flow.c) still read G's offsets on MH2; marked in the code.
