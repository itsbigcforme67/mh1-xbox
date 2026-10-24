/* G online (PC): text_lobby_trans_ot3, written from Monster Hunter G's code at lobby 0x57A8A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_on_dialog();
s32 font_set_stack_no();
extern u8 pNet;
s32 DispDialogData();                         /* extern */

void text_lobby_trans_ot3(void *arg0) {
    u8 temp_a0;

    font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
    temp_a0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D));
    if (temp_a0 == 1) {
        DispDialogData(temp_a0);
        Lb_on_dialog(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    }
}

