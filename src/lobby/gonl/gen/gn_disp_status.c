/* G online (PC): disp_status, written from Monster Hunter G's code at lobby 0x5751B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_weapon_job();
s32 Lb_put_icon();
s32 Lb_put_job();
s32 Lb_put_msg_type2();
s32 Lb_put_status();
s32 Put_page_num();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 flfntLocate();
s32 flfntSetSize();
s32 han2zen();
s32 lb_put_comment();
s32 lobby_func_00575150();
void * memcpy();
s32 reload_tex();
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
s32 font_print(); /* extern */
s32 func_0012BDD0();                         /* extern */
s32 nwDispStr_Html(s32, s32, f32);          /* extern */
s32 put_titles();                  /* extern */
s32 sprintf();           /* extern */
extern u8 ClassInfo;
extern s32 *lb_ibox_str;
extern s32 g_lb_636A00;
extern u8 g_lb_63E128;
extern u8 g_lb_63E278;
extern u8 g_lb_63E280;
extern s32 g_sub_51B9B0;
extern u8 PlazaInfo;
extern s32 hunter_appellation;
extern u8 lit_2316;
extern s32 tl_job_tbl;

void disp_status(s32 arg0, s32 arg1, void *arg2, s32 *arg3, void *arg4, s32 arg5, s32 arg6, void *arg7, s16 arg_sp0) {
    u8 g_frame[0xF0] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(s32 *)(g_frame + 0xA0))
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_s1_2;
    s32 temp_s1_3;
    s32 temp_s2;
    s32 temp_s4;
    s32 temp_s4_10;
    s32 temp_s4_11;
    s32 temp_s4_12;
    s32 temp_s4_2;
    s32 temp_s4_3;
    s32 temp_s4_4;
    s32 temp_s4_5;
    s32 temp_s4_6;
    s32 temp_s4_7;
    s32 temp_s4_8;
    s32 temp_s4_9;
    s32 temp_s5;
    s32 var_s4;
    u8 temp_a2;

    var_s4 = ((s16) ((arg1 + 0x28)));
    temp_s0 = ((s16) (arg0));
    put_titles(((s16) ((temp_s0 + 0xA))), var_s4, (*(s32 *)((u8 *)&tl_mail_tbl + 0x28)));
    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    SetFilterMode(1);
    flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
    temp_s2 = ((s16) ((temp_s0 + 0xA)));
    Put_page_num((s32) (((s16) ((temp_s2 + 0x12C)))), (s32) var_s4, (s32) (((s8) (arg5))), (s32) (((s8) (arg6))), 0);
    temp_s5 = ((s8) (arg5));
    if (temp_s5 < 2) {
        flfntSetSize(0x12, 0x12);
        temp_s4 = ((s16) ((var_s4 + 0x16)));
        Lb_put_msg_type2((s32) (((s16) ((temp_s0 + 0xA)))), (s32) temp_s4, 0x13, M2C_ERROR(/* Read from unset register $a3 */));
        temp_s1 = (((s16) ((temp_s0 + 0xA)))) + 0x3C;
        flfntLocate((s32) (((s16) (temp_s1))), (s32) temp_s4);
        font_print(&lit_2316, arg3);
        temp_s4_2 = ((s16) ((temp_s4 + 0x16)));
        Lb_put_msg_type2((s32) (((s16) ((temp_s0 + 0xA)))), (s32) temp_s4_2, 0x14, M2C_ERROR(/* Read from unset register $a3 */));
        flfntLocate((s32) (((s16) (temp_s1))), (s32) temp_s4_2);
        han2zen(arg2, &spA0);
        font_print(&lit_2316, &spA0);
        temp_s4_3 = ((s16) ((temp_s4_2 + 0x16)));
        Lb_put_msg_type2((s32) (((s16) ((temp_s0 + 0xA)))), (s32) temp_s4_3, 0x17, M2C_ERROR(/* Read from unset register $a3 */));
        flfntLocate((s32) (((s16) (temp_s1))), (s32) temp_s4_3);
        font_print(&lit_2316, (*(s32 *)((u8 *)&tl_job_tbl + ((*(u8 *)((u8 *)(arg4) + 0)) * 4))));
        temp_s4_4 = ((s16) ((temp_s4_3 + 0x16)));
        Lb_put_msg_type2((s32) (((s16) ((temp_s0 + 0xA)))), (s32) temp_s4_4, 0x18, M2C_ERROR(/* Read from unset register $a3 */));
        flfntLocate((s32) (((s16) (temp_s1))), (s32) temp_s4_4);
        temp_a2 = (*(u8 *)((u8 *)(arg4) + 1));
        if (temp_a2 != 0) {
            sprintf(&spA0, (*(s32 *)((u8 *)&net_etc_mes_tbl + 0x1C)), temp_a2, (*(s32 *)((u8 *)&hunter_appellation + (temp_a2 * 4))));
        } else {
            sprintf(&spA0, (*(s32 *)((u8 *)&net_etc_mes_tbl + 0x20)), temp_a2);
        }
        font_print(&lit_2316, &spA0);
        var_s4 = ((s16) ((temp_s4_4 + 0x16)));
        Lb_put_msg_type2((s32) (((s16) ((temp_s0 + 0xA)))), (s32) var_s4, 0x1C, M2C_ERROR(/* Read from unset register $a3 */));
        if ((*(u8 *)((u8 *)(arg4) + 2)) == 0) {
            flfntLocate((s32) (((s16) ((temp_s2 + 0x25)))), (s32) var_s4);
        } else {
            flfntLocate((s32) (((s16) ((temp_s2 + 0x49)))), (s32) var_s4);
        }
        if (lobby_func_00575150(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) == 1) {
            memcpy((*(s32 *)((u8 *)&g_lb_636A00 + ((*(u8 *)((u8 *)(arg4) + 2)) * 4))), &g_lb_63E128, 4);
            func_0012BDD0((*(s32 *)((u8 *)&g_lb_636A00 + ((*(u8 *)((u8 *)(arg4) + 2)) * 4))));
            Lb_put_status((s32) (((s16) ((temp_s2 + 0x49)))), (s32) var_s4, 0x14, -1, (s32) (*(u8 *)((u8 *)(arg4) + 2)));
        } else {
            font_print(&lit_2316, lb_ibox_str);
        }
    }
    switch (temp_s5) {                              /* irregular */
    case 0:
        temp_s4_5 = ((s16) ((var_s4 + 0x16)));
        Lb_put_msg_type2((s32) (((s16) ((temp_s0 + 0xA)))), (s32) temp_s4_5, 0x19, M2C_ERROR(/* Read from unset register $a3 */));
        flfntLocate((s32) (((s16) ((temp_s2 + 0x50)))), (s32) temp_s4_5);
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)) == 7) {
            font_print(&g_lb_63E278, Get_ServerName(), &PlazaInfo + ((ClassInfo - 1) * 0x15C) + 0x1A, ClassInfo);
            return;
        }
        nwDispStr_Html((*(s32 *)&cw) + (arg_sp0 * 0x24A) + 0x1064, 0x43960000, (f32) temp_s4_5);
        return;
    case 1:
        temp_s4_6 = ((s16) ((var_s4 + 0x16)));
        Lb_put_msg_type2((s32) (((s16) ((temp_s0 + 0xA)))), (s32) temp_s4_6, 0x1A, M2C_ERROR(/* Read from unset register $a3 */));
        lb_put_comment((s32) (((s16) ((temp_s2 + 0x5E)))), (s32) temp_s4_6, arg7, 0);
        return;
    case 2:
        temp_s4_7 = ((s16) ((var_s4 + 0x18)));
        flfntLocate((s32) (((s16) ((temp_s0 + 0x2C)))), (s32) temp_s4_7);
        if ((*(u8 *)((u8 *)(arg4) + 0xB)) == 6) {
            font_print(&g_lb_63E280, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&Ken_data + 0x14)) + ((*(u16 *)((u8 *)(arg4) + 0xC)) * 0x18))));
        } else {
            font_print(&g_lb_63E280, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&Gun_data + 0x10)) + ((*(u16 *)((u8 *)(arg4) + 0xC)) * 0x18))));
        }
        temp_s1_2 = ((s16) ((temp_s0 + 0x2C)));
        Lb_put_job((s32) (((s16) ((temp_s1_2 - 0x25)))), (s32) (((s16) (((((s16) (temp_s4_7))) - 5)))), 0x1C, -1, Get_weapon_job(arg4 + 0xA) & 0xFF, 1);
        reload_tex(1, 0x13F);
        SetTextureStage(0x13F);
        temp_s4_8 = ((s16) ((temp_s4_7 + 0x1E)));
        flfntLocate((s32) (((s16) ((temp_s0 + 0x2C)))), (s32) temp_s4_8);
        font_print(&g_lb_63E280, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&Armor_Head_Data + 0x10)) + ((*(u8 *)((u8 *)(arg4) + 5)) << 5))));
        temp_s1_3 = temp_s1_2 - 0x22;
        Lb_put_icon((s32) (((s16) (temp_s1_3))), (s32) (((s16) (((((s16) (temp_s4_8))) - 5)))), 0x14, -1);
        temp_s4_9 = ((s16) ((temp_s4_8 + 0x1E)));
        flfntLocate((s32) (((s16) ((temp_s0 + 0x2C)))), (s32) temp_s4_9);
        font_print(&g_lb_63E280, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&Armor_Head_Data + 0x1D90)) + ((*(u8 *)((u8 *)(arg4) + 6)) << 5))));
        Lb_put_icon((s32) (((s16) (temp_s1_3))), (s32) (((s16) (((((s16) (temp_s4_9))) - 5)))), 0x15, -1);
        temp_s4_10 = ((s16) ((temp_s4_9 + 0x1E)));
        flfntLocate((s32) (((s16) ((temp_s0 + 0x2C)))), (s32) temp_s4_10);
        font_print(&g_lb_63E280, (*(s32 *)((u8 *)&g_sub_51B9B0 + ((*(u8 *)((u8 *)(arg4) + 7)) << 5))));
        Lb_put_icon((s32) (((s16) (temp_s1_3))), (s32) (((s16) (((((s16) (temp_s4_10))) - 5)))), 0x17, -1);
        temp_s4_11 = ((s16) ((temp_s4_10 + 0x1E)));
        flfntLocate((s32) (((s16) ((temp_s0 + 0x2C)))), (s32) temp_s4_11);
        font_print(&g_lb_63E280, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&Armor_Waist_Data + 0x10)) + ((*(u8 *)((u8 *)(arg4) + 8)) << 5))));
        Lb_put_icon((s32) (((s16) (temp_s1_3))), (s32) (((s16) (((((s16) (temp_s4_11))) - 5)))), 0x16, -1);
        temp_s4_12 = ((s16) ((temp_s4_11 + 0x1E)));
        flfntLocate((s32) (((s16) ((temp_s0 + 0x2C)))), (s32) temp_s4_12);
        font_print(&g_lb_63E280, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&Armor_Leg_Data + 0x10)) + ((*(u8 *)((u8 *)(arg4) + 3)) << 5))));
        Lb_put_icon((s32) (((s16) (temp_s1_3))), (s32) (((s16) (((((s16) (temp_s4_12))) - 5)))), 0x18, -1);
        return;
    }
}

