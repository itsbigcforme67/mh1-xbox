/* lbnpc, run 2: npcMvDRUNKDOWN .. npc_move_common (lobby.bin 0x0059F0F0-0x0059F6AC): the matching functions of lbnpc_nm.c. */
#include "lbnpc_proto.h"

void npcMvDRUNKDOWN(em)
EMW *em;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x2A6) {
            Lb_pl_chr_set(em, 0x2A6, 0, 0);
        }
        break;
    }
}

void npcMvTOPL2(em)
EMW *em;
{
    switch (em->x05) {
    case 0:
        em->work08 = 8;
        em->x05++;
        break;
    case 1:
        if (--em->work08 <= 0) {
            Lb_act_set(em, 0, 0x64);
        }
        break;
    }
}

void npcMvRANDWAIT(em)
EMW *em;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((ran_suu(1) & 0xFFFF) + 0xFF) & 0xFF;
        Lb_pl_chr_set(em, 1, 0x14, 0);
        break;
    case 1:
        if (--em->work08 <= 0) {
            Lb_act_set(em, 0, 0);
        }
        break;
    }
}

void npcMvWALL(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        mv->f0F = 1;
        if (em->char0 != 0x27E) {
            if (em->char0 == 0x281) {
                if (em->x194 <= 0) {
                    Lb_pl_chr_set(em, 0x27E, 2, 0);
                    em->x05++;
                }
            } else {
                Lb_pl_chr_set(em, 0x27E, 2, 0);
                em->x05++;
            }
        } else {
            em->x05++;
        }
        break;
    }
}

void npcMvKEGA(em, kind)
EMW *em;
s8 kind;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        mv->f0F = 1;
        em->x05++;
        switch (kind) {
        case 0:
            if (em->char0 != 0x285) {
                Lb_pl_chr_set(em, 0x285, 0, 0);
                return;
            }
            break;
        case 1:
            if (em->char0 != 0x284) {
                Lb_pl_chr_set(em, 0x284, 0, 0);
            }
            break;
        }
        break;
    }
}

void npc_move_common(em)
EMW *em;
{
    switch (em->x15) {
    case 0x00:
        npcMvFOOTWORK(em, 0);
        return;
    case 0x75:
        npcMvFOOTWORK(em, 1);
        return;
    case 0x8E:
        npcMvFOOTWORK(em, 2);
        return;
    case 0x76:
        npcMvFOOTWORK(em, 3);
        return;
    case 0x02:
        npcMvWALK(em, 0);
        return;
    case 0x77:
        npcMvWALK(em, 1);
        return;
    case 0x8D:
        npcMvWALK(em, 2);
        return;
    case 0x78:
        npcMvWALK(em, 3);
        return;
    case 0x6A:
        npcMvENJOY(em, 0);
        return;
    case 0x6B:
        npcMvENJOY(em, 1);
        return;
    case 0x79:
        npcMvTALK(em, 0);
        return;
    case 0x7A:
        npcMvTALK(em, 1);
        return;
    case 0x7B:
        npcMvTALK(em, 2);
        return;
    case 0x6C:
        npcMvDOWN(em, 0);
        return;
    case 0x6D:
        npcMvDOWN(em, 1);
        return;
    case 0x6E:
        npcMvDOWN(em, 3);
        return;
    case 0x64:
        npcMvTOPL(em);
        return;
    case 0x7C:
        npcMvBOARD(em, 0);
        return;
    case 0x7D:
        npcMvBOARD(em, 1);
        return;
    case 0x6F:
        npcMvSHOP(em, 0);
        return;
    case 0x70:
        npcMvSHOP(em, 1);
        return;
    case 0x73:
        npcMvBEER(em);
        return;
    case 0x7E:
        npcMvDRUNKDOWN(em);
        return;
    case 0x74:
        npcMvTOPL2(em);
        return;
    case 0x7F:
        npcMvWALL(em);
        return;
    case 0x80:
        npcMvKEGA(em, 0);
        return;
    case 0x81:
        npcMvKEGA(em, 1);
        return;
    case 0x90:
        npcMvWARP(em);
        return;
    case 0x91:
        npcMvRANDWAIT(em);
        break;
    }
}
