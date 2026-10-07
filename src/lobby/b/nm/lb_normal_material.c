#include "lobby_a.h"
typedef struct { u8 pad0000[0x458]; int x0458; } ARG_lb_normal_material_arg0;

void lb_normal_material(ARG_lb_normal_material_arg0 *arg0, s32 arg1, s32 arg2, int arg3) {
    s16 temp_a0;
    int var_s1;

    var_s1 = arg0->x0458;
    if (var_s1 != 0) {
loop_2:
        temp_a0 = F(s16, var_s1, 0);
        if (temp_a0 != -1) {
            if ((temp_a0 == ( (arg3 << 0x30) >> 0x30)) && (F(s16, var_s1, 2) == arg2)) {
                SetDiffuseColor(arg1 + 4, F(s32, var_s1, 4));
            }
            var_s1 += 8;
            if (var_s1 == 0) {

            } else {
                goto loop_2;
            }
        }
    }
}
