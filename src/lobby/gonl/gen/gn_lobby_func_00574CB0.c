/* G online (PC): lobby_func_00574CB0, written from Monster Hunter G's code at lobby 0x574CB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntLocate();
s32 han2zen();
s32 lobby_func_005C14D0();
extern u8 BsProxyUrlstr;
extern u8 lb_shop_msg;
extern u8 tl_msg_tbl;
s32 font_print(); /* extern */
s32 sprintf();         /* extern */
extern s32 g_lb_63E268;
extern s32 g_lb_63E270;

void lobby_func_00574CB0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 g_frame[0xA0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s32 *)(g_frame + 0x40))
#define sp80 (*(s32 *)(g_frame + 0x80))
    s32 temp_s0;

    temp_s0 = lobby_func_005C14D0(&(*(s32 *)((u8 *)&BsProxyUrlstr + 0x110)), 8, arg2, arg3);
    flfntLocate((s32) (((s16) (((((s16) (arg0))) + 0x100)))), (s32) (((s16) (((((s16) (arg1))) + 0x10)))));
    sprintf(&sp80, &g_lb_63E268, 8 - temp_s0);
    han2zen(&sp80, &sp40);
    font_print(&g_lb_63E270, (*(s32 *)((u8 *)&tl_msg_tbl + 0x18)), &sp40, (*(s32 *)((u8 *)&lb_shop_msg + 0x8)));
}

