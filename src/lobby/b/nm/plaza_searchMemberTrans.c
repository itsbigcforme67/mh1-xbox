#include "lobby_a.h"
extern char text_lobby_msg[];
extern char text_lobby_msg[];
extern char seekStr[];
extern char seekStr[];
extern char seekStr[];
extern char lit_2316[];
extern char tl_job_tbl[];
extern char lit_2316[];
extern char tl_hr_tbl[];
extern char lit_2316[];
void plaza_searchMemberTrans(int arg0) {
    int sp60;
    int var_s3;
    int var_s3_2;
    int var_s4;
    s16 temp_s3;
    s32 temp_a1;
    s32 temp_a2;
    s32 var_s1;
    s32 var_s2_2;
    s32 var_s2_3;
    int var_s1_2;
    int var_s2;
    int temp_a0_2;
    u8 temp_a0;
    u8 temp_a0_3;
    u8 temp_a0_4;
    int temp_a2_2;
    int temp_s2;
    int temp_s2_2;

    temp_a0 = F(u8, arg0, 3);
    temp_s2 = F(int, &text_lobby_msg, 8);
    switch (temp_a0) {                              /* switch 1; irregular */
    case 10:                                        /* switch 1 */
        temp_a2 = (int)SearchResult + ((F(u8, arg0, 0xA) + (F(s16, arg0, 0x24) * 7)) * 0x5C) + 4;
        disp_status(0xD8, 0x50, temp_a2, temp_a2 + 8);
        return;
    case 11:                                        /* switch 1 */
    case 9:                                         /* switch 1 */
    case 8:                                         /* switch 1 */
    case 6:                                         /* switch 1 */
        put_titles2(temp_s2 + 0xF8);
        temp_s3 = F(s16, temp_s2, 0xF8);
        var_s2 =  ((F(s16, temp_s2, 0xFA) + 0x16) << 0x30) >> 0x30;
        if (SearchResult != 0) {
            put_main_cursor(F(u8, arg0, 0xA));
            var_s1 = 0;
            do {
                temp_a0_2 = (int)SearchResult;
                temp_a1 = var_s1 + (F(s16, arg0, 0x24) * 7);
                temp_a2_2 = temp_a0_2 + (temp_a1 * 0x5C) + 4;
                if (temp_a1 < (*(u8 *)temp_a0_2)) {
                    put_member_info(temp_s3, var_s2, temp_a2_2, temp_a2_2 + 8);
                }
                var_s1 += 1;
                var_s2 =  ((var_s2 + 0x16) << 0x30) >> 0x30;
            } while (var_s1 < 7);
            Put_page_num( ((((s16)temp_s3) + 0x12C) << 0x30) >> 0x30, var_s2, (u8) F(s16, arg0, 0x24), F(s16, arg0, 0x26));
        } else {
            flfntSetSize(0x14, 0x14);
            font_print_double( ((((s16)temp_s3) + 0x46) << 0x30) >> 0x30,  ((( (var_s2 << 0x30) >> 0x30) + 0x3C) << 0x30) >> 0x30, 1, 4);
        }
block_49:
        temp_a0_3 = F(u8, arg0, 3);
        if ((temp_a0_3 != 4) && (temp_a0_3 != 3) && (temp_a0_3 != 2) && (temp_a0_3 != 1) && (temp_a0_3 != 0)) {
            return;
        }
        if (temp_a0_3 < 3) {
            font_set_palette(0xA);
        } else {
            font_set_palette(0);
        }
        Lb_put_msg_type2(F(int, &text_lobby_msg, 8) + 0x148);
        return;
    default:                                        /* switch 1 */
        if (F(u8, arg0, 7) != 5) {
            if (F(u8, arg0, 0xC) == 0) {
                put_main_cursor(F(u8, arg0, 0xA) + 1);
            }
            temp_s2_2 = temp_s2 + ((F(u8, arg0, 4) + 0x21) * 8);
            put_titles2(temp_s2_2);
            var_s1_2 =  ((F(s16, temp_s2_2, 2) + 0x2C) << 0x30) >> 0x30;
            font_set_palette(0);
            Lb_put_msg_type2(temp_s2_2 + 0x20);
            temp_a0_4 = F(u8, arg0, 4);
            switch (temp_a0_4) {                    /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                if (F(u8, arg0, 3) == 3) {
                    flfntLocate( ((0x1B0 - ((u32) (strlen(&seekStr) * 9) >> 1)) << 0x30) >> 0x30, var_s1_2);
                    font_print_uf(&seekStr);
                }
                break;
            case 1:                                 /* switch 2 */
                if (F(u8, arg0, 3) == 3) {
                    flfntLocate(0x17B);
                    han2zen(&seekStr, &sp60);
                    font_print(&lit_2316, &sp60);
                }
                break;
            case 2:                                 /* switch 2 */
                var_s2_2 = 0;
                var_s3 = (int)&tl_job_tbl;
                do {
                    if (F(u8, arg0, 3) == 3) {
                        if (F(u8, arg0, 6) == var_s2_2) {
                            font_set_palette(4);
                            var_s4 = 0xFF8080FF;
                        } else {
                            font_set_palette(0xA);
                            var_s4 = 0xFF707070;
                        }
                    } else {
                        font_set_palette(0);
                        var_s4 = -1;
                    }
                    flfntLocate(0x144, var_s1_2);
                    font_print(&lit_2316, (*(u8 *)var_s3));
                    Lb_put_icon(0x12C,  ((( (var_s1_2 << 0x30) >> 0x30) - 2) << 0x30) >> 0x30, var_s2_2 + 2, var_s4);
                    var_s2_2 += 1;
                    var_s3 += 4;
                    var_s1_2 =  ((var_s1_2 + 0x16) << 0x30) >> 0x30;
                } while (var_s2_2 < 5);
                break;
            case 3:                                 /* switch 2 */
                var_s2_3 = 0;
                var_s3_2 = (int)&tl_hr_tbl;
                do {
                    if (F(u8, arg0, 3) == 3) {
                        if (F(u8, arg0, 6) == var_s2_3) {
                            font_set_palette(4);
                        } else {
                            font_set_palette(0xA);
                        }
                    } else {
                        font_set_palette(0);
                    }
                    flfntLocate(0x144, var_s1_2);
                    font_print(&lit_2316, (*(u8 *)var_s3_2));
                    var_s2_3 += 1;
                    var_s3_2 += 4;
                    var_s1_2 =  ((var_s1_2 + 0x16) << 0x30) >> 0x30;
                } while (var_s2_3 < 5);
                break;
            }
            Put_page_num(0x1F4, 0x78, F(u8, arg0, 4), 4);
        }
        goto block_49;
    }
}
