/* G online (PC): lbc_matching_failed_01, written from Monster Hunter G's code at lobby 0x584110 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 pNet;
s32 Lb_eat_to_bell();                     /* extern */
s32 SetDialogData();                     /* extern */
s32 fade_set();                          /* extern */

void lbc_matching_failed_01(void) {
    u8 *temp_a0;
    u8 *temp_v1;

    temp_a0 = (*(void **)&cw);
    (*(u8 *)((u8 *)(temp_a0) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9B)) + 1);
    (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_v1 = (*(void **)&cw);
    if ((*(u8 *)((u8 *)(temp_v1) + 0xB9A)) == 0) {
        SetDialogData(0x58);
    } else {
        Lb_eat_to_bell(temp_v1 + 0xF75D);
    }
    (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x14;
    fade_set(2);
}

