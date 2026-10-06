/* lbui, run 22: lb_npc_effect_move .. lb_npc_effect_move (lobby.bin 0x005C4890-0x005C48A0): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void lb_npc_effect_move(em)
void *em;
{
    (*(void (**)())(*(int *)((u8 *)em + 0x3CC) + 0xC))(em);
}
