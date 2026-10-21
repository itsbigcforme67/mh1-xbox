/* G online (PC): lobby_func_005E3000, written from Monster Hunter G's code at lobby 0x5E3000 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005E3300();
s32 lobby_func_005E33B0();
s32 __cnetSub_Get_RestBgWork();                     /* extern */

void lobby_func_005E3000(void *arg0) {
    if (__cnetSub_Get_RestBgWork() > 0) {
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        lobby_func_005E33B0((s32) lobby_func_005E3300, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    }
}

