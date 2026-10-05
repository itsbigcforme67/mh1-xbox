/* em_cmd_r13 - monster command interpreter 0x00560960-0x0056122C: em_cmd_pl_fishing_ck, em_cmd_target_pl_act_ck, em_cmd_fish_ok_ck, em_cmd_timer_set, em_cmd_pl_land_target, em_cmd_pl_look_ck, em_cmd_kehai_clear, em_cmd_hate_clear, em_cmd_horm_pos_set, em_cmd_thirst_add, em_cmd_hungry_add, em_cmd_suimin_add, em_cmd_swim_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_pl_fishing_ck(EMW *em, u8 *p) {
    u8 list[4];
    u8 *q;
    PLW *pl;
    s8 i;
    s8 n;
    u16 found;
    u8 *w;

    q = p;
    pl = player_work;
    switch (*q++) {
    case 0:
        i = 0;
        n = 0;
        found = 0;
        w = list;
        do {
            if (pl->be_flag != 0 && Pl_stg_ck_tw(em, pl) != 0 && pl_flag_ck(pl, 0x80000) != 0) {
                *w = i;
                w++;
                n++;
                found = 1;
            }
            i++;
            pl++;
        } while (i < 4);
        if (found) {
            em->x844 = list[em->x39A % n];
        } else {
            CMD_SKIP(em, q, 0x45);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x45);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_target_pl_act_ck(EMW *em, u8 *p) {
    u8 *q;
    u8 a;
    u8 b;

    q = p;
    switch (*q++) {
    case 0:
        a = q[0];
        b = q[1];
        q += 2;
        if ((s16)act_ck((EMW *)&player_work[em->x844 & 0xF], a, b) == 0) {
            CMD_SKIP(em, q, 0x46);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x46);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_fish_ok_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x8B9 != 0) {
            em->x8B9 = 0U;
        } else {
            CMD_SKIP(em, var_a1, 0x47);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x47);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_timer_set(EMW *em, u8 *p) {
    em->work08 = *p * 30;
    return p + 1;
}

u8 *em_cmd_pl_land_target(EMW *em, u8 *p) {
    u8 v = *p;

    if (em->x844 == -1) {
        em->x827 = 1;
        em->x828 = 0;
        em->x829 = 0xFF;
    } else {
        em->x827 = 0xB;
        em->x828 = (u8) (em->x844 & 0xF);
        em->x829 = v;
    }
    em->x881 = (u8) em->x827;
    em->x882 = (u8) em->x828;
    em->x883 = (u8) em->x829;
    return p + 1;
}

u8 *em_cmd_pl_look_ck(EMW *em, u8 *p) {
    u8 *q;
    s8 t;

    q = p;
    switch (*q++) {
    case 0:
        t = em->x844;
        if (!(em->x88C & (1 << (t & 0xF))) || t == -1) {
            CMD_SKIP(em, q, 0x4A);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x4A);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_kehai_clear(EMW *em, u8 *p) {
    if (em->x844 != -1) {
        em->x8F4[em->x844 & 0xF] = 0;
    }
    return p;
}

u8 *em_cmd_hate_clear(EMW *em, u8 *p) {
    if (em->x844 != -1) {
        em->x918[em->x844 & 0xF] = 0;
    }
    return p;
}

u8 *em_cmd_horm_pos_set(EMW *em, u8 *p) {
    cmd_target_kind_set(em, em->tgt_pos);
    return p;
}

u8 *em_cmd_thirst_add(EMW *em, u8 *p) {
    switch (*p) {
    case 0:
        em_thirst_add(em, em->thirst_max / 2);
        break;
    default:
        break;
    }
    return p + 1;
}

u8 *em_cmd_hungry_add(EMW *em, u8 *p) {
    switch (*p) {
    case 0:
        em_hungry_add(em, em->hungry_max / 2);
        break;
    default:
        break;
    }
    return p + 1;
}

u8 *em_cmd_suimin_add(EMW *em, u8 *p) {
    switch (*p) {
    case 0:
        em_suimin_add(em, em->x8AC / 2);
        break;
    default:
        break;
    }
    return p + 1;
}

u8 *em_cmd_swim_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x388 != 4) {
            CMD_SKIP(em, var_a1, 0x51);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x51);
        break;
    case 2:
        break;
    }
    return var_a1;
}
