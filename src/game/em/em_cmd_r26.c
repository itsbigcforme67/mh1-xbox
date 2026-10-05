/* em_cmd_r26 - monster command interpreter 0x00566490-0x005664F8: reset_flag_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































void reset_flag_ck(EMW *em) {
    if (EM_FIELD(em, u8 *, 0x84E) != 0) {
        EM_FIELD(em, s8 *, 0x839) = 0;
        em->x84E = 0;
        EM_FIELD(em, s8 *, 0x83B) = 0;
        em->cmd_idx = 0;
        em->cmd_pc = em_cmd_top(em);
        em->x928 = -1;
        em->x929 = -1;
        em->x92D = -1;
        em->x92C = -1;
        em->x92F = 0xFF;
        em->x92E = 0xFF;
        em->x854 = 0;
    }
}
