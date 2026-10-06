#include "lobby_a.h"

void CallBack_Event_ChatMessageTU(int arg0, int arg1, int arg2, int arg3) {
    u8 sp13F;
    u8 sp13E;
    u8 sp13D;
    int sp20;

    cnLBS_Get_ChatMessage(&sp20);
    arg3 = get_font_col(sp13D);
    arg2 = get_font_col(sp13E);
    arg1 = get_font_col(sp13F, &sp13F);
    if ((arg3 == 0) && (arg2 == 0) && (arg1 == 0)) {
        arg2 = 0;
        arg3 = 0;
        arg1 = 5;
    }
    if (F(u8, (u8 *)cw, 0x35D5) != 0) {
        Lb_chat_receipt(&sp20);
        return;
    }
    Plaza_chat_log_add(&sp20);
}
