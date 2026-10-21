/* G online (PC): lbc_in_plaza_03, written from Monster Hunter G's code at lobby 0x57F8F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EF390;
extern u8 cw;
extern u8 g_main_2EF390;
s32 CallBackWaitInit();                         /* extern */
s32 Check_CallBackWait();                    /* extern */
s32 lobby_func_005AE500();  /* extern */
s32 memset();        /* extern */
s32 strcpy();                     /* extern */
extern u8 CallBack_Result_Plaza_LobbyMember2;
extern u8 g_lb_636E08;

s32 lbc_in_plaza_03(s64 arg0) {
    union { s32 v; u8 b[0x150]; } u_sp20;
#define sp20 (u_sp20.v)
    s64 temp_v1_3;
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        memset(&sp20, 0, 0x14B);
        temp_v1_3 = (s64) (arg0 << 0x30) >> 0x30;
        if (temp_v1_3 == 0x10) {
            strcpy(&sp20, g_lb_636E08);
        } else {
            strcpy(&sp20, (*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + (temp_v1_3 << 5) + 8);
        }
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x12;
        lobby_func_005AE500(&sp20, &CallBack_Result_Plaza_LobbyMember2);
block_16:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait(temp_v1 + 0xB9D);
        goto block_16;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

