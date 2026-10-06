/* lb_by145 - agent B 0x005A22E0-0x005A2410: npcPigEXIT (the pig walks to the stage exit spot: turn towards it, then leave). Lb_get_angle takes the NPC and the target vector. */
#include "lbnpc_proto.h"
void npcPigEXIT(em)
EMW *em;
{
    VEC3 v;
    f32 *p = St_unique_tbl[game_w.stage];

    v.x = p[1];
    v.y = p[2];
    v.z = p[3];
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0xA;
        em->x0E = Lb_get_angle(em, &v);
        Lb_pl_chr_set0(em, 0x3F7, 2, 0, 0);
        return;
    case 1:
        if (--em->work08 >= 0) {
            em->ang[1] += em->x0E / 10;
            return;
        }
        em->x05++;
        return;
    case 2:
        if (em->work08 >= 0x1F4) {
            Lb_act_set(em, 0, 0);
        }
        break;
    }
}
