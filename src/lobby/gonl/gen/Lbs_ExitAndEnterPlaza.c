/* G online (PC): Lbs_ExitAndEnterPlaza, written from Monster Hunter G's code at lobby 0x57DBB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 CallBackWaitInit();                      /* extern */
s32 Check_CallBackWait();                    /* extern */
s32 cnLBS_Read_PlazaAllocation(); /* extern */
extern u8 CallBack_Result_Plaza_ReadAllocation;

s32 Lbs_ExitAndEnterPlaza(void) {
    s32 temp_a0;
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a0 = temp_v1 + 0xB9C;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a0);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 9;
        cnLBS_Read_PlazaAllocation(0, 0xF, &CallBack_Result_Plaza_ReadAllocation);
block_13:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait(temp_a0);
        goto block_13;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = 0U;
        return 1;
    }
}

