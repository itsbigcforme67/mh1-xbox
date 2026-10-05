#include "lobby_a.h"
extern char bh_pos[];
extern char bh_pos[];
extern char lit_1085_0065DA48[];
extern char bh_pos[];
extern char bh_pos[];
extern char strButtonHelp[];
void put_button_help(int arg0, int arg1, s32 arg2, s32 arg3) {
    int sp70;
    s16 temp_s1;
    s16 var_s0;
    s16 var_s1;
    s16 var_s2;
    s32 temp_s0;
    int var_s3;
    int temp_v1;
    int temp_v1_2;

    var_s3 = 0;
    if (arg3 & 0xFFFF) {
        var_s3 = 2;
    }
    temp_s0 = ((s8)arg0) * 8;
    var_s2 = (*(int *)((u8 *)&bh_pos + temp_s0));
    var_s1 = (*(int *)((u8 *)&bh_pos + 2 + temp_s0));
    temp_v1 = (int)cw;
    if ((F(u8, temp_v1, 0x35D5) != 0) && (F(s8, (game_w.master + temp_v1), 0x2BFE) != 0)) {
        var_s1 = (s16) ( ((var_s1 - 0x18) << 0x30) >> 0x30);
    }
    if (arg2 == 9) {
        var_s2 = (s16) ( ((var_s2 - 0x1E) << 0x30) >> 0x30);
        var_s1 = (s16) ( ((var_s1 - 2) << 0x30) >> 0x30);
        flfntLocate( ((var_s2 + 3) << 0x30) >> 0x30,  ((var_s1 + 6) << 0x30) >> 0x30);
        font_print(&lit_1085_0065DA48);
    }
    Lb_put_button( var_s2,  ((((s16)var_s1) + ( (var_s3 << 0x30) >> 0x30)) << 0x30) >> 0x30, arg2);
    temp_s1 = (*(int *)((u8 *)&bh_pos + 4 + temp_s0));
    var_s0 = (*(int *)((u8 *)&bh_pos + 6 + temp_s0));
    temp_v1_2 = (int)cw;
    if ((F(u8, temp_v1_2, 0x35D5) != 0) && (F(s8, (game_w.master + temp_v1_2), 0x2BFE) != 0)) {
        var_s0 = (s16) ( ((var_s0 - 0x18) << 0x30) >> 0x30);
    }
    strcpy(&sp70, ((int *)&strButtonHelp)[((s8)arg1)]);
    font_print_double(temp_s1,  var_s0, 1, 0);
}
