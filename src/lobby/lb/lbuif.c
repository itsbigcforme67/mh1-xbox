/* lbui, run 6: plaza_backToServer .. plaza_backToServer (lobby.bin 0x00594C20-0x00594D70): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void plaza_backToServer(a)
LB_NETW *a;
{
    int sw = Get_sw2(0) & 0xFFFF;

    if (BsLbsCount > 1) {
        switch (a->step) {
        case 0:
            a->step++;
            SetDialogData(0x28, 2);
            SetDialogYesNo(1);
            break;
        case 1:
            a->x28 = Get_sw_on2(0);
            a->x0C = 1;
            switch (Lb_select()) {
            case 0:
                a->x10 = 2;
                fade_set(0xA);
                str_stop(0);
                str_stop(1);
                break;
            case 3:
                tl_exit_sub_menu(1);
                break;
            }
            break;
        }
    } else {
        switch (a->step) {
        case 0:
            a->step++;
            SetDialogData(0x14, 3);
            break;
        case 1:
            a->x0C = 1;
            if ((u16)sw & 0x20) {
                tl_exit_sub_menu(0);
            }
            break;
        }
    }
}
