/* em_cmd_r24 - monster command interpreter 0x005655D0-0x00565840: em_cmd_reset, Em_Next_Stage_Pos. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































void em_cmd_reset(EMW *em) {
    if (em->x8C3 == 0 && em->x86F == 0) {
        em_g_init_flag_set(em, 2);
    }
    em->x84E = 1;
    reset_flag_ck(em);
}

void Em_Next_Stage_Pos(EMW *em) {
    EM_STG_BOX *from;
    EM_STG_BOX *to;
    f32 v[6];
    f32 x;
    f32 z;

    em->x92E = em->stg;
    em->stg = (u16)em->x73A;
    from = Stage_data_get(em->x92E);
    to = Stage_data_get(em->stg);
    v[0] = from->x + from->w / 2.0f;
    v[2] = from->z + from->d / 2.0f;
    v[3] = to->x + to->w / 2.0f;
    v[5] = to->z + to->d / 2.0f;
    switch ((u16)((((Em_Calc_angY(v, v + 3) & 0xFFFF) + 0x1000) & 0xFFFF) >> 13)) {
    case 4:
        x = to->w / 2.0f;
        z = to->d;
        break;
    case 5:
        x = to->w;
        z = to->d;
        break;
    case 6:
        x = to->w;
        z = to->d / 2.0f;
        break;
    case 7:
        x = to->w;
        z = 0.0f;
        break;
    case 0:
        x = to->w / 2.0f;
        z = 0.0f;
        break;
    case 1:
        x = 0.0f;
        z = x;
        break;
    case 2:
        x = 0.0f;
        z = to->d / 2.0f;
        break;
    case 3:
        x = 0.0f;
        z = to->d;
        break;
    }
    em->pos[0] = x;
    em->pos[2] = z;
    em->x5A0[0] = x;
    em->x5A0[2] = z;
    em->adj_z = 50.0f;
    em->x5A0[1] = area_move_high_y_tbl[em->stg];
    em->pos[1] = em->x5A0[1];
    em_area_move_init(em);
}
