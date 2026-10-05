/* lbnpc, run 6: lb_npc_pig_move .. lb_npc_pig_move (lobby.bin 0x005A2900-0x005A2A1C): the matching functions of lbnpc_nm.c. */
#include "lbnpc_proto.h"

void lb_npc_pig_move(em)
EMW *em;
{
    switch (em->x15) {
    case 0:
        npcPigFOOTWORK(em, 0);
        return;
    case 0x8F:
        npcPigFOOTWORK(em, 1);
        return;
    case 0x86:
        npcPigSLEEP(em, 0);
        return;
    case 0x87:
        npcPigSLEEP(em, 1);
        return;
    case 0x64:
        npcPigTOPL(em);
        return;
    case 0x88:
        npcPigATACK(em);
        return;
    case 0x89:
        npcPigJOY(em);
        return;
    case 0x8A:
        npcPigEXIT(em);
        return;
    case 0x8B:
        npcPigWALK(em);
        return;
    case 0x8C:
        npcPigWALK2(em);
        break;
    }
}
