/* lb_cb05 - agent C 0x005C0B10-0x005C0C8C: get_font_col (static, formerly lb_by21), CallBack_Event_ChatMessage and ...TU, one TU. The static callee and the 8-byte CNET_RES by-value parameter (spilled, 16-byte frame hole) both matter. */
#include "lobby_b.h"
typedef struct { u8 pad[0x11D]; u8 b, c, d; } CHATL;

extern s8 D_35C7B1[];
extern u8 color_tbl[8];

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


void CallBack_Event_ChatMessage(CNET_RES res) {
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

void CallBack_Event_ChatMessageTU(CNET_RES res) {
    CHATL chat;

    cnLBS_Get_ChatMessage(&chat);
    chat.b = get_font_col(chat.b);
    chat.c = get_font_col(chat.c);
    chat.d = get_font_col(chat.d);
    if (chat.b == 0 && chat.c == 0 && chat.d == 0) {
        chat.c = 0;
        chat.b = 0;
        chat.d = 5;
    }
    if (F(u8, (u8 *)cw, 0x35D5) != 0) {
        Lb_chat_receipt(&chat);
        return;
    }
    Plaza_chat_log_add(&chat);
}
