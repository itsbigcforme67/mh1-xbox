/* lbui, run 7: Lb_checkChatID .. Lb_checkChatID (lobby.bin 0x00598D30-0x00598DB0): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

int Lb_checkChatID(id)
u8 *id;
{
    s8 i;
    u8 *p = (u8 *)chatIDList;

    for (i = 0; ; ) {
        if (memcmp(p, id, 8) == 0) {
            return 1;
        }
        i++;
        p += 8;
        if (i >= 7) {
            return 0;
        }
    }
}
