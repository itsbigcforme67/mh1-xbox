/* lb_by33 - agent B promoted near-match 0x005BD9E0-0x005BDA94: Lbs_MatchEntry (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char CallBack_Result_InRoom_MatchEntry[];

s32 Lbs_MatchEntry(void) {
    u8 temp_v1;
    int temp_a0;
    int temp_a1;

    temp_a0 = (int)cw;
    temp_v1 = F(u8, temp_a0, 0x2C35);
    temp_a1 = temp_a0 + 0x2C35;
    switch (temp_v1) {                              /* irregular */
    case 0:
        F(u8, temp_a0, 0x2C35) = (u8) (temp_v1 + 1);
        CallBackWaitInit(temp_a0, temp_a1);
        F(s8, (u8 *)cw, 0x2C45) = 0x1C;
        cnLBS_MatchEntry(1, &CallBack_Result_InRoom_MatchEntry);
        break;
    case 1:
        Check_CallBackWait(temp_a0, temp_a1);
        break;
    case 2:
        *(u8 *)0x3F360A = F(u8, temp_a0, 0x32C5);
        F(u8, temp_a0, 0x2C35) = 0U;
        return 0;
    case 3:
        return 1;
    }
    return 2;
}
