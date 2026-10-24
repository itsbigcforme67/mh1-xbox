/* G online (PC): lb_select_trans, written from Monster Hunter G's code at lobby 0x58B3B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). HAND-FIXED: m2c's saved_reg_s0 / s2 (below). */
#include "../gonl.h"

s32 Paint_square();
s32 Put_2TF();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 flfntSetSize();
s32 font_print_double();
s32 font_set_palette();
s32 lb_put_room_member_005B1930();
s32 lobby_func_00559C30();
s64 lobby_func_005935C0();
s32 reload_tex();
extern u8 System_timer;
extern u8 joinQuest;
extern u8 lb_board_exp;
extern u8 lb_board_message;
extern u8 lb_quest_color_tex;
extern u8 pNet;
extern u8 tl_msg_tbl;
#ifdef __MWERKS__
s32 Lb_get_quest_type();                         /* extern */
#else
signed char Lb_get_quest_type();                         /* extern */   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_put_button();   /* extern */
s32 Lb_put_gold();                       /* extern */
s32 Lb_put_msg();                      /* extern */
s32 lobby_func_0055CA90();             /* extern */
extern s32 g_lb_635740;
extern void *lb_quest_level_str;
extern u16 lb_quest_font_color;

void lb_select_trans(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    u8 *var_s2;   /* (s32 *) */
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_s1_2;
    s32 var_s0;
    s32 var_s1;
    u8 temp_a0;

    var_s0 = 0;                     /* HAND-FIXED: G leaves the callee-saved s0 / s2 as they were; only cases that set them read them */
    var_s2 = NULL;
    temp_s1 = lobby_func_00559C30((s32) (*(u16 *)&joinQuest), arg1, arg2, arg3);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
    font_set_palette(0);
    Lb_put_gold(0);
    flfntSetSize(0x12, 0x12);
    Put_2TF(&(*(s32 *)((u8 *)&lb_quest_color_tex + 0x5C)));
    Paint_square(0x14A, 0x1B, 0x50, 0x18, 0x80000000);
    Lb_put_msg(&(*(s32 *)((u8 *)&lb_board_exp + 0xC8)));
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2F)) == 1) {
        Lb_put_msg(&(*(s32 *)((u8 *)&lb_board_exp + 0xC8)));
        font_print_double(0x1A7, 0xBE, 1, 0xA, lb_quest_level_str);
        return;
    }
    Put_2TF(&g_lb_635740);
    Paint_square(0x156, 0x17A, 0xF6, 0x18, 0x80000000);
    if ((*(u16 *)&System_timer) & 0x10) {
        Lb_put_button(0x1E6, 0x17A, 3);
    }
    temp_s1_2 = Lb_get_quest_type(temp_s1);
    font_print_double((s32) ((s32) (lobby_func_005935C0(&(*(s32 *)((u8 *)&lb_board_exp + 0x4)), 0x1D4, 0x12, M2C_ERROR(/* Read from unset register $a3 */)) << 0x30) >> 0x30), (s32) (*(s16 *)((u8 *)&lb_board_exp + 0x2)), 1, (s32) (*(u16 *)((u8 *)&lb_quest_font_color + (temp_s1_2 * 2))), &(*(s32 *)((u8 *)&lb_board_exp + 0x4)));
    font_print_double((s32) ((s32) (lobby_func_005935C0(&(*(s32 *)((u8 *)&lb_board_exp + 0x68)), 0x1D4, 0x12, M2C_ERROR(/* Read from unset register $a3 */)) << 0x30) >> 0x30), (s32) (*(s16 *)((u8 *)&lb_board_exp + 0x66)), 1, 0, &(*(s32 *)((u8 *)&lb_board_exp + 0x68)));
    temp_a0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29));
    switch (temp_a0) {                              /* irregular */
    case 0:
        font_print_double(0x157, 0x8C, 1, 6, (*(void * *)((u8 *)&tl_msg_tbl + 0x34)));
        font_print_double(0x157, 0xB4, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x1C)));
        font_print_double(0x157, 0xE6, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x20)));
        font_print_double(0x161, 0x17E, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x5C)));
        font_print_double(0x204, 0x17E, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x28)));
        var_s2 = &(*(s32 *)((u8 *)&lb_board_exp + 0x12C));
        var_s0 = 4;
        lobby_func_0055CA90(&(*(s32 *)((u8 *)&lb_board_exp + 0x2BC)));
        break;
    case 1:
        font_print_double(0x157, 0x8C, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x2C)));
        font_print_double(0x161, 0x17E, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x5C)));
        font_print_double(0x204, 0x17E, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x34)));
        lb_put_room_member_005B1930(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        var_s0 = 0;
        break;
    case 2:
        font_print_double(0x157, 0x8C, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x38)));
        font_print_double(0x157, 0xA0, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x3C)));
        font_print_double(0x157, 0xBE, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x40)));
        font_print_double(0x157, 0xD2, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x44)));
        font_print_double(0x157, 0xF0, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x48)));
        font_print_double(0x161, 0x17E, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x5C)));
        font_print_double(0x204, 0x17E, 1, 6, (*(void * *)((u8 *)&lb_quest_level_str + 0x50)));
        var_s0 = 5;
        var_s2 = &(*(u16 *)((u8 *)&lb_board_message + 0x20));
        break;
    }
    temp_s0 = var_s0 & 0xFFFF;
    var_s1 = 0;
    if (temp_s0 > 0) {
        do {
            font_print_double((s32) (*(s16 *)((u8 *)(var_s2) + 0)), (s32) (*(s16 *)((u8 *)(var_s2) + 2)), 1, 0, var_s2 + 4);
            var_s1 = (var_s1 + 1) & 0xFFFF;
            var_s2 += 0x64;
        } while (var_s1 < temp_s0);
    }
}

