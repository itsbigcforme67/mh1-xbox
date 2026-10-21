/* G online (PC): plaza_req_input, written from Monster Hunter G's code at lobby 0x5718D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 Psw;
extern u8 pNet;
s32 Get_sw();                                /* extern */
s32 SoftKeyboard_exit();                     /* extern */
s64 SoftKeyboard_move();               /* extern */
s32 SoftKeyboard_pos_set();     /* extern */
s32 cnWrap_SoundRequest();               /* extern */
s32 func_00220F40(); /* extern */

s32 plaza_req_input(s32 arg0, s32 arg1) {
    s32 temp_s0;
    s64 temp_v1_3;
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_4;
    u8 *temp_v1_5;
    u8 *temp_v1_6;

    temp_v1 = (*(void **)&pNet);
    temp_s0 = Get_sw(0) & 0xFFFF;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0x25));
    switch (temp_v1_2) {                            /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v1) + 0x25)) = (u8) (temp_v1_2 + 1);
        SoftKeyboard_pos_set(0x42C80000, 0x140);
        if (!(arg1 & 0xFF)) {
            func_00220F40(0, 6, 6, arg0);
        } else {
            func_00220F40(3, 0xF, 8, arg0);
        }
block_21:
    default:                                        /* switch 1 */
        return 0;
    case 1:                                         /* switch 1 */
        temp_v1_3 = (s64) (SoftKeyboard_move(arg0, (*(s16 *)&Psw), (*(s16 *)((u8 *)&Psw + 0x4))) << 0x38) >> 0x38;
        switch (temp_v1_3) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            if ((temp_s0 & 0xFFFF) == 0x4000) {
                temp_v1_4 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_4) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0x25)) + 1);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = 0;
                cnWrap_SoundRequest(3);
            }
            break;
        case 1:                                     /* switch 2 */
            temp_v1_5 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_5) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_5) + 0x25)) + 1);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = 0;
            break;
        case -1:                                    /* switch 2 */
            temp_v1_6 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_6) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_6) + 0x25)) + 1);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = 0;
            break;
        }
        goto block_21;
    case 2:                                         /* switch 1 */
        SoftKeyboard_exit(temp_v1 + 0x25);
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0U;
        return 1;
    }
}

