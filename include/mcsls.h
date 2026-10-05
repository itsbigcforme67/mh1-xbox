/* mcsls.h - session layer that synchronises the players of an online game (SLPM_654.95 0x230760-0x232C50).
 * Layout derived from the field accesses of the mcsls functions; names are guesses from their use. */
#ifndef MCSLS_H
#define MCSLS_H
#include "types.h"

/* CCnNetMsg byte queue (0x18 bytes) */
typedef struct CNMSG {
    s32 x00;
    s32 rd;             /* 0x04 read position */
    u8 *buf;            /* 0x08 */
    s32 size;           /* 0x0C bytes stored */
    s16 cap;            /* 0x10 */
    u8 pad12[6];
} CNMSG;

/* CCnNetMsg with 10 more bytes of state (stack temporaries in mcsls_recv) */
typedef struct CNMSGB {
    CNMSG m;
    u8 x18[10];
} CNMSGB;

typedef struct MCSPL {
    u8 alive;           /* 0x00 (+0x4C) 0 gone, 1 alive, 2 leaving */
    u8 sync;            /* 0x01 sync flags received (bit n = flag n) */
    u8 sync_lv;         /* 0x02 */
    u8 pad03;
    s32 nrecv;          /* 0x04 packets received from this player */
    f32 t_recv;         /* 0x08 time of the last packet */
    s32 sent;           /* 0x0C app data packets queued */
    u8 ping_id;         /* 0x10 */
    u8 pad11;
    u16 ping[8];        /* 0x12 round trip times in ms */
    u16 ping_min;       /* 0x22 */
    u16 ping_max;       /* 0x24 */
    u16 ping_ave;       /* 0x26 */
    s32 stock;          /* 0x28 app packets waiting in mcs_recv_que */
    s32 nfrag;          /* 0x2C fragments of the packet being assembled */
    s32 fragsize;       /* 0x30 */
    s32 npull;          /* 0x34 packets consumed */
    u8 pad38[4];
} MCSPL;

typedef struct MCSLS {
    s32 sock;           /* 0x00 */
    u8 me;              /* 0x04 own player number */
    u8 num;             /* 0x05 players */
    u8 state;           /* 0x06 */
    u8 x07;             /* 0x07 step inside a state */
    s8 x08;
    s8 x09;
    s16 x0A;
    s16 x0C;
    u8 master;          /* 0x0E */
    u8 alive;           /* 0x0F players alive */
    f32 f10;            /* 0x10 */
    u8 pad14[8];
    f32 time;           /* 0x1C */
    f32 t_send;         /* 0x20 time of the last send */
    f32 t_que;          /* 0x24 time since the last app queue flush */
    u8 pad28[4];
    u8 sync_need;       /* 0x2C */
    u8 ping_cur;        /* 0x2D */
    u8 pad2E[2];
    s32 x30;            /* 0x30 */
    s32 napp;           /* 0x34 app messages pushed */
    s32 nsent;          /* 0x38 bytes sent */
    u8 pad3C[2];
    u16 nsent16;        /* 0x3E */
    s32 nrecv;          /* 0x40 bytes received */
    u8 pad44[2];
    u16 nrecv16;        /* 0x46 */
    u8 pad48[4];
    MCSPL pl[4];        /* 0x4C */
    u8 pad13C[8];
    f32 f144;           /* 0x144 */
    f32 f148;           /* 0x148 */
    u32 i14C;           /* 0x14C */
    u8 x150;            /* 0x150 */
    u8 x151;            /* 0x151 */
    u8 pad152[2];
    s32 err;            /* 0x154 */
    s32 err_a;          /* 0x158 */
    s32 err_b;          /* 0x15C */
    s32 code;           /* 0x160 */
    s32 crit;           /* 0x164 critical error code */
    u16 x168;
    u8 pad16A[2];
} MCSLS;

extern MCSLS mcsls_w;
extern CNMSG tcp_send_buff;
extern CNMSG tcp_recv_buff;
extern CNMSG app_send_que;
extern CNMSG app_recv_que;
extern CNMSG mcs_recv_que[];
extern s32 MCSLS_SWIN_LIMIT;
extern f32 DROP_TIMEOUT;
extern s32 send_health_ans;

#endif
