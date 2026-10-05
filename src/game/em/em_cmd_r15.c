/* em_cmd_r15 - monster command interpreter 0x00561F80-0x0056221C: em_cmd_boss_pl_target_set, em_cmd_tenjo_ck, em_cmd_target_land_no_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_boss_pl_target_set(EMW *em, u8 *p) {
    EMW *b = em->boss;
    em->x827 = 1;
    em->x828 = 0;
    if (b == NULL) {
        em->x829 = 0xFF;
    } else {
        em->x829 = b->x617;
    }
    em->x844 = em->x829;
    return p;
}

u8 *em_cmd_tenjo_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (EM_FIELD(em, s8 *, 0x9EF) == 0) {
            CMD_SKIP(em, var_a1, 0x59);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x59);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_target_land_no_ck(EMW *em, u8 *p) {
    u8 *q;
    u8 v;
    s8 t;

    q = p;
    switch (*q++) {
    case 0:
        v = *q;
        q += 1;
        if (!(em->x881 == 0xB && (t = em->x844, t != -1) && v == EM_FIELD(&player_work[t & 0xF], u8 *, 0x70E))) {
            CMD_SKIP(em, q, 0x5A);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x5A);
        break;
    case 2:
        break;
    }
    return q;
}
