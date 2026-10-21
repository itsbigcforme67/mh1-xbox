/* G online (PC): lobby_func_00570260, written from Monster Hunter G's code at lobby 0x570260 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFBC0;
extern u8 g_main_2EFBC0;
extern u8 tl_member_buff;
s32 memcmp();             /* extern */
extern s32 g_lb_6B4514;
extern u16 g_lb_6B4518;
extern u16 g_lb_6B451A;
extern u8 g_lb_6B461D;

s32 lobby_func_00570260(void) {
    u8 *temp_s0;

    temp_s0 = (*(s32 *)&g_main_2EFBC0) + ((*(s16 *)((u8 *)&tl_member_buff + 0x1380)) * 0x10) + 4;
    if (memcmp(temp_s0, &g_lb_6B4514, 4) != 0) {
        return 0;
    }
    if ((*(u16 *)((u8 *)(temp_s0) + 0xA)) != g_lb_6B451A) {
        return 0;
    }
    if ((*(u8 *)((u8 *)(temp_s0) + 0xC)) != g_lb_6B461D) {
        return 0;
    }
    if ((*(u16 *)((u8 *)(temp_s0) + 8)) != g_lb_6B4518) {
        (*(u16 *)((u8 *)(temp_s0) + 8)) = (u16) g_lb_6B4518;
    }
    return 1;
}

