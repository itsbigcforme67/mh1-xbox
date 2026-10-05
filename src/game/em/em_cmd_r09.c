/* em_cmd_r09 - monster command interpreter 0x0055E920-0x0055ECDC: em_cmd_all_pl_same_stage_ck, em_cmd_stay_timer_ck, em_cmd_runaway_timer_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_all_pl_same_stage_ck(EMW *em, u8 *p) {
    u8 *q;
    PLW *pl;
    s32 i;
    u8 ok;

    q = p;
    pl = player_work;
    switch (*q++) {
    case 0:
        ok = 1;
        i = 0;
        for (; i < *(u8 *)0x3F34C3; i++, pl++) {
            if (Pl_stg_ck_tw(em, pl) != 0 && pl->be_flag != 0) {
                ok = 0;
                break;
            }
        }
        CMD_SKIPF(em, q, 0x28, ok);
        break;
    case 1:
        q = else_ck(em, q, 0x28);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_stay_timer_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->stay_tm > 0) {
            CMD_SKIP(em, var_a1, 0x29);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x29);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_runaway_timer_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->runaway_tm > 0) {
            CMD_SKIP(em, var_a1, 0x2A);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x2A);
        break;
    case 2:
        break;
    }
    return var_a1;
}
