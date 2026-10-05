/* em_cmd_r16 - monster command interpreter 0x005622A0-0x0056263C: em_cmd_tenjostage_ck, em_cmd_smell_set_ck, em_cmd_my_floor_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_tenjostage_ck(EMW *em, u8 *p) {
    f32 sp3C;
    f32 sp38;
    u8 *q;

    q = p;
    switch (*q++) {
    case 0:
        if (em->stg != *(u8 *)0x3F3404 || GetTenjoHit(em->pos, &sp3C, (u16 *)&sp38) == 0) {
            CMD_SKIP(em, q, 0x5C);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x5C);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_smell_set_ck(EMW *em, u8 *p) {
    u8 *q;

    q = p;
    switch (*q++) {
    case 0:
        cmd_target_kind_set(em, em->tgt_pos);
        if (em->tgt_pos[0] == 0.0f || em->tgt_pos[1] == 0.0f || em->tgt_pos[2] == 0.0f) {
            CMD_SKIP(em, q, 0x5D);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x5D);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_my_floor_ck(EMW *em, u8 *p) {
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
        if (em->x70E != v) {
            CMD_SKIP(em, var_a1, 0x5E);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x5E);
        break;
    case 2:
        break;
    }
    return var_a1;
}
