/* lb_bz29 - lobby UI/client 0x005B7B40-0x005B7B84: text_lobby_trans_ot3 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;

void text_lobby_trans_ot3(u8 *arg0) {
    u8 temp_a0;

    font_set_stack_no(F(s32, arg0, 0x18));
    temp_a0 = F(u8, pNet, 0xC);
    if (temp_a0 == 1) {
        DispDialogData(temp_a0);
        Lb_on_dialog();
    }
}
