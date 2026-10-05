/* em_cmd_r19 - monster command interpreter 0x005631E0-0x00563328: em_cmd_em_master_ck, em_cmd_em_cmd_reset. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_em_master_ck(EMW *em, u8 *p) {
    u8 *q;
    s32 s;

    q = p;
    switch (*q++) {
    case 0:
        s = em->x8C3 != 0;
        CMD_SKIPF(em, q, 0x67, s);
        break;
    case 1:
        q = else_ck(em, q, 0x67);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_em_cmd_reset(EMW *em, u8 *p) {
    em_cmd_reset(em);
    return p;
}
