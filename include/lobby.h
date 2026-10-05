#ifndef LOBBY_H
#define LOBBY_H
/* lobby.bin (online town) shared declarations. Field meanings are guesses
 * from usage; offsets in comments. */
#include "types.h"

/* lb_pit (0x006EAE40, size 0xC): current NPC talk script position */
typedef struct LB_PIT {
    s32 x0;             /* 0x00 */
    u8 *pos;            /* 0x04 current talk script entry (8-byte entries: u16 type, s32 text at +4) */
    s8 x08;             /* 0x08 talk script block index (x8 bytes) */
    s8 x09;             /* 0x09 */
    s8 step;            /* 0x0A talk step: 0 init, 1 running */
    u8 _pad0B;
} LB_PIT;

/* lb_sys (0x006EAE50, size 0x90): lobby town system state */
typedef struct LB_SYS {
    u8 _pad00[7];
    s8 step;            /* 0x07 event step */
    u8 _pad08[0x68 - 0x08];
    s32 x68;            /* 0x68 talk/event mode (0 = none) */
    s32 x6C;            /* 0x6C */
    u8 _pad70[0x87 - 0x70];
    s8 x87;             /* 0x87 */
    u8 _pad88[8];
} LB_SYS;

/* lobby NPC work: the per-monster area of an EMW at EMW+0x444 (EMW.ex) */
typedef struct LB_NPCW {
    u8 _pad00[0xE];
    u8 kind;            /* 0x0E npc kind (talk table key) */
    u8 _pad0F[0x26 - 0xF];
    u16 item;           /* 0x26 requested/given item id */
    u16 num;            /* 0x28 item count */
    u8 _pad2A[4];
    s8 x2E;             /* 0x2E talk variant (0 first time) */
    u8 _pad2F;
    s32 flag;           /* 0x30 event flag to set */
} LB_NPCW;

/* lbShop (0x006EABF0, size 0x90): shop / forge / armor-shop menu state shared
 * by lb_mix, lb_shop, lb_process, lb_armor (all field names are guesses) */
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

/* shopList entry (0x28 bytes) */
typedef struct LB_SHOPITEM {
    s32 price;          /* 0x00 */
    char name[0x20];    /* 0x04 */
    s16 state;          /* 0x24 0 ok, 1 cannot, 2 empty */
    u8 _pad26[2];
} LB_SHOPITEM;

/* Item_data row (0x10 bytes); the member offset folds into the symbol only with a struct array.
 * Do not include together with plf.h / plitem.h (they declare Item_data differently). */
typedef struct LB_ITEMROW {
    u8 type;            /* 0x00 */
    u8 use;             /* 0x01 */
    u8 rare;            /* 0x02 rarity (colour index) */
    u8 max;             /* 0x03 stack limit, 0xFF unlimited */
    u8 _04[8];
    u16 buy;            /* 0x0C buy price */
    u16 sell;           /* 0x0E sell price */
} LB_ITEMROW;
extern LB_ITEMROW Item_data[];

/* mixData entry (8 bytes): one forge recipe offered in the list */
typedef struct LB_MIXDATA {
    s16 no;             /* 0x00 recipe number (1..0x146) */
    s16 price;          /* 0x02 */
    u8 *rec;            /* 0x04 recipe from Item_preparation_get */
} LB_MIXDATA;
extern LB_MIXDATA mixData[];

extern LB_SHOP lbShop;
extern s32 shop_tex_rotate[];       /* 0x28 bytes; [2] and [7] are colours (alpha in the top byte) */
extern LB_SHOPITEM shopList[];
extern u8 *cw;
extern LB_PIT lb_pit;
extern LB_SYS lb_sys;

#endif
