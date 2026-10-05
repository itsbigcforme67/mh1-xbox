#include "lobby_a.h"

void lbc_admin_message_01(void) {
    s32 temp_a3;
    u8 temp_v1;
    int temp_a0;
    int temp_a1;
    int temp_a1_2;
    int temp_a2;

    temp_a1 = (int)cw;
    temp_a3 = Get_sw2(0) & 0xFFFF;
    temp_v1 = F(u8, temp_a1, 0x2F6F);
    temp_a2 = temp_a1 + 0x2F6F;
    switch (temp_v1) {                              /* irregular */
    case 0:
        if (temp_a3 & 0xFFFF & 0x20) {
            F(u8, temp_a1, 0x2F6F) = (u8) (temp_v1 + 1);
            cnWrap_SoundRequest(0, temp_a1, temp_a2, temp_a3);
            F(s16, (u8 *)cw, 0x2F74) = 8;
            cnLbc_EraseDialog(0x4C);
        }
        Lb_put_inputMsgForHTML();
        return;
    case 1:
        F(s16, temp_a1, 0x2F74) = (s16) (F(s16, temp_a1, 0x2F74) - 1);
        temp_a1_2 = (int)cw;
        if (F(s16, temp_a1_2, 0x2F74) < 0) {
            F(s8, temp_a1_2, 0x2F76) = 1;
            cnLBS_AnswerAdminMessage(1, temp_a1_2, temp_a2, temp_a3);
            temp_a0 = (int)cw;
            F(u8, temp_a0, 0x2F6E) = (u8) (F(u8, temp_a0, 0x2F6E) + 1);
            F(u8, (u8 *)cw, 0x2F6F) = 0U;
        }
        return;
    }
}
