/* em_cmd_r12 - monster command interpreter 0x005604C0-0x005605C4: em_cmd_em_mode_change, em_cmd_em_hp_vital_add. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_em_mode_change(EMW *em, u8 *p) {
    if (*p == 0) {
        Em_Mode_Chg(em, 0, 0);
    } else {
        Em_Mode_Chg(em, 1, em_atk_mode_timer_tbl[em->kind]);
    }
    return p + 1;
}

u8 *em_cmd_em_hp_vital_add(EMW *em, u8 *p) {
    u8 v = *p;

    switch (em->kind) {
    case 0x1B:
    case 0x1C:
    case 0x1F:
        if (v == 0) {
            em_hp_add(em, (s32)(0.15f * (f32)em->x792));
        }
        break;
    }
    return p + 1;
}
