/* lb_bz132 - lobby UI/client 0x005C2690-0x005C26DC: cnLbc_MoveMenuServerSelect (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s8 ret_stat_0038A904;
extern char server_select_jmp_189[];

s8 cnLbc_MoveMenuServerSelect(int arg0) {
    ret_stat_0038A904 = -1;
    ((int (**)())&server_select_jmp_189)[F(u8, arg0, 2)]();
    lbc_text_lobby_trans(&network_work);
    return ret_stat_0038A904;
}
