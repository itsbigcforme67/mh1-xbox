/* G online (PC): lm_member_list_i, written from Monster Hunter G's code at lobby 0x58BF40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lbc_init_network_work();
extern u8 game_w;
extern u8 pNet;
s32 lm_member_list_i(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 temp_v1_2;
    u8 *temp_v1;

    Lbc_init_network_work(arg0, arg1, arg2, arg3);
    temp_v1 = (*(void **)&pNet);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0x29));
    if (temp_v1_2 == (*(u8 *)((u8 *)&game_w + 0x3A))) {
        (*(u8 *)((u8 *)(temp_v1) + 0x29)) = (u8) (temp_v1_2 + 1);
    }
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0;
    return 0;
}

