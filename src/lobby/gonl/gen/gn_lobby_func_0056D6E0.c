/* G online (PC): lobby_func_0056D6E0, written from Monster Hunter G's code at lobby 0x56D6E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s64 lobby_func_00569320();
extern u8 tl_member_buff;
extern s16 g_lb_6B42B6;
extern s16 g_lb_6B42B8;

void lobby_func_0056D6E0(void) {
    g_lb_6B42B6 = lobby_func_00569320((s64) g_lb_6B42B8, 7);
    if (g_lb_6B42B6 == 0) {
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x606)) = 1;
    }
}

