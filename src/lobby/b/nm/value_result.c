#include "lobby_a.h"

s32 value_result(s32 arg0, s32 arg1) {
    s32 temp_a0;
    s32 temp_v1;
    s32 var_a0;
    s32 var_v0;
    s32 var_v0_2;
    u32 temp_v0;

    var_a0 = arg0;
    temp_v0 = arg1 & 0xFFFF;
    switch (temp_v0) {
    case 0:
        temp_a0 = var_a0 & 0xFFFF;
        temp_v1 = temp_a0 & 0xF;
        var_v0 = 4;
        if (temp_v1 >= 4) {

        } else {
            var_v0 = (temp_v1 + 1) & 0xFFFF;
        }
        var_v0_2 = (temp_a0 & 0x70) | (var_v0 & 0xFFFF);
block_19:
        var_a0 = var_v0_2 & 0xFFFF;
        break;
    case 1:
        var_v0_2 = (var_a0 | 0x10) & 0xFFFF & 0xFFDF;
        goto block_19;
    case 2:
        var_v0_2 = var_a0 & 0xFFEF;
        goto block_19;
    case 3:
        var_v0_2 = (var_a0 | 0x20) & 0xFFFF & 0xFFEF;
        goto block_19;
    case 4:
        var_v0_2 = var_a0 & 0xFFDF;
        goto block_19;
    case 5:
        var_v0_2 = var_a0 | 0x40;
        goto block_19;
    case 6:
        var_v0_2 = var_a0 & 0xFFBF;
        goto block_19;
    }
    return var_a0;
}
