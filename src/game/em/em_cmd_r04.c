/* em_cmd_r04 - monster command interpreter 0x0055D5E0-0x0055D7EC: em_cmd_action_set, em_cmd_area_route_set, em_cmd_area_route_move, em_cmd_area_route_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_action_set(EMW *em, u8 *p) {
    em->x845 = *p;
    em->cmd_p840 = p + 1;
    p = action_ptr_set(em, em->x845);
    em->cmd_top = p;
    return p;
}

u8 *em_cmd_area_route_set(EMW *em, u8 *p) {
    if (em->x92C != -1) {
        return p + 4;
    }
    em->x846 = *p++;
    em->x92C = *p++;
    em->x847 = *p++;
    em->x84C = *p++;
    em->x92D = 0;
    if (em->x92C == 0) {
        em->x92C = -1;
        return p;
    }
    return p;
}

u8 *em_cmd_area_route_move(EMW *em, u8 *p) {
    u16 done;
    u8 v;

    done = 0;
    if (em->x92C == -1) {
        return p;
    }
    em->x84D = 1;
    em->x92E = 0xFF;
    for (;;) {
        v = area_route_ptr_set(em, em->x846)[em->x92D];
        if (v >= 0x80) {
            em->x92F = *area_route_rnd32(em, (u8 *)em->cmd_tbl[7][v & 0x7F]);
        } else {
            em->x92F = v;
        }
        if (em->x92F == em->stg) {
            em->x92D = em->x92D + 1;
            if (!(em->x92C > em->x92D)) {
                if (em->x84C == 0) {
                    done = 1;
                    em->x92D = -1;
                    em->x92C = -1;
                } else {
                    em->x92D = 0;
                }
            }
            if ((done & 0xFF) == 1) {
                return p;
            }
            continue;
        }
        break;
    }
    em->cmd_p848 = p;
    p = area_move_ptr_set(em, em->x847);
    em->cmd_top = p;
    return p;
}

u8 *em_cmd_area_route_ck(EMW *em, u8 *p) {
    u8 temp_v1;

    EM_FIELD(em, s8 *, 0x827) = 3;
    temp_v1 = em->x92F;
    em->x829 = temp_v1;
    em->x828 = temp_v1;
    return p;
}
