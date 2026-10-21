/* G online (PC): lobby_func_00576F20, written from Monster Hunter G's code at lobby 0x576F20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon();
s32 lobby_func_00559C30();
s32 lobby_func_00559E20();
void * lobby_func_00559EB0();
s32 lobby_func_00559FF0();
extern u8 g_main_2EFBC0;
extern u8 g_main_2EFBC0;
extern u8 lb_num_str;
extern u8 lb_rule_msg_etc;
extern u8 shop_process2_help;
extern u8 tl_member_buff;
extern u8 tl_msg_tbl;
s32 KinshiYogo_chk();                       /* extern */
s32 Lb_back_money();                             /* extern */
s32 Put_comment(); /* extern */
s32 flfntLocate();                      /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();                 /* extern */
s32 font_set_palette();                      /* extern */
s32 func_0012BDD0();                      /* extern */
s32 han2zen();              /* extern */
s32 sprintf();    /* extern */
extern s32 lb_ibox_str;
extern s32 g_lb_63E2B8;
extern u16 g_lb_6B4510;
extern s8 g_lb_6B451C;
extern u8 g_lb_6B451D;
extern u8 g_lb_6B451E;
extern s32 g_main_3034B4;
extern u8 D_00528A80;
extern u8 LobbyInfo;
extern u8 lit_2316;

void lobby_func_00576F20(void) {
    u8 g_frame[0xB0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp70 (*(s32 *)(g_frame + 0x70))
#define sp90 (*(s32 *)(g_frame + 0x90))
    s16 temp_s0_2;
    s16 temp_s1;
    s32 temp_s0;
    s32 var_s1;
    s64 temp_s2;
    s64 temp_s3;
    s64 temp_s3_2;
    s64 temp_s3_3;
    s64 temp_s3_4;
    s64 temp_s3_5;
    s64 var_s0;
    u8 *temp_s4;
    u8 *temp_v0;

    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    temp_s0 = lobby_func_00559FF0((*(s32 *)&g_main_2EFBC0) + ((*(s16 *)((u8 *)&tl_member_buff + 0x1380)) * 0x10) + 4) & 0xFFFF;
    lobby_func_00559C30(temp_s0);
    temp_s4 = M2C_ERROR(/* Read from unset register $v0 */);
    flfntLocate(0xE2, 0x82);
    font_print((*(s32 * *)((u8 *)&tl_msg_tbl + 0x28)));
    flfntLocate(0x15A, 0x82);
    font_set_palette((*(s32 *)((u8 *)&g_main_3034B4 + (g_lb_6B4510 * 0x15C))));
    font_print((u8 *)&LobbyInfo + ((g_lb_6B4510 - 1) * 0x15C) + 0x1A);
    font_set_palette(0);
    temp_s3 = ((s16) (0x98));
    flfntLocate(0xE2, temp_s3);
    font_print(&lit_2316, (*(s32 *)((u8 *)&tl_msg_tbl + 0x2C)));
    flfntLocate(0x15A, temp_s3);
    font_set_palette(lobby_func_00559E20(temp_s0));
    func_0012BDD0(lobby_func_00559EB0(temp_s0));
    font_set_palette(0);
    if ((*(u8 *)((u8 *)((((*(s16 *)((u8 *)&tl_member_buff + 0x1380)) * 0x10) + (*(s32 *)&g_main_2EFBC0))) + 0x10)) != 0) {
        Lb_put_icon(0x142, (s32) temp_s3, 0x2D, -1);
    }
    temp_s3_2 = ((s16) ((temp_s3 + 0x16)));
    flfntLocate(0xE2, temp_s3_2);
    font_print(&lit_2316, (*(s32 *)((u8 *)&tl_msg_tbl + 0x30)));
    if (temp_s4 == NULL) {
        flfntLocate(0x15A, temp_s3_2);
        font_print(&lit_2316, lb_ibox_str);
    } else {
        temp_s2 = ((s16) ((Lb_back_money(temp_s0) & 0xFF)));
        var_s1 = 0;
        if (temp_s2 > 0) {
            var_s0 = 0x15A;
            do {
                flfntLocate(((s16) (var_s0)), temp_s3_2);
                font_print((*(s32 * *)((u8 *)&lb_num_str + 0x28)));
                var_s1 = (var_s1 + 1) & 0xFFFF;
                var_s0 += 0x12;
            } while (var_s1 < temp_s2);
        }
    }
    temp_s3_3 = ((s16) ((temp_s3_2 + 0x16)));
    flfntLocate(0xE2, temp_s3_3);
    temp_v0 = ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x10) + (*(s32 *)&g_main_2EFBC0);
    temp_s1 = (*(s16 *)((u8 *)(temp_v0) + 0xE));
    temp_s0_2 = (*(s16 *)((u8 *)(temp_v0) + 0xC));
    sprintf(&sp90, &g_lb_63E2B8, temp_s0_2, temp_s1);
    han2zen(&sp90, &sp70);
    font_print(&lit_2316, (*(s32 *)((u8 *)&tl_msg_tbl + 0x34)));
    if (temp_s0_2 == temp_s1) {
        font_set_palette(0xA);
    } else {
        font_set_palette(0);
    }
    flfntLocate(0x15A, temp_s3_3);
    font_print(&sp70);
    font_set_palette(0);
    temp_s3_4 = ((s16) ((temp_s3_3 + 0x16)));
    flfntLocate(0xE2, temp_s3_4);
    font_print(&lit_2316, (*(s32 *)((u8 *)&tl_msg_tbl + 0x38)));
    flfntLocate(0x15A, temp_s3_4);
    if (temp_s4 != NULL) {
        font_print(&lit_2316, (*(s32 *)((u8 *)&D_00528A80 + ((*(u8 *)((u8 *)(temp_s4) + 0x1C)) * 4))));
    } else {
        font_print(&lit_2316, (*(s32 *)((u8 *)&shop_process2_help + 0x1C)));
    }
    temp_s3_5 = ((s16) (((((s16) ((temp_s3_4 + 0x14)))) + 0x16)));
    flfntLocate(0xE2, temp_s3_5);
    font_print(&lit_2316, (*(s32 *)((u8 *)&tl_msg_tbl + 0x3C)));
    if (g_lb_6B451C != 0) {
        if (((g_lb_6B451C & 0xFF) == 0x81) && (g_lb_6B451D == 0x40) && (g_lb_6B451E == 0)) {
            goto block_18;
        }
        KinshiYogo_chk(&g_lb_6B451C);
        Put_comment(0x186, ((s16) (((((s16) (temp_s3_5))) - 0x16))), 0x14, &g_lb_6B451C, 0x15);
        return;
    }
block_18:
    flfntLocate(0x186, temp_s3_5);
    font_print((*(s32 * *)((u8 *)&lb_rule_msg_etc + 0x4)));
}

