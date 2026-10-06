/* lb_tos01 - agent C 0x005C2620-0x005C268C: transOtSelectHandleName (handle select dialog draw; typed pNet->x0C/menu and cw[0xB + menu*8] index form). */
#include "lobby_b.h"
extern char D_3C6FC8[];
void transOtSelectHandleName(int arg0) {
    font_set_stack_no(F(s32, arg0, 0x18));
    if (pNet->x0C != 0) {
        DispDialogData();
        if ((s8)cw[0xB + pNet->menu * 8] != 0) {
            DispNameAndIDonDialog(0x86, &D_3C6FC8, cw + 0xB + pNet->menu * 8);
        }
        pNet->x0C = 0;
    }
}
