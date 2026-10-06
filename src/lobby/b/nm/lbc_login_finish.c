#include "lobby_f.h"
extern u8 BsLbsCount;
extern char CnetWork[];
extern char D_3E5326[];
extern char D_3E5326[];
extern char CnetWork[];
extern char CnetWork[];
extern char CallBack_SendMiniData[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
typedef struct { u8 pad0000[0x2C34]; u8 x2C34; } CWS_lbc_login_finish;
void lbc_login_finish(void) {
    s8 var_a1;
    u8 temp_a0;

    temp_a0 = F(u8, (u8 *)cw, 0x2C34);
    switch (temp_a0) {                              /* irregular */
    case 0:
        CallBackWaitInit();
        if (F(u8, &CnetWork, 5) == 0) {
            if (F(u8, (u8 *)cw, 0x35D2) == 0) {
                var_a1 = 0x4C;
                *((u8 *)&D_3E5326 + (game_w.master * 0xA00)) = 0x4C;
            } else {
                var_a1 = 0x4D;
                *((u8 *)&D_3E5326 + (game_w.master * 0xA00)) = 0x4D;
            }
            *(s8 *)0x3F3404 = var_a1;
            Lb_set_mini_data(&my_user_mini_data, var_a1);
        }
        if (F(u8, &CnetWork, 5) != 3) {
            if (F(u8, &CnetWork, 5) == 0) {
                goto block_12;
            }
        } else {
block_12:
            F(s8, (u8 *)cw, 0x35D7) = 0;
            F(s8, (u8 *)cw, 0x35D6) = 0;
            *(s8 *)0x3F3603 = 0;
            *(s8 *)0x3F3604 = 0;
            *(s8 *)0x3F3605 = 0;
            *(s16 *)0x3F3606 = 0;
            Lb_make_quest_tbl();
        }
        F(s8, &my_user_mini_data, 2) = 0;
        F(s8, &my_user_mini_data, 0x15) = 0;
        F(s8, (u8 *)cw, 0x2C45) = 2;
        cnLBS_Send_UserMiniData(&my_user_mini_data, 0x40, &CallBack_SendMiniData);
        ((CWS_lbc_login_finish *)cw)->x2C34 = (u8) (((CWS_lbc_login_finish *)cw)->x2C34 + 1);
        return;
    case 1:
        Check_CallBackWait();
        return;
    case 2:
        cnLbc_Init_NgServerId();
        if ((F(u8, &CnetWork, 5) == 0) && (BsLbsCount == 1)) {
            F(u8, &CnetWork, 5) = 1U;
        }
        if (F(u8, &CnetWork, 5) == 0) {
            F(u8, &CnetWork, 5) = 1U;
            To_LogOut(0);
            return;
        }
        F(s8, (u8 *)cw, 0x2C33) = 8;
        F(u8, (u8 *)cw, 0x2C34) = 0U;
        return;
    }
}
