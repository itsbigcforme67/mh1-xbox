#ifndef UD_H
#define UD_H
/* User data (saved hunter data, global User_data) as used by f_ud (src/main/ud). Offsets from matched code;
 * field names are guesses. Warehouse/equip helpers take a pointer to this struct (not always &User_data). */
#include "types.h"

typedef struct UD_ITEM { u16 id; s16 num; } UD_ITEM;
typedef struct UD_WARE { u8 use; u8 kind; u16 id; u16 opt; } UD_WARE; /* one stored equipment (6 bytes) */

#ifdef VERSION_MH2
/* MH2 (0x37F8 bytes, the copy to the card image 0x3DB328): offsets read from MH2's twins of f_ud (sub_main 0x3B61D0..0x3BC690). Warehouse
 * and item box hold 100 per step, up to 500; a warehouse entry is 12 bytes; so is each worn piece. */
s16 sub_main_func_003BC650();          /* 100 * (1 + flags 621..624): MH2's count, where G tests two bits */
#define UD_STOCK_N(u) sub_main_func_003BC650(u)
#define UD_WARE_N(u) sub_main_func_003BC650(u)
#define WIDX_NONE 0xFFFF
typedef struct UD_WARE12 { u8 use; u8 kind; u16 id; u16 opt; u8 x06[6]; } UD_WARE12;
typedef UD_WARE12 UD_EQ;
#define UD_WARE UD_WARE12
typedef struct UDW {
    u8 _pad00[0x1];
    u8 x01;                  /* 0x001 */
    u8 _pad02[0x5C - 0x2];
    u32 point;               /* 0x05C hunter points (Get_hunter_rank) */
    u32 x60;                 /* 0x060 */
    u16 evflag[0x2C];        /* 0x064 event flags (Event_flag_set); the count is the room up to ware */
    UD_WARE12 ware[500];     /* 0x0BC warehouse equipment */
    UD_ITEM stock[500];      /* 0x182C item box */
    u32 qclear[16];          /* 0x1FFC quest-clear bits (Quest_clear_bit_ck); count is the room */
    u32 x374;                /* 0x203C lw; MH1's 0x374 by order, a guess */
    UD_ITEM item[20];        /* 0x2040 pouch (Ud_item_search_space) */
    union {
        struct { u8 _p0; u8 wkind; u16 wid; u16 wopt; }; /* 0x2090 Equip_idx_renew, ItemCopy; wopt by order */
        UD_EQ eq[6];         /* 0x2090 weapon, then leg, head, body, arm, waist: id at +2 */
    };
    u8 _pad20D8[0x2186 - 0x20D8];
    u16 widx[6];             /* 0x2186 warehouse index of weapon, leg, head, body, arm, waist (Warehouse_equip) */
    u8 wyv_kill[2];          /* 0x2192 by order after widx, a guess */
    u8 _pad2194[0x2726 - 0x2194];
    union { u16 rank16; u8 rank; }; /* 0x2726 lhu; Get_hunter_rank returns 0 when this is 0, else works it out from point. `rank` is a guess */
    u8 _pad2728[0x2984 - 0x2728];
    s32 gold;                /* 0x2984 (Gold_add) */
    u8 _pad2988[0x37F8 - 0x2988];
} UDW;
#define UD_ARMOR(u, i) ((u)->eq[(i) + 1].id)
#elif defined(VERSION_MHG)
/* G (0xE00 bytes): the warehouse and the item box hold 100/200/300 entries (byte 0xC97 bits 0 and 1 are the two bought
 * expansions, G sub_main 0x4C9250), the pouch and everything after it moved by the same amount, the worn-equipment indexes are
 * 16 bits (0xFFFF = none). Offsets from G's f_ud (sub_main 0x4C5000..0x4C9300). */
#define UD_STOCK_N(u) ((1 + (((u8 *)(u))[0xC97] & 1) + ((((u8 *)(u))[0xC97] >> 1) & 1)) * 100)
#define UD_WARE_N(u) UD_STOCK_N(u)
#define WIDX_NONE 0xFFFF
typedef struct UDW {
    u8 _pad00[0x1];
    u8 x01;                  /* 0x001 */
    u8 _pad02[0x1C - 0x2];
    u32 point;               /* 0x01C hunter points */
    s32 gold;                /* 0x020 gold (zeni) */
    u16 evflag[0x10];        /* 0x024 event flags */
    UD_WARE ware[300];       /* 0x044 warehouse equipment */
    UD_ITEM stock[300];      /* 0x74C item box */
    u32 qclear[8];           /* 0xBFC quest-clear bits */
    u32 x374;                /* 0xC1C */
    u8 _pad378[0x3];
    u8 rank;                 /* 0xC23 hunter rank */
    UD_ITEM item[20];        /* 0xC24 pouch */
    u8 _padC74[0x1];
    u8 wkind;                /* 0xC75 equipped weapon kind (7 = bowgun) */
    u16 wid;                 /* 0xC76 equipped weapon id */
    u16 wopt;                /* 0xC78 equipped weapon option bits */
    u8 armor[5];             /* 0xC7A leg, head, body, arm, waist */
    u8 _padC7F[0xCFE - 0xC7F];
    u16 widx[6];             /* 0xCFE warehouse index of weapon, leg, head, body, arm, waist (0xFFFF none) */
    u8 wyv_kill[2];          /* 0xD0A wyvern kill counters (online; guess) */
    u8 _padD0C[0xE00 - 0xD0C];
} UDW;
#else
#define UD_STOCK_N(u) 100
#define UD_WARE_N(u) 64
#define WIDX_NONE 0xFF
typedef struct UDW {
    u8 _pad00[0x1];
    u8 x01;                  /* 0x001 (selects armor bit 1 or 2: sex, a guess) */
    u8 _pad02[0x1C - 0x2];
    u32 point;               /* 0x01C hunter points */
    s32 gold;                /* 0x020 gold (zeni), clamped to 0..9999998 by Gold_add */
    u16 evflag[0x10];        /* 0x024 event flags */
    UD_WARE ware[64];        /* 0x044 warehouse equipment */
#ifdef VERSION_MHG
    u8 _padG1[0x588];        /* G: 0x588 bytes of new data here; everything after sits later (lbsys_g.h UDO) */
#endif
    UD_ITEM stock[100];      /* 0x1C4 item box */
    u32 qclear[8];           /* 0x354 quest-clear bits */
    u32 x374;                /* 0x374 */
    u8 _pad378[0x3];
    u8 rank;                 /* 0x37B hunter rank */
#ifdef VERSION_MHG
    u8 _padG2[0x320];        /* G: pouch starts at 0xC24 (0x37C + 0x8A8) */
#endif
    UD_ITEM item[20];        /* 0x37C pouch */
    u8 _pad3CC[0x1];
    u8 wkind;                /* 0x3CD equipped weapon kind (7 = bowgun) */
    u16 wid;                 /* 0x3CE equipped weapon id */
    u16 wopt;                /* 0x3D0 equipped weapon option bits */
    u8 armor[5];             /* 0x3D2 leg, head, body, arm, waist */
    u8 _pad3D7[0x456 - 0x3D7];
#ifdef VERSION_MHG
    u16 widx[6];             /* G: u16 each, at 0xCFE */
#else
    u8 widx[6];              /* 0x456 warehouse index of weapon, leg, head, body, arm, waist (0xFF none) */
#endif
    u8 wyv_kill[2];          /* 0x45C wyvern kill counters (online) */
} UDW;
#endif
#ifdef VERSION_MH2
#define UD_SLOT 0x37F8       /* MH2: saved hunter data per memory-card slot */
#elif defined(VERSION_MHG)
#define UD_SLOT 0xE00        /* G: saved hunter data per memory-card slot */
#else
#define UD_SLOT 0x480
#endif
#if defined(VERSION_MHG) || defined(VERSION_MH2)
typedef u16 UD_IDX;          /* G: warehouse indexes are 16 bits, 0xFFFF = none */
#define UD_NONE 0xFFFF
#else
typedef u8 UD_IDX;
#define UD_NONE 0xFF
#endif
#ifndef UD_ARMOR
#define UD_ARMOR(u, i) ((u)->armor[i])
#endif
extern UDW User_data[];
#endif
