/* em_cmd_r14 - monster command interpreter 0x005619E0-0x00561D8C: em_cmd_target_pl_samestage_ck, em_cmd_target_pl_hate_high_ck, em_cmd_quest_no_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_target_pl_samestage_ck(EMW *em, u8 *p) {
    u8 *q;
    s8 t;
    PLW *pl;

    q = p;
    switch (*q++) {
    case 0:
        t = em->x844;
        if (t == -1 || (pl = &player_work[t], Pl_stg_ck_tw(em, pl) == 0) || pl->be_flag == 0) {
            CMD_SKIP(em, q, 0x54);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x54);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_target_pl_hate_high_ck(EMW *em, u8 *p) {
    u8 *q;
    s8 t;

    q = p;
    switch (*q++) {
    case 0:
        t = em->x844;
        if (t == -1 || em->x918[t] < 0x7530) {
            CMD_SKIP(em, q, 0x55);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x55);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_quest_no_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    s16 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        if (*(s16 *)0x3C7448 != v) {
            CMD_SKIP(em, var_a1, 0x56);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x56);
        break;
    case 2:
        break;
    }
    return var_a1;
}
