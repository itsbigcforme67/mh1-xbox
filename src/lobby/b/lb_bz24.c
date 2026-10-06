/* lb_bz24 - lobby UI/client 0x005B65C0-0x005B65F4: server_select_02 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s32 lbs_select_timer;
extern s8 COM_R_No_2;
extern u8 COM_R_No_1;

s32 server_select_02(void) {
    COM_R_No_2 = 0;
    COM_R_No_1 = (u8) (COM_R_No_1 + 1);
    cnWrap_IsBBConnect();
    lbs_select_timer = 0x8CA0;
    return 0;
}
