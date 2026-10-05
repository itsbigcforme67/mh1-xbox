/* lbui, run 12: Lb_clearChatID .. plaza_checkChatLog (lobby.bin 0x00599020-0x00599278): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void Lb_clearChatID(id)
u8 *id;
{
    int i;
    u8 *p = (u8 *)chatIDList;

    for (i = 0; ; ) {
        if (memcmp(p, id, 8) == 0) {
            Lb_clearChatMember(i);
            return;
        }
        i = (s8)(i + 1);
        p += 8;
        if (i >= 7) {
            return;
        }
    }
}

void Lb_clearChatList(void) {
    s8 i = 0;
    u8 *a = (u8 *)chatIDList;
    u8 *b = (u8 *)chatHandleList;

    CW->chatmode = 0;
    do {
        memset(a, 0, 8);
        memset(b, 0, 0x10);
        i++;
        a += 8;
        b += 0x10;
    } while (i < 7);
}

void plaza_ReibunEdit(void) {
    int sw = Get_sw2(0) & 0xFFFF;

    switch (pNet->step) {
    case 0:
        Plaza_ReibunEdit_i();
        pNet->step++;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        if ((u16)Plaza_ReibunEdit_mv(sw) & 0x40) {
            tl_exit_sub_menu(0);
        }
        break;
    }
}

void plaza_checkChatLog(void) {
    int sw = Get_sw2(0) & 0xFFFF;

    switch (pNet->step) {
    case 0:
        Plaza_chatlog_i();
        pNet->step++;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        if ((u16)sw & 0x40) {
            Plaza_chatlog_i();
            tl_exit_sub_menu(0);
            break;
        }
        Plaza_chatlog_mv(sw);
        break;
    }
}
