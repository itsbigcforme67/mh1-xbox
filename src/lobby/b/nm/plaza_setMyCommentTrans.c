#include "lobby_a.h"
extern char tl_mail_tbl[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char D_3C738C[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2602[];
extern char lit_2316[];
extern char lit_2316[];
extern char D_3C73B4[];
extern char tl_job_tbl[];
void plaza_setMyCommentTrans(int arg0, s32 arg1, int arg2) {
    char sp70[0x20];
    char sp50[0x40];
    s32 var_s2;
    int temp_s0;
    int temp_s1;
    int temp_s1_2;
    int temp_s3;
    int temp_s3_2;
    int temp_s3_3;
    int temp_s3_4;
    int temp_s3_5;
    int temp_s3_6;
    int temp_v1;

    flfntSetSize(0x12, 0x12);
    temp_v1 = (s8)arg2;
    switch (temp_v1) {                              /* irregular */
    case 0:
        put_mainWindow();
        break;
    case 1:
        put_mainWindowTex(arg0, arg1);
        break;
    }
    temp_s0 =  ((s16)(arg0));
    temp_s3 =  ((s16)((arg1 + 0x28)));
    put_titles( ((s16)((temp_s0 + 0xA))), temp_s3, F(s32, &tl_mail_tbl, 0x2C));
    font_set_palette(0);
    temp_s3_2 =  ((s16)((temp_s3 + 0x16)));
    flfntLocate( ((s16)((temp_s0 + 0xA))), temp_s3_2);
    font_print(&lit_2316, F(int, &tl_mail_tbl, 8));
    temp_s1 = ( ((s16)((temp_s0 + 0xA)))) + 0x36;
    flfntLocate( ((s16)(temp_s1)), temp_s3_2);
    font_print(&lit_2316, (s32)cw + 0x448);
    temp_s3_3 =  ((s16)((temp_s3_2 + 0x16)));
    flfntLocate( ((s16)((temp_s0 + 0xA))), temp_s3_3);
    font_print(&lit_2316, F(int, &tl_mail_tbl, 0xC));
    flfntLocate( ((s16)(temp_s1)), temp_s3_3);     /* (asm: a1 = the line's y) */
    memcpy(sp70, (s32)cw + 0x440, 8);
    han2zen(sp70, sp50);
    font_print(&lit_2316, sp50);
    var_s2 = Get_weapon_job(&D_3C738C) & 0xFF;
    if (var_s2 == 5) {
        var_s2 = 1;
    }
    temp_s3_4 =  ((s16)((temp_s3_3 + 0x16)));
    flfntLocate( ((s16)((temp_s0 + 0xA))), temp_s3_4);
    font_print(&lit_2316, F(int, &tl_mail_tbl, 0x1C));
    temp_s1_2 = ( ((s16)((temp_s0 + 0xA)))) + 0x36;
    flfntLocate(((s16)(temp_s1_2)), temp_s3_4);
    font_print(&lit_2316, ((int *)&tl_job_tbl)[(var_s2 & 0xFF)]);
    temp_s3_5 =  ((s16)((temp_s3_4 + 0x16)));
    flfntLocate( ((s16)((temp_s0 + 0xA))), temp_s3_5);
    font_print(&lit_2316, F(int, &tl_mail_tbl, 0x20));
    flfntLocate(((s16)(temp_s1_2)), temp_s3_5);
    sprintf(sp70, &lit_2602, *(u8 *)0x3C733B);
    han2zen(sp70, sp50);
    font_print(&lit_2316, sp50);
    temp_s3_6 =  ((s16)((temp_s3_5 + 0x16)));
    flfntLocate( ((s16)((temp_s0 + 0xA))), temp_s3_6);
    font_print(&lit_2316, F(int, &tl_mail_tbl, 0x28));
    lb_put_comment( ((s16)((temp_s0 + 0x68))),  ((s16)((( ((s16)(temp_s3_6))) - 2))), &D_3C73B4, 1);
}
