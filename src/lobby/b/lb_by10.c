/* lb_by10 - agent B promoted near-match 0x005B6600-0x005B6710: server_select_03 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s32 lbs_select_timer;
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;
extern u8 * pNet;
extern u8 COM_R_No_1;
extern char ConnectLbsId[];
extern char BsLbsInfo[];

s32 server_select_03(void) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s0;

    var_s0 = 0;
    if (CpInetGetStatus() != 0) {
        COM_R_No_2 = 0;
        COM_R_No_1 = 5U;
        COM_R_No_3 = 0;
    }
    if (*(u16 *)0x3F3710 != 0) {
        cnWrap_IsBBConnect();
        lbs_select_timer = 0x8CA0;
    }
    temp_v0 = lbs_select_timer - 1;
    lbs_select_timer = temp_v0;
    if (temp_v0 < 0) {
        COM_R_No_3 = 0;
        COM_R_No_1 = 5U;
        COM_R_No_2 = 1;
        return 0;
    }
    temp_v0_2 = cnLbc_MoveMenuServerSelect(&network_work);
    switch (temp_v0_2) {                            /* irregular */
    case 0:
        memcpy(&ConnectLbsId, (u8 *)&BsLbsInfo + (F(u8, pNet, 8) * 0x102), 0xC);
        var_s0 = 1;
        break;
    case -1:
        break;
    case -2:
        COM_R_No_1 = (u8) (COM_R_No_1 + 1);
        break;
    }
    return var_s0;
}
