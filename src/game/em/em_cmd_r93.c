/* em_cmd_r93 - near-match fixes: em_cmd_before_stage_ck. Whole file in em_cmd_nm.c. 0x0055FD80-0x0055FEC0: em_cmd_before_stage_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_before_stage_ck(EMW *em, u8 *p) {
    u8 *q;
    u8 v;
    u8 flag;

    q = p;
    switch (*q++) {
    case 0:
        v = *q++;
        if (v == 0xFF) {
            flag = 1;
        } else {
            flag = 1;
            if (em->x92E == v) {
                break;
            }
            flag = 1;
        }
        while (flag) {
            if (q[0] == 0x3D && q[1] == 1) {
                break;
            }
            if (q[0] == 0x3D && q[1] == 2) {
                break;
            }
            q = cmd_end_search(em, q, 0x3D, 2);
        }
        q = next_cmd_search(em, q);
        if (q[0] == 0x3D && q[1] == 2) {
            q = next_cmd_search(em, q);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x3D);
        break;
    case 2:
        break;
    }
    return q;
}
