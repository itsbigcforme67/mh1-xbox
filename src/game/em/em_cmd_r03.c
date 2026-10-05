/* em_cmd_r03 - monster command interpreter 0x0055CB50-0x0055D138: em_cmd_stage_no_ck, em_cmd_route_set, em_cmd_route_ck, em_cmd_kehai_pl_set, em_cmd_find_ck, em_cmd_pl_target_set. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_stage_no_ck(EMW *em, u8 *p) {
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
        if (em->stg != v) {
            CMD_SKIP(em, var_a1, 0xE);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0xE);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_route_set(EMW *em, u8 *p) {
    EM_STG_POS *s;
    u8 max;
    u8 kind;

    em->x92A = 0;
    em->x830 = *p++;
    max = *p++;
    em->x831 = *p++;
    em->x832 = *p++;
    em->x833 = *p++;
    ret_cmd(em, p);
    em->x83B |= 1;
    kind = em->x830;
    switch (kind) {
    case 0:
    case 1:
    case 2:
        s = gp_ck(em, em->area->x4, em->stg);
        if (s == NULL) {
            em->x83B &= 0xFE;
            return p;
        }
        if (s->num < max) {
            max = s->num;
        }
        em->x929 = max;
        em->x92B = s->num;
        em->cmd_route = route_ptr_set(em, em->x832);
        break;
    default:
        break;
    }

    return em->cmd_route;
}

u8 *em_cmd_route_ck(EMW *em, u8 *p) {
    u8 list[16];
    s8 n;
    s8 i;

    switch (em->x830) {
    case 0:
        if (em->x928 == -1 || em->x928 >= em->x92B) {
            i = 0;
            n = 0;
            for (; i < em->x92B; i++) {
                list[n] = i;
                n++;
            }
            if (n == 0) {
                em->x928 = 0;
                em_cmd_reset(em);
            } else {
                em->x928 = list[(s8)(em->x39A % n)];
            }
        }
        break;
    case 1:
        if (em->x928 == -1 || em->x928 >= em->x92B) {
            em->x928 = *option_route_ptr_set(em, em->x831);
        }
        break;
    case 2:
        if (em->x928 == -1 || em->x928 >= em->x92B) {
            em->x928 = 0;
        }
        break;
    }
    em->x827 = 2;
    em->x828 = 1;
    em->x829 = em->x928;
    return p;
}

u8 *em_cmd_kehai_pl_set(EMW *em, u8 *p) {
    int i;

    for (i = 0; i < *(u8 *)0x3F34C3; i++) {
        if (em->x915 & (1 << i)) {
            em->x829 = i;
        }
    }
    if (em->x8C3 == 0) {
        em->x884 = 2;
    }
    em->x827 = 1;
    em->x828 = 0;
    return p;
}

u8 *em_cmd_find_ck(EMW *em, u8 *p) {
    int i;

    for (i = 0; i < *(u8 *)0x3F34C3; i++) {
        if (em->x88F & (1 << i)) {
            em->x829 = i;
            em->x844 = em->x829;
        }
    }
    em->x827 = 1;
    em->x828 = 0;
    if (em->x8C3 == 0) {
        em->x885 = 2;
    }
    em->x83B &= 0xEE;
    return p;
}

u8 *em_cmd_pl_target_set(EMW *em, u8 *p) {
    em->x827 = 1;
    em->x828 = 0;
    if (em->x844 == -1) {
        em->x829 = 0xFF;
    } else {
        em->x829 = em->x844 & 0xF;
    }
    return p;
}
