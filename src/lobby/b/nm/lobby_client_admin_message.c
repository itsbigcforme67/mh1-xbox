/* lobby_client_admin_message (0x5BEF30): 7/85 differ: cw reload register. if-chain instead of switch. Not built. */
#include "lobby_a.h"
extern char lbc_admin_message_jmp_3260[];
s32 lobby_client_admin_message(void) {
    s8 sx1;
    u8 temp_v1;
    int temp_a0;
    int temp_a0_2;
    int temp_v1_2;

    temp_a0 = (int)cw;
    if (F(u8, temp_a0, 0x2C5C) == 0) {
        return 0;
    }
    if (F(s8, temp_a0, 0x2C08) == 0) {
        return 0;
    }
    temp_v1 = F(u8, temp_a0, 0x2C31);
    if (temp_v1 == 0) return 0;
    if (temp_v1 == 5) return 0;
    if (temp_v1 == 4) return 0;
    {
        if ((F(u8, temp_a0, 0x35D5) != 0) && ((sx1 = SoftKeyboard_alive_check(temp_a0)) != 0)) {
            return 0;
        }
        temp_v1_2 = (int)cw;
        if (F(s8, temp_v1_2, 0x2C0C) != 0) {
            return 0;
        }
        if (F(s8, temp_v1_2, 0x2C30) != 0) {
            F(s8, temp_v1_2, 0x2C30) = 0;
            F(u8, (u8 *)cw, 0x2F79) = 0xFF;
            F(u8, (u8 *)cw, 0x2F78) = 0U;
            cnLbc_EraseDialog(0x4C);
        }
        temp_a0_2 = (int)cw;
        if ((F(u8, temp_a0_2, 0x2F78) != 0) && (F(u8, temp_a0_2, 0x2C5C) != 2)) {
            return 0;
        }
        ((int (**)())&lbc_admin_message_jmp_3260)[F(u8, temp_a0_2, 0x2F6E)](temp_a0_2);
        return 1;
    }
}
