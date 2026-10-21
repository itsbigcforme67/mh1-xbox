/* G online (PC): sub_main_func_004C8F90, written from Monster Hunter G's code at lobby 0x4C8F90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 view_work;
s64 sub_main_func_004C9250();              /* extern */
extern u8 g_main_2FF154;
extern u8 User_data;

s32 *sub_main_func_004C8F90(s32 arg0, s32 arg1) {
    u8 *var_v1;   /* (s32 *) */
    s32 temp_v1;
    s32 var_a0;
    s32 var_a5;
    u8 *var_a4;

    temp_v1 = ((s16) (sub_main_func_004C9250(&User_data)));
    switch (arg0) {                                 /* irregular */
    case 2:
        var_a0 = 0;
        var_v1 = &g_main_2FF154;
        var_a5 = ((s16) ((arg1 * 0x64)));
        var_a4 = &User_data + (var_a5 * 6);
        do {
            if ((((s16) (var_a5))) >= (((s16) (temp_v1)))) {
                *(&g_main_2FF154 + (((s16) (var_a0)))) = 0xFF;
                (*(u16 *)((u8 *)(var_v1) + 0x64)) = 0U;
            } else if ((*(u8 *)((u8 *)(var_a4) + 0x44)) != 0) {
                *(&g_main_2FF154 + (((s16) (var_a0)))) = (*(u8 *)((u8 *)(var_a4) + 0x45));
                (*(u16 *)((u8 *)(var_v1) + 0x64)) = (u16) (*(u16 *)((u8 *)(var_a4) + 0x46));
            } else {
                *(&g_main_2FF154 + (((s16) (var_a0)))) = 0xFF;
                (*(u16 *)((u8 *)(var_v1) + 0x64)) = 0U;
            }
            var_a0 = ((s16) ((var_a0 + 1)));
            var_v1 += 2;
            var_a4 += 6;
            var_a5 = ((s16) ((var_a5 + 1)));
        } while (var_a0 < 0x64);
        (*(s32 *)((u8 *)(&g_main_2FF154) + 0x12C)) = (s32) (*(s32 *)((u8 *)(&User_data) + 0xDE8));
        (*(s32 *)((u8 *)(&g_main_2FF154) + 0x130)) = (s32) (*(s32 *)((u8 *)(&User_data) + 0xDEC));
        (*(s32 *)((u8 *)(&g_main_2FF154) + 0x134)) = (s32) (*(s32 *)((u8 *)(&User_data) + 0xC1C));
        (*(u8 *)((u8 *)(&g_main_2FF154) + 0x138)) = (u8) (*(u8 *)((u8 *)(&User_data) + 0xC23));
        return &(*(u8 *)((u8 *)&view_work + 0x80));
    case 1:
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 0)) = (u8) (*(u8 *)((u8 *)(&User_data) + 0xC75));
        (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 2)) = (u16) (*(u16 *)((u8 *)(&User_data) + 0xC76));
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 1)) = (u8) (*(u8 *)((u8 *)(&User_data) + 0xC7B));
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 4)) = (u8) (*(u8 *)((u8 *)(&User_data) + 0xC7C));
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 5)) = (u8) (*(u8 *)((u8 *)(&User_data) + 0xC7D));
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 6)) = (u8) (*(u8 *)((u8 *)(&User_data) + 0xC7E));
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 7)) = (u8) (*(u8 *)((u8 *)(&User_data) + 0xC7A));
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 8)) = (s32) (*(s32 *)((u8 *)(&User_data) + 0xDE8));
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 0xC)) = (s32) (*(s32 *)((u8 *)(&User_data) + 0xDEC));
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 0x10)) = (s32) (*(s32 *)((u8 *)(&User_data) + 0x1C));
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&view_work + 0x64))) + 0x14)) = (u8) (*(u8 *)((u8 *)(&User_data) + 0xC23));
        return &(*(u8 *)((u8 *)&view_work + 0x50));
    default:
        return NULL;
    }
}

