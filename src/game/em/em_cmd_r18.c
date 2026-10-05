/* em_cmd_r18 - monster command interpreter 0x00562E00-0x00563078: em_cmd_male_ck, em_cmd_target_pl_hate_ck, em_cmd_all_pl_hate_clear. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_male_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x11 != 0) {
            CMD_SKIP(em, var_a1, 0x63);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x63);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_target_pl_hate_ck(EMW *em, u8 *p) {
    u8 *q;
    u8 v;
    s8 t;

    q = p;
    switch (*q++) {
    case 0:
        t = em->x844;
        v = *q;
        q += 1;
        if (!(t != -1 && !(em->x918[t] < check_hate_tbl[v]))) {
            CMD_SKIP(em, q, 0x64);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x64);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_all_pl_hate_clear(EMW *em, u8 *p) {
    EM_FIELD(em, s32 *, 0x918) = 0;
    EM_FIELD(em, s32 *, 0x91C) = 0;
    EM_FIELD(em, s32 *, 0x920) = 0;
    EM_FIELD(em, s32 *, 0x924) = 0;
    return p;
}
