/* G online (PC): plaza_setMyCommentTrans, written from Monster Hunter G's code at lobby 0x577E80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_status();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_set_palette();
s32 han2zen();
s32 lb_put_comment();
void * memcpy();
s32 put_mainWindow();
s32 put_mainWindowTex();
extern u8 User_data;
extern u8 cw;
extern u8 game_w;
extern u8 net_etc_mes_tbl;
extern u8 tl_mail_tbl;
s32 font_print();           /* extern */
s32 func_0012BDD0();                         /* extern */
s32 put_titles();                  /* extern */
s32 sprintf();           /* extern */
extern s32 g_lb_636A00;
extern s32 g_lb_63E128;
extern s32 hunter_appellation;
extern s32 lbCommer;
extern s32 lit_2316;
extern u8 my_user_mini_data;
extern s32 tl_job_tbl;

void plaza_setMyCommentTrans(s32 arg0, s32 arg1, s32 arg2) {
    u8 g_frame[0x100] __attribute__((aligned(16)));   /* G's stack frame */
#define sp60 (*(s32 *)(g_frame + 0x60))
#define spC0 (*(s32 *)(g_frame + 0xC0))
    u8 *var_s4;   /* (s32 *) */
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_s0_3;
    s32 temp_s0_4;
    s32 temp_s0_5;
    s32 temp_s0_6;
    s32 temp_s0_7;
    s32 temp_s2;
    s32 temp_s3;
    s32 temp_v1;
    u8 temp_a2;

    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
        var_s4 = (u8 *)&lbCommer + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x5C) + 0x1C;
    } else {
        var_s4 = &my_user_mini_data;
    }
    flfntSetSize(0x12, 0x12);
    temp_v1 = ((s8) (arg2));
    switch (temp_v1) {                              /* irregular */
    case 0:
        put_mainWindow(arg0, arg1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        break;
    case 1:
        put_mainWindowTex(arg0, arg1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        break;
    }
    temp_s0 = ((s16) ((arg1 + 0x28)));
    put_titles(arg0, temp_s0, (*(s32 *)((u8 *)&tl_mail_tbl + 0x28)));
    font_set_palette(0);
    temp_s0_2 = ((s16) ((temp_s0 + 0x16)));
    flfntLocate(arg0, (s32) temp_s0_2);
    font_print(&lit_2316, (*(s32 * *)((u8 *)&tl_mail_tbl + 0x8)));
    temp_s2 = ((s16) ((s32) arg0));
    temp_s3 = temp_s2 + 0x36;
    flfntLocate((s32) (((s16) (temp_s3))), (s32) temp_s0_2);
    font_print(&lit_2316, (*(void **)&cw) + 0x1C8);
    temp_s0_3 = ((s16) ((temp_s0_2 + 0x16)));
    flfntLocate(arg0, (s32) temp_s0_3);
    font_print(&lit_2316, (*(s32 * *)((u8 *)&tl_mail_tbl + 0xC)));
    flfntLocate((s32) (((s16) (temp_s3))), (s32) temp_s0_3);
    memcpy(&spC0, (*(void **)&cw) + 0x1C0, 8);
    han2zen(&spC0, &sp60);
    font_print(&lit_2316, &sp60);
    temp_s0_4 = ((s16) ((temp_s0_3 + 0x16)));
    flfntLocate(arg0, (s32) temp_s0_4);
    font_print(&lit_2316, (*(s32 * *)((u8 *)&tl_mail_tbl + 0x18)));
    flfntLocate((s32) (((s16) (temp_s3))), (s32) temp_s0_4);
    font_print(&lit_2316, (*(s32 *)((u8 *)&tl_job_tbl + ((*(u8 *)((u8 *)(var_s4) + 0)) * 4))));
    temp_s0_5 = ((s16) ((temp_s0_4 + 0x16)));
    flfntLocate(arg0, (s32) temp_s0_5);
    font_print(&lit_2316, (*(s32 * *)((u8 *)&tl_mail_tbl + 0x1C)));
    temp_a2 = (*(u8 *)((u8 *)&User_data + 0xC23));
    if (temp_a2 != 0) {
        sprintf(&spC0, (*(s32 *)((u8 *)&net_etc_mes_tbl + 0x1C)), temp_a2, (*(s32 *)((u8 *)&hunter_appellation + (temp_a2 * 4))));
    } else {
        sprintf(&spC0, (*(s32 *)((u8 *)&net_etc_mes_tbl + 0x20)), temp_a2);
    }
    flfntLocate((s32) (((s16) (temp_s3))), (s32) temp_s0_5);
    font_print(&lit_2316, &spC0);
    temp_s0_6 = ((s16) ((temp_s0_5 + 0x16)));
    flfntLocate(arg0, (s32) temp_s0_6);
    font_print(&lit_2316, (*(s32 * *)((u8 *)&tl_mail_tbl + 0x2C)));
    if ((*(u8 *)((u8 *)(var_s4) + 2)) == 0) {
        flfntLocate((s32) (((s16) ((temp_s2 + 0x1C)))), (s32) temp_s0_6);
    } else {
        flfntLocate((s32) (((s16) ((temp_s2 + 0x40)))), (s32) temp_s0_6);
    }
    memcpy((*(s32 *)((u8 *)&g_lb_636A00 + ((*(u8 *)((u8 *)(var_s4) + 2)) * 4))), &g_lb_63E128, 4);
    func_0012BDD0((*(s32 *)((u8 *)&g_lb_636A00 + ((*(u8 *)((u8 *)(var_s4) + 2)) * 4))));
    Lb_put_status((s32) (((s16) ((temp_s2 + 0x40)))), (s32) temp_s0_6, 0x14, -1, (s32) (*(u8 *)((u8 *)(var_s4) + 2)));
    temp_s0_7 = ((s16) ((temp_s0_6 + 0x16)));
    flfntLocate(arg0, (s32) temp_s0_7);
    font_print(&lit_2316, (*(s32 * *)((u8 *)&tl_mail_tbl + 0x24)));
    lb_put_comment((s32) (((s16) ((temp_s2 + 0x5E)))), (s32) (((s16) (((((s16) (temp_s0_7))) - 2)))), (*(void **)&cw) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x62) + 0x724, 1);
}

