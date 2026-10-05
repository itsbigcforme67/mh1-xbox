#ifndef LBNPC_H
#define LBNPC_H
/* lobby town NPC movement scripts (npcMv*, npcCat*, npcPig*, lb_npc_*_move): shared declarations. */
#include "lobby.h"
#include "em.h"
#include "pl.h"
#include "game.h"

/* one waypoint of an NPC route (0x14 bytes); the list ends with act == -1 */
typedef struct LB_ROUTE {
    f32 pos[3];         /* 0x00 */
    s32 act;            /* 0x0C action (EMW.x15) to switch to at this waypoint */
    s32 wait;           /* 0x10 frames to wait, 0xFF = until the action ends */
} LB_ROUTE;

/* movement part of the NPC work at EMW+0x444 */
typedef struct LB_NPCMV {
    u8 _pad00[8];
    LB_ROUTE *route;    /* 0x08 */
    s16 idx;            /* 0x0C current waypoint */
    u8 kind;            /* 0x0E npc kind (as LB_NPCW.kind) */
    s8 f0F;
    s32 cnt;            /* 0x10 frames waited at the waypoint */
    u8 _pad14[0x2D - 0x14];
    s8 x2D;             /* 0x2D */
} LB_NPCMV;

#define EM_S32(em, o) (*(s32 *)((u8 *)(em) + (o)))
int Lb_act_set();
int Lb_Pl_basic_flagset();
int Lb_pl_chr_set();
int Lb_pl_chr_set0();
s32 Lb_get_angle();
f32 flvecCalcDistance();
s32 ran_suu();
s32 frame_check2(EMW *, f32, int);
extern void (*npc_move_func_190[])();
extern void (*npc_move_func2_191[])();
#endif
