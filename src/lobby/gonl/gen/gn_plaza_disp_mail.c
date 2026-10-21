/* G online (PC): plaza_disp_mail, written from Monster Hunter G's code at lobby 0x578920 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 tl_mail_tbl;
s32 KinshiYogo_chk();                        /* extern */
s32 Put_comment(); /* extern */
s32 flfntLocate();                      /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();           /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();                    /* extern */
extern u8 lit_2316;

void plaza_disp_mail(s64 arg0, s64 arg1) {
    u8 g_frame[0xC0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s32 *)(g_frame + 0x40))
    s32 temp_s2;
    s64 temp_s0;
    s64 temp_s1;

    font_set_palette(0);
    flfntLocate(arg0, arg1);
    font_print(&lit_2316, (*(s32 * *)((u8 *)&tl_mail_tbl + 0x8)));
    temp_s0 = (((s16) (arg0))) + 0x3C;
    flfntLocate(((s16) (temp_s0)), arg1);
    flfntSetSize(0x16, 0x12);
    font_print(&lit_2316, (*(s32 *)&cw) + 0xEEF);
    flfntSetSize(0x12, 0x12);
    temp_s1 = ((s16) ((arg1 + 0x16)));
    flfntLocate(arg0, temp_s1);
    font_print(&lit_2316, (*(s32 * *)((u8 *)&tl_mail_tbl + 0xC)));
    flfntLocate(((s16) (temp_s0)), temp_s1);
    han2zen((*(s32 *)&cw) + 0xEE7, &sp40);
    font_print(&lit_2316, &sp40);
    flfntLocate(arg0, ((s16) (((((s16) (temp_s1))) + 0x16))));
    font_print(&lit_2316, (*(s32 * *)((u8 *)&tl_mail_tbl + 0x10)));
    font_set_palette(0);
    temp_s2 = (*(s32 *)&cw) + 0xF00;
    KinshiYogo_chk(temp_s2);
    Put_comment(((s16) (temp_s0)), temp_s1, 0x16, temp_s2, 0x24);
    font_set_palette(0);
}

