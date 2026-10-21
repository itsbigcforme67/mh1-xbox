/* G online (PC): lobby_func_00578A90, written from Monster Hunter G's code at lobby 0x578A90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFBAC;
extern u8 cw;
extern u8 g_main_2EFBAC;
extern u8 tl_mail_tbl;
s32 KinshiYogo_chk();                        /* extern */
s32 Put_comment();        /* extern */
s32 flfntLocate();                      /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();           /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();                    /* extern */
extern u8 lit_2316;

void lobby_func_00578A90(s64 arg0, s64 arg1) {
    union { s32 v; u8 b[0x80]; } u_sp80;
#define sp80 (u_sp80.v)
    s32 temp_s0;
    s32 var_s2;
    s32 var_s3;
    s64 temp_s6;
    s64 var_s4;

    var_s4 = arg1;
    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    var_s3 = 0;
    temp_s6 = (s64) (arg0 << 0x30) >> 0x30;
    var_s2 = 0;
    do {
        flfntLocate(arg0, var_s4);
        font_print(&lit_2316, (*(u8 *)((u8 *)&tl_mail_tbl + 0xC)));
        if ((*(s8 *)((u8 *)(((*(s32 *)&g_main_2EFBAC) + var_s2)) + 0x9A)) != 0) {
            flfntLocate((s64) ((temp_s6 + 0x3C) << 0x30) >> 0x30, var_s4);
            han2zen((*(s32 *)&g_main_2EFBAC) + var_s2 + 0x9A, &sp80);
            font_print(&lit_2316, &sp80);
            if ((*(s8 *)((u8 *)(((*(s32 *)&g_main_2EFBAC) + var_s2)) + 0xA2)) != 0) {
                flfntLocate((s64) ((temp_s6 + 0xD6) << 0x30) >> 0x30, var_s4);
                font_print(&lit_2316, (*(s32 *)&g_main_2EFBAC) + var_s2 + 0xA2);
            }
        }
        var_s3 += 1;
        var_s4 = (s64) ((var_s4 + 0x16) << 0x30) >> 0x30;
        var_s2 += 0x5C;
    } while (var_s3 < 3);
    flfntLocate(arg0, var_s4);
    font_print(&lit_2316, (*(u8 *)((u8 *)&tl_mail_tbl + 0x10)));
    font_set_palette(0);
    temp_s0 = (*(s32 *)&cw) + 0xF00;
    KinshiYogo_chk(temp_s0);
    Put_comment((s64) ((temp_s6 + 0x3C) << 0x30) >> 0x30, (s64) ((((s64) (var_s4 << 0x30) >> 0x30) - 0x16) << 0x30) >> 0x30, 0x16, temp_s0);
    font_set_palette(0);
}

