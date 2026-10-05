/* lbnpc, run 3: lb_npc_cat_move .. npcPigFOOTWORK (lobby.bin 0x005A19D0-0x005A1B3C): the matching functions of lbnpc_nm.c. */
#include "lbnpc_proto.h"

void lb_npc_cat_move(em)
EMW *em;
{
    switch (em->x15) {
    case 0:
        npcCatFOOTWORK(em);
        return;
    case 1:
        npcCatRUN(em);
        return;
    case 0x82:
        npcCatSLEEP(em);
        return;
    case 0x64:
        npcMvTOPL(em);
        return;
    case 0x83:
        npcCatKYORO(em);
        return;
    case 0x84:
        npcCatHELLO(em);
        return;
    case 0x85:
        npcCatWAITER(em);
        return;
    case 0x90:
        npcMvWARP(em);
        break;
    }
}

void npcPigFOOTWORK(em, kind)
EMW *em;
s8 kind;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        if (kind == 0) {
            Lb_pl_chr_set0(em, 0x3E9, 2, 0, 0);
            return;
        }
        Lb_pl_chr_set0(em, 0x3E9, 8, 0, 0);
        break;
    }
}
