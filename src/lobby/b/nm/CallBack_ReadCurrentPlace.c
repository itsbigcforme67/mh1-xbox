#include "lobby_a.h"
extern char temp_a1[];
extern char sp10[];
extern char temp_a1[];
extern char temp_a1[];
extern char sp10[];
extern char temp_v1[];
extern char temp_v1[];
extern char temp_a1[];
extern char ClassInfo[];
extern char ClassInfo[];
extern char ClassInfo[];
extern char ClassInfo[];
extern char temp_a0[];
extern char temp_a2[];
extern char sp1A[];
extern char temp_a2[];
void CallBack_ReadCurrentPlace(int arg0) {
    u16 sp18;
    long long sp10;
    u8 temp_a0;
    int temp_a1;
    int temp_a2;
    int temp_v1;

    temp_a1 = (int)cw;
    sp10 = arg0;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (temp_a0 = F(u8, temp_a1, 0x2C45), temp_a2 = temp_a1 + 0x2C45, (temp_a0 == 1))) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if ((s8) sp10 == 0) {
            temp_v1 = (int)cw;
            F(u8, temp_v1, 0x2C32) = (u8) (F(u8, temp_v1, 0x2C32) + 1);
            cnLBS_Get_CurrentPlace(&sp18, temp_a1, temp_a2);
            F(u8, &ClassInfo, 0) = (u8) sp18;
            F(u8, &ClassInfo, 4) = (u8) sp1A;
            if (F(u8, &ClassInfo, 0) != 0) {
                if (F(u8, &ClassInfo, 4) == 0) {
                    goto block_6;
                }
                F(s8, (u8 *)cw, 0x35D2) = 1;
                return;
            }
block_6:
            To_TopMenu(sp18);
            goto block_10;
        }
        To_TopMenu((u16) temp_a0, temp_a1, temp_a2);
block_10:
        F(s8, (u8 *)cw, 0x35D2) = 0;
    }
}
