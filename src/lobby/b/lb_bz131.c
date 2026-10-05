/* lb_bz131 - lobby UI/client 0x005C1F20-0x005C1F70: net_SetMenu_SelectHandleName (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s8 ret_stat_0038A900;
extern char ranking_jmp_175[];

s8 net_SetMenu_SelectHandleName(int arg0) {
    ret_stat_0038A900 = -1;
    ((int (**)())&ranking_jmp_175)[F(u8, arg0, 2)]();
    transSelectHandleName(arg0);
    return ret_stat_0038A900;
}
