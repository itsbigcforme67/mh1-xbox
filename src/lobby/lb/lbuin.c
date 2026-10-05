/* lbui, run 14: tl_exit_sub_menu .. tl_exit_sub_menu (lobby.bin 0x00599720-0x005997C0): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

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
