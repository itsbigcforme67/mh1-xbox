/* lb_bz28 - lobby UI/client 0x005B7020-0x005B70C4: internet_connect_minimum_cleanup, CallBackWaitInit, Check_CallBackWait (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char D_4E36F4[];

s32 internet_connect_minimum_cleanup(void) {
    CpInetTcpAbort(*(s32 *)0x4E36F4);
    CpInetTcpDelete(&D_4E36F4);
    return 2;
}

void CallBackWaitInit(void) {
    F(s32, (u8 *)cw, 0x35DC) = 0xE10;
    F(s8, (u8 *)cw, 0x35D9) = 0;
}

s32 Check_CallBackWait(void) {
    void *temp_v1;
    void *temp_v1_2;

    temp_v1 = (u8 *)cw;
    F(s32, temp_v1, 0x35DC) = (F(s32, temp_v1, 0x35DC) - 1);
    temp_v1_2 = (u8 *)cw;
    if (F(s32, temp_v1_2, 0x35DC) < 0) {
        F(s8, temp_v1_2, 0x35D9) = 1;
        To_LogOut(4);
        return 1;
    }
    return 0;
}
