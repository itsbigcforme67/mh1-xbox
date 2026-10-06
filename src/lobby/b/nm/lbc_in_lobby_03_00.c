#include "lobby_a.h"

void lbc_in_lobby_03_00(void) {
    s32 temp_a0_2;
    s32 temp_a3;
    u8 temp_a0;
    u8 temp_a1_2;
    int temp_a1;
    int temp_a2;

    temp_a1 = (int)cw;
    temp_a3 = Get_sw2(0) & 0xFFFF;
    temp_a0 = F(u8, temp_a1, 0x2C35);
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        F(u8, temp_a1, 0x2C35) = (u8) (temp_a0 + 1);
        F(s32, (u8 *)cw, 0x2C4C) = 0;
        nwSetEff_FreeDialog(0x14, (u8 *)cw + 0x32D1, temp_a1 + 0x2C35, temp_a3);
        return;
    case 1:                                         /* switch 1 */
        temp_a0_2 = F(s32, temp_a1, 0x2C4C);
        F(s32, temp_a1, 0x2C4C) = (temp_a0_2 + 1);
        if (temp_a0_2 < 0x258) {
            if ((F(s32, (u8 *)cw, 0x2C4C) >= 0x3C) && (temp_a3 & 0xFFFF & 0x60)) {
                goto block_8;
            }
        } else {
block_8:
            temp_a2 = (int)cw;
            temp_a1_2 = F(u8, temp_a2, 0x32BF);
            switch (temp_a1_2) {                    /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                F(s8, temp_a2, 0x2C31) = 3;
                F(s8, (u8 *)cw, 0x2C32) = 0;
                F(s8, (u8 *)cw, 0x2C33) = 0;
                F(s8, (u8 *)cw, 0x2C34) = 1;
block_19:
                F(s8, (u8 *)cw, 0x2C35) = 0;
                break;
            case 1:                                 /* switch 2 */
                F(s8, temp_a2, 0x2C31) = 3;
                F(s8, (u8 *)cw, 0x2C32) = 0;
                F(s8, (u8 *)cw, 0x2C33) = 0;
                F(s8, (u8 *)cw, 0x2C34) = 1;
                goto block_19;
            case 2:                                 /* switch 2 */
                F(s8, temp_a2, 0x2C31) = 3;
                F(s8, (u8 *)cw, 0x2C32) = 0;
                F(s8, (u8 *)cw, 0x2C33) = 0;
                F(s8, (u8 *)cw, 0x2C34) = 1;
                goto block_19;
            case 3:                                 /* switch 2 */
                F(s8, temp_a2, 0x2C31) = 3;
                F(s8, (u8 *)cw, 0x2C32) = 0;
                F(s8, (u8 *)cw, 0x2C33) = 0;
                F(s8, (u8 *)cw, 0x2C34) = 1;
                goto block_19;
            }
            cnLbc_EraseDialog(0x4C, temp_a1_2, temp_a2, temp_a3);
        }
    }
}
