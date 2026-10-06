#include "lobby_f.h"
typedef struct { u8 pad0000[0x2C4C]; s32 x2C4C; } CWS_lbc_in_plaza_04;

void lbc_in_plaza_04(void) {
    s32 temp_a2;
    s32 temp_a0;
    temp_a2 = Get_sw2(0) & 0xFFFF;
    temp_a0 = ((CWS_lbc_in_plaza_04 *)cw)->x2C4C;
    ((CWS_lbc_in_plaza_04 *)cw)->x2C4C = (temp_a0 + 1);
    if (temp_a0 < 0x258) {
        if ((F(s32, (u8 *)cw, 0x2C4C) >= 0x3C) && (temp_a2 & 0xFFFF & 0x60)) {
            goto block_4;
        }
    } else {
block_4:
        F(s8, (u8 *)cw, 0x2C33) = 0;
        F(s8, (u8 *)cw, 0x2C34) = 0;
        F(s8, (u8 *)cw, 0x2C3C) = 0;
    }
}
