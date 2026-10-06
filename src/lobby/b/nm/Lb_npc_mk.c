#include "lobby_a.h"

void Lb_npc_mk(int arg0) {
    int spA0;
    int sp60;
    f32 temp_f0;
    f32 temp_f20;
    f32 temp_f4;
    f32 var_f20;
    f32 var_f3;
    s32 var_s1;
    u16 temp_v1;
    int temp_s0;
    int temp_s2;

    temp_s0 = arg0 + 0x444;
    temp_s2 = F(int, arg0, 0x50C);
    flmatMakeScale(F(f32, arg0, 0xB8), F(f32, arg0, 0xC0), &sp60);
    cpRotMatrixYXZ2(arg0 + 0xA0, &spA0);
    flmatSetTrans(F(f32, arg0, 0xAC), F(f32, arg0, 0xB4), &spA0);
    flmatMul33_2(&spA0, &sp60);
    flmatCopy(arg0 + 0x60, &spA0);
    temp_v1 = F(u16, arg0, 0x2DC);
    if ((temp_v1 != 0x284) && (temp_v1 != 0x286) && (F(u8, temp_s0, 0xE) != 5)) {
        if (F(u8, arg0, 2) != 0) {
            var_s1 = get_joint_mat_em(arg0, 0xC);
            var_f20 = 10.0f;
        } else {
            var_s1 = get_joint_mat_em(arg0, 0x13);
            var_f20 = 20.0f;
        }
        var_f3 = (180.0f * flConvertStoR(Lb_get_angle(arg0, (int)&player_work + (game_w.master * 0xA00) + 0xAC) & 0xFFFF)) / 3.1415927f;
        if (!(var_f3 <= 180.0f)) {
            var_f3 = -(var_f3 - 180.0f);
        }
        if (var_f3 <= -90.0f) {
            if (var_f3 < 90.0f) {
                goto block_12;
            }
        } else {
block_12:
            temp_f0 = -var_f20;
            if (var_f3 < temp_f0) {
                var_f3 = temp_f0;
            } else if (!(var_f3 <= var_f20)) {
                var_f3 = var_f20;
            }
            temp_f4 = F(f32, temp_s0, 4);
            temp_f20 = temp_f4 + (0.2f * (var_f3 - temp_f4));
            flmatRotY33((3.1415927f * temp_f20) / 180.0f, var_s1);
            F(f32, temp_s0, 4) = temp_f20;
        }
    }
    flCalcTransSI(F(s32, temp_s2, 0x24), &spA0);
}
