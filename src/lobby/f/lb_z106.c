/* lb_z106 - auto-drafted 0x005E8BA0-0x005E8CE8: bs_url_slash_slash, bs_url_dot_slash, bs_url_dot_dot_slash (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

int bs_url_slash_slash(int arg0, u32 arg1) {
    int var_a0;

    var_a0 = arg0;
    if ((u32) (var_a0 + 1) < arg1) {
loop_1:
        if ((F(s8, var_a0, 0) == 0x2F) && (F(s8, var_a0, 1) == 0x2F)) {
            return var_a0;
        }
        var_a0 += 1;
        if ((u32) (var_a0 + 1) >= arg1) {
            goto block_6;
        }
        goto loop_1;
    }
block_6:
    return 0;
}

int bs_url_dot_slash(int arg0, u32 arg1) {
    int var_a0;

    var_a0 = arg0;
    if ((u32) (var_a0 + 2) < arg1) {
loop_2:
        if ((F(s8, var_a0, 0) == 0x2F) && (F(s8, var_a0, 1) == 0x2E) && (F(s8, var_a0, 2) == 0x2F)) {
            return var_a0;
        }
        var_a0 += 1;
        if ((u32) (var_a0 + 2) >= arg1) {
            goto block_9;
        }
        goto loop_2;
    }
block_9:
    return 0;
}

int bs_url_dot_dot_slash(int arg0, u32 arg1) {
    int var_a0;

    var_a0 = arg0;
    if ((u32) (var_a0 + 3) < arg1) {
loop_2:
        if ((F(s8, var_a0, 0) == 0x2F) && (F(s8, var_a0, 1) == 0x2E) && (F(s8, var_a0, 2) == 0x2E) && (F(s8, var_a0, 3) == 0x2F)) {
            return var_a0;
        }
        var_a0 += 1;
        if ((u32) (var_a0 + 3) >= arg1) {
            goto block_9;
        }
        goto loop_2;
    }
block_9:
    return 0;
}
