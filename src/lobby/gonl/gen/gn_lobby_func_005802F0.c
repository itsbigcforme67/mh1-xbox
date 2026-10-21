/* G online (PC): lobby_func_005802F0, written from Monster Hunter G's code at lobby 0x5802F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 Check_CallBackWait();
s32 lobby_func_005803C0();
s32 lobby_func_005E65F0();
extern u8 cw;
extern s32 g_lb_6B46A0;

s32 lobby_func_005802F0(void) {
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x14;
        lobby_func_005E65F0(&g_lb_6B46A0, (s32) lobby_func_005803C0);
block_13:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait();
        goto block_13;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

