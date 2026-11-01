/* G online (PC): lm_net_status_mv, written from Monster Hunter G's code at lobby 0x58D0A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_cursorUD();
s32 cnWrap_SoundRequest();
extern u8 g_main_2EF2A8;
extern u8 PitMenu;
extern u8 g_main_2EF2A8;
extern u8 pNet;
s32 lobby_func_00582E30();                      /* extern */

s32 lm_net_status_mv(s32 arg0, s32 unused_a1, s32 arg2, s32 arg3) {
    s32 temp_v1;

    temp_v1 = arg0 & 0xFFFF;
    if (temp_v1 & 0x20) {
        (*(u8 *)&g_main_2EF2A8) = (u8) (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29));
        lobby_func_00582E30();
        cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        goto block_7;
    }
    if (temp_v1 & 0x40) {
        return 0x40;
    }
    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) = Lb_cursorUD((s32) (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)), 7, arg2, arg3);
    (*(s16 *)((u8 *)&PitMenu + 0x12)) = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) + 0x13;
block_7:
    return arg0;
}

