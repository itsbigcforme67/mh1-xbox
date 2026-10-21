/* G online (PC): Lb_join_trans, written from Monster Hunter G's code at lobby 0x58B850 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 font_set_stack_no();
s32 lobby_func_00559C30();
extern u8 g_main_2EFAC0;
extern u8 lb_sys;
s32 lb_select_tag();                          /* extern */
s32 lb_select_trans();                        /* extern */

void Lb_join_trans(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    lobby_func_00559C30((s32) (*(u16 *)&g_main_2EFAC0), arg1, arg2, arg3);
    font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
    switch ((*(s8 *)((u8 *)&lb_sys + 0x1C))) {                           /* irregular */
    case 3:
        lb_select_tag((*(s8 *)((u8 *)&lb_sys + 0x1C)));
        return;
    case 6:
        lb_select_trans((*(s8 *)((u8 *)&lb_sys + 0x1C)));
        return;
    }
}

