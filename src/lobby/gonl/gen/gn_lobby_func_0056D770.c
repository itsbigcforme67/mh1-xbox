/* G online (PC): lobby_func_0056D770, written from Monster Hunter G's code at lobby 0x56D770 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Plaza_LobbyEntry();
s32 Lb_eat_to_bell();
s32 Lb_select();
s32 Lbs_request_enter_lobby2();
s32 SetDialogData();
s32 lobby_func_0056AF20();
s32 lobby_func_00580A40();
s32 lobby_func_00581210();
extern u8 cw;
extern u8 pNet;
extern u8 g_main_3064AF;

s32 lobby_func_0056D770(s32 unused_a0, s32 unused_a1, s32 arg2, s32 arg3) {
    s32 temp_v0;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v0_6;
    u16 temp_a0_2;
    u8 temp_a0;
    u8 *temp_a1;
    u8 *temp_a1_2;
    u8 *temp_v0_2;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_4;

    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    temp_a1 = (*(void **)&pNet);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0x25));
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        temp_v0 = Lb_select((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_a1_2 = (*(void **)&cw);
            temp_a0_2 = (*(u16 *)((u8 *)(temp_a1_2) + 0xF544));
            if (lobby_func_0056AF20((s32) temp_a0_2, (s32) (*(u16 *)((u8 *)(temp_a1_2) + 0xF546)), (s32) *(&g_main_3064AF + (temp_a0_2 * 0x15C)), M2C_ERROR(/* Read from unset register $a3 */)) == 0) {
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0U;
                return 1;
            }
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0;
            temp_v1 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0x25)) + 1);
            SetDialogData(0x1B, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
block_45:
            return 2;
        case 3:                                     /* switch 2 */
            return 3;
        }
        break;
    case 1:                                         /* switch 1 */
        temp_v0_2 = (*(void **)&cw);
        temp_v0_3 = lobby_func_00580A40((s32) (*(u16 *)((u8 *)(temp_v0_2) + 0xF544)), (s32) (*(u16 *)((u8 *)(temp_v0_2) + 0xF546)), arg2, arg3);
        switch (temp_v0_3) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            if ((*(u16 *)((u8 *)((*(void **)&cw)) + 0xF546)) == 0) {
                (*(s32 *)((u8 *)((*(void **)&pNet)) + 0x14)) = 3;
            } else {
                temp_v1_2 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_2) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0x25)) + 1);
            }
            goto block_45;
        case 1:                                     /* switch 3 */
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 1;
        }
        break;
    case 2:                                         /* switch 1 */
        temp_v0_4 = CallBack_Result_Plaza_LobbyEntry((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        switch (temp_v0_4) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            temp_v1_3 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_3) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x25)) + 1);
            goto block_45;
        case 1:                                     /* switch 4 */
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 1;
        }
        break;
    case 3:                                         /* switch 1 */
        (*(s8 *)((u8 *)(temp_a1) + 0x2D)) = 1;
        temp_v0_5 = Lbs_request_enter_lobby2((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        switch (temp_v0_5) {                        /* switch 5; irregular */
        case 0:                                     /* switch 5 */
            temp_v1_4 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_4) + 0x25)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0x25)) + 1);
            goto block_45;
        case 1:                                     /* switch 5 */
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 1;
        }
        break;
    case 4:                                         /* switch 1 */
        (*(s8 *)((u8 *)(temp_a1) + 0x2D)) = 1;
        temp_v0_6 = lobby_func_00581210((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        if ((temp_v0_6 != 1) && (temp_v0_6 != 0)) {

        } else {
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0U;
            (*(s32 *)((u8 *)((*(void **)&pNet)) + 0x14)) = 0;
        }
        goto block_45;
    default:                                        /* switch 1 */
        return 1;
    }
}

