/* lbnpc, run 5: npcPigWALK .. npcPigWALK (lobby.bin 0x005A2410-0x005A26C8): the matching functions of lbnpc_nm.c. */
#include "lbnpc_proto.h"

void npcPigWALK(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    LB_ROUTE *r;
    f32 d;
    s16 i;
    LB_ROUTE *rt;

    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 == 0x3EF) {
            Lb_pl_chr_set0(em, 0x3EB, 8, 0, 0);
            return;
        }
        Lb_pl_chr_set0(em, 0x3EB, 4, 0, 0);
        return;
    case 1:
        r = &mv->route[mv->idx];
        em->x0E = (s16)(u16)Lb_get_angle(em, r) / 10;
        em->ang[1] += em->x0E;
        d = flvecCalcDistance(em->pos, r);
        if (d < 0.0f) {
            d *= -1.0f;
        }
        if (d <= 50.0f) {
            mv->idx++;
            i = mv->idx;
            rt = mv->route;
            if (rt[i].wait == -1) {
                mv->idx = 0;
                return;
            }
            if ((u16)ran_suu(1) & 1) {
                em->x05++;
                em->work08 = ((u16)ran_suu(1) & 0x1F) + 0x136;
                Lb_pl_chr_set0(em, 0x3EF, 0xA, 0, 0);
                return;
            }
            r = &mv->route[mv->idx];
            if (em->x15 != r->act) {
                em->x05++;
                Lb_act_set(em, 0, (u16)r->act);
                mv->cnt = 0;
                return;
            }
            em->x05 = 0;
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            i = mv->idx;
            r = &mv->route[i];
            if (em->x15 != r->act) {
                em->x05++;
                Lb_act_set(em, 0, (u16)r->act, i);
                mv->cnt = 0;
                return;
            }
            em->x05 = 0;
        }
        break;
    }
}
