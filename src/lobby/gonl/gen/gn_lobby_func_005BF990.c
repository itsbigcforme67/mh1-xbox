/* G online (PC): lobby_func_005BF990, written from Monster Hunter G's code at lobby 0x5BF990 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005BFA20();
void * memset();
extern u8 CnetSys_w;
extern s8 g_lb_731162;
extern u8 g_lb_731170;

s32 lobby_func_005BF990(s32 arg0) {
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xF78))) + 0x20)) == 0) {
        memset(&(*(u8 *)((u8 *)&CnetSys_w + 0xF78)), 0, 0x28);
        g_lb_731162 = 0;
        memset(&g_lb_731170, 0, 0x100);
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xF78))) + 0x20)) = 1U;
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xF78))) + 4)) = arg0;
        (*(s32 (**)(s32, s32, s32, s32))((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xF78))) + 0)) = lobby_func_005BFA20;
        return 0;
    }
    return -1;
}

