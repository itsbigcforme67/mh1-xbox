/* G online (PC): lm_net_status_i, written from Monster Hunter G's code at lobby 0x58D040 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lbc_init_network_work();
s32 Lbs_InRoomCheck();
extern u8 pNet;
extern u8 my_user_mini_data;

s32 lm_net_status_i(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (Lbs_InRoomCheck(arg0, arg1, arg2, arg3) == 1) {
        return 1;
    }
    Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) = (u8) (*(u8 *)((u8 *)(&my_user_mini_data) + 2));
    return 0;
}

