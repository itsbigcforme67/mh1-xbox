/* lb_cb02 - agent C 0x005C1260-0x005C13BC: CallBack_Event_LobbyCommer (add a lobby member to lbCommer + chat log line; local 0x5C user record, 0x120 chat record). */
#include "lobby_b.h"
extern char lit_4327[];
typedef struct { u8 pad[0x1C]; char msg[0x100]; u8 a, b, c, d; } CHATM;
typedef struct { char id[8]; char name[0x10]; u8 pad[0x5C - 0x18]; } LUSER;
void CallBack_Event_LobbyCommer(CNET_RES res) {
    LUSER user;
    CHATM chat;
    s32 i;
    u8 *p;
    u8 *q;
    s32 j;
    u8 *p3;

    cnLBS_Get_RoomLeaveUser(&user);
    if ((p3 = cw + 3, *(s8 *)p3 == 0) || 0 > memcmp(p3, &user, 8)) {
        memcpy(cw + 3, &user, 8);
    }
    i = 0;
    p = (u8 *)lbCommer;
    do {
        if (memcmp(p, &user, 8) == 0) {
            return;
        }
        i++;
        p += 0x5C;
    } while (i < 8);
    j = 0;
    q = (u8 *)lbCommer;
    do {
        if (*(s8 *)q == 0) {
            memcpy(&lbCommer[j], &user, 0x5C);
            break;
        }
        j++;
        q += 0x5C;
    } while (j < 8);
    if (cw[0x35D5] != 0 && user.name[0] != 0) {
        memset(&chat, 0, 0x120);
        chat.d = 6;
        chat.c = 6;
        chat.b = 6;
        sprintf(chat.msg, lit_4327, user.name);
        Chat_log_add(0, &chat);
    }
}
