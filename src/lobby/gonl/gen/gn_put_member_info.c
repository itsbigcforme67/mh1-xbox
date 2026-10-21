/* G online (PC): put_member_info, written from Monster Hunter G's code at lobby 0x5759A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
extern u8 tl_msg_tbl;
s32 Lb_put_icon(); /* extern */
s32 flfntLocate();                  /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();             /* extern */
s32 font_print_double(); /* extern */
s32 font_print_uf();                   /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();                   /* extern */
s32 memcmp();               /* extern */
s32 sprintf(); /* extern */
extern u8 g_lb_63E288;
extern u8 lit_2316;
extern u8 lit_2633;
extern u8 my_user_id;

void put_member_info(s64 arg0, s32 arg1, s8 *arg2, s32 arg3) {
    u8 g_frame[0xE0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp70 (*(s32 *)(g_frame + 0x70))
#define spD0 (*(s32 *)(g_frame + 0xD0))
    s32 var_a0;
    s32 var_s2;
    s32 var_s5;
    u8 *temp_s0;

    temp_s0 = M2C_ERROR(/* Read from unset register $t0 */);
    if (*arg2 != 0) {
        han2zen(arg2, &spD0);
        if (temp_s0 != NULL) {
            sprintf(&sp70, &g_lb_63E288, &spD0, (*(u8 *)((u8 *)&tl_msg_tbl + 0x20)));
        } else {
            sprintf(&sp70, &lit_2633, &spD0);
        }
        flfntSetSize(0x12, 0x12);
        if ((((s64) (M2C_ERROR(/* Read from unset register $t1 */) << 0x38) >> 0x38) == 0) && ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0)) {
            if (memcmp(arg2, &my_user_id, 8) == 0) {
                var_s2 = 0xA;
                var_s5 = 0xFF808080;
            } else {
                var_s2 = 4;
                var_s5 = 0xFF8080FF;
            }
            font_print_double(arg0, arg1, 1, var_s2);
            font_print_double((s64) ((((s64) (arg0 << 0x30) >> 0x30) + 0xA2) << 0x30) >> 0x30, arg1, 1, var_s2);
        } else {
            if (memcmp(arg2, &my_user_id, 8) == 0) {
                var_a0 = 0xA;
                var_s5 = 0xFF808080;
            } else {
                var_a0 = 0;
                var_s5 = -1;
            }
            font_set_palette(var_a0);
            flfntLocate(arg0, arg1);
            font_print(&lit_2316, arg3);
            flfntLocate((s64) ((((s64) (arg0 << 0x30) >> 0x30) + 0xA2) << 0x30) >> 0x30, arg1);
            font_print_uf(&sp70);
        }
        if (temp_s0 != NULL) {
            Lb_put_icon(0x1FE, arg1, (*(u8 *)((u8 *)(temp_s0) + 0)) + 2, var_s5);
        }
    }
}

