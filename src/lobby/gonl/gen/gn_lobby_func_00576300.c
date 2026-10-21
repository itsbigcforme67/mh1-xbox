/* G online (PC): lobby_func_00576300, written from Monster Hunter G's code at lobby 0x576300 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_page_num();
s32 disp_status();
s32 lobby_func_005737C0();
s32 lobby_func_00575FB0();
s32 lobby_func_005775C0();
s32 lobby_func_005791D0();
s32 plaza_searchMemberTrans();
s32 put_main_cursor();
s32 put_member_info();
extern u8 g_main_2EFBB0;
extern u8 g_main_2EFBC4;
extern u8 cw;
extern u8 g_main_2EFBB0;
extern u8 g_main_2EFBC4;
extern u8 pNet;
extern u8 tl_hr_tbl;
extern u8 tl_member_buff;
s32 flfntLocate();                  /* extern */
s32 font_print();           /* extern */
s32 font_print_double(); /* extern */
s32 font_print_ex(); /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();                   /* extern */
s32 memcmp();               /* extern */
s32 sprintf(); /* extern */
s32 strcpy();                     /* extern */
extern s32 g_lb_636B48;
extern s32 *g_lb_636DF8;
extern u8 tl_etc;
extern s32 g_lb_63E2A8;
extern u8 lit_2316;
extern u8 my_user_id;

void lobby_func_00576300(void) {
    u8 g_frame[0x100] __attribute__((aligned(16)));   /* G's stack frame */
#define sp80 (*(s32 *)(g_frame + 0x80))
#define spA0 (*(s32 *)(g_frame + 0xA0))
    s32 temp_a0;
    s32 temp_a1;
    s32 var_a4;
    s32 var_s0;
    s32 var_s2_2;
    s32 var_s3;
    s32 var_s4;
    s64 temp_s2;
    s64 temp_s2_2;
    s64 temp_s3;
    s64 var_v0;
    s64 var_v0_2;
    s8 *var_s2;
    s8 *var_s4_2;
    s8 *var_s5;
    s8 *var_s5_2;
    u8 temp_v1;
    u8 *temp_a2;
    u8 *var_a2;

    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        switch ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A))) { /* irregular */
        case 2:
            if (((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) != 4) && ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) != 3) && ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) != 2)) {
                lobby_func_00575FB0();
                return;
            }
            lobby_func_005775C0(0xD8, 0x50, (s64) (*(s8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1374)), (s64) (*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)));
            return;
        case 6:
            plaza_searchMemberTrans(0x144);
            return;
        case 8:
            var_s0 = 0x90;
            if ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371)) == 2) {
                if ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) == 0) {
                    lobby_func_005737C0((s64) g_lb_636B48);
                    return;
                }
                lobby_func_005791D0(3);
                put_main_cursor((s32) (*(s8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)));
                var_s4 = 0;
                var_s2 = &(*(s8 *)((u8 *)&tl_member_buff + 0x18));
                var_s5 = (*(s32 *)&g_main_2EFBB0) + ((*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) * 0x118) + 4;
                do {
                    temp_a0 = var_s4 + ((*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) * 7);
                    temp_s3 = var_s4 != (*(s8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372));
                    if ((temp_a0 < 0x64) && (*var_s5 != 0)) {
                        han2zen(var_s5, &sp80);
                        flfntLocate(0xE2, var_s0);
                        temp_v1 = (*(u8 *)((u8 *)(var_s2) + 0x35D));
                        if (temp_v1 != 2) {
                            if (temp_v1 == 0) {
                                var_a4 = (*(s32 *)((u8 *)&tl_hr_tbl + 0x38));
                            } else {
                                var_a4 = (*(s32 *)((u8 *)&tl_hr_tbl + 0x3C));
                            }
                            sprintf(&spA0, &g_lb_63E2A8, var_s5 + 8, &sp80, var_a4);
                            if ((((s8) (temp_s3))) == 0) {
                                if (memcmp(&my_user_id, var_s5, 8) == 0) {
                                    font_print_double(0xE2, var_s0, 1, 0xA, &spA0);
                                } else {
                                    font_print_double(0xE2, var_s0, 1, 4, &spA0);
                                }
                            } else {
                                if (memcmp(&my_user_id, var_s5, 8) == 0) {
                                    font_set_palette(0xA);
                                } else {
                                    font_set_palette(0);
                                }
                                font_print(&lit_2316, &spA0);
                            }
                        } else {
                            sprintf(&spA0, &g_lb_63E2A8, var_s5 + 8, &sp80, (*(s32 *)((u8 *)&tl_hr_tbl + 0x40)));
                            if ((((s8) (temp_s3))) == 0) {
                                font_print_double(0xE2, var_s0, 1, 0xA, &spA0);
                            } else {
                                font_set_palette(0xA);
                                font_print(&lit_2316, &spA0);
                            }
                        }
                    } else if (((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) == temp_a0) && ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) == 0x64)) {
                        font_print_double(0x11E, var_s0, 1, 4, g_lb_636DF8);
                    }
                    var_s4 += 1;
                    var_s0 = (s32) (((s16) ((var_s0 + 0x16))));
                    var_s2 += 0x24A;
                    var_s5 += 0x28;
                } while (var_s4 < 7);
                Put_page_num(0x1FA, (s64) var_s0, (s64) (*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)), (s64) (*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1366)), 0);
                return;
            }
            if ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371)) == 3) {
                if ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) == 0) {
                    lobby_func_005737C0((s64) g_lb_636B48);
                    return;
                }
                lobby_func_005791D0(8);
                put_main_cursor((s32) (*(s8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)));
                var_s3 = 0;
                var_s5_2 = &(*(s8 *)((u8 *)&tl_member_buff + 0x18));
                var_s4_2 = &(*(s8 *)((u8 *)&tl_member_buff + 0x18));
                do {
                    if (var_s3 == (*(s8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372))) {
                        if (*var_s5_2 != 0) {
                            han2zen(var_s5_2, &spA0);
                        } else {
                            strcpy(&spA0, (*(s32 *)((u8 *)&tl_hr_tbl + 0x48)));
                        }
                        var_v0 = 4;
                        if ((*(u8 *)((u8 *)(var_s4_2) + 0x35D)) == 2) {
                            var_v0 = 0xA;
                        }
                        temp_s2 = ((s16) (var_v0));
                        font_print_double(0xE2, var_s0, 1, temp_s2, &spA0);
                        if (*var_s5_2 != 0) {
                            font_print_double(0x1F8, var_s0, 1, temp_s2, (*(s32 **)((u8 *)(((u8 *)&tl_etc + ((*(u8 *)((u8 *)(var_s4_2) + 0x35D)) * 4))) + 0x18)));
                        } else {
                            font_print_double(0x1F8, var_s0, 1, temp_s2, (*(s32 * *)((u8 *)&tl_hr_tbl + 0x44)));
                        }
                    } else {
                        if (*var_s5_2 != 0) {
                            han2zen(var_s5_2, &spA0);
                        } else {
                            strcpy(&spA0, (*(s32 *)((u8 *)&tl_hr_tbl + 0x48)));
                        }
                        var_v0_2 = 0;
                        if ((*(u8 *)((u8 *)(var_s4_2) + 0x35D)) == 2) {
                            var_v0_2 = 0xA;
                        }
                        temp_s2_2 = ((s16) (var_v0_2));
                        font_print_ex(0xE2, var_s0, temp_s2_2, &spA0);
                        if (*var_s5_2 != 0) {
                            font_print_ex(0x1F8, var_s0, temp_s2_2, (*(s32 **)((u8 *)(((u8 *)&tl_etc + ((*(u8 *)((u8 *)(var_s4_2) + 0x35D)) * 4))) + 0x18)));
                        } else {
                            font_print_ex(0x1F8, var_s0, temp_s2_2, (*(s32 * *)((u8 *)&tl_hr_tbl + 0x44)));
                        }
                    }
                    var_s3 += 1;
                    var_s0 = (s32) (((s16) ((var_s0 + 0x16))));
                    var_s5_2 += 0x5C;
                    var_s4_2 += 0x24A;
                } while (var_s3 < 3);
                return;
            }
            if ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) == 0) {
                lobby_func_005737C0((s64) g_lb_636B48);
                return;
            }
            lobby_func_005791D0(0);
            put_main_cursor((s32) (*(s8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)));
            var_s2_2 = 0;
            do {
                temp_a1 = var_s2_2 + ((*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) * 7);
                temp_a2 = (*(s32 *)&g_main_2EFBC4) + (temp_a1 * 0x5C) + 4;
                if (temp_a1 < (s32) (*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D))) {
                    put_member_info(0xE2, var_s0, temp_a2, temp_a2 + 8, temp_a2 + 0x1C, var_s2_2 != (*(s8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)));
                } else if (((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) == temp_a1) && ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) == 0x64)) {
                    font_print_double(0x11E, var_s0, 1, 4, g_lb_636DF8);
                }
                var_s2_2 += 1;
                var_s0 = (s32) (((s16) ((var_s0 + 0x16))));
            } while (var_s2_2 < 7);
            Put_page_num(0x1FA, (s64) var_s0, (s64) (*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)), (s64) (*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1366)), 0);
            return;
        case 11:
            if ((*(u8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) == 2) {
                var_a2 = &(*(s8 *)((u8 *)&tl_member_buff + 0x18)) + (((s16) (*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) % 3) * 0x5C);
            } else {
                var_a2 = (*(s32 *)&g_main_2EFBC4) + ((*(s16 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x5C) + 4;
            }
            disp_status(0xD8, 0x50, (s32) var_a2, var_a2 + 8, var_a2 + 0x1C, (s64) (*(s8 *)((u8 *)(&(*(s8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)), 3, (*(s32 *)&cw) + 0xAF8, (s32) 0x64);
            break;
        }
    }
}

