/* em_cmd_r07 - monster command interpreter 0x0055E170-0x0055E2A4: em_cmd_thirst_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_thirst_ck(EMW *em, u8 *p) {
    u8 *q;

    q = p;
    switch (*q++) {
    case 0:
        if (em->thirst > (s32)(0.3f * (f32)em->thirst_max)) {
            CMD_SKIP(em, q, 0x21);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x21);
        break;
    case 2:
        break;
    }
    return q;
}
