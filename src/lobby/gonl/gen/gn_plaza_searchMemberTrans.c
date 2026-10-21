/* G online (PC): plaza_searchMemberTrans, written from Monster Hunter G's code at lobby 0x575C00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon();
s32 Lb_put_msg_type2();
s32 Put_page_num();
s32 lobby_func_005799F0();
s64 lobby_func_005935C0();
s32 put_main_cursor();
extern u8 tl_member_buff;
extern u8 tl_msg_tbl;
s32 flfntLocate();                      /* extern */
s32 font_print();                 /* extern */
s32 font_print_double(); /* extern */
s32 font_print_uf();                        /* extern */
s32 font_set_palette();                  /* extern */
extern s8 g_lb_6B475C;
extern u8 g_main_2EEE68;
extern u8 lit_2316;
extern s8 seekStr;
extern s32 tl_hr_tbl;
extern u8 tl_job_tbl;

void plaza_searchMemberTrans(s64 arg0) {
    u8 *var_s4_2;   /* (s32 *) */
    s32 var_s6;
    s64 var_s1;
    u32 var_s3;
    u32 var_s5;
    u8 *var_s4;

    if ((*(u8 *)((u8 *)&tl_member_buff + 0x1388)) == 0) {
        lobby_func_005799F0(0xD8, 0x50, 1, -0xFAE0FB, -0xCDA9C4);
        if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) == 0) {
            put_main_cursor(1);
        } else {
            put_main_cursor(7);
        }
    } else if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) == 0) {
        put_main_cursor((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) + 1);
    } else if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) == 2) {
        put_main_cursor(7);
    }
    font_print_double(0xE2, 0x78, 1, 5, (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&tl_msg_tbl + 0x40)) + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) * 4))));
    font_set_palette(0);
    Lb_put_msg_type2(0xE2);
    var_s1 = 0xA4;
    switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370))) { /* irregular */
    case 0:
        flfntLocate(lobby_func_005935C0((s64) &seekStr, 0x1B0), 0xA4);
        font_print_uf(&seekStr);
        break;
    case 1:
        var_s5 = 0;
        var_s4 = &g_main_2EEE68;
        do {
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) == 2) {
                if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) == var_s5) {
                    font_set_palette(4);
                    var_s6 = -0x7F7F01;
                } else {
                    font_set_palette(0xA);
                    var_s6 = -0x8F8F90;
                }
            } else {
                font_set_palette(0);
                var_s6 = -1;
            }
            flfntLocate(arg0, var_s1);
            font_print(&lit_2316, (*(s32 *)((u8 *)&tl_job_tbl + (*var_s4 * 4))));
            Lb_put_icon((s32) (((s16) (((((s16) (arg0))) - 0x18)))), (s32) (((s16) (((((s16) (var_s1))) - 2)))), *var_s4 + 2, var_s6);
            var_s5 += 1;
            var_s4 += 1;
            var_s1 = ((s16) ((var_s1 + 0x16)));
        } while (var_s5 < 5U);
        break;
    case 2:
        var_s3 = 0;
        var_s4_2 = &tl_hr_tbl;
        do {
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) == 2) {
                if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) == var_s3) {
                    font_set_palette(4);
                } else {
                    font_set_palette(0xA);
                }
            } else {
                font_set_palette(0);
            }
            flfntLocate(arg0, var_s1);
            font_print(&lit_2316, (*(s32 *)var_s4_2));
            var_s3 += 1;
            var_s4_2 += 4;
            var_s1 = ((s16) ((var_s1 + 0x16)));
        } while (var_s3 < 6U);
        break;
    }
    Put_page_num(0x208, 0x78, (s64) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)), 3, 0);
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) != 2) {
        font_set_palette(0xA);
        if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
            if ((seekStr == 0) && ((*(s8 *)((u8 *)&seekStr + 0x44)) == 0)) {
                if (g_lb_6B475C != 0) {
                    goto block_37;
                }
            } else {
block_37:
                font_set_palette(0);
            }
        }
    } else {
        font_set_palette(0);
    }
    Lb_put_msg_type2(0xE2);
}

