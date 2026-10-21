/* G online (PC): put_member_info, written from Monster Hunter G's code at lobby 0x5759A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
extern u8 tl_msg_tbl;
s32 Lb_put_icon();    /* extern */
s32 flfntLocate();                      /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();           /* extern */
s32 font_print_double(); /* extern */
s32 font_print_uf();                   /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();                   /* extern */
s32 memcmp();               /* extern */
s32 sprintf(); /* extern */
extern s32 g_lb_63E288;
extern u8 lb_num_str;
extern u8 lit_2316;
extern u8 lit_2633;
extern u8 my_user_id;

void put_member_info(s64 arg0, s32 arg1, s8 *arg2, s32 *arg3, void *arg4, s64 arg5) {
    u8 g_frame[0xE0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp70 (*(s32 *)(g_frame + 0x70))
#define spD0 (*(s32 *)(g_frame + 0xD0))
    s32 var_a0;
    s32 var_s2;
    s32 var_s5;
    u8 temp_a6;

    if (*arg2 != 0) {
        han2zen(arg2, &spD0);
        if (arg4 != NULL) {
            temp_a6 = (*(u8 *)((u8 *)(arg4) + 1));
            sprintf(&sp70, &g_lb_63E288, &spD0, (*(s32 *)((u8 *)&tl_msg_tbl + 0x20)), (*(s32 *)((u8 *)&lb_num_str + ((((*(u8 *)((u8 *)(arg4) + 1)) / 10) + (temp_a6 >> 0x1F)) * 4))), (*(s32 *)((u8 *)&lb_num_str + (((s32) temp_a6 % 10) * 4))), temp_a6);
        } else {
            sprintf(&sp70, &lit_2633, &spD0);
        }
        flfntSetSize(0x12, 0x12);
        if (((((s8) (arg5))) == 0) && ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0)) {
            if (memcmp(arg2, &my_user_id, 8) == 0) {
                var_s2 = 0xA;
                var_s5 = 0xFF808080;
            } else {
                var_s2 = 4;
                var_s5 = 0xFF8080FF;
            }
            font_print_double(arg0, arg1, 1, var_s2, arg3);
            font_print_double(((s16) (((((s16) (arg0))) + 0xA2))), arg1, 1, var_s2, &sp70);
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
            flfntLocate(((s16) (((((s16) (arg0))) + 0xA2))), arg1);
            font_print_uf(&sp70);
        }
        if (arg4 != NULL) {
            Lb_put_icon(0x1FE, arg1, (*(u8 *)((u8 *)(arg4) + 0)) + 2, var_s5);
        }
    }
}

