/* G online (PC): disp_status, written from Monster Hunter G's code at lobby 0x5751B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 Armor_Head_Data;
extern u8 Armor_Leg_Data;
extern u8 Armor_Waist_Data;
extern u8 Gun_data;
extern u8 Ken_data;
extern u8 cw;
extern u8 net_etc_mes_tbl;
extern u8 pNet;
extern u8 tl_mail_tbl;
s32 *Get_ServerName();                          /* extern */
s32 Get_weapon_job();                     /* extern */
s32 Lb_put_icon();    /* extern */
s32 Lb_put_job();     /* extern */
s32 Lb_put_msg_type2();        /* extern */
s32 Lb_put_status();  /* extern */
s32 Put_page_num();           /* extern */
s32 SetFilterMode();                     /* extern */
s32 SetTextureStage();                   /* extern */
s32 flSetRenderState();         /* extern */
s32 flfntLocate();                      /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print(); /* extern */
s32 func_0012BDD0();                         /* extern */
s32 han2zen();                /* extern */
s32 lb_put_comment(); /* extern */
s32 lobby_func_00575150();                          /* extern */
s32 memcpy();            /* extern */
s32 nwDispStr_Html();               /* extern */
s32 put_titles();                  /* extern */
s32 reload_tex();               /* extern */
s32 sprintf();           /* extern */
extern u8 ClassInfo;
extern s32 *lb_ibox_str;
extern u8 g_lb_636A00;
extern u8 g_lb_63E128;
extern u8 g_lb_63E278;
extern u8 g_lb_63E280;
extern u8 g_sub_51B9B0;
extern u8 PlazaInfo;
extern u8 hunter_appellation;
extern u8 lit_2316;
extern u8 tl_job_tbl;

void disp_status(s64 arg0, s32 arg1, s32 arg2, s32 *arg3) {
    union { s32 v; u8 b[0x50]; } u_spA0;
#define spA0 (u_spA0.v)
    s64 temp_s0;
    s64 temp_s1;
    s64 temp_s1_2;
    s64 temp_s1_3;
    s64 temp_s1_4;
    s64 temp_s2;
    s64 temp_s4;
    s64 temp_s4_10;
    s64 temp_s4_11;
    s64 temp_s4_12;
    s64 temp_s4_2;
    s64 temp_s4_3;
    s64 temp_s4_4;
    s64 temp_s4_5;
    s64 temp_s4_6;
    s64 temp_s4_7;
    s64 temp_s4_8;
    s64 temp_s4_9;
    s64 temp_s5;
    s64 var_s4;
    u8 temp_a2;
    u8 *temp_s3;

    var_s4 = (s64) ((arg1 + 0x28) << 0x30) >> 0x30;
    temp_s0 = (s64) (arg0 << 0x30) >> 0x30;
    temp_s3 = M2C_ERROR(/* Read from unset register $t0 */);
    temp_s1 = M2C_ERROR(/* Read from unset register $t1 */);
    put_titles((s64) ((temp_s0 + 0xA) << 0x30) >> 0x30, var_s4, (*(u8 *)((u8 *)&tl_mail_tbl + 0x28)));
    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    temp_s2 = (s64) ((temp_s0 + 0xA) << 0x30) >> 0x30;
    Put_page_num((s64) ((temp_s2 + 0x12C) << 0x30) >> 0x30, var_s4, (s64) (temp_s1 << 0x38) >> 0x38, (s64) (M2C_ERROR(/* Read from unset register $t2 */) << 0x38) >> 0x38);
    temp_s5 = (s64) (temp_s1 << 0x38) >> 0x38;
    if (temp_s5 < 2) {
        flfntSetSize(0x12, 0x12);
        temp_s4 = (s64) ((var_s4 + 0x16) << 0x30) >> 0x30;
        Lb_put_msg_type2((s64) ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4, 0x13);
        temp_s1_2 = ((s64) ((temp_s0 + 0xA) << 0x30) >> 0x30) + 0x3C;
        flfntLocate((s64) (temp_s1_2 << 0x30) >> 0x30, temp_s4);
        font_print(&lit_2316, arg3);
        temp_s4_2 = (s64) ((temp_s4 + 0x16) << 0x30) >> 0x30;
        Lb_put_msg_type2((s64) ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4_2, 0x14);
        flfntLocate((s64) (temp_s1_2 << 0x30) >> 0x30, temp_s4_2);
        han2zen(arg2, &spA0);
        font_print(&lit_2316, &spA0);
        temp_s4_3 = (s64) ((temp_s4_2 + 0x16) << 0x30) >> 0x30;
        Lb_put_msg_type2((s64) ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4_3, 0x17);
        flfntLocate((s64) (temp_s1_2 << 0x30) >> 0x30, temp_s4_3);
        font_print(&lit_2316, *(&tl_job_tbl + ((*(u8 *)((u8 *)(temp_s3) + 0)) * 4)));
        temp_s4_4 = (s64) ((temp_s4_3 + 0x16) << 0x30) >> 0x30;
        Lb_put_msg_type2((s64) ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4_4, 0x18);
        flfntLocate((s64) (temp_s1_2 << 0x30) >> 0x30, temp_s4_4);
        temp_a2 = (*(u8 *)((u8 *)(temp_s3) + 1));
        if (temp_a2 != 0) {
            sprintf(&spA0, (*(u8 *)((u8 *)&net_etc_mes_tbl + 0x1C)), temp_a2, *(&hunter_appellation + (temp_a2 * 4)));
        } else {
            sprintf(&spA0, (*(u8 *)((u8 *)&net_etc_mes_tbl + 0x20)), temp_a2);
        }
        font_print(&lit_2316, &spA0);
        var_s4 = (s64) ((temp_s4_4 + 0x16) << 0x30) >> 0x30;
        Lb_put_msg_type2((s64) ((temp_s0 + 0xA) << 0x30) >> 0x30, var_s4, 0x1C);
        if ((*(u8 *)((u8 *)(temp_s3) + 2)) == 0) {
            flfntLocate((s64) ((temp_s2 + 0x25) << 0x30) >> 0x30, var_s4);
        } else {
            flfntLocate((s64) ((temp_s2 + 0x49) << 0x30) >> 0x30, var_s4);
        }
        if (lobby_func_00575150() == 1) {
            memcpy(*(&g_lb_636A00 + ((*(u8 *)((u8 *)(temp_s3) + 2)) * 4)), &g_lb_63E128, 4);
            func_0012BDD0(*(&g_lb_636A00 + ((*(u8 *)((u8 *)(temp_s3) + 2)) * 4)));
            Lb_put_status((s64) ((temp_s2 + 0x49) << 0x30) >> 0x30, var_s4, 0x14, -1);
        } else {
            font_print(&lit_2316, lb_ibox_str);
        }
    }
    switch (temp_s5) {                              /* irregular */
    case 0:
        temp_s4_5 = (s64) ((var_s4 + 0x16) << 0x30) >> 0x30;
        Lb_put_msg_type2((s64) ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4_5, 0x19);
        flfntLocate((s64) ((temp_s2 + 0x50) << 0x30) >> 0x30, temp_s4_5);
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)) == 7) {
            font_print(&g_lb_63E278, Get_ServerName(), &PlazaInfo + ((ClassInfo - 1) * 0x15C) + 0x1A, ClassInfo);
            return;
        }
        nwDispStr_Html(0x43960000, (*(s32 *)&cw) + ((s16) arg0 * 0x24A) + 0x1064);
        return;
    case 1:
        temp_s4_6 = (s64) ((var_s4 + 0x16) << 0x30) >> 0x30;
        Lb_put_msg_type2((s64) ((temp_s0 + 0xA) << 0x30) >> 0x30, temp_s4_6, 0x1A);
        lb_put_comment((s64) ((temp_s2 + 0x5E) << 0x30) >> 0x30, temp_s4_6, M2C_ERROR(/* Read from unset register $t3 */), 0);
        return;
    case 2:
        temp_s4_7 = (s64) ((var_s4 + 0x18) << 0x30) >> 0x30;
        flfntLocate((s64) ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_7);
        if ((*(u8 *)((u8 *)(temp_s3) + 0xB)) == 6) {
            font_print(&g_lb_63E280, *(&(*(u8 *)((u8 *)&Ken_data + 0x14)) + ((*(u16 *)((u8 *)(temp_s3) + 0xC)) * 0x18)));
        } else {
            font_print(&g_lb_63E280, *(&(*(u8 *)((u8 *)&Gun_data + 0x10)) + ((*(u16 *)((u8 *)(temp_s3) + 0xC)) * 0x18)));
        }
        temp_s1_3 = (s64) ((temp_s0 + 0x2C) << 0x30) >> 0x30;
        Get_weapon_job(temp_s3 + 0xA);
        Lb_put_job((s64) ((temp_s1_3 - 0x25) << 0x30) >> 0x30, (s64) ((((s64) (temp_s4_7 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x1C, -1);
        reload_tex(1, 0x13F);
        SetTextureStage(0x13F);
        temp_s4_8 = (s64) ((temp_s4_7 + 0x1E) << 0x30) >> 0x30;
        flfntLocate((s64) ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_8);
        font_print(&g_lb_63E280, *(&(*(u8 *)((u8 *)&Armor_Head_Data + 0x10)) + ((*(u8 *)((u8 *)(temp_s3) + 5)) << 5)));
        temp_s1_4 = temp_s1_3 - 0x22;
        Lb_put_icon((s64) (temp_s1_4 << 0x30) >> 0x30, (s64) ((((s64) (temp_s4_8 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x14, -1);
        temp_s4_9 = (s64) ((temp_s4_8 + 0x1E) << 0x30) >> 0x30;
        flfntLocate((s64) ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_9);
        font_print(&g_lb_63E280, *(&(*(u8 *)((u8 *)&Armor_Head_Data + 0x1D90)) + ((*(u8 *)((u8 *)(temp_s3) + 6)) << 5)));
        Lb_put_icon((s64) (temp_s1_4 << 0x30) >> 0x30, (s64) ((((s64) (temp_s4_9 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x15, -1);
        temp_s4_10 = (s64) ((temp_s4_9 + 0x1E) << 0x30) >> 0x30;
        flfntLocate((s64) ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_10);
        font_print(&g_lb_63E280, *(&g_sub_51B9B0 + ((*(u8 *)((u8 *)(temp_s3) + 7)) << 5)));
        Lb_put_icon((s64) (temp_s1_4 << 0x30) >> 0x30, (s64) ((((s64) (temp_s4_10 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x17, -1);
        temp_s4_11 = (s64) ((temp_s4_10 + 0x1E) << 0x30) >> 0x30;
        flfntLocate((s64) ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_11);
        font_print(&g_lb_63E280, *(&(*(u8 *)((u8 *)&Armor_Waist_Data + 0x10)) + ((*(u8 *)((u8 *)(temp_s3) + 8)) << 5)));
        Lb_put_icon((s64) (temp_s1_4 << 0x30) >> 0x30, (s64) ((((s64) (temp_s4_11 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x16, -1);
        temp_s4_12 = (s64) ((temp_s4_11 + 0x1E) << 0x30) >> 0x30;
        flfntLocate((s64) ((temp_s0 + 0x2C) << 0x30) >> 0x30, temp_s4_12);
        font_print(&g_lb_63E280, *(&(*(u8 *)((u8 *)&Armor_Leg_Data + 0x10)) + ((*(u8 *)((u8 *)(temp_s3) + 3)) << 5)));
        Lb_put_icon((s64) (temp_s1_4 << 0x30) >> 0x30, (s64) ((((s64) (temp_s4_12 << 0x30) >> 0x30) - 5) << 0x30) >> 0x30, 0x18, -1);
        return;
    }
}

