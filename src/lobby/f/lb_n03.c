/* lb_n03 - lobby senders 0x005D6140-0x005D61EC: Lb_send_item_result. Whole file in lb_n.c. */
#include "lobby_f.h"








void Lb_send_item_result(a0, res)
int a0;
s8 res;
{
    struct { s16 item; s16 num; u8 id[8]; s8 r; } t;
    PLW *pl = &player_work[game_w.master];
    memcpy(t.id, CWPLAYER(pl->work909) + 0x132C, 8);
    t.item = pl->work904;
    t.num = pl->work906;
    t.r = res;
    lb_send_dataTU(0xE, 0xE, &t, a0);
}
