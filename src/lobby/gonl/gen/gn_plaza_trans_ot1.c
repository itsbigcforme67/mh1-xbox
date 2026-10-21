/* G online (PC): plaza_trans_ot1, written from Monster Hunter G's code at lobby 0x5797C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispSoftkeyboard();
s32 Lb_on_dialog();
s32 font_set_stack_no();
extern u8 cw;
extern u8 pNet;
s32 DispDialogData();                         /* extern */
s32 SoftKeyboard_alive_check();                     /* extern */

void plaza_trans_ot1(void *arg0) {
    u8 temp_a0;

    font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
    if (((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBC4)) == 0) && (SoftKeyboard_alive_check() != 0)) {
        DispSoftkeyboard(1);
    }
    temp_a0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D));
    if (temp_a0 == 1) {
        DispDialogData(temp_a0);
        Lb_on_dialog(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0U;
    }
}

