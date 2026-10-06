#include "lobby_a.h"
extern char tl_mail_tbl[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2602[];
extern char lit_2316[];
extern char lit_2603[];
extern char lit_2316[];
extern char lit_193_0065DBE8[];
extern char lit_193_0065DBE8[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char tl_job_tbl[];
extern char hunter_appellation[];
extern char PlazaInfo[];
extern char LobbyInfo[];
extern char D_3351D4[];
extern char D_3367BC[];
extern char D_336C00[];
extern char D_3371E0[];
extern char D_337800[];
extern char D_337E10[];
extern char D_338360[];
void disp_status(int arg0, s32 arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7) {
    int sp100;
    int spB0;
    s32 spAC;
    int temp_s0;
    int temp_s0_2;
    int temp_s1;
    int temp_s1_2;
    int temp_s1_4;
    int temp_s2;
    int temp_s4;
    int temp_s4_10;
    int temp_s4_11;
    int temp_s4_2;
    int temp_s4_3;
    int temp_s4_4;
    int temp_s4_5;
    int temp_s4_6;
    int temp_s4_7;
    int temp_s4_8;
    int temp_s4_9;
    int temp_s5;
    int temp_s6;
    int var_s4;
    u16 temp_s0_3;
    u16 temp_s1_3;
    int temp_s3;

    var_s4 =  ((arg1 + 0x28) << 0x30) >> 0x30;
    temp_s0 =  (arg0 << 0x30) >> 0x30;
    temp_s3 = arg4;
    temp_s1 = arg5;
    spAC = arg7;
    put_titles( ((temp_s0 + 0xA) << 0x30) >> 0x30, var_s4, F(s32, &tl_mail_tbl, 0x2C));
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    temp_s5 =  ((temp_s0 + 0xA) << 0x30) >> 0x30;
    Put_page_num( ((temp_s5 + 0x12C) << 0x30) >> 0x30, var_s4, (s8)temp_s1, (s8)arg6);
    temp_s1_2 = (s8)temp_s1;
    if (temp_s1_2 < 2) {
        temp_s4 =  ((var_s4 + 0x16) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4);
        font_print(&lit_2316, F(int, &tl_mail_tbl, 8));
        flfntSetSize(0x12, 0x12);
        temp_s6 =  ((temp_s0 + 0xA) << 0x30) >> 0x30;
        temp_s2 = temp_s6 + 0x3C;
        flfntLocate( (temp_s2 << 0x30) >> 0x30, temp_s4);
        font_print(&lit_2316, arg3);
        flfntSetSize(0x12, 0x12);
        temp_s4_2 =  ((temp_s4 + 0x16) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4_2);
        font_print(&lit_2316, F(int, &tl_mail_tbl, 0xC));
        flfntLocate( (temp_s2 << 0x30) >> 0x30);
        han2zen(arg2, &sp100);
        font_print(&lit_2316, &sp100);
        temp_s4_3 =  ((temp_s4_2 + 0x16) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4_3);
        font_print(&lit_2316, F(int, &tl_mail_tbl, 0x1C));
        flfntLocate( (temp_s2 << 0x30) >> 0x30);
        font_print(&lit_2316, ((int *)&tl_job_tbl)[F(u8, temp_s3, 0)]);
        var_s4 =  ((temp_s4_3 + 0x16) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0xA) << 0x30) >> 0x30, var_s4);
        font_print(&lit_2316, F(int, &tl_mail_tbl, 0x20));
        flfntLocate( (temp_s2 << 0x30) >> 0x30, var_s4);
        sprintf(&sp100, &lit_2602, F(u8, temp_s3, 1));
        han2zen(&sp100, &spB0);
        font_print(&lit_2316, &spB0);
        flfntLocate( ((temp_s6 + 0x64) << 0x30) >> 0x30, var_s4);
        font_print(&lit_2603, ((int *)&hunter_appellation)[F(u8, temp_s3, 1)]);
    }
    switch (temp_s1_2) {                            /* irregular */
    case 0:
        temp_s4_4 =  ((var_s4 + 0x16) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4_4);
        font_print(&lit_2316, F(int, &tl_mail_tbl, 0x24));
        temp_s0_2 = temp_s5 + 0x50;
        flfntLocate( (temp_s0_2 << 0x30) >> 0x30);
        temp_s1_3 = F(u16, (u8 *)cw, 0x30B4);
        if (temp_s1_3 != 0) {
            font_print(&lit_193_0065DBE8, Get_ServerName(), (int)&PlazaInfo + ((temp_s1_3 - 1) * 0x15C) + 0x14);
        }
        flfntLocate( (temp_s0_2 << 0x30) >> 0x30,  ((( (temp_s4_4 << 0x30) >> 0x30) + 0x16) << 0x30) >> 0x30);
        temp_s0_3 = F(u16, (u8 *)cw, 0x30B6);
        if (temp_s0_3 != 0) {
            sprintf(&sp100, &lit_193_0065DBE8, Get_ServerName(), (int)&LobbyInfo + ((temp_s0_3 - 1) * 0x15C) + 0x14);
            han2zen(&sp100, &spB0);
            font_print(&lit_2316, &spB0);
            return;
        }
        return;
    case 1:
        temp_s4_5 =  ((var_s4 + 0x16) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4_5);
        font_print(&lit_2316, F(int, &tl_mail_tbl, 0x28));
        lb_put_comment( ((temp_s5 + 0x5E) << 0x30) >> 0x30, temp_s4_5, spAC, 0);
        return;
    case 2:
        temp_s4_6 =  ((var_s4 + 0x18) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_6);
        if (F(u8, temp_s3, 9) == 6) {
            font_print(&lit_2316, (*(int *)((u8 *)&D_3351D4 + (F(u16, temp_s3, 0xA) * 0x18))));
        } else {
            font_print(&lit_2316, (*(int *)((u8 *)&D_3367BC + (F(u16, temp_s3, 0xA) * 0x14))));
        }
        temp_s1_4 = ( ((temp_s0 + 0x2C) << 0x30) >> 0x30) - 0x20;
        Lb_put_job( (temp_s1_4 << 0x30) >> 0x30,  ((( (temp_s4_6 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x1C, -1);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        temp_s4_7 =  ((temp_s4_6 + 0x1E) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_7);
        font_print(&lit_2316, (*(int *)((u8 *)&D_336C00 + (F(u8, temp_s3, 0x10) * 0x14))));
        Lb_put_icon( (temp_s1_4 << 0x30) >> 0x30,  ((( (temp_s4_7 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x13, -1);
        temp_s4_8 =  ((temp_s4_7 + 0x1E) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_8);
        font_print(&lit_2316, (*(int *)((u8 *)&D_3371E0 + (F(u8, temp_s3, 0x11) * 0x14))));
        Lb_put_icon( (temp_s1_4 << 0x30) >> 0x30,  ((( (temp_s4_8 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x14, -1);
        temp_s4_9 =  ((temp_s4_8 + 0x1E) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_9);
        font_print(&lit_2316, (*(int *)((u8 *)&D_337800 + (F(u8, temp_s3, 0x12) * 0x14))));
        Lb_put_icon( (temp_s1_4 << 0x30) >> 0x30,  ((( (temp_s4_9 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x16, -1);
        temp_s4_10 =  ((temp_s4_9 + 0x1E) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_10);
        font_print(&lit_2316, (*(int *)((u8 *)&D_337E10 + (F(u8, temp_s3, 0x13) * 0x14))));
        Lb_put_icon( (temp_s1_4 << 0x30) >> 0x30,  ((( (temp_s4_10 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x15, -1);
        temp_s4_11 =  ((temp_s4_10 + 0x1E) << 0x30) >> 0x30;
        flfntLocate( ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_11);
        font_print(&lit_2316, (*(int *)((u8 *)&D_338360 + (F(u8, temp_s3, 0xE) * 0x14))));
        Lb_put_icon( (temp_s1_4 << 0x30) >> 0x30,  ((( (temp_s4_11 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x17, -1);
        break;
    }
}
