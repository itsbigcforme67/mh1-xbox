/* G online (PC): plaza_mailBoxTrans, written from Monster Hunter G's code at lobby 0x577A20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBAC;
extern u8 pNet;
extern u8 tl_mail_tbl;
extern u8 tl_msg_tbl;
s32 Lb_put_icon();    /* extern */
s32 flfntLocate();                      /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();           /* extern */
s32 font_print_double(); /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();                 /* extern */
s32 lobby_func_005737C0();                   /* extern */
s32 lobby_func_005778B0();              /* extern */
s32 lobby_func_005791D0();       /* extern */
s32 plaza_disp_mail();                  /* extern */
s32 put_mainWindow();                        /* extern */
s32 put_mainWindowTex();                     /* extern */
s32 put_main_cursor2();             /* extern */
s32 put_titles();                  /* extern */
extern u8 g_lb_636B44;
extern u8 RecvMailInfo;
extern u8 lb_num_str;
extern u8 lit_2316;

void plaza_mailBoxTrans(s64 arg0, s64 arg1, s64 arg2) {
    union { s32 v; u8 b[0x20]; } u_spA0;
#define spA0 (u_spA0.v)
    s32 *var_s0;
    s32 *var_s1;
    s32 *var_s3;
    s64 temp_a2;
    s64 temp_fp;
    s64 temp_s0;
    s64 temp_s1;
    s64 temp_s2;
    s64 temp_s5;
    s64 temp_s6;
    s64 temp_s7;
    s64 temp_v1;
    s64 var_s2;
    s64 var_s4;
    u8 temp_a0;
    u8 *temp_a1;

    temp_a2 = (s64) (arg2 << 0x38) >> 0x38;
    var_s3 = &RecvMailInfo;
    switch (temp_a2) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        put_mainWindow(temp_a2);
        break;
    case 1:                                         /* switch 1 */
        put_mainWindowTex(temp_a2);
        break;
    }
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        font_set_palette(0);
        flfntSetSize(0x12, 0x12);
        temp_a1 = (*(void **)&g_main_2EFBAC);
        temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0x230));
        switch (temp_a0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            if ((*(u8 *)((u8 *)(temp_a1) + 0x235)) != 0) {
                put_main_cursor2(arg0, arg1, (*(s8 *)((u8 *)(temp_a1) + 0x236)));
                lobby_func_005791D0(5);
                var_s2 = 0;
                var_s4 = (s64) ((arg1 + 0x3E) << 0x30) >> 0x30;
                if ((s32) (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x235)) > 0) {
                    var_s1 = &lb_num_str;
                    var_s0 = &RecvMailInfo;
                    temp_v1 = (s64) ((((s64) (arg0 << 0x30) >> 0x30) + 0xA) << 0x30) >> 0x30;
                    temp_s5 = temp_v1 + 9;
                    temp_s6 = temp_v1 + 0x78;
                    temp_s7 = temp_v1 + 0x104;
                    temp_fp = temp_v1 + 0x40;
loop_14:
                    if ((*(s8 *)((u8 *)(var_s3) + 1)) != 0) {
                        if ((*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x236)) == ((s64) (var_s2 << 0x30) >> 0x30)) {
                            font_print_double((s64) (temp_s5 << 0x30) >> 0x30, var_s4, 1, 4);
                            flfntSetSize(0x16, 0x12);
                            font_print_double((s64) (temp_s6 << 0x30) >> 0x30, var_s4, 1, 4);
                            flfntSetSize(0x12, 0x12);
                            han2zen(var_s0 + 1, &spA0);
                            font_print_double((s64) (temp_s7 << 0x30) >> 0x30, var_s4, 1, 4);
                        } else {
                            font_set_palette(0);
                            flfntLocate((s64) (temp_s5 << 0x30) >> 0x30, var_s4);
                            font_print(&lit_2316, (*(s32 **)((u8 *)(var_s1) + 4)));
                            flfntSetSize(0x16, 0x12);
                            flfntLocate((s64) (temp_s6 << 0x30) >> 0x30, var_s4);
                            font_print(&lit_2316, var_s0 + 9);
                            flfntSetSize(0x12, 0x12);
                            flfntLocate((s64) (temp_s7 << 0x30) >> 0x30, var_s4);
                            han2zen(var_s0 + 1, &spA0);
                            font_print(&lit_2316, &spA0);
                        }
                        if ((*(u8 *)((u8 *)(var_s3) + 0)) == 0) {
                            Lb_put_icon((s64) (temp_fp << 0x30) >> 0x30, var_s4, 0xA, -1);
                        } else {
                            Lb_put_icon((s64) (temp_fp << 0x30) >> 0x30, var_s4, 9, -1);
                        }
                        var_s3 += 0x9A;
                        var_s1 += 4;
                        var_s4 = (s64) ((var_s4 + 0x16) << 0x30) >> 0x30;
                        var_s2 = (s64) ((var_s2 + 1) << 0x30) >> 0x30;
                        var_s0 += 0x9A;
                        if (var_s2 >= (s32) (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x235))) {
                            return;
                        }
                        goto loop_14;
                    }
                }
            } else {
                lobby_func_005791D0(5, temp_a1);
                flfntSetSize(0x14, 0x14);
                lobby_func_005737C0(g_lb_636B44);
                return;
            }
            break;
        case 1:                                     /* switch 2 */
            temp_s1 = ((s64) (arg1 << 0x30) >> 0x30) + 0x28;
            temp_s0 = ((s64) (arg0 << 0x30) >> 0x30) + 0xA;
            put_titles((s64) (temp_s0 << 0x30) >> 0x30, (s64) (temp_s1 << 0x30) >> 0x30, (*(u8 *)((u8 *)&tl_msg_tbl + 0xC)));
            temp_s2 = (s64) (temp_s0 << 0x30) >> 0x30;
            put_titles((s64) ((temp_s2 + 0x12C) << 0x30) >> 0x30, (s64) (temp_s1 << 0x30) >> 0x30, (*(u8 *)((u8 *)&tl_mail_tbl + 0x4)));
            put_titles((s64) ((temp_s2 + 0x168) << 0x30) >> 0x30, (s64) (temp_s1 << 0x30) >> 0x30, *(&(*(u8 *)((u8 *)&lb_num_str + 0x4)) + ((*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x236)) * 4)));
            plaza_disp_mail(temp_s2, (s64) ((((s64) (temp_s1 << 0x30) >> 0x30) + 0x16) << 0x30) >> 0x30);
            return;
        case 2:                                     /* switch 2 */
            lobby_func_005778B0(arg0, arg1);
            break;
        }
    }
}

