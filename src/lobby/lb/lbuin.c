/* lbui, run 14: plaza_logOut .. tl_exit_sub_menu (lobby.bin 0x00599460-0x005997C0): the matching functions of lbui_nm.c. */
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

void plaza_chatMain(a)
LB_NETW *a;
{
    int tbl = (int)plazaMenuTbl[a->menu];
    int off;

    a->x28 = Get_sw(0);
    switch (a->step) {
    case 0:
        a->step++;
        Plaza_chat_init();
        break;
    case 1:
        a->x28 = Get_sw_on2(0);
        if (Plaza_chat_move(*(u16 *)0x3F3714) == -1) {
            a->step++;
        }
        break;
    case 2:
        a->step++;
        break;
    case 3:
        tl_exit_sub_menu(1);
        off = a->cur * 0x24;
        SetHelpLineMsg(2, *(u16 *)(off + tbl + 2) + 2);
        break;
    }
}

void tl_exit_sub_menu(silent)
int silent;
{
    if (!(silent & 0xFF)) {
        cnWrap_SoundRequest(3);
    }
    SetHelpLineMsg(2, pNet->sel + 2);
    pNet->depth--;
    pNet->step = 0;
    pNet->x04 = 0;
    pNet->x05 = 0;
    pNet->x0A = 0;
    pNet->x24 = 0;
    pNet->x12 = 0;
    pNet->x28 = 0;
    pNet->x26 = 0;
    pNet->sel = 0xE;
    pNet->x0D = 1;
}
