/* em_cmd_r99 - monster command interpreter 0x00562220-0x0056229C: em_cmd_ninshiki_timer_sub. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"

u8 *em_cmd_ninshiki_timer_sub(EMW *em, u8 *p) {
    u8 v;
    s8 i;

    v = *p;
    for (i = 0; i < game_w.pl_num; i++) {
        switch (v) {
        case 0:
            if (!(em->x88C & (1 << i))) {
                em->x890[i] = 0;
            }
            break;
        }
    }
    return p + 1;
}
