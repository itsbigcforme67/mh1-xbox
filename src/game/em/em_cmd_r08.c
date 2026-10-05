/* em_cmd_r08 - monster command interpreter 0x0055E430-0x0055E740: em_cmd_emtype_ck, em_cmd_repeat_cnt_set, em_cmd_repeat_cnt_clr, em_cmd_demo_flag_set. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_emtype_ck(EMW *em, u8 *p) {
    u8 *q;
    s8 found;
    s8 i;
    EMW *w;
    u8 kind;
    u8 type;

    w = em_work;
    q = p;
    switch (*q++) {
    case 0:
        kind = q[0];
        type = q[1];
        i = 0;
        found = 0;
        q += 2;
        for (; i < 20; i++, w++) {
            if (w->be_flag != 0 && w->kind == kind && (Pl_stg_ck_tw(em, (PLW *)w) & 0xFF) &&
                (w->type == type || type == 0xFF)) {
                found = 1;
                break;
            }
        }
        if (found == 0) {
            em->x944 = NULL;
            CMD_SKIP(em, q, 0x23);
        } else {
            em->x944 = w;
        }
        break;
    case 1:
        q = else_ck(em, q, 0x23);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_repeat_cnt_set(EMW *em, u8 *p) {
    u8 *q;
    s8 t;

    q = p;
    switch (*q++) {
    case 0:
        em->x84F = *q;
        q += 1;
        em->cmd_p858 = q;
        if (em->x84F <= 0) {
            for (;;) {
                if (q[0] == 0x24 && q[1] == 2) {
                    break;
                }
                q = cmd_end_search(em, q, 0x24, 1);
            }
            q = next_cmd_search(em, q);
            if (q[0] == 0x24 && q[1] == 1) {
                q = next_cmd_search(em, q);
            }
        }
        break;
    case 1:
        t = em->x84F - 1;
        em->x84F = t;
        if (t > 0) {
            q = em->cmd_p858;
        }
        break;
    }
    return q;
}

u8 *em_cmd_repeat_cnt_clr(EMW *em, u8 *p) {
    EM_FIELD(em, s8 *, 0x84F) = 0;
    return p;
}

u8 *em_cmd_demo_flag_set(EMW *em, u8 *p) {
    em->x8C2 = (u8) *p;
    return p + 1;
}
