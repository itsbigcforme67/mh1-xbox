#include "lobby_a.h"
extern char D_3C6FC8[];
void transOtSelectHandleName(int arg0) {
    int temp_a0;
    int i;

    font_set_stack_no(F(s32, arg0, 0x18));
    if (F(u8, pNet, 0xC) != 0) {
        DispDialogData();
        i = (s32)cw; temp_a0 = (F(u8, pNet, 8) * 8) + i;
        if (F(s8, temp_a0, 0xB) != 0) {
            DispNameAndIDonDialog(0x86, &D_3C6FC8, temp_a0 + 0xB);
        }
        F(u8, pNet, 0xC) = 0U;
    }
}
