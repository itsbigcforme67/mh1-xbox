/* G online (PC): lobby_func_005BFD60, written from Monster Hunter G's code at lobby 0x5BFD60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005BFE20();
extern u8 CnetSys_w;
s32 memset();        /* extern */
extern s8 g_lb_731163;
extern s32 g_lb_731164;
extern s32 g_lb_731168;
extern s32 g_lb_73116C;

s32 lobby_func_005BFD60(s8 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0))) + 0x20)) == 0) {
        memset(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0)), 0, 0x28);
        g_lb_731163 = arg0;
        g_lb_731164 = arg1;
        g_lb_731168 = arg2;
        g_lb_73116C = 0;
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0))) + 0x20)) = 1U;
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0))) + 4)) = arg3;
        (*(s32 (**)())((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xFA0))) + 0)) = lobby_func_005BFE20;
        return 0;
    }
    return -1;
}

