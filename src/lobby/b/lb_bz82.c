/* lb_bz82 - lobby UI/client 0x005B6ED0-0x005B6EFC: cmcs_06 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s8 COM_R_No_1;

void cmcs_06(void) {
    int a = *(u8 *)0x3F34C2;
    COM_R_No_1 = 0;
    AQ_init(a);
    AQSession_init_online();
}
