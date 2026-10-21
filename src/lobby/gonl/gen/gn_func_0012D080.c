/* G online (PC): func_0012D080, written from Monster Hunter G's code at lobby 0x12D080 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Pl_chat_act_req();
s32 func_001193E0();
extern void *lpSKey;

s32 func_0012D080(void) {
    u8 temp_a1;
    u8 temp_v0;
    u8 temp_v1;

    temp_a1 = (*(u8 *)((u8 *)(lpSKey) + 0x667));
    if (temp_a1 & 0x22) {
        temp_v1 = (*(u8 *)((u8 *)(lpSKey) + 0x666));
        if (((s32) temp_v1 >= 0x3A) && ((s32) temp_v1 < 0x46)) {
            Pl_chat_act_req((temp_v1 - 0x39) & 0xFF);
            func_001193E0(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 0;
        }
    }
    if (temp_a1 & 0x55) {
        return 1;
    }
    temp_v0 = (*(u8 *)((u8 *)(lpSKey) + 0x666));
    if (temp_v0 == 0) {
        return 0;
    }
    return temp_v0 != 0x29;
}

