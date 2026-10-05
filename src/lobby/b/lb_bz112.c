/* lb_bz112 - lobby UI/client 0x005BAE30-0x005BAEC4: CallBack_Result_Plaza_PlazaExit (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x13]; u8 x2C45; } CWS_CallBack_Result_Plaza_PlazaExit;

void CallBack_Result_Plaza_PlazaExit(CNET_RES res) {
    if ((((CWS_CallBack_Result_Plaza_PlazaExit *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Plaza_PlazaExit *)cw)->x2C45 == 9)) {
        ((CWS_CallBack_Result_Plaza_PlazaExit *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            F(u8, (u8 *)cw, 0x2C31) = 1U;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 0;
            F(s8, (u8 *)cw, 0x2C34) = 0;
            Init_InterruptFlag();
            return;
        }
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 0;
        F(s8, (u8 *)cw, 0x2C34) = 0;
    }
}
