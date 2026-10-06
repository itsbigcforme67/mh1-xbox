/* lbui, run 8: plaza_backToServer .. plaza_backToServer (lobby.bin 0x00594C20-0x00594D70): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"


/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();

/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */

void Paint_square();

/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */

void put_button_help(int a, int b, int c, u16 d);

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

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
