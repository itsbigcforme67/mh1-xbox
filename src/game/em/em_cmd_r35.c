/* em_cmd_r35 - monster command 0x0055F460-0x0055F59C: em_cmd_body_status_sel. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_body_status_sel(EMW *em, u8 *p) {
    u8 n;
    s32 i;
    u16 more;

    switch (*p++) {
    case 0:
        n = *p;
        p += 3;
        for (i = 0; i < n; i++) {
            u8 vv = *p;
            p += 1;
            if ((u8)em->x762 != vv) {
                p = cmd_end_search(em, p, 0x33, 3);
                if (p[1] == 2 || p[1] == 3) {
                    p = p + 2;
                    break;
                }
                p = p + 2;
            } else {
                break;
            }
        }
        break;
    case 1:
        p += 1;
    case 2:
        more = 1;
        do {
            p = cmd_end_search(em, p, 0x33, 3);
            if (p[0] == 0x33 && p[1] == 3) {
                more = 0;
            }
            p = next_cmd_search(em, p);
        } while (more);
        break;
    case 3:
        break;
    }
    return p;
}
