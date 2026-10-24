/* Lobby client/UI overlay views (agent B): typed overlays for the byte-offset globals of the lobby
   (lb_sys, pNet, ...). Casting the address of the symbol to a struct gives the original's `symbol+offset`
   addressing (lui/lw %lo(lb_sys + LBSYS_OFS(0x6))), and MWCC allocates registers like the original, which it does not
   for F(T, p, off) macros. Add only. */
#ifndef LOBBY_B_H
#define LOBBY_B_H
#define lb_sys lb_sys_a_unused
#define pNet pNet_a_unused
#include "lobby_a.h"
#undef lb_sys
#undef pNet

#include "lbsys_g.h"
#ifdef VERSION_MHG
typedef LBSYS_G LBSYS_B;
#else
typedef struct LBSYS_B {          /* lb_sys, 0x90 bytes (same object as LBSYS) */
    u8 _p00; s8 x01; s8 x02; s8 x03; s8 x04; s8 x05; s8 x06; s8 x07; s8 x08; u8 _p09; s8 x0A;
    u8 _p0B[0x28 - 0xB]; s8 x28;
    u8 _p29[0x64 - 0x29]; u16 x64; u16 x66; s32 x68; s32 x6C;
    u8 x70; s8 x71; s8 x72; u8 x73; s16 x74; u16 x76; u8 x78; u8 _p79[3]; s32 x7C; s32 x80; s8 x84; u8 _p85;
    s8 x86; u8 x87; u8 _p88[5]; u8 x8D; u8 x8E; u8 _p8F;
} LBSYS_B;
#endif
extern LBSYS_B lb_sys;
#define LBS (&lb_sys)

typedef struct LBPIT_B {          /* lb_pit (0xC bytes): current NPC talk script position */
    s32 x0;                       /* 0x00 */
    u8 *pos;                      /* 0x04 */
    s8 x08;                       /* 0x08 */
    s8 x09;                       /* 0x09 */
    s8 step;                      /* 0x0A */
    u8 _pad0B;
} LBPIT_B;
extern LBPIT_B lb_pit;

#ifdef MHG_ONLINE
/* the PC G online build: G's layout (include/lbsys_g.h PNET_OFS), shared with G's generated lobby code */
typedef struct LBNETW_B {
    u8 _prim[0x18];     /* 0x00 G: five prim pointers (MH1 0x14..) and +0x14 */
    s16 idx;            /* 0x18 (MH1 0x00) */
    u8 _pad1A[0x1C - 0x1A];
    s16 x24;            /* 0x1C (MH1 0x24) */
    s16 x26;            /* 0x1E (MH1 0x26) */
    s16 x28;            /* 0x20 (MH1 0x28) */
    u8 depth;           /* 0x22 (MH1 0x02) */
    u8 step;            /* 0x23 (MH1 0x03) */
    u8 x04;             /* 0x24 */
    u8 x05;             /* 0x25 */
    u8 gx26;            /* 0x26 G only */
    s8 x06;             /* 0x27 (MH1 0x06) */
    u8 sel;             /* 0x28 (MH1 0x07) */
    u8 menu;            /* 0x29 (MH1 0x08) */
    u8 cur;             /* 0x2A (MH1 0x09) */
    s8 x0A;             /* 0x2B (MH1 0x0A) */
    u8 _pad0B;          /* 0x2C */
    u8 x0C;             /* 0x2D (MH1 0x0C) */
    s8 x0D;             /* 0x2E */
    u8 _pad0E;            /* 0x2F (MH1 0x0E) */
    u8 yesno;           /* 0x30 (MH1 0x0F) */
    u8 _pad11;          /* 0x31 (MH1 0x11) */
    s8 x12;             /* 0x32 (MH1 0x12) */
    u8 _pad33;          /* 0x33 (MH1 0x13) */
    u8 x10;             /* 0x34 (MH1 0x10: PC only, lbsys_g.h) */
} LBNETW_B;
#else
typedef struct LBNETW_B {         /* pNet: network window state */
    s16 idx;                      /* 0x00 */
    u8 depth;                     /* 0x02 */
    u8 step;                      /* 0x03 */
    u8 x04;
    u8 x05;
    s8 x06;
    u8 sel;                       /* 0x07 */
    u8 menu;                      /* 0x08 */
    u8 cur;                       /* 0x09 */
    s8 x0A;
    u8 _pad0B;
    u8 x0C;                       /* 0x0C */
    s8 x0D;
    u8 _pad0E;
    u8 yesno;                     /* 0x0F */
    u8 x10;
    u8 _pad11;
    s8 x12;                       /* 0x12 */
    u8 _pad13[0x24 - 0x13];
    s16 x24;
    s16 x26;
    s16 x28;
} LBNETW_B;
#endif
extern LBNETW_B *pNet;
typedef struct CNET_RES {         /* result/event record passed by value (8 bytes, spilled to the stack) */
    s8 val;                       /* 0x00 result (0 ok, -1 error) */
    s8 id;                        /* 0x01 */
    u8 _pad02[6];
} CNET_RES;
#endif
