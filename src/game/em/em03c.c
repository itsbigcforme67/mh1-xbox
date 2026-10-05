/* em03 (part 3) - game.bin 0x00588E90-0x0058B84C. Monster kind 3: move
 * state 1 (walking/turning: em_mv13 jump, em_move01 dispatch), attacks
 * em_atk00-08 (breath/shell attacks), damage reactions em_dmg00-04, death
 * (em_die00/04/05), demo, em03_move_sub (mode dispatch), em03_act_set (sets
 * the distance/time for the chosen action, then em_act_set2), em03_to_normal,
 * em03_char_set (small variant uses animation 0x4B1..) and the sound/effect
 * script (ef_move_sub, em03_effect_move). */
#include "em03.h"

static void em_mv13(EMW *em, EM03W *w) {
    u8 f = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 20, 0, 0);
        em_rate_clear(em);
        em->adj_y = 75.0f;
        em->x3C0[1] = -10.0f;
        em->adj_z = 50.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        if (em_frame_check2(em, 26.0f, 0)) {
            em->x388 = 2;
            w->spd[1] = em->ang[1];
            f = 1;
            speed_add_g(em, w->spd);
        }
        if (em_frame_check2(em, 36.0f, 0)) {
            em->act_spd = 0.0f;
        }
        GetGroundHitArea(em, em->pos, &em->x5AC);
        if (f) {
            if (em->pos[1] <= em->x5AC) {
                em->x05++;
                em->x388 = 0;
                em->pos[1] = em->x5AC;
                em->act_spd = 1.0f;
            }
        }
        break;
    case 2:
        if (em_frame_check2(em, 44.0f, 0)) {
            em->x05++;
            em_char_set(em, 14, 4, 10);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_move01_00589040(EMW *em, EM03W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_mv00_00588290(em); break;
    case 1: em_mv01_00588350(em, w); break;
    case 2: em_mv02_00588480(em, w, 0); break;
    case 3: em_mv03_005885F0(em, w, 0); break;
    case 4: em_mv04_00588760(em); break;
    case 5: em_mv05_00588810(em); break;
    case 6: em_mv06_005888C0(em); break;
    case 7: em_mv07_00588990(em); break;
    case 8: em_mv08_00588A60(em); break;
    case 9: em_mv02_00588480(em, w, 1); break;
    case 10: em_mv03_005885F0(em, w, 1); break;
    case 11: em_mv11_00588AD0(em); break;
    case 12: em_mv12_00588BF0(em); break;
    case 13: em_mv13(em, w); break;
    }
}

static void em_atk00_00589170(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->kind == 3) {
            em03_char_set(em, 16, 0, 0);
        } else {
            em03_char_set(em, 38, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_atk01(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 17, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 14, 0, 26);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_atk02(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 20, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 24, 0, 8);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_atk03(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 24, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 22.0f, 0)) {
            Shell08_set_ang(em, 23, 8, 0, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_atk04(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 90.0f, 0)) {
            Shell08_set_ang(em, 23, 8, 1, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_atk05(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 90.0f, 0)) {
            Shell08_set_ang(em, 23, 8, 2, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_atk06(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 90.0f, 0)) {
            Shell08_set_ang(em, 23, 8, 3, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_atk07(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 90.0f, 0)) {
            Shell08_set_ang(em, 23, 8, 4, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_atk08(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 90.0f, 0)) {
            Shell08_set_ang(em, 23, 8, 5, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_move03_00589910(EMW *em, EM03W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_atk00_00589170(em, w); break;
    case 1: em_atk01(em, w); break;
    case 2: em_atk02(em, w); break;
    case 3: em_atk03(em, w); break;
    case 4: em_atk04(em, w); break;
    case 5: em_atk05(em, w); break;
    case 6: em_atk06(em, w); break;
    case 7: em_atk07(em, w); break;
    case 8: em_atk08(em, w); break;
    }
}

static void em_dmg00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 60, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_dmg01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 61, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_dmg02(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 62, 0, 0);
        em_rate_clear(em);
        em->rate_x = 0.0f;
        em->adj_y = 17.0f;
        em->adj_z = -20.0f;
        em->x3C0[1] = -2.38f;
        em->x3C0[2] = 0.11f;
        em->x388 = 2;
        em_cmd_reset(em);
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add_g(em, w->spd);
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em03_char_set(em, 63, 0, 0);
            em->work08 = 90;
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em03_char_set(em, 64, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_dmg03(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 68, 0, 0);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em_cmd_reset(em);
        break;
    case 1:
        em_mahi_eff_set(em, 2);
        if (--em->work08 <= 0) {
            em->x05++;
            em03_act_set(em, 0, 10, 4);
        }
        break;
    }
}

static void em_dmg04(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 65, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 64, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_move04_00589E00(EMW *em, EM03W *w) {
    switch (em->x15) {
    case 0: em_dmg00(em); break;
    case 1: em_dmg01(em); break;
    case 2: em_dmg02(em, w); break;
    case 3: em_dmg03(em); break;
    case 4: em_dmg04(em); break;
    }
}

static void em_die00_00589EA0(EMW *em, EM03W *w) {
    em->x40C = 10;
    em->x40E = 10;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 62, 0, 0);
        em->act_spd = 1.0f;
        em_rate_clear(em);
        em->rate_x = 0.0f;
        em->adj_y = 17.0f;
        em->adj_z = -23.0f;
        em->x3C0[1] = -3.15f;
        em->x3C0[2] = 0.08f;
        em->x388 = 2;
        Quest_enemy_die(em);
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add_g(em, w->spd);
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em03_char_set(em, 63, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 2400;
            em->act_spd = 0.0f;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        if (--em->work08 <= 0 || Em_hagi_point_cnt_ck(em) <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 4:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
        }
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

static void em_die04_0058A0E0(EMW *em) {
    switch (em->x05) {
    case 0:
        if (Quest_enemy_revival_ck(em) == 1) {
            em_status_init(em);
            em03_init(em);
            Quest_enemy_revival_set(em);
            Quest_enemy_escape(em);
        }
        em->x04++;
        em->x01 = 0;
        break;
    }
}

static void em_die05_0058A160(EMW *em) {
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 100;
        break;
    case 1:
        em->work08--;
        em->x798 = (f32)em->work08 / 100.0f;
        if (em->work08 <= 0) {
            Quest_enemy_escape(em);
            em->x04++;
            em->x01 = 0;
        }
        break;
    }
}

static void em_move05_0058A210(EMW *em, EM03W *w) {
    switch (em->x15) {
    case 0: em_die00_00589EA0(em, w); break;
    case 4: em_die04_0058A0E0(em); break;
    case 5: em_die05_0058A160(em); break;
    }
}

static void em_demo00_0058A280(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        break;
    case 1:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            Quest_enemy_escape(em);
            em->x04++;
            em->x01 = 0;
        }
        break;
    }
}

static void em_move06_0058A310(EMW *em) {
    switch (em->x15) {
    case 0:
        em_demo00_0058A280(em);
        break;
    }
}

void em03_move_sub(EMW *em) {
    EM03W *w = (EM03W *)em->ex;

    switch (em->mode) {
    case 0: em_move00_00588180(em, w); break;
    case 1: em_move01_00589040(em, w); break;
    case 2: em_move00_00588180(em, w); break;
    case 3: em_move03_00589910(em, w); break;
    case 4: em_move04_00589E00(em, w); break;
    case 5: em_move05_0058A210(em, w); break;
    case 6: em_move06_0058A310(em); break;
    case 7: em_move06_0058A310(em); break;
    }
}

/* Sets the monster's next action: first the distance / time it moves for
 * (from the target), then the generic em_act_set2. */
void em03_act_set(EMW *em, int kind, u16 no, u16 arg) {
    EM03W *w = (EM03W *)em->ex;
    f32 v;

    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    switch ((u16)kind) {
    case 0:
        break;
    case 1:
        switch ((u16)no) {
        case 0:
            if (em->x881 == 0) {
                w->x18 = 0;
                em->work08 = ((u16)em->x39A % 120 + 180) / 2;
                w->x14 = 1000.0f;
                break;
            }
            w->x18 = 1;
            w->x14 = CalcDistanceXZ(em->pos, em->tgt_pos);
            if (em->x881 == 1 && em->x882 == 0) {
                if (em->x617 == -1) {
                    w->x18 = 0;
                    w->x14 = 1000.0f;
                    break;
                }
                w->x14 = w->x14 - 100.0f;
                if (w->x14 <= 0.0f) {
                    w->x14 = 0.0f;
                }
            }
            if (em->x881 == 7) {
                w->x14 = w->x14 - 100.0f;
                if (w->x14 <= 0.0f) {
                    w->x14 = 0.0f;
                }
            }
            em->work08 = (s32)w->x14 / 7;
            break;
        case 1:
        case 2:
        case 3:
            v = 100.0f;
            goto common;
        case 9:
        case 10:
            v = 300.0f;
        common:
            if (em->x881 == 0) {
                w->x18 = 0;
                em->work08 = ((u16)em->x39A % 120 + 180) / 2;
                w->x14 = 1000.0f;
                break;
            }
            w->x18 = 1;
            w->x14 = CalcDistanceXZ(em->pos, em->tgt_pos);
            if (em->x881 == 1 && em->x882 == 0) {
                if (em->x617 == -1) {
                    w->x18 = 0;
                    w->x14 = 1000.0f;
                    break;
                }
                w->x14 = w->x14 - v;
                if (w->x14 <= 0.0f) {
                    w->x14 = 0.0f;
                }
            }
            if (em->x881 == 7) {
                w->x14 = w->x14 - v;
                if (w->x14 <= 0.0f) {
                    w->x14 = 0.0f;
                }
            }
            em->work08 = (s32)w->x14 / 7;
            break;
        }
        break;
    case 3:
        break;
    }
    em->act_spd = 1.0f;
    em_act_set2(em, kind, no, arg);
}

void em03_to_normal(EMW *em) {
    em->act_spd = 1.0f;
    em->x388 = 0;
    em->x3F4 = 0;
    em->x839 = 1;
    em03_act_set(em, 0, 1, 0);
}

void em03_char_set(EMW *em, int no, int a, int b) {
    int n;

    em_char_set(em, no, a, b);
    if (em->x11 != 0) {
        switch (em->char0) {
        case 0x3E9: n = 0x4B1; break;
        case 0x3EA: n = 0x4B2; break;
        case 0x3EB: n = 0x4B3; break;
        case 0x3ED: n = 0x4B5; break;
        case 0x3F1: n = 0x4B9; break;
        case 0x3F2: n = 0x4BA; break;
        case 0x3F3: n = 0x4BB; break;
        default: n = 0; break;
        }
        if (n != 0) {
            em_char_set2(em, n, a, b, 1);
        }
    }
}

void em03_local_init(void) {
}

void dummy_em_prog_0058A8D0(void) {
}

static void sound_call_sub_0058A8E0(EMW *em, int se, int idx) {
    f32 pos[3];

    flmatGetTrans(pos, (u8 *)em->mdl->bone + idx * 400);
    Em_se_req2(em, se, 0, pos, 6, 0);
}

static void sound_call_0058A950(EMW *em, int frame, int se, int idx) {
    if (em_frame_check(em, (f32)frame, 0)) {
        sound_call_sub_0058A8E0(em, se, idx);
    }
}

static void move_default_0058A9B0(EMW *em) {
}

/* Sound and effect script per animation (sound_call(em, frame, se, joint)
 * plays a sound at the joint's position once the animation reaches the
 * frame; Eft13_set_em_scl and shell02_set spawn effects). */
static void ef_move_sub_0058A9C0(EMW *em, EM03W *w) {
    f32 v[3];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        break;
    case 0x3EA:
        sound_call_0058A950(em, 92, 13, 23);
        sound_call_0058A950(em, 122, 13, 23);
        sound_call_0058A950(em, 146, 13, 23);
        sound_call_0058A950(em, 182, 13, 23);
        sound_call_0058A950(em, 210, 13, 23);
        sound_call_0058A950(em, 240, 13, 23);
        sound_call_0058A950(em, 270, 13, 23);
        break;
    case 0x3EB:
        sound_call_0058A950(em, 14, 11, 23);
        break;
    case 0x3EC:
        sound_call_0058A950(em, 6, 4, 23);
        sound_call_0058A950(em, 26, 19, 0);
        sound_call_0058A950(em, 32, 19, 0);
        sound_call_0058A950(em, 50, 0, 0);
        break;
    case 0x3ED:
        sound_call_0058A950(em, 4, Code_Make(16,4,17,4), 23);
        sound_call_0058A950(em, 84, Code_Make(17,4,18,4), 23);
        sound_call_0058A950(em, 200, Code_Make(16,4,18,4), 23);
        break;
    case 0x3EE:
        sound_call_0058A950(em, 22, 14, 23);
        sound_call_0058A950(em, 58, 14, 23);
        break;
    case 0x3F1:
        sound_call_0058A950(em, 10, 0, 0);
        sound_call_0058A950(em, 28, 0, 0);
        sound_call_0058A950(em, 58, 0, 0);
        sound_call_0058A950(em, 64, 0, 0);
        break;
    case 0x3F2:
        sound_call_0058A950(em, 6, 19, 0);
        sound_call_0058A950(em, 14, 19, 0);
        sound_call_0058A950(em, 18, 19, 0);
        sound_call_0058A950(em, 26, 19, 0);
        if (em_frame_check(em, 4.0f, 0)) {
            Eft13_set_em_scl(em, 21, 0.6f, 3);
        }
        if (em_frame_check(em, 28.0f, 0)) {
            Eft13_set_em_scl(em, 17, 0.6f, 3);
        }
        break;
    case 0x3F3:
        sound_call_0058A950(em, 16, 19, 0);
        sound_call_0058A950(em, 26, 19, 0);
        sound_call_0058A950(em, 12, 19, 0);
        sound_call_0058A950(em, 20, 19, 0);
        if (em_frame_check(em, 12.0f, 0)) {
            Eft13_set_em_scl(em, 21, 0.7f, 3);
        }
        if (em_frame_check(em, 18.0f, 0)) {
            Eft13_set_em_scl(em, 17, 0.7f, 3);
        }
        break;
    case 0x3F4:
    case 0x3F5:
        sound_call_0058A950(em, 12, 1, 0);
        sound_call_0058A950(em, 10, 19, 0);
        sound_call_0058A950(em, 14, 19, 0);
        if (w->anim == 0x3F7) {
            if (em_frame_check(em, 12.0f, 0)) {
                Eft13_set_em_scl(em, 21, 0.9f, 3);
            }
        } else {
            if (em_frame_check(em, 12.0f, 0)) {
                Eft13_set_em_scl(em, 17, 0.9f, 3);
            }
        }
        break;
    case 0x3F6:
        sound_call_0058A950(em, 12, 19, 0);
        sound_call_0058A950(em, 16, 19, 0);
        sound_call_0058A950(em, 46, 0, 0);
        if (em_frame_check(em, 12.0f, 0)) {
            Eft13_set_em_scl(em, 11, 0.6f, 3);
        }
        if (em_frame_check(em, 16.0f, 0)) {
            Eft13_set_em_scl(em, 11, 0.6f, 3);
        }
        if (em_frame_check(em, 12.0f, 0)) {
            Eft13_set_em_scl(em, 7, 0.6f, 3);
        }
        if (em_frame_check(em, 16.0f, 0)) {
            Eft13_set_em_scl(em, 7, 0.6f, 3);
        }
        break;
    case 0x3F7:
    case 0x3FE:
        sound_call_0058A950(em, 12, 0, 0);
        sound_call_0058A950(em, 16, 0, 0);
        sound_call_0058A950(em, 22, 19, 0);
        sound_call_0058A950(em, 30, 1, 0);
        sound_call_0058A950(em, 56, 19, 0);
        sound_call_0058A950(em, 58, 19, 0);
        if (w->anim == 0x3F7) {
            if (em_frame_check(em, 34.0f, 0)) {
                Eft13_set_em_scl(em, 21, 0.6f, 3);
            }
        } else {
            if (em_frame_check(em, 34.0f, 0)) {
                Eft13_set_em_scl(em, 17, 0.6f, 3);
            }
        }
        break;
    case 0x3F8:
        sound_call_0058A950(em, 4, 7, 23);
        sound_call_0058A950(em, 62, 8, 23);
        sound_call_0058A950(em, 76, 9, 23);
        sound_call_0058A950(em, 148, 10, 23);
        sound_call_0058A950(em, 18, 0, 0);
        sound_call_0058A950(em, 24, 0, 0);
        break;
    case 0x3F9:
        sound_call_0058A950(em, 14, 2, 23);
        sound_call_0058A950(em, 24, 1, 0);
        sound_call_0058A950(em, 46, 0, 0);
        sound_call_0058A950(em, 48, 0, 0);
        if (em_frame_check(em, 20.0f, 0)) {
            if (em->kind == 3) {
                shell02_set(em, 5);
            } else {
                shell02_set(em, 11);
            }
        }
        if (em_frame_check(em, 24.0f, 0)) {
            Eft13_set_em_scl(em, 22, 0.9f, 3);
        }
        break;
    case 0x3FA:
        sound_call_0058A950(em, 24, 1, 0);
        sound_call_0058A950(em, 26, 19, 0);
        sound_call_0058A950(em, 44, 19, 0);
        sound_call_0058A950(em, 46, 19, 0);
        if (em_frame_check(em, 26.0f, 0)) {
            Eft13_set_em_scl(em, 22, 0.9f, 3);
        }
        break;
    case 0x3FC:
        sound_call_0058A950(em, 12, 19, 0);
        sound_call_0058A950(em, 14, 19, 0);
        sound_call_0058A950(em, 24, 19, 0);
        break;
    case 0x400:
        sound_call_0058A950(em, 12, 3, 23);
        sound_call_0058A950(em, 54, 0, 0);
        sound_call_0058A950(em, 58, 0, 0);
        if (em_frame_check(em, 14.0f, 0)) {
            if (em->kind == 3) {
                shell02_set(em, 6);
            } else {
                shell02_set(em, 12);
            }
        }
        break;
    case 0x40E:
        sound_call_0058A950(em, 12, 0, 0);
        sound_call_0058A950(em, 46, 0, 0);
        sound_call_0058A950(em, 64, 1, 0);
        sound_call_0058A950(em, 4, 17, 0);
        break;
    case 0x40F:
        sound_call_0058A950(em, 2, 11, 0);
        sound_call_0058A950(em, 4, 23, 0);
        sound_call_0058A950(em, 90, 2, 0);
        break;
    case 0x424:
    case 0x425:
        sound_call_0058A950(em, 4, 5, 23);
        sound_call_0058A950(em, 20, 0, 0);
        sound_call_0058A950(em, 28, 0, 0);
        sound_call_0058A950(em, 34, 0, 0);
        break;
    case 0x426:
        sound_call_0058A950(em, 4, 12, 23);
        break;
    case 0x427:
        sound_call_0058A950(em, 4, 6, 0);
        sound_call_0058A950(em, 44, 20, 0);
        if (em_frame_check(em, 44.0f, 0)) {
            Eft13_set_em_scl(em, 2, 0.5f, 6);
        }
        break;
    case 0x428:
        sound_call_0058A950(em, 14, 1, 0);
        sound_call_0058A950(em, 50, 1, 0);
        sound_call_0058A950(em, 24, 0, 0);
        sound_call_0058A950(em, 44, 0, 0);
        sound_call_0058A950(em, 58, 0, 0);
        sound_call_0058A950(em, 76, 0, 0);
        break;
    case 0x429:
        sound_call_0058A950(em, 14, 0, 0);
        sound_call_0058A950(em, 38, 0, 0);
        sound_call_0058A950(em, 66, 0, 0);
        sound_call_0058A950(em, 124, 20, 0);
        sound_call_0058A950(em, 98, 1, 0);
        sound_call_0058A950(em, 4, 4, 0);
        break;
    case 0x42B:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 10.0f;
        em_sleep_eff_set(em, 23, v, 1.0f);
        break;
    case 0x42C:
        sound_call_0058A950(em, 4, 15, 23);
        sound_call_0058A950(em, 88, 19, 0);
        sound_call_0058A950(em, 138, 15, 23);
        break;
    case 0x42D:
        sound_call_0058A950(em, 4, Code_Make(16,4,17,4), 23);
        sound_call_0058A950(em, 10, 0, 0);
        sound_call_0058A950(em, 20, 0, 0);
        break;
    default:
        move_default_0058A9B0(em);
        break;
    }
}

void em03_effect_move(EMW *em) {
    EM03W *w = (EM03W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_0058A9C0(em, w);
        break;
    }
}
