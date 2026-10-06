/* lb_by127 - agent B 0x005C4660-0x005C488C: Lb_npc_mk (NPC model matrix: scale, rotation, head turn towards the hunter, then joint transform). */
#include "lbnpc_proto.h"
#include "fl.h"
f32 flConvertStoR();
void *get_joint_mat_em();
void flCalcTransSI();
void cpRotMatrixYXZ2();
void flmatCopy();
void Lb_npc_mk(EMW *em) {
    FLMAT m2;
    FLMAT m1;
    void *mdl;
    void *j;
    u8 *ex;
    f32 lim;
    f32 ang;
    f32 cur;
    f32 old;

    ex = em->ex;
    mdl = *(void **)((u8 *)em + 0x50C);
    flmatMakeScale(&m1, em->scale[0], em->scale[1], em->scale[2]);
    cpRotMatrixYXZ2(em->ang, &m2);
    flmatSetTrans(&m2, em->pos[0], em->pos[1], em->pos[2]);
    flmatMul33_2(&m2, &m1);
    flmatCopy((u8 *)em + 0x60, &m2);
    if (em->char0 != 0x284 && em->char0 != 0x286 && ex[0xE] != 5) {
        if (em->kind != 0) {
            j = get_joint_mat_em(em, 0xC);
            lim = 10.0f;
        } else {
            j = get_joint_mat_em(em, 0x13);
            lim = 20.0f;
        }
        ang = 180.0f * flConvertStoR(Lb_get_angle(em, (u8 *)player_work + game_w.master * 0xA00 + 0xAC) & 0xFFFF) / 3.1415927f;
        if (!(ang <= 180.0f)) {
            ang = -(ang - 180.0f);
        }
        if (ang <= -90.0f && ang >= 90.0f) {
        } else {
            if (ang < -lim) {
                ang = -lim;
            } else if (!(ang <= lim)) {
                ang = lim;
            }
            old = *(f32 *)(ex + 4);
            cur = old + 0.2f * (ang - old);
            flmatRotY33(j, 3.1415927f * cur / 180.0f);
            *(f32 *)(ex + 4) = cur;
        }
    }
    flCalcTransSI(*(s32 *)((u8 *)mdl + 0x24), &m2);
}
