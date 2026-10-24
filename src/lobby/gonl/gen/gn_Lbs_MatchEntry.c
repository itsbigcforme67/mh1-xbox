/* G online (PC): Lbs_MatchEntry, written from Monster Hunter G's code at lobby 0x583770 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_InRoom_MatchEntry();
s32 Check_CallBackWait();
s32 cnLBS_MatchEntry();
extern u8 D_0035EF10;
extern u8 cw;
s32 Lbs_MatchEntry(s32 unused_a0, s32 unused_a1, s32 arg2, s32 arg3) {
    u8 temp_v1;
    u8 *temp_a0;

    temp_a0 = (*(void **)&cw);
    temp_v1 = (*(u8 *)((u8 *)(temp_a0) + 0xB9D));
    switch (temp_v1) {                              /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) (temp_v1 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x2C;
        cnLBS_MatchEntry(1, (s32) CallBack_Result_InRoom_MatchEntry);
block_9:
    default:
        return 2;
    case 1:
        Check_CallBackWait((s32) temp_a0, (s32) (temp_a0 + 0xB9D), arg2, arg3);
        goto block_9;
    case 2:
        (*(u8 *)&D_0035EF10) = (*(u8 *)((u8 *)(temp_a0) + 0xF751));
        (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = 0U;
        return 0;
    case 3:
        return 1;
    }
}

