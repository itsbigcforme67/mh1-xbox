/* em_cmd_r92 - near-match fixes: em_cmd_boss_atk_ck. Whole file in em_cmd_nm.c. 0x0055FC30-0x0055FD80: em_cmd_boss_atk_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_boss_atk_ck(EMW *em, u8 *p) {
    u8 *q;
    EMW *b;
    u8 flag;

    q = p;
    switch (*q++) {
    case 0:
        b = em->boss;
        flag = 1;
        if (b == NULL) {
            flag = 1;
        } else {
            if (b->x888 == 1 && b->stg == em->stg) {
                break;
            }
            flag = 1;
        }
        while (flag) {
            if (q[0] == 0x3C && q[1] == 1) {
                break;
            }
            if (q[0] == 0x3C && q[1] == 2) {
                break;
            }
            q = cmd_end_search(em, q, 0x3C, 2);
        }
        q = next_cmd_search(em, q);
        if (q[0] == 0x3C && q[1] == 2) {
            q = next_cmd_search(em, q);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x3C);
        break;
    case 2:
        break;
    }
    return q;
}
