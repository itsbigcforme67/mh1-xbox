/* lb_by150 - agent B 0x005A1B40-0x005A1CAC: npcPigSLEEP (pig lies down next to the sleeping hunter; pl_flag_set/clr take the NPC itself). */
#include "lbnpc_proto.h"

void npcPigSLEEP(em, kind)
EMW *em;
s8 kind;
{
    PLW *pl = &player_work[game_w.master];

    switch (em->x05) {
    case 0:
        em->x05++;
        pl_flag_set((PLW *)em, 0x20000);
        if (kind == 0) {
            Lb_pl_chr_set0(em, 0x432, 0, 0x58, 0);
            return;
        }
        Lb_pl_chr_set0(em, 0x432, 0xE, 0, 0);
        return;
    case 1:
        if (((EMW *)pl)->x15 != 0x33 && ((EMW *)pl)->x15 != 0x34) {
            pl_flag_clr((PLW *)em, 0x20000);
            Lb_pl_chr_set0(em, 0x431, 4, 0, 0);
            em->x05++;
            return;
        }
        break;
    case 2:
        if (em->x194 <= 0) {
            Lb_pl_chr_set0(em, 0x3E9, 0xA, 0, 0);
            em->x05++;
            return;
        }
        break;
    case 3:
        if (em->x194 <= 0) {
            Lb_act_set(em, 0, 0x8C);
        }
        break;
    }
}
