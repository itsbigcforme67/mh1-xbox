/* G online (PC): CallBack_ReadCurrentPlace, written from Monster Hunter G's code at lobby 0x57D140 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). HAND-FIXED: the current place's three u16 as an array. */
#include "../gonl.h"
extern u8 cw;

s32 To_TopMenu();            /* extern */
s32 cnLBS_Get_CurrentPlace(); /* extern */
extern u8 ClassInfo;

void CallBack_ReadCurrentPlace(s64 arg0) {
    u16 sp18[4];
    s64 sp10;
    u8 temp_a0;
    u8 *temp_a1;
    u8 *temp_a2;
    u8 *temp_v1;
    u8 *var_at;

    temp_a1 = (*(void **)&cw);
    sp10 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && (temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xBA9)), temp_a2 = temp_a1 + 0xBA9, (temp_a0 == 1))) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        if ((s8) sp10 == 0) {
            temp_v1 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1) + 0xB9A)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9A)) + 1);
            cnLBS_Get_CurrentPlace(sp18, temp_a1, temp_a2);
            ClassInfo = (u8) sp18[0];
            (*(u8 *)((u8 *)&ClassInfo + 0x4)) = (u8) sp18[1];
            if (ClassInfo != 0) {
                if ((*(u8 *)((u8 *)&ClassInfo + 0x4)) == 0) {
                    goto block_6;
                }
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5E)) = 1;
                return;
            }
block_6:
            To_TopMenu(sp18);
            var_at = (*(void **)&cw) + 0x10000;
            goto block_10;
        }
        To_TopMenu((u16) temp_a0, temp_a1, temp_a2);
        var_at = (*(void **)&cw) + 0x10000;
block_10:
        (*(s8 *)((u8 *)(var_at) + -0x5A2)) = 0;
    }
}

