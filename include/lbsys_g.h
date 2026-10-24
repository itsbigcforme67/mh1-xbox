#ifndef LBSYS_G_H
#define LBSYS_G_H
/* lb_sys in PS2 Monster Hunter G (VERSION_MHG). G re-laid it out: the fields of MH1's 0x90-byte lb_sys sit at other
 * offsets of a 0x98-byte object at 0x6548F0, plus a few G-only ones. Field names are MH1's. The map was voted from the
 * twin functions with tools/mhg_addrmap.py (`lobby 0x6EAE50 0x6EAEE0`); LBSYS_OFS(mh1_offset) gives the G offset for
 * code that addresses lb_sys by a raw byte offset. In an MH1 build LBSYS_OFS is the identity. */
#include "types.h"

#ifdef VERSION_MHG
#define LBSYS_OFS(o) ((o) < 0x64 ? (o) + 0x16 : (o) == 0x64 ? 0x14 : (o) == 0x66 ? 0x7A : (o) == 0x68 ? 0 : (o) == 0x6C ? 8 : \
    (o) < 0x74 ? (o) - 0x70 + 0x7B : (o) == 0x74 ? 0x10 : (o) == 0x76 ? 0x12 : (o) == 0x78 ? 0x7F : (o) == 0x7C ? 0x80 : \
    (o) == 0x80 ? 0xC : (o) < 0x87 ? (o) : (o) < 0x8D ? (o) + 1 : (o) + 2)

/* cw 0x32BE..0x35CF (the room: owner flag 0x32C5 = G 0xF751, ready count 0x32C6 = G 0xF752, the server message 0x32D1
 * = G 0xF75D) sit 0xC48C higher in G, like 0x35D0..0x35FF (G's lb_guild_make_room / lb_basic_master, 24 Oct 2026;
 * 0x35E0.. from G's cmcs_03 (the game-server ticket, G cw+0xFA6C) and Lbc_connect (0x35F4 = G 0xFA80), 24 Oct round 6) */
/* pNet (the net window state, network_work) and cw (client work): MH1 offset -> G offset, from tools/mhg_addrmap.py
 * --ptr pNet / --ptr cw (votes) and G's code; unknown offsets stay as they are. */
/* PC: pNet points at a host buffer in MH1's layout (Clear_lobby_ram, lb_n.c), so MH1's C (raw pNet[0xC] ... offsets) runs
 * unchanged and PNET_OFS is the identity. G's own layout (prims at +0..0x10, the flags at +0x2D / +0x31) is not used. */
#define PNET_OFS(o) (o)
#define CW_OFS(o) ((o) < 0x2BFE ? (o) : (o) < 0x2C06 ? (o) - 0x2BFE + 0xB64 : (o) < 0x2C2F ? (o) - 0x2C06 + 0xB6E : \
    (o) < 0x2C3C ? (o) - 0x2C2F + 0xB97 : (o) < 0x2C43 ? (o) - 0x2C3C + 0xBA4 : (o) < 0x2C46 ? (o) - 0x2C43 + 0xBA7 : \
    (o) < 0x2C4C ? (o) - 0x2C46 + 0xBB0 : (o) < 0x2C70 ? (o) - 0x2C4C + 0xBB4 : \
    ((o) >= 0x2F6E && (o) < 0x2F9A) ? (o) - 0x2F6E + 0xED6 : ((o) >= 0x32BE && (o) < 0x35D0) ? (o) + 0xC48C : \
    ((o) >= 0x35D0 && (o) < 0x3600) ? (o) - 0x35D2 + 0xFA5E : (o))

typedef struct LBSYS_G {       /* at 0x6548F0, 0x98 bytes; comments: G offset (MH1 name) */
    s32 x68;                   /* 0x00 talk/event mode, the exit code Local_main tests (0x22 = quest, 0x27 = title) */
    s32 gx04;                  /* 0x04 G only */
    s32 x6C;                   /* 0x08 */
    s32 x80;                   /* 0x0C */
    s16 x74;                   /* 0x10 */
    u16 x76;                   /* 0x12 */
    union { u16 x64; u16 chair_mask; };   /* 0x14 */
    u8 _pad00;                 /* 0x16 */
    s8 x01;                    /* 0x17 */
    s8 x02;                    /* 0x18 */
    s8 x03;                    /* 0x19 mode (index into vs_square_func_213) */
    s8 x04;                    /* 0x1A */
    s8 x05;                    /* 0x1B */
    s8 x06;                    /* 0x1C */
    union { s8 x07; s8 step; };           /* 0x1D */
    s8 x08;                    /* 0x1E */
    u8 _pad09;                 /* 0x1F */
    union { s8 x0A; u8 _s0[0x1E]; };      /* 0x20 hint string 0 (MH1 0x0A) */
    union { s8 x28; u8 _s1[0x5A - 0x1E]; };   /* 0x3E hint string 1 (MH1 0x28); the strings run on to 0x7A */
    u8 x66;                    /* 0x7A chair number (a byte in G's code) */
    u8 x70;                    /* 0x7B */
    s8 x71;                    /* 0x7C */
    s8 x72;                    /* 0x7D */
    union { u8 x73; u8 _pad73; };   /* 0x7E */
    u8 x78;                    /* 0x7F */
    s32 x7C;                   /* 0x80 */
    s8 x84;                    /* 0x84 */
    s8 x85;                    /* 0x85 */
    s8 x86;                    /* 0x86 */
    u8 _pad87;                 /* 0x87 */
    u8 x87;                    /* 0x88 */
    union { u8 x88[8]; struct { u8 _p[6]; u8 x8D; u8 x8E; }; };   /* 0x89 */
    u8 _pad91[0x98 - 0x91];
} LBSYS_G;
#else
#define LBSYS_OFS(o) (o)
#define PNET_OFS(o) (o)
#define CW_OFS(o) (o)
#endif
#ifdef VERSION_MHG
/* G (lobby item box, ib = item_box 0x653330): the fields moved and the cursors grew to 16 bits; IBO(MH1 offset) = G offset
 * (tools/mhg_addrmap.py --ptr ib and the twin functions: 8/9/0xA are now u16 at 0x10/0x12/0x14) */
#define IBO(o) ((o) <= 6 ? (o) : (o) == 8 ? 0x10 : (o) == 9 ? 0x12 : (o) == 0xA ? 0x14 : (o) == 0xB ? 0x16 : (o) == 0xC ? 0x18 : \
    (o) == 0x10 ? 0x1C : (o) == 0x14 ? 0x20 : (o) == 0x18 ? 0x24 : (o) == 0x19 ? 0x26 : (o) == 0x1A ? 0x28 : (o) == 0x1C ? 0x27 : \
    (o) == 0x1D ? 0x2C : (o) == 0x1E ? 0x2A : (o) == 0x1F ? 0x2D : (o) == 0x20 ? 0x2E : (o) == 0x21 ? 0x17 : (o))
#else
#define IBO(o) (o)
#endif
#ifdef VERSION_MHG
/* G's User_data (0x32C210, tools/mhg_addrmap.py votes: pouch 0x1C4 -> 0x74C, item box 0x37C -> 0xC24, the six equipped-slot
 * bytes at 0x456.. -> u16 at 0xCFE..): UDO(MH1 offset) = G offset for the places the village C reads by raw offset */
#define UDO(o) ((o) < 0x1C4 ? (o) : (o) < 0x37C ? (o) + 0x588 : (o) + 0x8A8)
#define UDEQO(i) (0xCFE + 2 * (i))
#define UDEQ(u, i) (*(u16 *)((u8 *)(u) + UDEQO(i)))
#else
#define UDO(o) (o)
#define UDEQO(i) (0x456 + (i))
#define UDEQ(u, i) (((u8 *)(u))[UDEQO(i)])
#endif

/* LBSHOP_OFS(MH1 offset) = the offset from the symbol lbShop in the running build, for code that reaches the shop state by a
 * raw byte offset (files written against `u8 lbShop[0x90]`). In G the fields moved (include/lbshop_g.h; the MH1 s8 page / count at
 * 0x6C / 0x6D are s16 there and are reached at their low byte). Identity in an MH1 build. */
#ifdef VERSION_MHG
#define LBSHOP_G_OFS(o) ( (o) < 0x14 ? (o) : (o) == 0x14 ? 0x82 : (o) == 0x15 ? 0x83 : (o) == 0x16 ? 0x84 : (o) == 0x17 ? 0x85 : \
    (o) == 0x18 ? 0x86 : (o) == 0x19 ? 0x87 : (o) == 0x1A ? 0x88 : (o) == 0x1B ? 0x8B : (o) == 0x1C ? 0x8C : (o) < 0x6C ? (o) - 0xC : \
    (o) == 0x6C ? 0x78 : (o) == 0x6D ? 0x7A : (o) == 0x6E ? 0x89 : (o) == 0x70 ? 0x60 : (o) == 0x74 ? 0x64 : (o) == 0x78 ? 0x8A : \
    (o) == 0x7C ? 0x68 : (o) == 0x80 ? 0x6C : (o) == 0x84 ? 0x70 : (o) == 0x8C ? 0x80 : (o) == 0x8E ? 0x8D : (o) == 0x8F ? 0x8E : (o) )
#define LBSHOP_OFS(o) (0xC + LBSHOP_G_OFS(o))
#define LBSHOP_SIZE 0xA0      /* G: the fields end at 0x9B (lbshop_g.h) */
#else
#define LBSHOP_OFS(o) (o)
#define LBSHOP_SIZE 0x90
#endif
/* the monster-list items of the shop (collection items that need the monster to be in the hunter's monster list): ids 293..322
 * in MH1, 474..503 in G (G renumbered its items) */
#ifdef VERSION_MHG
#define ML_BASE 474
#define ML_END 504
#else
#define ML_BASE 293
#define ML_END 323
#endif
/* The shop's help messages. MH1 keeps them in shop_default_help[0..7]; G has one table (shop_mix_help, G 0x636A70, 12
 * entries) whose entries 1.. serve both: the MH1 index -> G entry map below was made by comparing the texts (G dropped the
 * "free of charge" message, it sits at the end). SDH(k) = MH1's shop_default_help[k]; SDH_F(byte offset) the raw-offset form. */
#ifdef VERSION_MHG
extern s32 shop_mix_help[];
/* the map {1, 2, 3, 5, 11, 8, 9, 10} packed in a constant (no data in the objects that include this header) */
#define SDH(k) (shop_mix_help[(int)((0x0A09080B05030201ULL >> (8 * (k))) & 0xFF)])
#define SDH_F(o) SDH((o) / 4)
#else
#define SDH(k) (shop_default_help[k])
#define SDH_F(o) F(s32, &shop_default_help, o)
#endif
#endif
