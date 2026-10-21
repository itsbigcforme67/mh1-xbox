/* G online (PC): lobby_func_005803C0, written from Monster Hunter G's code at lobby 0x5803C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 cnLBS_Get_ServerMessage();            /* extern */
s32 lobby_func_005E6680(); /* extern */
extern u8 g_main_2EFBC0;

void lobby_func_005803C0(s64 arg0) {
    s64 sp18;
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    sp18 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 0x14)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        lobby_func_005E6680(&g_main_2EFBC0, temp_a1, temp_a1 + 0xBA9);
        if ((s8) sp18 == 0) {
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
            return;
        }
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0x63U;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
    }
}

