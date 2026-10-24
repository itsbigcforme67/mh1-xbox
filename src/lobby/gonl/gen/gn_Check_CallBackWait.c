/* G online (PC): Check_CallBackWait, written from Monster Hunter G's code at lobby 0x579BF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 To_LogOut();
extern u8 cw;
s32 Check_CallBackWait(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    u8 *temp_v1;
    u8 *temp_v1_2;

    temp_v1 = (*(void **)&cw);
    (*(s32 *)((u8 *)(temp_v1) + 0xFA68)) = (s32) ((*(s32 *)((u8 *)(temp_v1) + 0xFA68)) - 1);
    temp_v1_2 = (*(void **)&cw);
    if ((*(s32 *)((u8 *)(temp_v1_2) + 0xFA68)) < 0) {
        (*(s8 *)((u8 *)(temp_v1_2) + 0xFA65)) = 1;
        To_LogOut(4, arg1, arg2, arg3);
        return 1;
    }
    return 0;
}

