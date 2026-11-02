/* lb_tos01 - agent C 0x005C2620-0x005C268C: transOtSelectHandleName (handle select dialog draw; typed pNet->x0C/menu and cw[0xB + menu*8] index form). */
#include "lobby_b.h"
#include "lbsys_g.h"
extern char D_3C6FC8[];
#ifdef VERSION_MH2
int lobby_func_007B18A0();
void transOtSelectHandleName(int arg0) {
    font_set_stack_no(F(s32, arg0, 0x18));
    if (F(u8, pNet, 57) != 0) {
        DispDialogData();
        if (F(u8, pNet, 46) == 2 && (s16)lobby_func_007B18A0() == -1) {
            if ((s8)cw[38 + F(u8, pNet, 53) * 8] != 0) {
                DispNameAndIDonDialog(0x86, &D_3C6FC8, cw + 38 + F(u8, pNet, 53) * 8);
            }
        }
        F(u8, pNet, 57) = 0;
    }
}
#else
void transOtSelectHandleName(int arg0) {
    font_set_stack_no(F(s32, arg0, 0x18));
    if (pNet->x0C != 0) {
        DispDialogData();
        if ((s8)cw[0xB + pNet->menu * 8] != 0) {
            DispNameAndIDonDialog(0x86, &D_3C6FC8, cw + CW_OFS(0xB) + pNet->menu * 8);
        }
        pNet->x0C = 0;
    }
}
#endif
