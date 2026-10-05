/* em_cmd_r10 - monster command interpreter 0x0055F120-0x0055F45C: em_cmd_smell_set, em_cmd_search_data_set, em_cmd_egg_ck, em_cmd_egg_cancel_ck, em_cmd_yobi_pos_set, em_cmd_body_status_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_smell_set(EMW *em, u8 *p) {
    EM_FIELD(em, s8 *, 0x827) = 7;
    em->x828 = (u8) em->x951;
    em->x829 = (u8) em->x952;
    return p;
}

u8 *em_cmd_search_data_set(EMW *em, u8 *p) {
    em_search_data_set(em, *p);
    return p + 1;
}

u8 *em_cmd_egg_ck(EMW *em, u8 *p) {
    s8 i;
    u16 found;
    u8 *q;
    u8 n;

    q = p;
    switch (*q++) {
    case 0:
        n = *(u8 *)0x3F34C3;
        found = 0;
        for (i = 0; i < n; i++) {
            if (player_work[i].work56B & 0xF) {
                found = 1;
                break;
            }
        }
        if (!found) {
            CMD_SKIP(em, q, 0x2F);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x2F);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_egg_cancel_ck(EMW *em, u8 *p) {
    s8 i;
    s32 found;
    u8 n;

    found = 0;
    n = *(u8 *)0x3F34C3;
    for (i = 0; i < n; i++) {
        if (player_work[i].work56B & 0xF) {
            found = 1;
            break;
        }
    }
    if (found != 0) {
        em->x9E5 = 1;
    }
    return p;
}

u8 *em_cmd_yobi_pos_set(EMW *em, u8 *p) {
    EM_FIELD(em, s8 *, 0x827) = 8;
    return p;
}

u8 *em_cmd_body_status_ck(EMW *em, u8 *p) {
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
        if (EM_FIELD(em, u8 *, 0x762) != v) {
            CMD_SKIP(em, var_a1, 0x32);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x32);
        break;
    case 2:
        break;
    }
    return var_a1;
}
