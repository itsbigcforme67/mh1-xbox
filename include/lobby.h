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

extern u8 *cw;
extern LB_PIT lb_pit;
extern LB_SYS lb_sys;

#endif
