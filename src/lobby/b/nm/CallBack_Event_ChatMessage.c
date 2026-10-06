#include "lobby_a.h"

void CallBack_Event_ChatMessage(int arg0, int arg1) {
    u8 sp13F;
    u8 sp13E;
    u8 sp13D;
    int sp20;

    cnLBS_Get_ChatMessage(&sp20);
    arg1 = get_font_col(sp13D, &sp13D);
    arg1 = get_font_col(sp13E, &sp13E);
    arg1 = get_font_col(sp13F, &sp13F);
    if (F(u8, (u8 *)cw, 0x35D5) != 0) {
        Lb_chat_receipt(&sp20);
        return;
    }
    Plaza_chat_log_add(&sp20);
}
