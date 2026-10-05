/* em_cmd_r11 - monster command interpreter 0x0055F5A0-0x0055FC2C: em_cmd_act_st_ck, em_cmd_ikari_ck, em_cmd_water_ck, em_cmd_eye_dmg_ck, em_cmd_sensor_ck, em_cmd_boss_work_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_act_st_ck(EMW *em, u8 *p) {
    u8 *q;
    u8 a;
    u8 b;

    q = p;
    switch (*q++) {
    case 0:
        a = q[0];
        b = q[1];
        q += 2;
        if (em->mode == a && em->x15 == (b & 0xFF)) {
        } else {
            CMD_SKIP(em, q, 0x34);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x34);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_ikari_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x8B6 == 0) {
            CMD_SKIP(em, var_a1, 0x35);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x35);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_water_ck(EMW *em, u8 *p) {
    u8 *q;

    q = p;
    switch (*q++) {
    case 0:
        if (GetWaterData() == 0) {
            CMD_SKIP(em, q, 0x38);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x38);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_eye_dmg_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x94E == 0) {
            CMD_SKIP(em, var_a1, 0x39);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x39);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_sensor_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x9E1 != 0) {
            CMD_SKIP(em, var_a1, 0x3A);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x3A);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_boss_work_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (EM_FIELD(em, s32 *, 0x9D4) == 0) {
            CMD_SKIP(em, var_a1, 0x3B);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x3B);
        break;
    case 2:
        break;
    }
    return var_a1;
}
