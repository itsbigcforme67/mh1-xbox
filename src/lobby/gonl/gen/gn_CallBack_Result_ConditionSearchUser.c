/* G online (PC): CallBack_Result_ConditionSearchUser, written from Monster Hunter G's code at lobby 0x580260 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_ServerMessage();
extern u8 cw;
s32 cnLBS_Get_ConditionSearchUser(); /* extern */
extern s32 g_main_2EFBC4;

void CallBack_Result_ConditionSearchUser(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    sp18 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 0xE)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        cnLBS_Get_ConditionSearchUser(&g_main_2EFBC4, temp_a1, temp_a1 + 0xBA9);
        if ((s8) sp18 == 0) {
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
            return;
        }
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 3U;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
    }
}

