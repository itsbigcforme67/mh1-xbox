/* G online (PC): Lbs_SeekId, written from Monster Hunter G's code at lobby 0x5805D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 CallBackWaitInit();                      /* extern */
s32 Check_CallBackWait();                    /* extern */
s32 cnLBS_Get_SerchUserPlace(); /* extern */
s32 cnLBS_Get_SerchUserPlaceMessage();    /* extern */
s32 cnLBS_Get_ServerMessage();       /* extern */
s32 cnLBS_SerchUserPlace();   /* extern */
extern u8 CallBack_Result_SearchUserPlace;

s32 Lbs_SeekId(s64 arg0) {
    s32 temp_a2;
    s32 temp_s0;
    u8 temp_v1_2;
    u8 *temp_v0;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a2 = temp_v1 + 0xB9D;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a2);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0xE;
        cnLBS_SerchUserPlace(M2C_ERROR(/* Read from unset register $a1 */), &CallBack_Result_SearchUserPlace);
block_10:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait(temp_a2);
        goto block_10;
    case 0x2:
        temp_s0 = (((s16) (arg0))) * 0x24A;
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        (*(s8 *)((u8 *)((temp_s0 + (*(void **)&cw))) + 0x1062)) = 1;
        temp_v0 = (*(void **)&cw) + temp_s0;
        cnLBS_Get_SerchUserPlace(temp_v0 + 0x105C, temp_v0 + 0x105E, temp_v0 + 0x1060, temp_v0 + 0x1063);
        cnLBS_Get_SerchUserPlaceMessage((*(void **)&cw) + temp_s0 + 0x1064);
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, temp_a2);
        return 1;
    }
}

