/* G online (PC): gnet_6205_both, written from Monster Hunter G's code at lobby 0x5E3700 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 _cnet_Return_CallBack();
s32 _sub_ReceiveJoinUser();
extern u8 CnetSys_w;

void gnet_6205_both(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    if ((*(s8 *)((u8 *)&CnetSys_w + 0xFEC)) == 0) {
        _sub_ReceiveJoinUser((s32) &(*(s32 *)((u8 *)&CnetSys_w + 0x43C8)), arg1, arg2, arg3);
    }
    _cnet_Return_CallBack(0x11, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

