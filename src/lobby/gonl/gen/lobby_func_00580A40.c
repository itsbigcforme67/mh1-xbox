/* G online (PC): lobby_func_00580A40, written from Monster Hunter G's code at lobby 0x580A40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 CallBackWaitInit();                  /* extern */
s32 Check_CallBackWait();                /* extern */
s32 MH_lobbyClear();                     /* extern */
s32 To_LogOut();                     /* extern */
s32 cnLBS_PlazaEntry(); /* extern */
extern s8 ClassInfo;
extern u8 lobby_func_00580B20;

s32 lobby_func_00580A40(s8 arg0, s8 arg1) {
    s32 temp_a0;
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a0 = temp_v1 + 0xB9D;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a0, arg0);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0xA;
        cnLBS_PlazaEntry(M2C_ERROR(/* Read from unset register $a2 */) & 0xFFFF, M2C_ERROR(/* Read from unset register $a1 */) & 0xFFFF, 0, &lobby_func_00580B20);
block_10:
        return 2;
    case 0x1:
        Check_CallBackWait(temp_a0, arg0);
        goto block_10;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        ClassInfo = arg0;
        (*(u8 *)((u8 *)&ClassInfo + 0x4)) = arg1;
        MH_lobbyClear(temp_a0, arg0);
        return 0;
    default:
        To_LogOut(4, arg0);
        /* fallthrough */
    case 0x63:
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
        return 1;
    }
}

