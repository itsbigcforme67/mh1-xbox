# MH Dos (MH2, JP 1.04): the memory-card save and User_data (SAVE agent, 4 Nov 2026)

- **Source:** our disassembly of the owner's disc (asm/mh2, mostly the sub_main overlay that is carved from DATA.BIN, and the
  select overlay), our G/MH1 C (src/main/mc, src/main/ud, src/select/edit00.c), the PC and Xbox card layers (src/pc/rt/rt_mc.c,
  src/pc/xbox/mc_xbox.c). The community mh2-decomp repo was not opened. Nothing was run: no build, no game, no emulator.
- **Tags:** *code* = read from the instruction that does it (address given); *order* = no instruction names it, it follows from
  the order of neighbouring stores in a twin function; **guess** = said as a guess.
- Function names are from our G/MH1 pairing (config/mh2/func_pairs.txt) where one exists, else sub_<addr>. Several pairing names
  are wrong in MH2's card code (MH2 restructured it); they are used here only as labels.
- The only Capcom strings in this file are the save folder / file names the port's code has to use.

## 1. Summary (plain words)

MH2 does **not** save one big file like MH1 and G. Its card folder holds seven files: the icon pair, one small "system" file
(options, chat phrases, a few flag words), **one file per hunter** (three hunter slots), and the online patch. Each data file
starts with a 64-byte header of random bytes that carries a version word, the scramble key and a 16-bit checksum; the rest is
scrambled with the same key stream as MH1 and G (`key = key * 0xB0 % 65363`). Nothing ties a save to a console or card.

The PS2 folder name is **BISLPM-66280MH2**. It is in the sub_main overlay's data (0x4B05C0), which is why the earlier search of
SLPM_662.80 itself found no "BISLPM" string (docs/mh2/xbox.md section 2 said so; that line can now be closed).

At power-on MH2 also looks on both cards for an **MH1 or G save folder** (existence only) and, if one is there, gives every new
hunter a starting bonus (section 6).

## 2. The card folder (mc_file_tbl[0], 0x468850, *code*)

MH2's MCFILE is not MH1's. It is: +0x00 title line 1 pointer, +0x04 title line 2 pointer (the icon.sys titles, Shift-JIS at
0x48AD10 / 0x48AD31), then **9 file records of 16 bytes** from +0x08, then +0xA0 the space needed in KB (filled at run time by
McActAvailSet 0x3E0740; the table holds 0x7FFFFFFF). A record is `{ s32 type; char *name; u8 *data (set at run time); s32 size }`;
type 0 = no file, 1 = icon (skipped on a save when it exists with the right size), 2 = data.

| rec | file | size | what (section) |
|---|---|---|---|
| 0 | `icon.sys` | 0x3C4 (964) | PS2 icon header, built by mc_icon_sys_set (0x3E0890) from the two title pointers |
| 1 | `icon00.ico` | 0x84B8 in the table; the real size comes from the icon pack (disc file 0x1714, loaded by card_data_init) | the 3D icon |
| 2, 3 | (type 0) | | unused icon slots (copy / delete icons are icon00.ico too, **guess** from icon.sys practice) |
| 4 | `BISLPM-66280MH2` (named like the folder) | 0x550 | system file (3.1) |
| 5..7 | `play00.bin`, `play01.bin`, `play02.bin` | 0x8050 each | hunter slot 0..2 (3.2) |
| 8 | `patch.bin` | 0x10080 | the online patch (3.3) |

Space needed (McActAvailSet's rule: sum of ceil(size / 1 KB) of the present files + (files + 1) / 2 + 2): with the table's icon
size 1 + 34 + 2 + 3 x 33 + 65 + 4 + 2 = **207 KB** (the real icon pack can change the 34). MH1's folder is about 108 KB.

The other tables (same layout, *code*):
- mc_file_tbl[1] 0x468900: the shared Capcom network-settings folder `BISLPM-65286NET` (icons icon_20/21/22.ico, a 0x1BEC
  file named like the folder). MH1 and G have the same folder; it is not hunter data.
- mc_file_tbl[2] 0x4689B0: dir `BISLPM-65495MH` (MH1), every file record type 0 except rec 4's name; [3] 0x468A60: dir
  `BISLPM-65869MHG` (G). Used only by the power-on check in section 6.

**How a save writes** (mc_act_save, 0x3DFD60, *code*): for each record 0..8 with a type: path = `<dir>/<name>` (rec 4 uses the
folder name), look the file up; MemcardWork+0xE4 is a "leave it alone if it already exists" bit mask; an icon that exists with
the right size is left; a missing file is created at its table size; then it is written. sub_3E0160(n) sets up a save of file n
(the other data files get their "leave alone" bit), sub_3DFC90 one of all five data files at once (pointers laid out at 64-byte
rounded sizes, see 4). So a bed save rewrites icon.sys and one or two data files, and creates any data file that is missing.

## 3. The file formats

### 3.0 The 64-byte header and the scramble (encode sub_3D6580, decode sub_3D6710, *code*)

Encode (the file kind is card_w+0x88: 1 system, 2/4/8 hunter slot 0/1/2, 0x10 patch):
1. Fill bytes 0x00..0x3F with ran_suu(0) (one call per byte).
2. u16 +0x00 = version: **0x100** for the system file, **1** for hunter and patch files. u16 +0x04 = 0 (checksum). u16 +0x02 keeps
   its random value: it is the key seed. Bytes +0x06..+0x3F stay random (MH1's 0x5963 marker is gone).
3. For each u16 word from +0x40 to the end of the file ((size - 0x40) / 2 words: 0x288 system, 0x4008 hunter, 0x8020 patch):
   checksum += word (of the plain word); word ^= key; if key == 0 then key = 1; key = key * 0xB0 % 65363.

Decode checks the version word (the right one for the kind) and recomputes the 16-bit sum of the unscrambled words; result flags
at card_w+0x10 (bad version) and +0x12 (bad sum). decode_to_ck (sub_3D6930) returns non-zero if either is set.

Within the scrambled part every data file starts the same way: **+0x40 8 bytes: the save time** (sub_3E0840 reads the clock into
it; MH1's McReadClock), +0x48..+0x4F not written (zero). The time is also kept in card_w+0xA8 / +0xAC on save and compared with
the card on the next save (sub_3D6860 / sub_3D68F0). **Guess:** as MH1's check_sum_ck, a "same card, nobody saved in between"
check.

### 3.1 System file `BISLPM-66280MH2`, 0x550 bytes (sub_3DDC40: argument 0 = load, 1 = save; *code*)

| file + | size | option_w (0x6A9E50) + | notes |
|---|---|---|---|
| 0x00 | 0x40 | | header (3.0) |
| 0x40 | 8 | | save time |
| 0x50 | 0x10 | 0x00 | options block 1 |
| 0x60 | 0x10 | 0x10 | options block 2 |
| 0x70 | 4 | 0x458 | flag word (OR-merged in the "options only" path sub_3D6990 / sub_3D6B00) |
| 0x74, 0x75 | 1 + 1 | 0x45C, 0x45D | |
| 0x76 | 4 | 0x45E..0x461 | four flag bytes (OR-merged in the options-only path) |
| 0x7A | 1 | 0x3C5E and 0x6ADFD6 | the old-save bonus byte (section 6): 0 none, 1 MH1, 2 G. Load copies it into 0x6ADFD6 only when non-zero |
| 0x80 | 0x10 | 0x3C64 | two pairs of u32 (OR-merged in the options-only path) |
| 0x90 | 0x438 | 0x20 | chat phrases (Init_reibun runs after the load; MH1 kept 0x21C of them) |
| 0x4C8..0x54F | | | zero (save clears 0x50..0x54F first) |

After a load: Init_reibun, then sub_36B050 (**guess:** MH1's system_w_set). option_w+0x464 is a 0x37F8 hunter-sized buffer
(user_data_copy's target when the slot is not 0xFF; cleared by sub_3D6DC0).

### 3.2 Hunter file `play0N.bin`, 0x8050 bytes (sub_3DE190: load / save of slot N; *code*)

| file + | size | what |
|---|---|---|
| 0x00 | 0x40 | header (3.0), version 1 |
| 0x40 | 8 | save time |
| 0x50 | 0x37F8 | the hunter: one User_data (section 5) |
| 0x3848..0x804F | | zero (save clears 0x8000 bytes from +0x50 first): room the game never uses |

The three slots live in a 3 x 0x37F8 array at data_load_ptr + 0x60800 (pointer in card_w+0x98 and in 0x6ADAB0). On load
(0x3DC2E0) a slot that fails decode is cleared and marked 0x80 in card_w+0x9D+N; a good one is copied in and marked 1; 0 = no
file. A slot is "in use" when its byte 0 is non-zero (0x3DAA58). Saving the hunter in play: memcpy(slot[0x6ADC90], User_data,
0x37F8) at 0x3DB320 (also 0x3DC944 with the slot from 0x6ADB51). **Not found in this pass:** the copy slot -> User_data on
Continue (no memcpy of 0x37F8 into 0x5D4A60 with an immediate address; it probably takes the size or address from a register).

### 3.3 Patch file `patch.bin`, 0x10080 bytes (sub_3D6CF0 / sub_3D6C50, *code*)

+0x40: 0x40 bytes <-> 0xAC6780; +0x80: 0x10000 bytes <-> 0xAA6780 (the patch buffer; config/mh2/data_map.txt's patch_buff
0x4A0240 "medium" is not what this code uses). The online DNAS patch: the PC and Xbox ports have no use for its content, but the
game expects the file to exist (it is created on the first save).

## 4. RAM side (for the port's C)

| what | address | size / fields | how known |
|---|---|---|---|
| card_w (save screens) | 0x6AEF90 | +0x04 timer, +0x18 port, +0x34 state (jtbl 0x4B0520), +0x40 result per port, +0x48 space needed, +0x54 patch present, +0x5C message, +0x60/+0x64 time read, +0x88 file kind (1/2/4/8/0x10), +0x8C icon buffer, +0x90 file images, +0x94 scratch / directory buffer, +0x98 hunter slots, +0x9D..+0x9F slot state, +0xA2 slot, +0xA8/+0xAC time written, +0xB1 port the old save was on | code (McCardOperation 0x3DD8xx uses the symbol card_w) |
| MemcardWork | 0x6AF100 | 0xF4: +0x00 step, +0x08 file record, +0x14 port, +0x24 result, +0x50 folder name, +0x72 path, +0xB8 record type, +0xBC size, +0xC4 write flag, +0xCC action (2 load, 3 save, 4 save all), +0xD0 buffer, +0xD4 table index, +0xE0, +0xE4 leave-alone mask | code (McAct* 0x3DF5A0..0x3E0830) |
| data_load_ptr buffer | | 0x6AFE8 bytes cleared by card_data_init: +0 icon pack, +0x20000 file images (system at 0, slot N at 0x580 + N * 0x8080, patch at 0x18700), +0x40400 scratch, +0x60800 hunter slots (0xA7E8) | code (card_data_init 0x3D6F50, sub_3DDBE0) |
| option_w | 0x6A9E50 | see 3.1; at least 0x3C74 bytes | code |
| User_data | 0x5D4A60 | 0x37F8 | code (memsets / copies of that size) |

## 5. User_data (0x5D4A60, 0x37F8 bytes): MH2 offsets, G and MH1 for comparison

New-hunter set-up is user_data_copy (select 0x6AF3D0, MH1 src/select/edit00.c): the store order there pins several fields.

| MH2 + | type | field | G / MH1 | how known |
|---|---|---|---|---|
| 0x000 | u8 | in use (1) | same | code (user_data_copy; slot test 0x3DAA58) |
| 0x001, 0x002, 0x003 | u8 | creation choices (edit_w+4, +5, +7; +7 = 0x11 becomes 0x13) | same | code; meanings **guess** (sex, face, hair as MH1's notes) |
| 0x004 | u32 | edit_w+8 | same | code |
| 0x008 | 18 bytes | hunter name | same | code |
| 0x01A | | MH1 sets s16 0x7D00 here; MH2's user_data_copy does not | | code (absence) |
| 0x05C | u32 | hunter points (Get_hunter_rank) | G/MH1 0x1C | code |
| **0x060** | s32 | **money** (sub_3B6500: clamp 0..9,999,999, unlock 0x1E at 2,000,000; 24 callers: shops, quest fee, rewards). New bonus hunters get 3000 | G/MH1 **0x20** | code |
| 0x064 | u16[0x2C] | event flags (Event_flag_set) | 0x24 | code; count = room |
| 0x0BC | 12 x 500 | warehouse equipment | G 0x44 (6 x 300) | code |
| 0x182C | 4 x 500 | item box | G 0x74C | code |
| 0x1FFC | u32[16] | quest clear bits (index list 0x44C3A0) | G 0xBFC | code |
| 0x203C | u32 | play time (+1 every 30 ticks, village 0x7C9C50) | G 0xC1C, MH1 0x374 | code |
| 0x2040 | 4 x 20 | pouch | G 0xC24 | code |
| 0x2090 | 12 x 6 | worn: weapon, then leg, head, body, arm, waist; +0 use, +1 kind, +2 id, +4 option | G 0xC74..0xC7E | code; creation sets kind bytes 0x209D = 0, 0x20A9 = 2, 0x20B5 = 3, 0x20C1 = 4, 0x20CD = 5 |
| 0x20D8 | u8 | creation choice edit_w+6 | MH1 0x3D7 | code (same place in both user_data_copy) |
| 0x20D9 | u8 | set to 1 at creation | MH1 0x3ED (**order**: the "= 1" store after the 0xFF fills in both) | order |
| 0x2186 | u16[6] | warehouse index of each worn piece (0xFFFF none) | G 0xCFE, MH1 u8 0x456 | code |
| 0x2192 | u16[32] | 32 random values (ran_suu(1)) at creation. Meaning **unknown** | (ud.h's `wyv_kill` here is doubtful) | code |
| 0x21D2 / 0x22D2 | u16[128] | biggest / smallest size record per kind (74 used) | | code (result_screen.md) |
| 0x25E4 | 0x20 x 10 | ten equipment sets: 6 x u16 index (0xFFFF), then a 20-byte name made by sprintf at +0xC | | code (creation); "equipment set" is a **guess** |
| 0x2724 | u16 | counter, +1 (max 200) after quests 20007-20010 | | code (result_screen.md) |
| 0x2726 | u16 | hunter rank (Get_hunter_rank) | G 0xC23, MH1 0x37B (u8) | code |
| 0x2728, 0x2729, 0x2730 | u8 | season, day in season, a third clock copy | | code (village.md) |
| 0x27B4 | u16 | title bits | | code (result_screen.md) |
| 0x27B7.. | | five 16-byte rings of hunted kinds (**guess**: recent hunts) | | code |
| 0x2928 | 0x1C x 3 | three records, purpose **unknown** (sub_3A0040) | | code |
| **0x2984** | s32 | a **second** counter, clamp 0..9,999,999, no unlock: added once, by the result screen's score sheet (plsel 0xD6CB28, quests with flag 0x200000). Read by sub_3BD3E0. **Not money** | | code |
| 0x2994 | u8 | creation choice edit_w+0x47 | | code; meaning unknown |
| 0x2CCC | u16 | compared with 100 for unlock 0x63 | | code |
| 0x3660 | bits | unlock bits (sub_3BD550 / 3BD560) | | code |
| 0x369A, 0x36A2, 0x36AA.. | 8 bytes each | read by the village (lobby 0x7B9CE0) into 0x8BECA4.. | | code; meaning unknown |
| 0x36E8 | u8 | picks the village area at entry (0x57 / 0x89) | | code (village.md) |

Everything not in the table is unknown. G-only fields (kills 0xD76, box-expansion byte 0xC97) have no MH2 place found: MH2 sizes
the box and warehouse from event flags 621..624 (sub_3BC650, include/ud.h).

### 5.1 UDMO entries that are wrong or doubtful (include/mh2_lay_ofs.h, written by tools/mh2_structs.py)

Checked against the MH2 code above. Right: 0x01, 0x08, 0x1C -> 0x5C, 0x24.. -> 0x64, 0x44 -> 0xBC, 0x1C4 / 0x74C -> 0x182C,
0x354 / 0xBFC -> 0x1FFC, 0x374 / 0xC1C -> 0x203C, 0x37B / 0xC23 -> 0x2726, 0x37C / 0xC24 -> 0x2040, 0xC75 -> 0x2091,
0xC76 -> 0x2092, 0x3D2.. / 0xC7A.. -> 0x209E + 12 i, 0x456 and G 0xCFE.. -> 0x2186.

| UDMO(o) | gives | should be | why | used by |
|---|---|---|---|---|
| 0x20..0x23 (money) | 0x2984 | **0x60** | 0x2984 is the score-sheet counter (5). Every money read through UDMO(0x20) reads the wrong counter | rt_dbg.c, rt_flow.c, lb_guild_g.c, f_reward2.c, status_g.c |
| 0x3D7 | 0x20DA | 0x20D8 | the creation byte (5) | none today |
| 0x3ED | 0x20F0 | 0x20D9 (**order**) | the "= 1 at creation" byte | omake.c (2 uses) |
| 0x457..0x45B (MH1 u8 widx) | 0x2187.. | 0x2188 + 2 (o - 0x457) | MH2's indexes are u16; only 0x456 and the G form are right | none seen |
| 0x45C / G 0xD0A (wyv_kill) | 0x2192 | unknown | 0x2192.. holds 32 random words at creation | f_reward_g.c (0xD0A) |
| G 0xC97 (box expansion) | 0x20F2 | none | MH2 uses event flags 621..624 | lb_ib.c, lbshop2_nm.c, lbshop2b.c |
| G 0xD76 (kills), 0xDDC, 0xDE8, 0xDEC | 0x21FE, 0x2264, 0x2270, 0x2274 | unknown | these land inside the biggest-size records (0x21D2..0x22D1) | f_reward_g.c, gn_sub_main_func_004C8F90.c |
| G 0xD10 | 0x2198 | unknown | inside the random block | menu_disp_nm.c |

Also: UDMO takes MH1 offsets below 0x74C and G offsets from 0x74C up. G offsets between 0x44 and 0x74C (G's warehouse) go through
the MH1 rules; that happens to be right for 0x44 only.

### 5.2 include/ud.h and f_ud.c on MH2

- ud.h's UDW names 0x2984 `gold` and 0x60 `x60`. Swap the meaning: 0x60 is money. Shared `Gold_add` (f_ud.c, byte-matched at
  MH2 0x3BD380) is the 0x2984 adder; MH2's money adder is sub_3B6500 (already called `Gold_add_mh2_003B6500` in
  config/mh2/c_symfix.txt). PC host code that pays or charges money on MH2 must call that one (with its unlock 0x1E), not
  Gold_add.
- `wyv_kill` at 0x2192 and `x374` at 0x203C (play time, now code) can be updated as in the table.

## 6. The old-save bonus (CardAtld at power-on, sub_3D7EE0 / sub_3D7F50 / sub_3D8080, *code*)

McActInit(2) (MH1's table) and a probe of `BISLPM-65495MH/BISLPM-65495MH` on port 0, then port 1; if found, 0x6ADFD6 = 1. If
not, McActInit(3) and the same for `BISLPM-65869MHG/BISLPM-65869MHG`: found -> 2. Only the file's existence is tested (the action
is a save set-up with its write flag 0, which stops after the look-up). The byte is saved in the system file (+0x7A). When it is
non-zero, user_data_copy gives the new hunter seven item stacks, 3000 money and event flag 0x77 (select 0x6AF5B0..0x6AF634).

## 7. What the PC and Xbox save layers must change for MH2

Today the PC MH2 build is VERSION_MH2 + VERSION_MHG, so every save-related `#ifdef VERSION_MHG` takes G's values:

1. **Card code.** The PC links MH1/G's card C (mclow_nm.c, mcact_nm.c, mccomb.c in MH1 layout; `MC_GL` is PS2-only) and MH1's
   MCFILE (include/mcw.h: data file first, 5 records, blocks at +0x58). MH2 needs its own: 9 records, blocks at +0xA0, the
   per-file save loop (2), the 64-byte header encode/decode (3.0), the three pack functions (3.1-3.3) and the slot array. Best as
   `#ifdef VERSION_MH2` host C written from the functions named here (sub_3D6580, 3D6710, 3DDC40, 3DE190, 3D6CF0, 3DFD60,
   3E0160, 3DFC90, 3E0740), or the McAct layer (0x3DF5A0..0x3E0830) and the card screens as MH2 asm-in-C. If the PC imports
   MH2's mc_file_tbl from the disc (rt_data.c imports tables by symbol), the MH1-layout McAct reads it wrongly: check before the
   first save. Today title_mh2.c skips the card check, so nothing has been written yet.
2. **No libmc change.** rt_mc.c and mc_xbox.c are file-level and already handle several files in one folder, creation by size,
   directory listings and missing files. MH2 needs no new sceMc call (the same set is called: Open, Read, Write, Close, Mkdir,
   GetDir, GetInfo, Delete, SetFileInfo; not re-checked call by call).
3. **PC folder.** rt_mc.c APPDIR is "mhgpc" for MH2 (VERSION_MHG). Give MH2 "mh2pc" (`#ifdef VERSION_MH2` first), so its
   card folder is its own. (Side effect today: MH2 would find G's real save beside it and set the bonus to 2.)
4. **Xbox identity.** xbox_title.h: an `#if defined(VERSION_MH2)` block before the MHG one with `PS2_SAVE_DIR "BISLPM-66280MH2"`,
   title id 4D480003, save id 4D48000300000001 (docs/mh2/xbox.md 4). host_path maps only PS2_SAVE_DIR to the save folder; that
   is enough.
5. **The bonus probe on the Xbox / PC** (optional, nice to have): MH2 looks for `BISLPM-65495MH/BISLPM-65495MH` and
   `BISLPM-65869MHG/BISLPM-65869MHG`. On the Xbox those paths map to E:\UDATA\4D480003\BISLPM-65495MH\..., which never exists.
   Mapping them read-only to E:\UDATA\4D480001\4D48000100000001\BISLPM-65495MH and E:\UDATA\4D480002\4D48000200000001\
   BISLPM-65869MHG (on the PC: mh1pc/memcard0 and mhgpc/memcard0) gives owners of our MH1 / G ports the bonus, as a PS2 card
   holding both saves would. Only existence is tested, so read-only is safe. Owner's choice.
6. **mc_xbox.c log line:** money `*(int *)(User_data + 0x20)` -> +0x60, UD_WEAR 0xC75 -> 0x2091 (id at 0x2092) under
   VERSION_MH2.
7. **rt_np.c "the player's own save"** (co-op hunter from the card): SAVE_FILE / SAVE_SIZE / SAVE_WORDS / SAVE_SLOT are G's.
   MH2: read `BISLPM-66280MH2/play0N.bin` (0x8050), decode with the 3.0 header (key at +2, sum at +4, words from +0x40), the
   hunter is at +0x50, 0x37F8 bytes; the slot is the file, not an offset in option_w.
8. **rt_save.c / ps2save.c (import / export of real PS2 saves):** RT_SAVE_DIR is MH1's for every version and
   ps2s_check_mh1_data tests MH1's header. For MH2: the folder above, seven files, and a per-file check (version 0x100 for the
   system file, 1 for the others; the sum). The containers (psu, max, cbs, sps, card image) need no change.
9. **Gold** (5.2) and the UDMO fixes (5.1) before any host code charges, pays or shows money on MH2.
10. Lesson from G (rt_np.c note): the PC's G image is encoded over MH1's 0x8A20 words, not G's 0x9860, so PC G saves are not
    byte-compatible with PS2 G. For MH2 keep the PS2 format exactly, so real PS2 saves import and our saves go back to a PS2.

## 8. Test plan

Host-only first (no game run, cheap on the laptop):
1. A stdlib Python or C test of the 3.0 scramble: encode a buffer of known words with a fixed header, decode, check the version,
   sum and words; flip one byte and see the sum flag; check the word counts 0x288 / 0x4008 / 0x8020.
2. **The real test of this reading:** a real MH2 PS2 save (the owner's card dump or a .psu/.max of BISLPM-66280MH2, if the owner
   has one; not in disc/). Unwrap it with ps2save.c, then decode each of the five data files: every one should pass (system
   version 0x100, others 1, sums equal). Then: play0N +0x50 byte 0 = 1 for used slots, the name at +0x58, money at +0x50+0x60
   matching the game's screen, file sizes exactly 0x550 / 0x8050 / 0x10080, icon.sys 964 bytes. Any failure means a mistake here.

In the PC game (hidden runs, MH1_SAVE_DIR pointing at a scratch folder):
3. New game -> character creation -> save: the folder BISLPM-66280MH2 with icon.sys, icon00.ico, BISLPM-66280MH2, play00.bin
   (and play01/02 and patch.bin created at their sizes); all decode; play00 +0x50 is User_data byte for byte (dump User_data
   with RT_DBG and compare).
4. Continue: the slot comes back (money +0x60, pouch 0x2040, worn weapon 0x2091/0x2092, rank 0x2726, play time 0x203C).
   A second hunter in slot 1 does not touch play00.bin (its time stamp and bytes unchanged).
5. Damage play01.bin by one byte: that slot shows as broken (state 0x80), the others load.
6. Bonus: with an MH1 or G save folder in the card folder, the new hunter starts with 3000 money and the seven stacks; without,
   0 money. Check the system file's +0x7A (1 / 2 / 0).
7. Options: change an option and a chat phrase, save, restart: they come back (system file 0x50.., 0x90..).

On the Xbox (xemu, then the owner): step 3 and 4 with E:\UDATA\4D480003\4D48000300000001\ holding the seven files; the
dashboard shows the save; MH1's and G's saves on the same disk are untouched. Power the Xbox off after bench work.

## 9. Open

- Where Continue copies a slot into User_data (3.2).
- The meaning of 0x2984 (score sheet points), 0x2192 (random words), 0x2928, 0x25E4 (equipment sets?), the creation bytes.
- The system file's flag words (option_w 0x458..0x461, 0x3C64..0x3C73): which unlocks they are.
- The exact sceMc call list of MH2's mclow (mc_check_card .. mc_get_dir, just before McActInit 0x3DF5A0) against rt_mc.c's results.
- The icon pack (disc file 0x1714) layout beyond "count at +4, then offset / size pairs" (McActAvailSet).
