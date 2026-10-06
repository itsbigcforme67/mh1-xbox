#ifndef NETCW_H
#define NETCW_H
/* net_common_w (0x3A6E90, 0x94 bytes): work of the network menu step machines (f_network_work_init,
 * f_ms, f_ncm). Offsets from matched code; names are guesses. */
#include "types.h"

typedef struct NETCW {
    u8 x00;             /* 0x00 menu state (ms_network_sub dispatches on it) */
    u8 step;            /* 0x01 step within the state */
    u8 sub;             /* 0x02 sub step */
    u8 x03;
    s16 timer;          /* 0x04 */
    s16 x06;            /* 0x06 */
    s16 x08;            /* 0x08 menu cursor */
    s16 x0A;            /* 0x0A */
    u8 x0C;
    u8 x0D;             /* 0x0D selected drive */
    u8 x0E;             /* 0x0E bit 0: skip Net_work_move, bit 1: skip Net_trans_set */
    u8 pad0F;
    u8 x10;
    u8 x11;
    u8 x12;
    u8 x13;
    u8 x14;
    u8 x15;
    u8 pad16[0x28 - 0x16];
    u8 x28;
    u8 x29;             /* 0x29 yes/no cursor (0 yes) */
    u8 pad2A[2];
    s32 x2C;            /* 0x2C sprite request mask (Ncm_spr_*) */
    u8 x30;
    u8 pad31;
    s8 ia;              /* 0x32 request list a count */
    s8 ib;              /* 0x33 request list b count */
    u8 x34;
    u8 x35;
    u8 x36;
    u8 x37;
    u8 x38;
    u8 x39;
    u8 x3A;
    u8 x3B;
    u8 x3C;
    u8 x3D;
    u8 x3E;
    u8 x3F;
    u8 qa[8];           /* 0x40 request list a: ids */
    u8 ka[8];           /* 0x48 kinds */
    u8 qb[8];           /* 0x50 request list b: ids */
    u8 kb[8];           /* 0x58 kinds */
    u8 pad60[0x79 - 0x60];
    s8 x79;
    s16 x7A;            /* 0x7A McActAvailSet result */
    s8 x7C;             /* 0x7C dialog state (0x64 open, 0x6E close) */
    s8 x7D;             /* 0x7D dialog kind */
    s8 x7E;             /* 0x7E patch menu: message index base */
    s8 x7F;             /* 0x7F patch menu: 0 = new patch, 1 = other message */
    s32 x80;
    s32 x84;            /* 0x84 patch size */
    u8 pad88;
    u8 x89;             /* 0x89 ms_net_patch_set state (u8: lbu) */
    u8 x8A;             /* 0x8A its sub step (u8: lbu) */
    s8 x8B;             /* 0x8B */
    s8 x8C;             /* 0x8C */
    char x8D[5];        /* 0x8D remaining-time digits copy (dialog_limit_disp) */
    u8 pad92[0x94 - 0x92];
} NETCW;
extern NETCW net_common_w;
#endif
