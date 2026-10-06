#ifndef LOBBY_S_H
#define LOBBY_S_H
/* lobby_a.h plus a typed lbShop / shopList for the lobby shop files (types copied from include/lobby.h).
 * lobby_a.h declares lbShop as an untyped u8[0x90]; keep that one under another name. */
#define lbShop lbShop_hdr   /* lobby_a.h declares lbShop as an untyped u8[0x90]; keep that one under another name */
#include "lobby_a.h"
#undef lbShop

typedef struct LB_SHOP {
    s16 pos[5][2];      /* 0x00 tag positions (x, y) */
    s8 step;            /* 0x14 main step (Lb_mix / Lb_shop switch) */
    s8 x15;             /* 0x15 sub step */
    s8 x16;             /* 0x16 */
    s8 x17;             /* 0x17 */
    s8 x18;             /* 0x18 */
    s8 mode;            /* 0x19 shop mode: 0 make, 1 buy, 2 sell (lb_mix) */
    s8 x1A;             /* 0x1A */
    s8 x1B;             /* 0x1B */
    s8 x1C;             /* 0x1C */
    u8 _pad1D[3];
    void (*f20)();      /* 0x20 init */
    void (*f24)();      /* 0x24 tag decide */
    void (*f28)();      /* 0x28 */
    int (*f2C)();       /* 0x2C select */
    int (*f30)();       /* 0x30 item select */
    void (*f34)();      /* 0x34 decide */
    int (*f38)();       /* 0x38 */
    void (*f3C)();      /* 0x3C put item detail */
    void (*f40)();      /* 0x40 put help */
    void (*f44)(int, int, int, s16); /* 0x44 list icon (x, y, z, entry index) */
    s32 *tag;           /* 0x48 tag name table (string pointers) */
    s32 help;           /* 0x4C help message */
    void *list;         /* 0x50 */
    u8 x54[6];          /* 0x54 equip compare window data A */
    u8 x5A[6];          /* 0x5A equip compare window data B */
    s32 wait;           /* 0x60 tag slide counter */
    s32 *tbl;           /* 0x64 item id table */
    u8 _pad68[4];
    s8 x6C;             /* 0x6C page */
    s8 x6D;             /* 0x6D page count */
    s8 x6E;             /* 0x6E */
    u8 _pad6F;
    s32 x70;            /* 0x70 */
    s32 cur;            /* 0x74 cursor (list index) */
    s8 x78;             /* 0x78 */
    u8 _pad79[3];
    s32 qty;            /* 0x7C quantity */
    s32 count;          /* 0x80 list entry count */
    s32 x84;            /* 0x84 */
    u8 _pad88[4];
    u16 key;            /* 0x8C pressed keys */
    s8 x8E;             /* 0x8E */
    s8 x8F;             /* 0x8F */
} LB_SHOP;

typedef struct LB_SHOPITEM {
    s32 price;          /* 0x00 */
    char name[0x20];    /* 0x04 */
    s16 state;          /* 0x24 0 ok, 1 cannot, 2 empty */
    u8 _pad26[2];
} LB_SHOPITEM;
extern LB_SHOP lbShop;
extern LB_SHOPITEM shopList[];
#endif
