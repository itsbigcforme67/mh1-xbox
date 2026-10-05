#include "lobby_a.h"
extern char gh_boardStr[];
extern char gh_boardStr2[];
extern char lit_519_0065E690[];
extern char lit_519_0065E690[];
extern char lit_520_0065E698[];
typedef struct { u8 pad0000[0x18]; s32 x0018; } ARG_Lb_gh_board_trans_arg0;
void Lb_gh_board_trans(ARG_Lb_gh_board_trans_arg0 *arg0) {
    int var_s0;
    int var_s1;
    int var_s2;
    int var_s3;

    font_set_stack_no(arg0->x0018);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    Draw_menu_square(0x12C, 0x20, 0x140, 0xB4);
    var_s3 = 0x34;
    var_s2 = 0;
    var_s1 = (int)&gh_boardStr;
    var_s0 = (int)&gh_boardStr2;
    do {
        flfntSetSize(0x12, 0x12);
        flfntLocate(0x140, var_s3);
        font_print(&lit_519_0065E690, (*(s32 *)var_s1));
        if (( (var_s2 << 0x30) >> 0x30) != 0) {
            flfntSetSize(0x18, 0x12);
            flfntLocate(0x1AC, var_s3);
            font_print(&lit_519_0065E690, (*(s32 *)var_s0));
            flfntSetSize(0x12, 0x12);
            flfntLocate(0x228, var_s3);
            font_print(&lit_520_0065E698);
        }
        var_s1 += 4;
        var_s3 =  ((var_s3 + 0x18) << 0x30) >> 0x30;
        var_s2 =  ((var_s2 + 1) << 0x30) >> 0x30;
        var_s0 += 4;
    } while (var_s2 < 6);
}
