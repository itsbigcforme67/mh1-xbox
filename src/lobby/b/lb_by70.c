/* lb_by70 - agent B promoted near-match 0x005BE580-0x005BE604: To_LogOut (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char CnetWork[];

void To_LogOut(arg0)
u8 arg0;
{
    void *temp_a0;
    void *temp_a2;

    F(s8, (u8 *)cw, 0x2C31) = 5;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = arg0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    temp_a2 = (u8 *)cw;
    F(s8, temp_a2, 0x2C46) = arg0;
    temp_a0 = (u8 *)cw;
    F(s16, temp_a0, 0x35FC) = -1;
    if (arg0 == 7) {
        F(s8, &CnetWork, 6) = 1;
        cnLbc_Set_NgServerId(temp_a0, -1, temp_a2, 5);
        return;
    }
    cnLbc_Init_NgServerId(temp_a0, -1, temp_a2, 5);
}
