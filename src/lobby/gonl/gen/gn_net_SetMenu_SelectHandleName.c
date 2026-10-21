/* G online (PC): net_SetMenu_SelectHandleName, written from Monster Hunter G's code at lobby 0x5BA480 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 ret_stat_0038A900;
s32 id_select_00();
s32 id_select_01();
s32 id_select_02();
static s32 (*const ranking_jmp_175_g[])() = { id_select_00, id_select_01, id_select_02 };   /* G's table of functions, by name */
s32 transSelectHandleName();              /* extern */
extern u8 ranking_jmp_175;

s8 net_SetMenu_SelectHandleName(void *arg0) {
    (*(s8 *)&ret_stat_0038A900) = -1;
    ranking_jmp_175_g[(*(u8 *)((u8 *)(arg0) + 0x22))](arg0);
    transSelectHandleName(arg0);
    return (*(s8 *)&ret_stat_0038A900);
}

