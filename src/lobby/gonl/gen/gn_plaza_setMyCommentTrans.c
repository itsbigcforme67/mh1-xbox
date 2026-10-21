/* G online (PC): plaza_setMyCommentTrans, written from Monster Hunter G's code at lobby 0x577E80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 User_data;
extern u8 cw;
extern u8 game_w;
extern u8 net_etc_mes_tbl;
extern u8 tl_mail_tbl;
s32 Lb_put_status();  /* extern */
s32 flfntLocate();                      /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();           /* extern */
s32 font_set_palette();                  /* extern */
s32 func_0012BDD0();                         /* extern */
s32 han2zen();              /* extern */
s32 lb_put_comment();  /* extern */
s32 memcpy();      /* extern */
s32 put_mainWindow();                   /* extern */
s32 put_mainWindowTex();                /* extern */
s32 put_titles();                  /* extern */
s32 sprintf();           /* extern */
extern u8 g_lb_636A00;
extern u8 g_lb_63E128;
extern u8 hunter_appellation;
extern u8 lbCommer;
extern u8 lit_2316;
extern u8 my_user_mini_data;
extern u8 tl_job_tbl;

void plaza_setMyCommentTrans(s64 arg0, s32 arg1, s64 arg2) {
    u8 g_frame[0x100] __attribute__((aligned(16)));   /* G's stack frame */
#define sp60 (*(s32 *)(g_frame + 0x60))
#define spC0 (*(s32 *)(g_frame + 0xC0))
    u8 *var_s4;   /* (s32 *) */
    s64 temp_s0;
    s64 temp_s0_2;
    s64 temp_s0_3;
    s64 temp_s0_4;
    s64 temp_s0_5;
    s64 temp_s0_6;
    s64 temp_s0_7;
    s64 temp_s2;
    s64 temp_s3;
    s64 temp_v1;
    u8 temp_a2;

    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
        var_s4 = (u8 *)&lbCommer + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x5C) + 0x1C;
    } else {
        var_s4 = &my_user_mini_data;
    }
    flfntSetSize(0x12, 0x12);
    temp_v1 = (s64) (arg2 << 0x38) >> 0x38;
    switch (temp_v1) {                              /* irregular */
    case 0:
        put_mainWindow(arg0, arg1);
        break;
    case 1:
        put_mainWindowTex(arg0, arg1);
        break;
    }
    temp_s0 = (s64) ((arg1 + 0x28) << 0x30) >> 0x30;
    put_titles(arg0, temp_s0, (*(u8 *)((u8 *)&tl_mail_tbl + 0x28)));
    font_set_palette(0);
    temp_s0_2 = (s64) ((temp_s0 + 0x16) << 0x30) >> 0x30;
    flfntLocate(arg0, temp_s0_2);
    font_print(&lit_2316, (*(u8 *)((u8 *)&tl_mail_tbl + 0x8)));
    temp_s2 = (s64) (arg0 << 0x30) >> 0x30;
    temp_s3 = temp_s2 + 0x36;
    flfntLocate((s64) (temp_s3 << 0x30) >> 0x30, temp_s0_2);
    font_print(&lit_2316, (*(void **)&cw) + 0x1C8);
    temp_s0_3 = (s64) ((temp_s0_2 + 0x16) << 0x30) >> 0x30;
    flfntLocate(arg0, temp_s0_3);
    font_print(&lit_2316, (*(u8 *)((u8 *)&tl_mail_tbl + 0xC)));
    flfntLocate((s64) (temp_s3 << 0x30) >> 0x30, temp_s0_3);
    memcpy(&spC0, (*(void **)&cw) + 0x1C0, 8);
    han2zen(&spC0, &sp60);
    font_print(&lit_2316, &sp60);
    temp_s0_4 = (s64) ((temp_s0_3 + 0x16) << 0x30) >> 0x30;
    flfntLocate(arg0, temp_s0_4);
    font_print(&lit_2316, (*(u8 *)((u8 *)&tl_mail_tbl + 0x18)));
    flfntLocate((s64) (temp_s3 << 0x30) >> 0x30, temp_s0_4);
    font_print(&lit_2316, (*(s32 *)((u8 *)&tl_job_tbl + ((*(u8 *)((u8 *)(var_s4) + 0)) * 4))));
    temp_s0_5 = (s64) ((temp_s0_4 + 0x16) << 0x30) >> 0x30;
    flfntLocate(arg0, temp_s0_5);
    font_print(&lit_2316, (*(u8 *)((u8 *)&tl_mail_tbl + 0x1C)));
    temp_a2 = (*(u8 *)((u8 *)&User_data + 0xC23));
    if (temp_a2 != 0) {
        sprintf(&spC0, (*(u8 *)((u8 *)&net_etc_mes_tbl + 0x1C)), temp_a2, (*(s32 *)((u8 *)&hunter_appellation + (temp_a2 * 4))));
    } else {
        sprintf(&spC0, (*(u8 *)((u8 *)&net_etc_mes_tbl + 0x20)), temp_a2);
    }
    flfntLocate((s64) (temp_s3 << 0x30) >> 0x30, temp_s0_5);
    font_print(&lit_2316, &spC0);
    temp_s0_6 = (s64) ((temp_s0_5 + 0x16) << 0x30) >> 0x30;
    flfntLocate(arg0, temp_s0_6);
    font_print(&lit_2316, (*(u8 *)((u8 *)&tl_mail_tbl + 0x2C)));
    if ((*(u8 *)((u8 *)(var_s4) + 2)) == 0) {
        flfntLocate((s64) ((temp_s2 + 0x1C) << 0x30) >> 0x30, temp_s0_6);
    } else {
        flfntLocate((s64) ((temp_s2 + 0x40) << 0x30) >> 0x30, temp_s0_6);
    }
    memcpy((*(s32 *)((u8 *)&g_lb_636A00 + ((*(u8 *)((u8 *)(var_s4) + 2)) * 4))), &g_lb_63E128, 4);
    func_0012BDD0((*(s32 *)((u8 *)&g_lb_636A00 + ((*(u8 *)((u8 *)(var_s4) + 2)) * 4))));
    Lb_put_status((s64) ((temp_s2 + 0x40) << 0x30) >> 0x30, temp_s0_6, 0x14, -1);
    temp_s0_7 = (s64) ((temp_s0_6 + 0x16) << 0x30) >> 0x30;
    flfntLocate(arg0, temp_s0_7);
    font_print(&lit_2316, (*(u8 *)((u8 *)&tl_mail_tbl + 0x24)));
    lb_put_comment((s64) ((temp_s2 + 0x5E) << 0x30) >> 0x30, (s64) ((((s64) (temp_s0_7 << 0x30) >> 0x30) - 2) << 0x30) >> 0x30, (*(void **)&cw) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x62) + 0x724, 1);
}

