/* lb_z11 - auto-drafted 0x005E0ED0-0x005E0FE8: chopLine, stockDrawStartPoint (first drafted by tools/lbauto.py). */
#include "lobby.h"

s32 chopLine(f32 *arg0, f32 *arg1, f32 *arg2, f32 *arg3) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;

    temp_f1 = *arg0;
    if (temp_f1 < 0.0f) {
        *arg0 = 0.0f;
        goto block_4;
    }
    if (!(temp_f1 <= 640.0f)) {
        return 0;
    }
block_4:
    temp_f1_2 = *arg2;
    if (temp_f1_2 < 0.0f) {
        return 0;
    }
    if (!(temp_f1_2 <= 640.0f)) {
        *arg2 = 640.0f;
    }
    temp_f1_3 = *arg1;
    if (temp_f1_3 < 0.0f) {
        *arg1 = 0.0f;
        goto block_12;
    }
    if (!(temp_f1_3 <= 448.0f)) {
        return 0;
    }
block_12:
    temp_f1_4 = *arg3;
    if (temp_f1_4 < 0.0f) {
        return 0;
    }
    if (!(temp_f1_4 <= 448.0f)) {
        *arg3 = 448.0f;
    }
    return 1;
}

void stockDrawStartPoint(void) {
    BsInitAllObj();
}
