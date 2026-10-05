/* lbui, run 8: mail_input .. get_page_num (lobby.bin 0x00595F70-0x005961C8): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

int mail_input(a, buf)
LB_NETW *a;
int buf;
{
    s8 r;

    Get_sw(0);
    Get_kb_input();
    switch (a->x05) {
    case 0:
        a->x05++;
        SoftKeyboard_pos_set(100.0f, 0x140);
        SoftKeyboard_set(1, 0xE, 0x7E, buf);
        break;
    case 1:
        r = SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (r) {
        case 1:
        case -1:
            a->x05++;
            break;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        a->x05 = 0;
        return 1;
    }
    return 0;
}

void get_friend_page_num(a)
LB_NETW *a;
{
    a->x26 = net_Check_FriendSuu(Friend_data, 0x32);
    if (a->x26 % 7 != 0) {
        a->x26 = a->x26 / 7 + 1;
        return;
    }
    a->x26 = a->x26 / 7;
    if (a->x26 == 0) {
        a->x26 = 1;
    }
}

int get_page_num(a, b)
s16 a;
s16 b;
{
    int r;

    if (a % b != 0) {
        return (s16)(a / b + 1);
    }
    r = (s16)(a / b);
    if (r == 0) {
        r = 1;
    }
    return r;
}
