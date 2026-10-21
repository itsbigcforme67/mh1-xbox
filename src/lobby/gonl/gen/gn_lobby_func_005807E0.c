/* G online (PC): lobby_func_005807E0, written from Monster Hunter G's code at lobby 0x5807E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFBB8;
extern u8 g_main_2EFBBA;
extern u8 g_main_2EFBBC;
extern u8 cw;
extern u8 g_main_2EFBB8;
extern u8 g_main_2EFBBA;
extern u8 g_main_2EFBBC;
s32 CallBackWaitInit();                         /* extern */
s32 Check_CallBackWait();                   /* extern */
s16 Visual_main();                      /* extern */
s32 cnLBS_Get_ServerMessage();      /* extern */
s32 lobby_func_005E6E30();    /* extern */
extern u8 lobby_func_00580760;

s32 lobby_func_005807E0(s8 *arg0) {
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
            CallBackWaitInit();
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x16;
            M2C_ERROR(/* unknown instruction: ldr $a0, -0x2fb8($gp) */);
            lobby_func_005E6E30(M2C_ERROR(/* unknown instruction: ldl $a0, -0x2fb1($gp) */), &lobby_func_00580760);
        }
block_15:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait(temp_a1);
        goto block_15;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, temp_a1);
        return 1;
    }
}

