/* lb_bz109 - lobby UI/client 0x005B9D70-0x005B9DEC: CallBack_Result_Plaza_PlazaEntry (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x13]; u8 x2C45; } CWS_CallBack_Result_Plaza_PlazaEntry;

void CallBack_Result_Plaza_PlazaEntry(CNET_RES res) {
    if ((((CWS_CallBack_Result_Plaza_PlazaEntry *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Plaza_PlazaEntry *)cw)->x2C45 == 4)) {
        ((CWS_CallBack_Result_Plaza_PlazaEntry *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            To_EnterPlaza();
            return;
        }
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 0;
        F(s8, (u8 *)cw, 0x2C34) = 0;
        F(s8, (u8 *)cw, 0x2C35) = 0;
    }
}
