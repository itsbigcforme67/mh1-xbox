/* G online (PC): net_SetMenu_SelectHandleName, written from Monster Hunter G's code at lobby 0x5BA480 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 transSelectHandleName();
extern u8 ret_stat_0038A900;
extern u8 ranking_jmp_175;

s8 net_SetMenu_SelectHandleName(void *arg0) {
    (*(s8 *)&ret_stat_0038A900) = -1;
    *(&ranking_jmp_175 + ((*(u8 *)((u8 *)(arg0) + 0x22)) * 4))();
    transSelectHandleName(arg0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    return (*(s8 *)&ret_stat_0038A900);
}

