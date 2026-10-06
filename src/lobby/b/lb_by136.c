/* lb_by136 - agent B 0x005C4330-0x005C4480: lb_npc_move (village NPC per-frame move: timers, route step, matrix, ground height). */
#include "lbnpc_proto.h"
void lb_npc_chr_sub();
int Lb_pl_timer_calc();
int Lb_hit_stop_calc();
int Lb_Em_pos_adj();
int cpRotMatrixYXZ2();
int GetGroundHitStatusAreaEm();
void Lb_npc_move_sub();
void lb_npc_move(EMW *em) {
    em->x40E = 10;
    Lb_pl_timer_calc();
    Lb_hit_stop_calc(em);
    em->x5A0[0] = em->pos[0];
    em->x5A0[1] = em->pos[1];
    em->x5A0[2] = em->pos[2];
    Lb_Em_pos_adj(em);
    Lb_npc_move_sub(em);
    em->ang[0] = (u16)em->ang[0];
    em->ang[1] = (u16)em->ang[1];
    em->ang[2] = (u16)em->ang[2];
    cpRotMatrixYXZ2(em->ang, em->mat);
    EM_F32(em, 0x1A0) = 2.0f * em->act_spd;
    EM_F32(em, 0x1F0) = 2.0f * em->act_spd;
    EM_F32(em, 0x240) = 2.0f * em->act_spd;
    EM_F32(em, 0x290) = 2.0f * em->act_spd;
    lb_npc_chr_sub(em);
    switch (*(u8 *)((u8 *)em + 0x452)) {
    case 0x4A:
    case 0x4D:
    case 0x48:
    case 0x15:
    case 0x14:
    case 0x2C:
    case 0x34:
    case 0x35:
    case 0x2F:
        return;
    }
    GetGroundHitStatusAreaEm(em, em->pos, (u8 *)em + 0x70C, &em->x5AC, &em->x7E4);
    em->pos[1] = em->x5AC;
}
