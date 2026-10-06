#include "lobby_a.h"
extern char text_lobby_trans_ot0[];
void lbc_login_error(void) {
    s32 temp_a2;
    s32 temp_v1_2;
    u8 temp_a1;
    int temp_a0;
    int temp_a1_2;
    int temp_a1_3;
    int temp_v1;

    temp_a0 = (int)cw;
    temp_a1 = F(u8, temp_a0, 0x2C34);
    temp_a2 = Get_sw2(0) & 0xFFFF;
    switch (temp_a1) {                              /* irregular */
    case 0:
        Lbc_init_network_work(1);
        Lbc_set_prim(&text_lobby_trans_ot0, 0, 0);
        temp_v1 = (int)cw;
        F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
        fade_set(2);
        F(s32, (u8 *)cw, 0x2C4C) = 0x258;
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        return;
    case 1:
        F(s8, pNet, 0xC) = 1;
        temp_a1_2 = (int)cw;
        F(s32, temp_a1_2, 0x2C4C) = (F(s32, temp_a1_2, 0x2C4C) - 1);
        temp_a1_3 = (int)cw;
        temp_v1_2 = F(s32, temp_a1_3, 0x2C4C);
        if (temp_v1_2 > 0) {
            if ((temp_v1_2 < 0x1E0) && (temp_a2 & 0xFFFF & 0x20)) {
                F(u8, temp_a1_3, 0x2C34) = (u8) (F(u8, temp_a1_3, 0x2C34) + 1);
                cnWrap_SoundRequest(0);
                fade_set(1);
                return;
            }
            return;
        }
        F(u8, temp_a1_3, 0x2C34) = (u8) (F(u8, temp_a1_3, 0x2C34) + 1);
        fade_set(1);
        return;
    case 2:
        if ((Fade_busy_ck(temp_a0) & 0xFF) != 1) {
            F(s8, pNet, 0x11) = 1;
            F(s8, (u8 *)cw, 0x2C33) = 4;
            F(u8, (u8 *)cw, 0x2C34) = 1U;
        }
        break;
    }
}
