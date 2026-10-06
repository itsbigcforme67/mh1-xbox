/* lb_lg01 - agent C 0x005B8980-0x005B8B88: lbc_login_finish (login step machine; named cw/CnetWork fields). */
#include "lobby_f.h"
extern u8 BsLbsCount;
extern char D_3E5326[];
extern char D_3E5326[];
extern char CallBack_SendMiniData[];
typedef struct { u8 pad0[2]; u8 x02; u8 pad03[0x12]; u8 x15; u8 pad16[0x2A]; } MINI40;
#define MYMINI ((MINI40 *)&my_user_mini_data)
typedef struct { u8 pad0[5]; u8 x05; u8 pad06[0x26]; } CNW5;
extern CNW5 CnetWork;
typedef struct { u8 pad0000[0x2C33]; s8 x2C33; u8 x2C34; u8 pad2C35[0x10]; s8 x2C45; u8 pad2C46[0x98C]; u8 x35D2; u8 pad35D3[3]; s8 x35D6; s8 x35D7; } CWS_lbc_login_finish;
#define CWX ((CWS_lbc_login_finish *)cw)
void lbc_login_finish(void) {
    MINI40 *mini = (MINI40 *)&my_user_mini_data;

    switch (CWX->x2C34) {
    case 0:
        CallBackWaitInit();
        if (CnetWork.x05 == 0) {
            if (CWX->x35D2 == 0) {
                D_3E5326[game_w.master * 0xA00] = 0x4C;
                *(s8 *)0x3F3404 = 0x4C;
            } else {
                D_3E5326[game_w.master * 0xA00] = 0x4D;
                *(s8 *)0x3F3404 = 0x4D;
            }
            Lb_set_mini_data(&my_user_mini_data);
        }
        if (CnetWork.x05 == 3 || CnetWork.x05 == 0) {
            CWX->x35D7 = 0;
            CWX->x35D6 = 0;
            *(s8 *)0x3F3603 = 0;
            *(s8 *)0x3F3604 = 0;
            *(s8 *)0x3F3605 = 0;
            *(s16 *)0x3F3606 = 0;
            Lb_make_quest_tbl();
        }
        mini->x02 = 0;
        mini->x15 = 0;
        CWX->x2C45 = 2;
        cnLBS_Send_UserMiniData(&my_user_mini_data, 0x40, &CallBack_SendMiniData);
        CWX->x2C34++;
        return;
    case 1:
        Check_CallBackWait();
        return;
    case 2:
        cnLbc_Init_NgServerId();
        if (CnetWork.x05 == 0 && BsLbsCount == 1) {
            CnetWork.x05 = 1;
        }
        if (CnetWork.x05 == 0) {
            CnetWork.x05 = 1;
            To_LogOut(0);
            return;
        }
        CWX->x2C33 = 8;
        CWX->x2C34 = 0;
    }
}
