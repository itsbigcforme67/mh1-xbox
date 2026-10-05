/* lbui, run 2: event_eat_trans_ot1 .. SetDialogYesNo (lobby.bin 0x00591B90-0x00591BD4): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void event_eat_trans_ot1(a)
u8 *a;
{
    font_set_stack_no(*(int *)(a + 0x18));
}

void SetDialogData_HTML(arg0)
int arg0;
{
    htmlStr = arg0;
    dialogData.html = 1;
    set_dialog_square(0x1F4, 0x17C);
}

void SetDialogYesNo(v)
s8 v;
{
    dialogData.yesno = v;
    pNet->yesno = v;
}
