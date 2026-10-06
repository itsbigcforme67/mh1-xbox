/* lb_by148 - agent B 0x005A26D0-0x005A28FC: npcPigWALK2 (pig follows the hunter: turn towards, walk or stand by distance). Lb_get_angle takes (npc, target position); the then-part of the hunter-state if falls out instead of a final return. */
#include "lbnpc_proto.h"

void npcPigWALK2(em)
EMW *em;
{
    PLW *pl = &player_work[game_w.master];
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        Lb_pl_chr_set0(em, 0x3EB, 4, 0, 0);
        return;
    case 1:
        em->x0E = Lb_get_angle(em, pl->pos);
        em->ang[1] += em->x0E / 10;
        d = flvecCalcDistance(em->pos, pl->pos);
        if (((EMW *)pl)->x15 == 0x33 || ((EMW *)pl)->x15 == 0x34) {
            if (d < 65.0f) {
                em->x05++;
                Lb_pl_chr_set0(em, 0x3E9, 4, 0, 0);
                return;
            }
            if (em->char0 != 0x3EB) {
                Lb_pl_chr_set0(em, 0x3EB, 2, 0, 0);
                return;
            }
        } else {
            if (d < 130.0f) {
                if (em->char0 != 0x3E9) {
                    Lb_pl_chr_set0(em, 0x3E9, 4, 0, 0);
                    return;
                }
            } else if (em->char0 != 0x3EB) {
                Lb_pl_chr_set0(em, 0x3EB, 2, 0, 0);
                return;
            }
        }
        break;
    case 2:
        if (em->x194 <= 0) {
            em->x05++;
            Lb_pl_chr_set0(em, 0x3EF, 0xA, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 <= 0) {
            Lb_act_set(em, 0, 0x87);
        }
        break;
    }
}
