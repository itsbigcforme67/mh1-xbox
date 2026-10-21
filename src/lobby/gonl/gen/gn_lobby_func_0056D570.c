/* G online (PC): lobby_func_0056D570, written from Monster Hunter G's code at lobby 0x56D570 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_kb_input();
s32 Get_sw();
s32 SoftKeyboard_exit();
s32 SoftKeyboard_move();
s32 cnWrap_SoundRequest();
s32 func_00220F40();
extern u8 g_main_2EFBAC;
extern u8 Psw;
extern u8 g_main_2EFBAC;
s32 SoftKeyboard_pos_set();     /* extern */

s32 lobby_func_0056D570(void *arg0) {
    s32 temp_s0;
    s64 temp_v1_3;
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_4;
    u8 *temp_v1_5;

    temp_s0 = Get_sw(0) & 0xFFFF;
    Get_kb_input();
    temp_v1 = (*(void **)&g_main_2EFBAC);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0x232));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0x232)) = (u8) (temp_v1_2 + 1);
        SoftKeyboard_pos_set(0x140, 0x42C80000);
        func_00220F40(1, 0xE, 0x7E, arg0);
block_16:
        return 0;
    case 1:
        temp_v1_3 = ((s8) (SoftKeyboard_move(arg0, (s32) (*(s16 *)&Psw), (s32) (*(s16 *)((u8 *)&Psw + 0x4)))));
        if ((temp_v1_3 != -1) && (temp_v1_3 != 1)) {
            if ((temp_s0 & 0xFFFF) == 0x4000) {
                temp_v1_4 = (*(void **)&g_main_2EFBAC);
                (*(u8 *)((u8 *)(temp_v1_4) + 0x232)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0x232)) + 1);
                cnWrap_SoundRequest(3);
            }
        } else {
            temp_v1_5 = (*(void **)&g_main_2EFBAC);
            (*(u8 *)((u8 *)(temp_v1_5) + 0x232)) = (u8) ((*(u8 *)((u8 *)(temp_v1_5) + 0x232)) + 1);
        }
        goto block_16;
    case 2:
        SoftKeyboard_exit();
        (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x232)) = 0U;
        return 1;
    default:
        SoftKeyboard_exit();
        (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x232)) = 0U;
        return 1;
    }
}

