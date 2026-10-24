/* G online (PC): lbc_matching_failed_01, written from Monster Hunter G's code at lobby 0x584110 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_eat_to_bell();
s32 SetDialogData();
s32 fade_set();
extern u8 cw;
extern u8 pNet;
void lbc_matching_failed_01(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    u8 *temp_a0;
    u8 *temp_v1;

    temp_a0 = (*(void **)&cw);
    (*(u8 *)((u8 *)(temp_a0) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9B)) + 1);
    (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_v1 = (*(void **)&cw);
    if ((*(u8 *)((u8 *)(temp_v1) + 0xB9A)) == 0) {
        SetDialogData(0x58, arg1, arg2, arg3);
    } else {
        Lb_eat_to_bell((s32) (temp_v1 + 0xF75D), arg1, arg2, arg3);
    }
    (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x14;
    fade_set(2);
}

