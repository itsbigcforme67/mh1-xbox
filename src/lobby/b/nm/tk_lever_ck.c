#include "lobby_a.h"
extern char wait_157[4];
extern char D_38A82E[];
s32 tk_lever_ck(int arg0, u8 arg1, s32 arg2) {
    int var_s1;
    int var_s2;
    s32 temp_s0;
    int var_s3;
    u16 temp_v0;
    u8 temp_v0_2;
    u8 temp_v0_3;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 var_v0;
    u8 var_v0_2;

    temp_s0 = arg2 & 0xFF;
    if (((temp_s0 == 0) | (temp_s0 == 2)) != 0) {
        var_s2 = 0x2000;
        var_s1 = 0x1000;
        var_s3 = (int)&wait_157;
    } else {
        var_s2 = 0x800;
        var_s1 = 0x400;
        var_s3 = (int)&D_38A82E;
    }
    if (tk_sw_on_ck(var_s2) != 0) {
        (*(u16 *)var_s3) = 0x14;
        goto block_31;
    }
    if (tk_sw_on_ck(var_s1) != 0) {
        (*(u16 *)var_s3) = 0x14;
        goto block_20;
    }
    temp_v0 = (*(u16 *)var_s3);
    if (temp_v0 != 0) {
        (*(u16 *)var_s3) = (temp_v0 & 0xFFFF) - 1;
        return 0;
    }
    if (tk_sw_new_ck(var_s2) != 0) {
        (*(u16 *)var_s3) = 0xA;
block_31:
        if (temp_s0 < 2) {
            temp_v0_2 = (*(u8 *)arg0);
            if (temp_v0_2 == 0) {
                (*(u8 *)arg0) = arg1;
            } else {
                var_v0 = temp_v0_2 - 1;
                goto block_40;
            }
            goto block_41;
        }
        temp_v0_3 = (*(u8 *)arg0);
        if (temp_v0_3 == 0) {
            return 0;
        }
        var_v0 = temp_v0_3 - 1;
block_40:
        (*(u8 *)arg0) = var_v0;
block_41:
        cnWrap_SoundRequest(1);
        return 1;
    }
    if (tk_sw_new_ck(var_s1) != 0) {
        (*(u16 *)var_s3) = 0xA;
block_20:
        if (temp_s0 < 2) {
            temp_v1 = (*(u8 *)arg0);
            if (temp_v1 == (arg1 & 0xFF)) {
                (*(u8 *)arg0) = 0;
            } else {
                var_v0_2 = temp_v1 + 1;
                goto block_29;
            }
            goto block_30;
        }
        temp_v1_2 = (*(u8 *)arg0);
        if (temp_v1_2 == (arg1 & 0xFF)) {
            return 0;
        }
        var_v0_2 = temp_v1_2 + 1;
block_29:
        (*(u8 *)arg0) = var_v0_2;
block_30:
        cnWrap_SoundRequest(1);
        return 1;
    }
    return 0;
}
