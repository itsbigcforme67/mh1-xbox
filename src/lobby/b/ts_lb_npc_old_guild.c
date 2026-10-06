/* lbnpc - lobby.bin 0x0059DB40-0x005A2A20: movement scripts of the town NPCs (humans npcMv*, cats npcCat*, pigs npcPig*)
 * and the lb_npc_*_move dispatchers. Near-match file in address order (tools/lbmerge.py ... include/lbnpc_proto.h lbnpc.h). */
#include "lbnpc_proto.h"


void lb_npc_old_guild(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x15) {
    case 0:
        if (em->x05 == 0) {
            em->x05++;
            em->work08 = ((ran_suu(1) & 0xFFFF) + 0x12C) & 0xFF;
            if (em->char0 != 0x3FF) {
                Lb_pl_chr_set0(em, 0x3FF, 2, 0, 0);
                return;
            }
        } else {
            if (--em->work08 <= 0) {
                Lb_act_set(em, 0);
                return;
            }
        }
        break;
    case 0x69:
        switch (em->x05) {
        case 0:
            em->x05++;
            Lb_pl_chr_set0(em, 0x400, 2, 0, 0);
            return;
        case 1:
            if (em->x194 <= 0) {
                Lb_act_set(em, 0, 0);
                return;
            }
            break;
        }
        break;
    case 0x64:
        switch (em->x05) {
        case 0:
            mv->x2D = 1;
            em->x05++;
            Lb_pl_chr_set0(em, 0x401, 8, 0, 0);
            return;
        case 1:
            mv->x2D = 1;
            if (em->x194 <= 0) {
                em->x05++;
                Lb_pl_chr_set0(em, 0x402, 2, 0, 0);
                return;
            }
            if (lb_sys.x68 == 0) {
                em->x05 += 3;
                em->work08 = ((u16)ran_suu(1) & 0x1F) + 0x60;
                return;
            }
            break;
        case 2:
            mv->x2D = 1;
            if (em->x194 < 2) {
                em->x05++;
                Lb_pl_chr_set0(em, 0x403, 2, 0, 0);
                return;
            }
            if (lb_sys.x68 == 0) {
                em->x05 += 2;
                em->work08 = ((u16)ran_suu(1) & 0x1F) + 0x60;
                Lb_pl_chr_set0(em, 0x402, 0xA, 0, 0);
                return;
            }
            break;
        case 3:
            mv->x2D = 1;
            if (lb_sys.x68 == 0) {
                em->x05++;
                em->work08 = ((u16)ran_suu(1) & 0x1F) + 0x60;
                Lb_pl_chr_set0(em, 0x402, 0xA, 0, 0);
                return;
            }
            break;
        case 4:
            if (--em->work08 <= 0) {
                em->x05++;
                em->work08 = (ran_suu(1) & 0xFF) + 0x258;
                Lb_pl_chr_set0(em, 0x3FF, 0x3C, 0, 0);
                return;
            }
            break;
        case 5:
            if (--em->work08 <= 0) {
                Lb_act_set(em, 0, 0);
            }
            break;
        }
        break;
    }
}
