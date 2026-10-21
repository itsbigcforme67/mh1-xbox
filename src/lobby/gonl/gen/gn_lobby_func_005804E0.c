/* G online (PC): lobby_func_005804E0, written from Monster Hunter G's code at lobby 0x5804E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFBC0;
extern u8 cw;
extern u8 g_main_2EFBC0;
s32 CallBackWaitInit();                 /* extern */
s32 Check_CallBackWait();               /* extern */
s32 lobby_func_005E6C80();   /* extern */
s32 lobby_func_005E6D20();        /* extern */
extern u8 g_lb_6B4510;
extern u8 lobby_func_00580450;

s32 lobby_func_005804E0(s32 arg0) {
    s32 temp_a0;
    u8 temp_v1_2;
    u8 *temp_v0;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a0 = temp_v1 + 0xB9D;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a0, arg0);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x15;
        temp_v0 = (M2C_ERROR(/* Read from unset register $a1 */) * 0x10) + (*(s32 *)&g_main_2EFBC0);
        lobby_func_005E6C80(((*(u16 *)((u8 *)(temp_v0) + 8)) + 1) & 0xFFFF, ((*(u16 *)((u8 *)(temp_v0) + 0xA)) + 1) & 0xFFFF, &lobby_func_00580450);
block_11:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait(temp_a0, arg0);
        goto block_11;
    case 0x2:
        lobby_func_005E6D20(&g_lb_6B4510, arg0);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

