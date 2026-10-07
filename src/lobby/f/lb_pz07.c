/* lb_pz07 - lobby.bin 0x00594840-0x00594C1C: plaza_movePlaza(a), the plaza switch menu (cursor over 10 plazas in two columns of 7, enter/ack dialogs). X0A is read as u8; the byte of each PlazaInfo entry at +0x10 is reached through the absolute alias D_3A2940 (config/lobby_aliases.txt). */
#pragma readonly_strings on
#include "lbui_proto.h"
#define X0A (*(u8 *)&a->x0A)
extern u8 D_3A2940[];
int Lbs_ExitAndEnterPlaza();
void To_EnterPlaza();
void To_TopMenu();

void plaza_movePlaza(a)
LB_NETW *a;
{
    u16 sw = Get_sw2(0);

    switch (a->step) {
    case 0:
        a->x28 = Get_sw_on2(0);
        if (sw & 0x20) {
            if (ClassInfo.plaza == X0A + 1) {
                a->step = 5;
                SetDialogData(0x13, 3);
                cnWrap_SoundRequest(7);
                return;
            }
            if (D_3A2940[X0A * 0x15C] == 3) {
                a->step++;
                cnWrap_SoundRequest(0, 3);
                return;
            }
            a->step = 5;
            SetDialogData(0x12, 3);
            cnWrap_SoundRequest(7);
            return;
        }
        if (sw & 0x40) {
            tl_exit_sub_menu(0);
            return;
        }
        if (sw & 0x2000) {
            if (X0A % 7 != 0) {
                X0A--;
            } else {
                X0A = X0A / 7 * 7 + 6;
                if (X0A >= 10) {
                    X0A = 9;
                }
            }
            cnWrap_SoundRequest(1);
            return;
        }
        if (sw & 0x1000) {
            X0A++;
            if (X0A >= 10 || X0A % 7 == 0) {
                if (X0A > 7) {
                    X0A = 7;
                } else {
                    X0A -= 7;
                }
            }
            cnWrap_SoundRequest(1);
            return;
        }
        if (sw & 0xC00) {
            a->x24 ^= 1;
            if (X0A >= 7) {
                X0A -= 7;
            } else {
                X0A += 7;
                if (X0A >= 10) {
                    X0A = 9;
                }
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 1:
        switch (Lbs_ExitAndEnterPlaza(X0A + 1)) {
        case 0:
            ClassInfo.plaza = X0A + 1;
            To_EnterPlaza();
            return;
        case 1:
            a->step++;
            SetDialogData(0x10, 3);
            return;
        }
        break;
    case 2:
        a->x0C = 1;
        if (sw & 0x20) {
            a->step++;
            cnWrap_SoundRequest(0);
            return;
        }
        break;
    case 3:
        switch (Lbs_ExitAndEnterPlaza(ClassInfo.plaza)) {
        case 0:
            To_EnterPlaza();
            return;
        case 1:
            a->step++;
            SetDialogData(0x11, 0);
            return;
        }
        break;
    case 4:
        a->x0C = 1;
        if (sw & 0x20) {
            To_TopMenu(1);
            cnWrap_SoundRequest(0);
            return;
        }
        break;
    case 5:
        a->x0C = 1;
        if (sw & 0x20) {
            a->step = 0;
            cnWrap_SoundRequest(3);
        }
        break;
    }
}
