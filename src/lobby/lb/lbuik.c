/* lbui, run 11: lb_chatMemberCheck .. Lb_checkChatID (lobby.bin 0x00598B80-0x00598DB0): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

int lb_chatMemberCheck(void) {
    u8 *p;
    s16 i;
    LB_NETW *n;

    switch (pNet->x04) {
    case 0:
        p = (u8 *)chatIDList + pNet->idx * 8;
        if ((s8)p[0] != 0) {
            memcpy(CW->x2F80, p, 8);
            pNet->x04++;
            break;
        }
        return 0;
    case 1:
        switch (Lbs_SeekId()) {
        case 0:
            if (ClassInfo.plaza == CW->x30B4 && CW->x30B6 == 0) {
                n = pNet;
                chatListFlag = chatListFlag | (1 << n->idx);
            } else {
                n = pNet;
                chatListFlag = chatListFlag & ~(1 << n->idx);
            }
            i = n->idx + 1;
            n->idx = i;
            if (i < 8) {
                pNet->x04 = 0;
                break;
            }
            return 0;
        case 1:
            n = pNet;
            chatListFlag = chatListFlag & ~(1 << n->idx);
            i = n->idx + 1;
            n->idx = i;
            if (i < 8) {
                pNet->x04 = 0;
                break;
            }
            return 0;
        }
        break;
    }
    return 2;
}

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
