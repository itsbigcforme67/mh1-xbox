/* G online (PC): CallBack_Result_Match_MatchInformation, written from Monster Hunter G's code at lobby 0x583D50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_ServerMessage();
extern u8 cw;
s32 all_reset();              /* extern */

void CallBack_Result_Match_MatchInformation(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    sp18 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && (temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xBA9)), (temp_a0 == 0x30))) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        if ((s8) sp18 == 0) {
            all_reset(temp_a0, temp_a1, temp_a1 + 0xBA9);
            temp_a0_2 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) + 1);
            return;
        }
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB75)) = 1;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
    }
}

