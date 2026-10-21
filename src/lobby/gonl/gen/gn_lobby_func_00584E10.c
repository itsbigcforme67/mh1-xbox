/* G online (PC): lobby_func_00584E10, written from Monster Hunter G's code at lobby 0x584E10 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_inputMsgForHTML();
extern u8 cw;
s32 Disp_back();                                /* extern */
s32 font_set_stack_no();                 /* extern */
s32 nwDispStr_Html();      /* extern */

void lobby_func_00584E10(void) {
    u8 *temp_a0;

    Disp_back();
    font_set_stack_no(5);
    nwDispStr_Html((*(void **)&cw) + 0xBD6, 0x42C80000, 0x42700000);
    temp_a0 = (*(void **)&cw);
    if (((*(u8 *)((u8 *)(temp_a0) + 0xBC4)) == 0) && ((*(s8 *)((u8 *)(temp_a0) + 0xB71)) == 0) && ((*(s8 *)((u8 *)(temp_a0) + 0xB72)) == 0) && ((*(s8 *)((u8 *)(temp_a0) + 0xB73)) == 0)) {
        Lb_put_inputMsgForHTML();
    }
}

