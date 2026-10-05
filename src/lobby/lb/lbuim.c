/* lbui, run 13: plaza_logOut .. plaza_logOut (lobby.bin 0x00599460-0x005995F8): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void plaza_logOut(a)
LB_NETW *a;
{
    int sw = Get_sw2(0) & 0xFFFF;
    int t;

    switch (a->step) {
    case 0:
        a->step++;
        SetDialogData(0x27, 2);
        SetDialogYesNo(1);
        return;
    case 1:
        t = sw & 0xFFFF;
        a->x0C = 1;
        if (t & 0x20) {
            a->step++;
            return;
        }
        if (t & 0x800) {
            if (a->yesno != 0) {
                SetDialogYesNo(0);
                cnWrap_SoundRequest(1);
                return;
            }
        } else if (t & 0x400) {
            if (a->yesno != 1) {
                SetDialogYesNo(1);
                cnWrap_SoundRequest(1);
                return;
            }
        } else {
            if (t & 0x40) {
                if (a->yesno != 1) {
                    SetDialogYesNo(1);
                    cnWrap_SoundRequest(1);
                    return;
                }
                a->step++;
                return;
            }
        }
        break;
    case 2:
        if (a->yesno == 0) {
            a->step++;
            cnWrap_SoundRequest(0);
            fade_set(1);
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 3:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            To_LogOut(1);
        }
        break;
    }
}
