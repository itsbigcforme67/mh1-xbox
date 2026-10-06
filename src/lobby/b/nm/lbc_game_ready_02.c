#include "lobby_a.h"
extern s8 net_char_change;
void lbc_game_ready_02(int arg0, int arg1, s32 arg2) {
    s32 var_a2;
    s32 var_a3;
    u8 temp_v0;
    u8 temp_v1_2;
    int temp_a0;
    int temp_v1;

    var_a2 = arg2;
    temp_v1 = (int)cw;
    temp_v1_2 = F(u8, temp_v1, 0x2C34);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C34) = (u8) (temp_v1_2 + 1);
        F(s8, (u8 *)cw, 0x2C48) = 0;
        F(s8, (u8 *)cw, 0x2C49) = 0;
        var_a3 = 0;
        if (F(u8, (u8 *)cw, 0x2C47) > 0) {
            var_a2 = 0;
            do {
                temp_v0 = F(u8, ((u8 *)cw + var_a2), 0x7B1);
                if (temp_v0 == 1) {
                    F(u8, (u8 *)cw, 0x2C48) = (u8) (F(u8, (u8 *)cw, 0x2C48) + 1);
                } else if (temp_v0 == 2) {
                    F(u8, (u8 *)cw, 0x2C49) = (u8) (F(u8, (u8 *)cw, 0x2C49) + 1);
                }
                var_a3 += 1;
                var_a2 += 0x2FC;
            } while (var_a3 < F(u8, (u8 *)cw, 0x2C47));
        }
        cnLbc_LoadNetModel(2, (u8 *)cw, var_a2, var_a3);
        net_char_change = 1;
        return;
    case 1:
        if (cnLbc_LoadModelWait(2, temp_v1 + 0x2C34) != 0) {
            temp_a0 = (int)cw;
            F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
            F(u8, (u8 *)cw, 0x2C34) = 0U;
        }
        return;
    }
}
