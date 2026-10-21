/* G online (PC): lb_rule_seet_trans, written from Monster Hunter G's code at lobby 0x55C940 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntSetSize();
s32 font_print_double();
s32 font_set_stack_no();
extern u8 lb_sys;
extern u8 tl_msg_tbl;
s32 lb_rule_seet_trans_ot();              /* extern */
s32 lobby_func_0055CA90();             /* extern */
extern void *g_lb_63682C;
extern void *g_lb_636830;
extern void *g_lb_636864;
extern void *g_lb_636868;
extern u8 lb_rule_exp;

void lb_rule_seet_trans(void *arg0) {
    u8 *var_s0;   /* (s32 *) */
    s32 var_s2;

    var_s0 = &lb_rule_exp;
    font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
    if ((*(s8 *)((u8 *)&lb_sys + 0x1E)) == 1) {
        flfntSetSize(0x12, 0x12);
        font_print_double(0x175, 0x46, 1, 6, g_lb_636864);
        font_print_double(0x157, 0x68, 1, 6, (*(void * *)((u8 *)&tl_msg_tbl + 0x34)));
        font_print_double(0x157, 0x86, 1, 6, g_lb_63682C);
        font_print_double(0x157, 0xB4, 1, 6, g_lb_636830);
        font_print_double(0x16B, 0x13C, 1, 6, g_lb_636868);
        var_s2 = 0;
        do {
            if ((*(s8 *)((u8 *)(var_s0) + 4)) != 0) {
                if (var_s2 == 4) {
                    lobby_func_0055CA90(var_s0);
                } else {
                    font_print_double((s32) (*(s16 *)((u8 *)(var_s0) + 0)), (s32) (*(s16 *)((u8 *)(var_s0) + 2)), 1, 0, var_s0 + 4);
                }
            }
            var_s2 += 1;
            var_s0 += 0x64;
        } while (var_s2 < 5);
        lb_rule_seet_trans_ot(arg0);
    }
}

