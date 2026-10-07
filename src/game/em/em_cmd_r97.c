/* em_cmd_r97 - monster command interpreter 0x0055E2B0-0x0055E424: em_cmd_near_pos_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"

u8 *em_cmd_near_pos_ck(EMW *em, u8 *p) {
    u8 *q;
    f32 lim;

    q = p;
    switch (*q++) {
    case 0:
        lim = 100.0f * *q;
        q += 1;
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= lim)) {
            CMD_SKIP(em, q, 0x22);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x22);
        break;
    case 2:
        break;
    }
    return q;
}
