#include "lobby_a.h"
typedef struct { u8 pad0[0x2C31]; s8 x2C31; s8 x2C32; s8 x2C33; s8 x2C34; u8 x2C35; u8 pad2C36[0x16]; s32 x2C4C; u8 pad2C50[0x66F]; u8 x32BF; } CWS_l300;
#define CWX ((CWS_l300 *)cw)
void lbc_in_lobby_03_00(void) {
    u16 sw;
    sw = Get_sw2(0);
    switch (CWX->x2C35) {
    case 0:
        CWX->x2C35++;
        CWX->x2C4C = 0;
        nwSetEff_FreeDialog(0x14, (u8 *)cw + 0x32D1);
        return;
    case 1:
        if (CWX->x2C4C++ >= 0x258 || (CWX->x2C4C >= 0x3C && (sw & 0x60))) {
            switch (CWX->x32BF) {
            case 0:
                CWX->x2C31 = 3;
                CWX->x2C32 = 0;
                CWX->x2C33 = 0;
                CWX->x2C34 = 1;
                CWX->x2C35 = 0;
                break;
            case 1:
                CWX->x2C31 = 3;
                CWX->x2C32 = 0;
                CWX->x2C33 = 0;
                CWX->x2C34 = 1;
                CWX->x2C35 = 0;
                break;
            case 2:
                CWX->x2C31 = 3;
                CWX->x2C32 = 0;
                CWX->x2C33 = 0;
                CWX->x2C34 = 1;
                CWX->x2C35 = 0;
                break;
            case 3:
                CWX->x2C31 = 3;
                CWX->x2C32 = 0;
                CWX->x2C33 = 0;
                CWX->x2C34 = 1;
                CWX->x2C35 = 0;
                break;
            }
            cnLbc_EraseDialog(0x4C);
        }
    }
}
