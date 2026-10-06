/* CallBack_Event_RoomLeaver (0x5C18A0): logic complete (shift following room members down); 22/152 differ only in s0-s3 naming of j/off/o2. Not built. */
#include "lobby_b.h"
extern char lit_4622[];
typedef struct { u8 pad[0x1C]; char msg[0x100]; u8 a, b, c, d; } CHATM;
typedef struct { char id[8]; char name[0x10]; u8 pad18[4]; char mini[0x40]; } LUSER;
void CallBack_Event_RoomLeaver(CNET_RES res) {
    LUSER user;
    CHATM chat;
    s32 i;
    s32 found;
    s32 off;
    s32 j;
    s32 o2;

    cnLBS_Get_RoomLeaveUser(&user);
    i = 0;
    off = 0;
    do {
        if ((s8)cw[0x2C0C] == 0 && memcmp(cw + off + 0x73C, &user, 8) == 0) {
            if (cw[0x35D5] != 0 && (s8)cw[off + 0x744] != 0) {
                memset(&chat, 0, 0x120);
                chat.d = 6;
                chat.c = 6;
                chat.b = 6;
                sprintf(chat.msg, lit_4622, cw + off + 0x744);
                Chat_log_add(0, &chat);
            }
            memset(cw + off + 0x73C, 0, 8);
            memset(cw + off + 0x744, 0, 0x10);
            memset(cw + off + 0x756, 0, 0x40);
            found = i;
        }
        i++;
        off += 0x2FC;
    } while (i < 4);
    j = found;
    if (j < 4) {
        found = found + 1;
        off = j * 0x2FC;
        o2 = found * 0x2FC;
        do {
            if (found >= 4) {
                memset(cw + off + 0x73C, 0, 8);
                memset(cw + off + 0x744, 0, 0x10);
                memset(cw + off + 0x756, 0, 0x40);
            } else {
                strcpy(cw + off + 0x73C, cw + o2 + 0x73C);
                strcpy(cw + off + 0x744, cw + o2 + 0x744);
                memcpy(cw + off + 0x756, cw + o2 + 0x756, 0x40);
            }
            j++;
            off += 0x2FC;
            o2 += 0x2FC;
            found++;
        } while (j < 4);
    }
}
