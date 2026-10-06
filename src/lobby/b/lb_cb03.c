/* lb_cb03 - agent C 0x005C17A0-0x005C18A0: CallBack_Event_RoomCommer (add a room member record to cw table + chat log line). */
#include "lobby_b.h"
extern char lit_4548[];
typedef struct { u8 pad[0x1C]; char msg[0x100]; u8 a, b, c, d; } CHATM;
typedef struct { char id[8]; char name[0x10]; u8 pad18[4]; char mini[0x40]; } LUSER;
void CallBack_Event_RoomCommer(CNET_RES res) {
    LUSER user;
    CHATM chat;
    u8 *c;
    s32 i;
    u8 *p;
    s32 off;

    cnLBS_Get_RoomLeaveUser(&user);
    c = cw;
    i = 0;
    p = c;
    do {
        if ((s8)p[0x73C] == 0) {
            off = i * 0x2FC;
            strcpy(c + off + 0x73C, user.id);
            strcpy(cw + off + 0x744, user.name);
            memcpy(cw + off + 0x756, user.mini, 0x40);
            break;
        }
        i++;
        p += 0x2FC;
    } while (i < 4);
    if (cw[0x35D5] != 0 && user.name[0] != 0) {
        memset(&chat, 0, 0x120);
        chat.d = 6;
        chat.c = 6;
        chat.b = 6;
        sprintf(chat.msg, lit_4548, user.name);
        Chat_log_add(0, &chat);
    }
}
