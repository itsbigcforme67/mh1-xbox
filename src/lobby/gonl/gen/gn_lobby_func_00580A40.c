/* G online (PC): lobby_func_00580A40, written from Monster Hunter G's code at lobby 0x580A40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 MH_lobbyClear();
s32 To_LogOut();
s32 cnLBS_PlazaEntry();
s32 lobby_func_00580B20();
extern u8 cw;
extern s8 ClassInfo;

s32 lobby_func_00580A40(s8 arg0, s8 arg1) {
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0xA;
        cnLBS_PlazaEntry(arg0 & 0xFFFF, arg1 & 0xFFFF, 0, (s32) lobby_func_00580B20);
block_10:
        return 2;
    case 0x1:
        Check_CallBackWait();
        goto block_10;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        ClassInfo = arg0;
        (*(s8 *)((u8 *)&ClassInfo + 0x4)) = arg1;
        MH_lobbyClear();
        return 0;
    default:
        To_LogOut(4);
        /* fallthrough */
    case 0x63:
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
        return 1;
    }
}

