/* em_cmd_r94 - near-match fixes: em_cmd_boss_same_stage_ck. Whole file in em_cmd_nm.c. 0x00560820-0x00560960: em_cmd_boss_same_stage_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_boss_same_stage_ck(EMW *em, u8 *p) {
    u8 *q;
    EMW *b;
    u8 flag;

    q = p;
    switch (*q++) {
    case 0:
        b = em->boss;
        if (b == NULL) {
            flag = 1;
        } else {
            if (em->stg == b->stg) {
                break;
            }
            flag = 1;
        }
        while (flag) {
            if (q[0] == 0x44 && q[1] == 1) {
                break;
            }
            if (q[0] == 0x44 && q[1] == 2) {
                break;
            }
            q = cmd_end_search(em, q, 0x44, 2);
        }
        q = next_cmd_search(em, q);
        if (q[0] == 0x44 && q[1] == 2) {
            q = next_cmd_search(em, q);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x44);
        break;
    case 2:
        break;
    }
    return q;
}
