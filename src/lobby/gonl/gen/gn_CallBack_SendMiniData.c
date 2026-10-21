/* G online (PC): CallBack_SendMiniData, written from Monster Hunter G's code at lobby 0x57B6D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 cnLBS_Get_ServerMessage(); /* extern */
s32 memcpy();         /* extern */
extern u8 my_user_mini_data;

void CallBack_SendMiniData(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_v1;

    temp_a1 = (*(void **)&cw);
    sp18 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 2)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        if ((s8) sp18 == 0) {
            memcpy((*(void **)&cw) + 0x1DC, &my_user_mini_data, 0x40);
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) + 1);
            return;
        }
        temp_v1 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9C)) + 1);
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, temp_a1, temp_a1 + 0xBA9);
    }
}

