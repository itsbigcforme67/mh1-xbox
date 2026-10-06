/* lb_by32 - agent B promoted near-match 0x005BD880-0x005BD93C: Lbs_MatchEntryCancel (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char CallBack_Result_InRoom_MatchEntryCancel[];

s32 Lbs_MatchEntryCancel(void) {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0x1D;
        cnLBS_MatchEntry(0, &CallBack_Result_InRoom_MatchEntryCancel);
        break;
    case 1:
        Check_CallBackWait(temp_a0);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}
