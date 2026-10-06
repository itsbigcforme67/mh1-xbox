/* lb_bz61 - lobby UI/client 0x005C3150-0x005C3194: ss_text_lobby_trans_ot (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;

void ss_text_lobby_trans_ot(u8 *arg0) {
    u8 temp_a0;

    font_set_stack_no(F(s32, arg0, 0x18));
    temp_a0 = F(u8, pNet, 0xC);
    if (temp_a0 == 1) {
        DispDialogData(temp_a0);
        F(u8, pNet, 0xC) = 0U;
    }
}
