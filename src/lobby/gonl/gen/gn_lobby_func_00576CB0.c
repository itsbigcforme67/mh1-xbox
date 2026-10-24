/* G online (PC): lobby_func_00576CB0, written from Monster Hunter G's code at lobby 0x576CB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon_free();
s32 Lb_put_msg_type2();
s32 Put_page_num();
s32 flfntLocate();
s32 font_print_double();
s32 font_set_palette();
extern u8 tl_member_buff;
extern u8 tl_msg_tbl;
s32 font_print();                 /* extern */
extern s32 g_lb_636CF0;
extern s32 lb_quest_color_tbl;
extern s32 lit_2316;

void lobby_func_00576CB0(void) {
    u8 *var_s1;   /* (s32 *) */
    s32 var_a3;
    s32 temp_s0;
    s32 var_s2;
    s32 var_s3;
    s32 var_v0;

    font_print_double(0xE2, 0x78, 1, 5, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&tl_msg_tbl + 0x54)) + ((*(u8 *)((u8 *)&tl_member_buff + 0x1388)) * 4))));
    switch ((*(u8 *)((u8 *)&tl_member_buff + 0x1388))) {                           /* irregular */
    case 0:
        var_a3 = lb_quest_color_tbl;
        break;
    case 1:
        var_a3 = (*(s32 *)((u8 *)&lb_quest_color_tbl + 0x4));
        break;
    case 2:
        var_a3 = (*(s32 *)((u8 *)&lb_quest_color_tbl + 0x8));
        break;
    default:
        var_a3 = 0;
        break;
    }
    if (var_a3 != 0) {
        Lb_put_icon_free(0x13E, 0x76, 0x14, var_a3 & (&lb_quest_color_tbl)[(*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370))], 0xF);
    }
    var_s3 = ((s16) (0x8E));
    font_set_palette(0);
    Lb_put_msg_type2(0xE2, (s32) var_s3, 0x39, M2C_ERROR(/* Read from unset register $a3 */));
    if (((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) != 2) && ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) != 1)) {
        var_v0 = 7;
    } else {
        var_v0 = 4;
    }
    temp_s0 = ((s16) (var_v0));
    var_s2 = 0;
    if (temp_s0 > 0) {
        var_s1 = &g_lb_636CF0;
        do {
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) == 1) {
                if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) == (((s16) (var_s2)))) {
                    font_set_palette(4);
                } else {
                    font_set_palette(0xA);
                }
            } else {
                font_set_palette(0);
            }
            flfntLocate(0x186, (s32) var_s3);
            font_print(&lit_2316, (*(s32 *)((u8 *)(var_s1) + 0xE8)));
            var_s1 += 4;
            var_s2 = ((s16) ((var_s2 + 1)));
            var_s3 = ((s16) ((var_s3 + 0x16)));
        } while (var_s2 < temp_s0);
    }
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) != 1) {
        font_set_palette(0xA);
    } else {
        font_set_palette(0);
    }
    Lb_put_msg_type2(0xE2, 0x128, 0x41, M2C_ERROR(/* Read from unset register $a3 */));
    Put_page_num(0x208, 0x78, (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)), 5, 0);
}

