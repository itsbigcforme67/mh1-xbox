/* em_cmd_r05 - monster command interpreter 0x0055DB20-0x0055DD3C: em_cmd_mind_ck, em_cmd_mind_no_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_mind_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    u8 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        if (v != em->x889) {
            CMD_SKIP(em, var_a1, 0x1B);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x1B);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_mind_no_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    u8 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        if (v != em->x88A) {
            CMD_SKIP(em, var_a1, 0x1C);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x1C);
        break;
    case 2:
        break;
    }
    return var_a1;
}
