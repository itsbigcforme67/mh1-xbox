#include "lobby_b.h"
extern s8 D_35C7B1[];
extern u8 color_tbl[8];
typedef struct { u8 pad[0x11D]; u8 b, c, d; } CHATL;

static s32 get_font_col(s32 arg0) {
    s32 temp_v1;

    temp_v1 = arg0 & 0xFF;
    if (temp_v1 == 0 || !(D_35C7B1[temp_v1] & 4)) {
        arg0 = 0;
    } else {
        arg0 = color_tbl[(temp_v1 - 0x30) & 7];
    }
    return arg0;
}

void CallBack_Event_ChatMessage(int arg0, int arg1) {
    CHATL chat;

    cnLBS_Get_ChatMessage(&chat);
    chat.b = get_font_col(chat.b);
    chat.c = get_font_col(chat.c);
    chat.d = get_font_col(chat.d);
    if (F(u8, (u8 *)cw, 0x35D5) != 0) {
        Lb_chat_receipt(&chat);
        return;
    }
    Plaza_chat_log_add(&chat);
}
