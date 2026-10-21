/* G online (PC): CallBack_Event_ChatMessage (G lobby 0x5869D0; HAND-FIXED: m2c lost the three colour bytes, which G
 * keeps in a1-a3 across its own leaf get_font_col). A chat line arrived: unless its sender is on the ignore list
 * (lobby_func_005C1560 over BsProxyUrlstr+0x110, 8 entries), its three colour bytes (+0x11D..0x11F of the message) go
 * through get_font_col (0xFF -> 0), then it goes to the town's chat (cw+0xFA61 set) or the plaza log. */
#include "../gonl.h"
extern u8 BsProxyUrlstr;
extern u8 *cw;

s32 Lb_chat_receipt();
s32 Plaza_chat_log_add();
s32 cnLBS_Get_ChatMessage();
s32 get_font_col();
s32 lobby_func_005C1560();

void CallBack_Event_ChatMessage(void)
{
    u8 m[0x120];
    int k;
    cnLBS_Get_ChatMessage(m);
    if (lobby_func_005C1560(&BsProxyUrlstr + 0x110, 8, m) != -1)
        return;
    for (k = 0x11D; k <= 0x11F; k++)
        m[k] = (u8)get_font_col(m[k]);
    for (k = 0x11D; k <= 0x11F; k++)
        if (m[k] == 0xFF)
            m[k] = 0;
    if (cw[0xFA61] != 0)
        Lb_chat_receipt(m);
    else
        Plaza_chat_log_add(m);
}
