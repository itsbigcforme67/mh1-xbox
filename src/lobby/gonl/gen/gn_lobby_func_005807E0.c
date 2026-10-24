/* G online (PC): lobby_func_005807E0, written from Monster Hunter G's code at lobby 0x5807E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 cnLBS_Get_ServerMessage();
s32 lobby_func_00580760();
s32 lobby_func_005E6E30();
extern u8 g_main_2EFBB8;
extern u8 g_main_2EFBBA;
extern u8 g_main_2EFBBC;
extern u8 cw;
extern u8 g_main_2EFBB8;
extern u8 g_main_2EFBBA;
extern u8 g_main_2EFBBC;
s16 Visual_main();                      /* extern */

s32 lobby_func_005807E0(s8 *arg0, s32 unused_a1, s32 arg2, s32 arg3) {
    s8 *temp_a1;
    u8 temp_v1_2;
    u8 *temp_a1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a1 = temp_v1 + 0xB9D;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        if ((*arg0 == 0) || (Visual_main(temp_a1) >= 0x11)) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0x63U;
        } else {
            temp_a1_2 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a1_2) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a1_2) + 0xB9D)) + 1);
            (*(s8 *)&g_main_2EFBB8) = 0x64;
            (*(s8 **)&g_main_2EFBBC) = arg0;
            (*(s16 *)&g_main_2EFBBA) = Visual_main(arg0, temp_a1_2);
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x16;
            M2C_ERROR(/* unknown instruction: ldr $a0, -0x2fb8($gp) */);
            lobby_func_005E6E30(M2C_ERROR(/* unknown instruction: ldl $a0, -0x2fb1($gp) */), (s32) lobby_func_00580760, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
block_15:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait((s32) arg0, (s32) temp_a1, arg2, arg3);
        goto block_15;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
        return 1;
    }
}

