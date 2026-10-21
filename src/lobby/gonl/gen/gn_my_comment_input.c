/* G online (PC): my_comment_input, written from Monster Hunter G's code at lobby 0x5709E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnWrap_SoundRequest();
extern u8 Psw;
extern u8 pNet;
s32 Get_kb_input();                             /* extern */
s32 Get_sw();                                /* extern */
s32 SoftKeyboard_exit();                     /* extern */
s64 SoftKeyboard_move();               /* extern */
s32 SoftKeyboard_pos_set();     /* extern */
s32 func_00220F40(); /* extern */

s32 my_comment_input(s32 arg0) {
    s32 temp_s0;
    s64 temp_v1_3;
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_4;
    u8 *temp_v1_5;

    temp_s0 = Get_sw(0) & 0xFFFF;
    Get_kb_input();
    temp_v1 = (*(void **)&pNet);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0x25));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0x25)) = (u8) (temp_v1_2 + 1);
        SoftKeyboard_pos_set(0x140, 0x42C80000);
        func_00220F40(1, 0xE, 0x60, arg0);
block_15:
    default:
        return 0;
    case 1:
        temp_v1_3 = ((s8) (SoftKeyboard_move(arg0, (*(s16 *)&Psw), (*(s16 *)((u8 *)&Psw + 0x4)))));
        if ((temp_v1_3 != -1) && (temp_v1_3 != 1)) {
            if ((temp_s0 & 0xFFFF) == 0x4000) {
                temp_v1_4 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_4) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0x25)) + 1);
                cnWrap_SoundRequest(3);
            }
        } else {
            temp_v1_5 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_5) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_5) + 0x25)) + 1);
        }
        goto block_15;
    case 2:
        SoftKeyboard_exit(temp_v1 + 0x25);
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0U;
        return 1;
    }
}

