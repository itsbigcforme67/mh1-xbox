/* G online (PC): lobby_func_0057FEA0, written from Monster Hunter G's code at lobby 0x57FEA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 CallBack_Result_ConditionSearchUser();
s32 Check_CallBackWait();
s32 lobby_func_005E64F0();
extern u8 cw;
s32 lobby_func_0057FEA0(s32 arg0) {
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0xE;
        lobby_func_005E64F0(M2C_ERROR(/* Read from unset register $a1 */), (s32) CallBack_Result_ConditionSearchUser);
block_11:
    default:
        return 2;
    case 1:
        Check_CallBackWait();
        goto block_11;
    case 2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 3:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

