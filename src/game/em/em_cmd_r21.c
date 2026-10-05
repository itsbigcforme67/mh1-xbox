/* em_cmd_r21 - monster command interpreter 0x00563580-0x005636E8: em_cmd_target_set. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_target_set(EMW *em, u8 *p) {
    em->x827 = *p;
    switch (em->x827) {
    case 0:
        em->x827 = 0;
        p++;
        em->x828 = *p++;
        em->x829 = *p++;
        break;
    case 1:
        em->x827 = 1;
        p++;
        em->x828 = *p++;
        em->x829 = *p++;
        break;
    case 2:
    case 4:
    case 5:
    case 6:
        p++;
        em->x828 = *p++;
        em->x829 = *p++;
        break;
    case 3:
        p++;
        em->x828 = *p++;
        em->x829 = *p++;
        em->x828 = em->x829;
        em->x73A = em->x829;
        em->x92F = 0xFF;
        break;
    case 10:
        em->x827 = 3;
        p++;
        em->x828 = *p++;
        em->x829 = *p++;
        if (em->x844 == -1) {
            em->x829 = em->stg;
        } else {
            em->x829 = player_work[em->x844].stg;
        }
        em->x828 = em->x829;
        em->x73A = em->x829;
        em->x92F = 0xFF;
        break;
    default:
        p++;
        em->x828 = *p++;
        em->x829 = *p++;
        break;
    }
    return p;
}
