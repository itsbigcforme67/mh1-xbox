/* lbui, run 10: plaza_req_input .. getHandleFromID (lobby.bin 0x00597B10-0x00597D9C): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

int plaza_req_input(a, buf)
LB_NETW *a;
int buf;
{
    s8 r;

    Get_sw(0);
    switch (a->x05) {
    case 0:
        a->x05++;
        SoftKeyboard_pos_set(100.0f, 0x140);
        if (a->x04 == 1) {
            SoftKeyboard_set(0, 6, 6, buf);
        } else {
            SoftKeyboard_set(3, 0xF, 8, buf);
        }
        break;
    case 1:
        r = SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (r) {
        case 0:
            break;
        case 1:
            a->x05++;
            a->x06 = 0;
            break;
        case -1:
            a->x05++;
            a->x06 = 0;
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

int getHandleFromID(a)
LB_NETW *a;
{
    switch (a->x04) {
    case 0:
        a->x04++;
        a->x06 = 0;
    case 1:
        a->x04++;
        strcpy(SearchCondition.s, CW->x2F80);
        SearchCondition.len = strlen(CW->x2F80);
        SearchCondition.flag = 1;
        break;
    case 2:
        switch (Lbc_ConditionSearch(&SearchCondition, 1)) {
        case 0:
            if (SearchResult[0] != 0) {
                memcpy(CW->x2F80 + 8, SearchResult + 0xC, 0x11);
                return 0;
            }
            SetDialogData(0x29, 3);
            return 1;
        case 1:
            SetDialogData(0x29, 3);
            return 1;
        }
        break;
    }
    return 2;
}
