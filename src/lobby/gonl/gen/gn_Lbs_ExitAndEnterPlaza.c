/* G online (PC): Lbs_ExitAndEnterPlaza, written from Monster Hunter G's code at lobby 0x57DBB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 cnLBS_Read_PlazaAllocation();
extern u8 cw;
s32 Lbs_ExitAndEnterPlaza(void) {
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_v1_2 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 9;
        cnLBS_Read_PlazaAllocation(0, 0xF);
block_13:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait();
        goto block_13;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = 0U;
        return 1;
    }
}

