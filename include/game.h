#ifndef GAME_H
#define GAME_H
/* game_w (0x3F33F0, 0x224 bytes): global game/session state.
 * Offsets from matched code: master (Pl_master_ck, pl_sw_set). */
#include "types.h"

typedef struct GAME_W {
    u8 _pad000[0x0D];
    u8 pad_on;          /* 0x00D read controllers this frame (swset) */
    u8 _pad00E[0x14 - 0x0E];
    u8 stage;           /* 0x014 stage number (0x4E, 0x57 in set06) */
    u8 _pad015[0x20 - 0x15];
    u8 port[2];         /* 0x020 controller port per player (get_sw) */
    u8 _pad022[2];
    u8 sw_mask;         /* 0x024 buttons ignored until released (get_sw) */
    u8 _pad025[0xD1 - 0x25];
    u8 master;          /* 0x0D1 player number of the session master */
    u8 _pad0D2[0x1B2 - 0xD2];
    u8 x1B2;            /* 0x1B2 set05: kind-2 fixtures fire once set */
    u8 flag1B3;         /* 0x1B3 bit 0 hides set04 on stage 28 */
    u8 _pad1B4[0x1DE - 0x1B4];
    u8 info_seq;        /* 0x1DE set01 message sequence number (7 bits) */
    u8 info_now;        /* 0x1DF set01 message being shown, 0xFF = none */
    u8 _pad1E0[0x1E6 - 0x1E0];
    u8 gate_open;       /* 0x1E6 set20 gate state, set from the quest */
    u8 _pad1E7[0x210 - 0x1E7];
    u8 shl10_num;       /* 0x210 live shell10s owned by the master player */
    u8 _pad211[0x21F - 0x211];
    u8 info_stop;       /* 0x21F set01 queue paused while set */
    u8 _pad220[0x224 - 0x220];
} GAME_W;

extern GAME_W game_w;

#endif
