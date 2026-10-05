/* lbui, run 19: plaza_trans_ot1 .. plaza_trans_ot1 (lobby.bin 0x0059D820-0x0059D884): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void plaza_trans_ot1(a)
u8 *a;
{
    font_set_stack_no(*(int *)(a + 0x18));
    if (SoftKeyboard_alive_check() != 0) {
        DispSoftkeyboard(1);
    }
    if (pNet->x0C == 1) {
        DispDialogData(pNet->x0C);
        Lb_on_dialog();
        pNet->x0C = 0;
    }
}
