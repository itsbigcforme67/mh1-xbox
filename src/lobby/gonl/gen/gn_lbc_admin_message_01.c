/* G online (PC): lbc_admin_message_01, written from Monster Hunter G's code at lobby 0x584FE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_inputMsgForHTML();
s32 cnWrap_SoundRequest();
extern u8 Psw;
extern u8 cw;
s32 cnLBS_AnswerAdminMessage(); /* extern */

void lbc_admin_message_01(s32 unused_a0, s32 unused_a1, s32 unused_a2, s32 arg3) {
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a1;
    u8 *temp_a2;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xED7));
    temp_a2 = temp_a1 + 0xED7;
    switch (temp_a0) {                              /* irregular */
    case 0:
        if ((*(u16 *)((u8 *)&Psw + 0x4)) & 0x20) {
            (*(u8 *)((u8 *)(temp_a1) + 0xED7)) = (u8) (temp_a0 + 1);
            cnWrap_SoundRequest(0, (s32) temp_a1, (s32) temp_a2, arg3);
        }
        Lb_put_inputMsgForHTML(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 1:
        (*(s8 *)((u8 *)(temp_a1) + 0xEDE)) = 1;
        cnLBS_AnswerAdminMessage(temp_a0, temp_a1, temp_a2);
        temp_a0_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_2) + 0xED6)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xED6)) + 1);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xED7)) = 0U;
        return;
    default:
        (*(u8 *)((u8 *)(temp_a1) + 0xED6)) = 2U;
        return;
    }
}

