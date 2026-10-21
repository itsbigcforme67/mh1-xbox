/* G online (PC): lobby_func_00570E80, written from Monster Hunter G's code at lobby 0x570E80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
s32 Get_sw2();                               /* extern */
u8 Lb_cursorUD();       /* extern */
s32 SetHelpLineMsg(); /* extern */
s32 cnWrap_SoundRequest(); /* extern */
s32 lobby_func_00570DA0(); /* extern */
extern u8 my_user_mini_data;

s32 lobby_func_00570E80(void) {
    s16 temp_v0;
    s32 temp_a3;
    s32 temp_v1_2;
    u8 temp_s0;
    u8 temp_v1;
    u8 temp_v1_4;
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_v1_3;

    temp_a0 = (*(void **)&pNet);
    temp_a3 = Get_sw2(0) & 0xFFFF;
    temp_v1 = (*(u8 *)((u8 *)(temp_a0) + 0x23));
    temp_a1 = temp_a0 + 0x23;
    switch (temp_v1) {                              /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_a0) + 0x23)) = (u8) (temp_v1 + 1);
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x32)) = (u8) (*(u8 *)((u8 *)(&my_user_mini_data) + 2));
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B)) = (u8) (*(u8 *)((u8 *)(&my_user_mini_data) + 2));
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x18)) = 0;
        SetHelpLineMsg((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B)) + 0x29, temp_a1, &my_user_mini_data, temp_a3);
block_18:
    default:
        return 2;
    case 1:
        temp_v1_2 = temp_a3 & 0xFFFF;
        temp_s0 = (*(u8 *)((u8 *)(temp_a0) + 0x2B));
        if (temp_v1_2 & 0x20) {
            (*(u8 *)((u8 *)(temp_a0) + 0x32)) = temp_s0;
            cnWrap_SoundRequest(0, temp_a1, &my_user_mini_data, temp_a3);
            goto block_13;
        }
        if (temp_v1_2 & 0x40) {
            lobby_func_00570DA0((*(u8 *)((u8 *)(temp_a0) + 0x32)), temp_a1, &my_user_mini_data, temp_a3);
            return 3;
        }
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B)) = Lb_cursorUD(temp_s0 & 0xFF, 7, &my_user_mini_data, temp_a3);
block_13:
        temp_v1_3 = (*(void **)&pNet);
        temp_v0 = (*(s16 *)((u8 *)(temp_v1_3) + 0x18)) + 1;
        (*(s16 *)((u8 *)(temp_v1_3) + 0x18)) = temp_v0;
        if ((((s64) ((s64) temp_v0 << 0x30) >> 0x30) >= 0x12D) && (lobby_func_00570DA0((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x32))) == 1)) {
            (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x18)) = 0;
        }
        temp_v1_4 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B));
        if ((temp_s0 & 0xFF) != temp_v1_4) {
            SetHelpLineMsg(temp_v1_4 + 0x29);
        }
        goto block_18;
    }
}

