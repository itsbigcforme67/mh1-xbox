/* em_cmd_r06 - monster command interpreter 0x0055DF20-0x0055DF54: em_cmd_mind_move_end. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_mind_move_end(EMW *em, u8 *p) {
    EM_FIELD(em, s8 *, 0x889) = 0;
    EM_FIELD(em, s8 *, 0x88A) = 0;
    EM_FIELD(em, s8 *, 0x9E3) = 0;
    EM_FIELD(em, s8 *, 0x9E4) = 0;
    EM_FIELD(em, s8 *, 0x9E5) = 0;
    EM_FIELD(em, s8 *, 0x9E6) = 0;
    EM_FIELD(em, s8 *, 0x9E7) = 0;
    EM_FIELD(em, s8 *, 0x9E8) = 0;
    em->x8C1 = 0;
    em->x8C0 = 0;
    EM_FIELD(em, s8 *, 0x8BF) = 0;
    return p;
}
