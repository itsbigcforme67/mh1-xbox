/* G online (PC): lbc_top_menu_05, written from Monster Hunter G's code at lobby 0x57EC30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 cnLBS_Read_PlazaAllocation();
s32 lobby_func_0057ECE0();
extern u8 cw;
s32 lbc_top_menu_05(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x17;
        cnLBS_Read_PlazaAllocation(0, 0xF, (s32) lobby_func_0057ECE0, arg3);
block_9:
    default:
        return 2;
    case 1:
        Check_CallBackWait(temp_v1 + 0xB9D, arg1, arg2, arg3);
        goto block_9;
    case 2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    }
}

