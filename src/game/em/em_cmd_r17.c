/* em_cmd_r17 - monster command interpreter 0x00562AB0-0x00562BE4: em_cmd_st25_gate_ck, em_cmd_runaway_timer_set. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_st25_gate_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (*(u8 *)0x3F35D6 == 0) {
            CMD_SKIP(em, var_a1, 0x60);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x60);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_runaway_timer_set(EMW *em, u8 *p) {
    em->runaway_tm = em02_runaway_timer_tbl[em->stg];
    return p;
}
