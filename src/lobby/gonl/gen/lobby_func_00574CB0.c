/* G online (PC): lobby_func_00574CB0, written from Monster Hunter G's code at lobby 0x574CB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 BsProxyUrlstr;
extern u8 lb_shop_msg;
extern u8 tl_msg_tbl;
s32 flfntLocate();                      /* extern */
s32 font_print(); /* extern */
s32 han2zen();              /* extern */
s32 lobby_func_005C14D0();        /* extern */
s32 sprintf();         /* extern */
extern u8 g_lb_63E268;
extern u8 g_lb_63E270;

void lobby_func_00574CB0(s64 arg0, s64 arg1) {
    union { s32 v; u8 b[0x20]; } u_sp80;
#define sp80 (u_sp80.v)
    union { s32 v; u8 b[0x60]; } u_sp40;
#define sp40 (u_sp40.v)
    s32 temp_s0;

    temp_s0 = lobby_func_005C14D0(&(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)), 8);
    flfntLocate((s64) ((((s64) (arg0 << 0x30) >> 0x30) + 0x100) << 0x30) >> 0x30, (s64) ((((s64) (arg1 << 0x30) >> 0x30) + 0x10) << 0x30) >> 0x30);
    sprintf(&sp80, &g_lb_63E268, 8 - temp_s0);
    han2zen(&sp80, &sp40);
    font_print(&g_lb_63E270, (*(u8 *)((u8 *)&tl_msg_tbl + 0x18)), &sp40, (*(u8 *)((u8 *)&lb_shop_msg + 0x8)));
}

