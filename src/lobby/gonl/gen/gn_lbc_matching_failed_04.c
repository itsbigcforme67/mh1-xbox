/* G online (PC): lbc_matching_failed_04, written from Monster Hunter G's code at lobby 0x584260 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lbs_load();
s32 To_GoToTop();
s32 lobby_func_0055A140();
extern u8 cw;
extern u8 pNet;
s32 all_reset();                                /* extern */
s32 cnWrap_BgmRequest();                 /* extern */

void lbc_matching_failed_04(void) {
    all_reset();
    Lbs_load(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
    To_GoToTop(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    cnWrap_BgmRequest(0);
    lobby_func_0055A140(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

