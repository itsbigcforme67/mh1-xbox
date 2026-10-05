/* lbui, run 9: my_comment_input .. plaza_checkMyStatus (lobby.bin 0x00597230-0x00597478): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

int my_comment_input(buf)
int buf;
{
    s8 r;

    Get_sw(0);
    Get_kb_input();
    switch (pNet->x05) {
    case 0:
        pNet->x05++;
        SoftKeyboard_pos_set(100.0f, 0x140);
        SoftKeyboard_set(1, 0xE, 0x61, buf);
        break;
    case 1:
        r = SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (r) {
        case 1:
        case -1:
            pNet->x05++;
            break;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        pNet->x05 = 0;
        return 1;
    }
    return 0;
}

void plaza_checkMyStatus(void) {
    int sw = Get_sw2(0) & 0xFFFF;
    s16 *p;
    int t;
    s16 v;

    switch (pNet->step) {
    case 0:
        pNet->x24 = 0;
        pNet->step++;
        CW->x30B4 = ClassInfo.plaza;
        CW->x30B6 = ClassInfo.lobby;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        t = sw & 0xFFFF;
        if (t & 0x800) {
            p = &pNet->x24;
            if (*p == 0) {
                *p = 2;
            } else {
                *p = *p - 1;
            }
            cnWrap_SoundRequest(1);
        } else if (t & 0x400) {
            v = pNet->x24 + 1;
            pNet->x24 = v;
            if (v > 2) {
                pNet->x24 = 0;
            }
            cnWrap_SoundRequest(1);
        } else if (t & 0x40) {
            tl_exit_sub_menu(0);
        }
        break;
    }
}
