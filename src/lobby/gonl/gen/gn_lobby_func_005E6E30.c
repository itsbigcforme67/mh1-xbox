/* G online (PC): lobby_func_005E6E30, written from Monster Hunter G's code at lobby 0x5E6E30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005E6EE0();
void * memset();
extern u8 pFunc;
s16 __cnetSub_Set_BgProcess(); /* extern */
extern s16 g_lb_733664;
extern s32 g_lb_733E3A;

s16 lobby_func_005E6E30(s32 arg0, s32 arg1) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp28 (*(s64 *)(g_frame + 0x28))
    s16 temp_v0;
    s16 var_v0;

    sp28 = arg0;
    memset(&g_lb_733E3A, 0, 0xFA4);
    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&pFunc + 0xEE)) + (temp_v0 * 0x1C))) = lobby_func_005E6EE0(sp28);
        g_lb_733664 = temp_v0;
        var_v0 = temp_v0;
    }
    return var_v0;
}

