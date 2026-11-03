# MH Dos (MH2): struct layouts, what is known and how (STRUCTS agent, 2 Nov 2026)

- **Source:** our disassembly of the owner's disc and our G/MH1 sources. The community mh2-decomp repo was not opened.
- **Proof:** sizes and bases in the table below are proven by a memset or copy in MH2's code. Everything else is marked.

## Tools

- `tools/mh2_structs.py gvotes`: for every global, symbol + offset in our object against the absolute address in the aligned MH2 twin, following a
  register that holds an address. `gshow NAME` prints one. A symbol with two bases is a struct that moved, or a wrong base address. Pairing errors happen: treat one file's vote as a hint.
- `tools/mh2_structs.py layouts`: `config/mh2/layouts.txt` -> `include/mh2_layouts.h`, `include/mh2_lay_ofs.h`. Each field line carries a tag: code, order, guess.
- `QWO`, `UDMO`, `NCO`, `LBM`, `PMO` (include/types.h, identity outside MH2): the MH2 offset of a raw number in quest_w, User_data, net_common_w, lb_sys, PitMenu. Same rule as MH2_OFS: write MH1's number (UDMO also takes G's, from 0x74C up).
- `qno_t`: the quest number in local copies of quest_w's head, u16 in MH2.

## Addresses and sizes proven by a memset or copy call

| what | address | size | evidence |
|---|---|---|---|
| quest_w | 0x5D8260 | 0x560 | Quest_init 0x3A1070 memset. The old 0x5D8248 was a moved field |
| User_data | 0x5D4A60 | 0x37F8 | copy to the card image, 0x3DB328 and others. The old 0x5D73C4 came from gold |
| net_common_w | 0x5D46A0 | 0xE0 | two memsets |
| PitMenu | 0x5BB080 | 0x1420 | memset; the struct in menu.h only reaches its log's end |
| game_w | 0x6ADCB0 | 0x2BC | seven memsets. docs/mh2.md's "guessed" list said 0x2C0 (**Conflict**, kept there with a note) |
| shell_work | 0x5B70D0 | 0x3500 | memset: 64 x 0xD4, which agrees with SHLW 0xD4 if there are 64; the count itself is not proven |
| system_w | 0x6ADF90 | 0xF4 | memset; MH1's is 0x80. Only moved field seen: little, not laid out |
| MemcardWork | 0x6AF100 | 0xF4 | memset; MH1 0xA4 votes for 0xCC, not laid out |
| lb_sys | 0x8BDC10 | 0xF68 | a whole copy; 0xF00 of it from +0x10 is cleared in 16 places |

CNET_SYS's 0x4B000 stays a guess: no memset of the whole. reward_item[48] stays a guess.

## CameraWork: not laid out, MH2 restructured it

Base 0x5AF420. Numbers below are from the base. Not a field move: the slots differ in size, so `CAMS sl[5]` has no MH2 form.
sl[0] 0x80, sl[1] 0x100, sl[2] 0x200, sl[4] 0x300 (votes 0x180, 0x280, 0x4F1 from MH1), then init 0x380, pl 0x384, data 0x388, area 0x38C. Mode work sits further on: 0x3B0 (BBQcamera_set),
0x4B0 (pachinger), 0x530, 0x580. Quakes at 0x610, 0x624, 0x638: 0x14 bytes each, one byte more than MH1 after `time`. reset 0x668, demo_arg 0x66C, demo_req 0x670, zoom 0x672 by order.
Needs the camera C rewritten against this, which is function work.
