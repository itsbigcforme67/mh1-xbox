/* lb_bz21 - lobby UI/client 0x005B5C70-0x005B5C88: connect_00 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 COM_R_No_1;
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;

s32 connect_00(void) {
    COM_R_No_2 = 0;
    COM_R_No_1 = 1;
    COM_R_No_3 = 0;
    return 0;
}
