/* lb_bz27 - lobby UI/client 0x005B6F00-0x005B6F18: cmcs_99 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 COM_R_No_1;
extern s8 mcs_connect_flag;
extern s8 net_game_invalid_flag;

void cmcs_99(void) {
    COM_R_No_1 = 0;
    net_game_invalid_flag = 3;
    mcs_connect_flag = 1;
}
