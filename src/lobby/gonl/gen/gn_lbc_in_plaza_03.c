/* G online (PC): lbc_in_plaza_03, written from Monster Hunter G's code at lobby 0x57F8F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Plaza_LobbyMember2();
s32 Check_CallBackWait();
s32 lobby_func_005AE500();
void * memset();
void * strcpy();
extern u8 g_main_2EF390;
extern u8 cw;
extern u8 g_main_2EF390;
extern void *g_lb_636E08;

s32 lbc_in_plaza_03(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 g_frame[0x170] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(s32 *)(g_frame + 0x20))
    s32 temp_v1_3;
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        memset(&sp20, 0, 0x14B);
        temp_v1_3 = ((s16) (arg0));
        if (temp_v1_3 == 0x10) {
            strcpy(&sp20, g_lb_636E08);
        } else {
            strcpy(&sp20, (*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + (temp_v1_3 << 5) + 8);
        }
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x12;
        lobby_func_005AE500(&sp20, (s32) CallBack_Result_Plaza_LobbyMember2, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
block_16:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait(temp_v1 + 0xB9D, arg1, arg2, arg3);
        goto block_16;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

