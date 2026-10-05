/* em_cmd_r02 - monster command interpreter 0x0055C5C0-0x0055C914: em_cmd_main_jump, em_cmd_stand_ck, em_cmd_fly_ck, em_cmd_body_status_set, em_cmd_mode_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_main_jump(EMW *em, u8 *p) {
    em->cmd_idx = *p;
    return em_cmd_top(em);
}

u8 *em_cmd_stand_ck(EMW *em, u8 *p) {
    u8 *q;

    q = p;
    switch (*q++) {
    case 0:
        if (em->x388 != 0) {
            CMD_SKIP(em, q, 8);
        }
        break;
    case 1:
        q = else_ck(em, q, 8);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_fly_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a2;
    u8 temp_v1;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x388 != 2) {
            CMD_SKIP(em, var_a1, 9);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 9);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_body_status_set(EMW *em, u8 *p) {
    EM_FIELD(em, u8 *, 0x762) = (u8) *p;
    return p + 1;
}

u8 *em_cmd_mode_ck(EMW *em, u8 *p) {
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
        em->x838 = (u8) em->x888;
        if (em->x888 != v) {
            CMD_SKIP(em, var_a1, 0xB);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0xB);
        break;
    case 2:
        break;
    }
    return var_a1;
}
