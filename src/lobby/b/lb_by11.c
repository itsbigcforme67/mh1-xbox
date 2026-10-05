/* lb_by11 - agent B promoted near-match 0x005BF370-0x005BF4DC: Check_InterruptFlag (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 Check_InterruptFlag(void) {
    s8 temp_v1;
    u8 temp_v1_2;
    int temp_a1;

    temp_a1 = (int)cw;
    if (F(s8, temp_a1, 0x2C08) != 0) {
        if (F(s8, temp_a1, 0x2C0C) != 0) {
            To_ReadyBattle();
            return 1;
        }
        temp_v1 = F(s8, temp_a1, 0x2C0D);
        if (temp_v1 == 1) {
            To_MatchingFailed(0, temp_a1);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            return 1;
        }
        if (temp_v1 == 2) {
            To_MatchingFailed(1, temp_a1);
            return 1;
        }
        temp_v1_2 = F(u8, temp_a1, 0x2C31);
        if (temp_v1_2 == 2) {
            if (F(s8, temp_a1, 0x2C09) == 1) {
                To_PlazaExit(1, temp_a1);
                return 1;
            }
            goto block_20;
        }
        if ((temp_v1_2 == 3) && (F(u8, temp_a1, 0x35D5) == 1)) {
            if (F(u8, temp_a1, 0x35D3) == 0) {
                if (F(s8, temp_a1, 0x2C0A) == 1) {
                    To_LobbyExit(1, temp_a1);
                    return 1;
                }
                goto block_20;
            }
            if (F(s8, temp_a1, 0x2C0B) == 1) {
                SetDialogData_HTML(temp_a1 + 0x32D1, temp_a1);
                *(s8 *)0x3F36AB = 0;
                F(s32, &lb_sys, 0x68) = 0x15;
                F(s32, &lb_sys, 0x6C) = 1;
                F(s8, (u8 *)cw, 0x2C45) = 0;
                F(s8, (u8 *)cw, 0x2C35) = 0;
                Lbc_init_network_work();
                Lbc_set_prim(0, 0, 0);
                Init_InterruptFlag();
                return 1;
            }
            goto block_20;
        }
        goto block_20;
    }
block_20:
    return 0;
}
