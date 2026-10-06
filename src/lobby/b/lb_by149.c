/* lb_by149 - agent B 0x005A1CB0-0x005A1F24: npcPigTOPL (pig turns to the hunter and steps up to 78 units from them). */
#include "lbnpc_proto.h"

void npcPigTOPL(em)
EMW *em;
{
    PLW *pl = &player_work[game_w.master];
    VEC3 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        Lb_act_set(pl, 0, 0x58);
        em->work08 = 0xA;
        em->x0E = Lb_get_angle(em, *(s32 *)((u8 *)em + 0x7A0) + 0xAC);
        Lb_pl_chr_set0(em, 0x3EA, 2, 0, 0);
        return;
    case 1:
        if (--em->work08 >= 0) {
            em->ang[1] += em->x0E / 10;
            return;
        }
        em->x05++;
        return;
    case 2:
        if (flvecCalcDistance(em->pos, pl->pos) < 78.0f) {
            d.x = em->pos[0] - pl->pos[0];
            d.y = em->pos[1] - pl->pos[1];
            d.z = em->pos[2] - pl->pos[2];
            flvecNormalize(&d);
            d.x = d.x * 78.0f;
            d.y *= 78.0f;
            d.z *= 78.0f;
            EM_F32(em, 0x934) = pl->pos[0] + d.x;
            EM_F32(em, 0x938) = pl->pos[1] + d.y;
            EM_F32(em, 0x93C) = pl->pos[2] + d.z;
            Lb_Em_adj_calc(em, 0x14);
            Lb_pl_chr_set0(em, 0x3F2, 4, 0, 0);
            em->x05++;
            return;
        }
        break;
    case 3:
        em->ang[1] += (s16)(u16)Lb_get_angle(em, pl->pos) / 5;
        break;
    }
}
