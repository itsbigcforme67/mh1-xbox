/* em04 (part 1) - game.bin 0x0058BCC0-0x0058D4D8. Monster kind 4: action
 * steps em_act00-10 (waiting, turning, sleeping...), move states em_move00/
 * 01/03 (turning, walking, attack with shells 3-15) and damage reactions
 * em_dm00-02 (knock back from the hit direction em->dm_ang). Names of the
 * steps follow the split. Meanings of most fields are guesses; the
 * animation numbers are the monster's own. The rest of the monster is in
 * em04b.c / em04c.c (em04_nm.c holds the whole file, with the near-matches). */
#include "em04.h"

void em04_next_act_set(EMW *em) {
    if (em->x734 == 3) {
        em->x839 = 1;
        em_act_set(em, 0, 1);
    } else {
        em_act_set(em, 0, em_act_search(em04_act_tbl));
    }
}

static void em_act00(EMW *em) {
}

static void em_act01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->work08 = 80;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 10, 0);
        }
        break;
    case 1:
        if (--em->work08 <= 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act02(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->work08 = (u16)ran_suu(1) % 60 + 30;
        em_char_set(em, 10, 6, 0);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 13, 6, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act03(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 7, 6, 0);
        em_action_timer_calc(em, 0);
        break;
    case 1:
        if (em->x39C >= 240) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act04(EMW *em, int n) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->work08 = n;
        em_char_set(em, 14, 6, 0);
        em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
        cpRotMatrix(em->ang, em->mat);
        if (em->x194 == 0) {
            if (--em->work08 <= 0) {
                em04_next_act_set(em);
            }
        }
        break;
    }
}

static void em_act05(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 12, 6, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act06(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 17, 6, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act07(EMW *em) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 70, 0, 0);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 10.0f;
        em_sleep_eff_set(em, 11, v, 1.0f);
        if (--em->work08 <= 0) {
            em->x05++;
            em_act_set(em, 0, 11);
        }
        break;
    }
}

static void em_act08(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x88B = 1;
        Em_Sleep_End(em);
        em_char_set(em, 73, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_act_set(em, 0, 9);
        }
        break;
    }
}

static void em_act09(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 11, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act10(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 19, 6, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

void em_move00_0058C350(EMW *em) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_act00(em); break;
    case 1: em_act01(em); break;
    case 2: em_act02(em); break;
    case 3: em_act03(em); break;
    case 4: em_act04(em, 1); break;
    case 5: em_act05(em); break;
    case 6: em_act06(em); break;
    case 7: em_act04(em, 2); break;
    case 8: em_act04(em, 4); break;
    case 9: em_act09(em); break;
    case 12: em_act10(em); break;
    case 10: em_act07(em); break;
    case 11: em_act08(em); break;
    }
}

static void em_mov00(EMW *em, int flag) {
    EM04W *w = (EM04W *)em->ex;
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        if (flag == 0) {
            w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        } else {
            w->tgt_ang = em->ang[1] + 0x4000;
        }
        d = (u16)(w->tgt_ang - em->ang[1]);
        if (d <= 0x1000 || d >= 0xF000) {
            pl_flag_set(em, 0x20000);
            em_char_set(em, 2, 0, 0);
        } else if (d >= 0x8000) {
            em_char_set(em, 5, 0, 0);
        } else {
            em_char_set(em, 4, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            u32 spd = (u32)(16384.0f / (em->x1A8 / 2.0f) * em->act_spd);
            u32 ang = em->ang[1];

            d = (u16)(w->tgt_ang - (u16)ang);
            if (em->x194 == 0) {
                if ((u16)(d + spd) < spd * 2) {
                    em->x05++;
                    pl_flag_clr(em, 0x20000);
                    em04_next_act_set(em);
                } else if (d <= 0x1000 || d >= 0xF000) {
                    pl_flag_set(em, 0x20000);
                    em_char_set(em, 2, 0, 0);
                } else {
                    pl_flag_clr(em, 0x20000);
                    if (d >= 0x8000) {
                        em_char_set(em, 5, 0, 0);
                    } else {
                        em_char_set(em, 4, 0, 0);
                    }
                }
            } else if ((u16)(d + spd) < spd * 2) {
                em->ang[1] = w->tgt_ang;
            } else if (d < 0x8000) {
                em->ang[1] = (u16)(ang + spd);
            } else {
                em->ang[1] = (u16)(ang - spd);
            }
        }
        break;
    }
}

static void em_mov01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 2, 6, 0);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 200.0f || --em->work08 <= 0) {
            em04_next_act_set(em);
        } else {
            em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

static void em_mov02(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 3, 6, 0);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em04_next_act_set(em);
        } else {
            em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

static void em_mov03(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 8, 6, 0);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 200.0f || --em->work08 <= 0) {
            em04_next_act_set(em);
        } else {
            em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

static void em_mov04(EMW *em) {
    u32 a;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 2, 0, 0);
        a = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        em->horm_ang = a;
        em->ang[1] = a;
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 200.0f || --em->work08 <= 0) {
            em_cmd_reset(em);
            em04_next_act_set(em);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

static void em_mov05(EMW *em) {
    EM04W *w = (EM04W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((u16)ran_suu(1) & 0x7F) + 120;
        em_char_set(em, 2, 6, 0);
        break;
    case 1:
        if (em->work08 > 0) {
            em->work08--;
        }
        if (em->x8C3 == 0 && em->work08 <= 0) {
            em04_next_act_set(em);
        } else {
            em->horm_ang = (u16)Em_Calc_angY(em->pos, w->home);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x80);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

void em_move01_0058CBD0(EMW *em) {
    switch (em->x15) {
    case 0: em_mov00(em, 0); break;
    case 1: em_mov01(em); break;
    case 2: em_mov02(em); break;
    case 3: em_mov03(em); break;
    case 4: em_mov04(em); break;
    case 5: em_mov05(em); break;
    case 6: em_mov00(em, 1); break;
    }
}

static void em_atk00(EMW *em, int kind) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 15, 10, 0);
        switch (kind) {
        case 0: shell02_set(em, 3); break;
        case 1: shell02_set(em, 8); break;
        case 2: shell02_set(em, 9); break;
        case 3: shell02_set(em, 10); break;
        case 4: shell02_set(em, 14); break;
        case 5: shell02_set(em, 15); break;
        }
        ang[0] = 0;
        ang[1] = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        ang[2] = 0;
        cpRotMatrix(ang, mat);
        in[0] = 0.0f;
        in[1] = 0.0f;
        in[2] = 100.0f;
        flvecApplyMat33(out, in, &mat);
        em->tgt_pos[0] += out[0];
        em->tgt_pos[2] += out[2];
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 16, 10, 0);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
            if (em->work08 > 15) {
                s8 n = em->x617;

                if (n == -1) {
                    em->work08 = 15;
                } else if (em->stg != ((PLW *)player_work)[n].stg) {
                    em->work08 = 15;
                }
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

void em_move03_0058CF10(EMW *em) {
    switch (em->x15) {
    case 0: em_atk00(em, 0); break;
    case 1: em_atk00(em, 1); break;
    case 2: em_atk00(em, 2); break;
    case 3: em_atk00(em, 3); break;
    case 4: em_atk00(em, 4); break;
    case 5: em_atk00(em, 5); break;
    }
}

void em_dm00_0058CFB0(EMW *em) {
    s32 a;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        a = (u16)(em->dm_ang - em->ang[1]);
        if (a > 0x6000 && a < 0xA000) {
            em_char_set(em, 20, 0, 0);
        } else if (a < 0x8000) {
            em_char_set(em, 60, 0, 0);
        } else {
            em_char_set(em, 61, 0, 0);
        }
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

void em_dm01_0058D0A0(EMW *em) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        ang[0] = (u16)(em->dm_ang - em->ang[1]);
        if (ang[0] < 0x8000) {
            em_char_set(em, 62, 0, 0);
        } else {
            em_char_set(em, 67, 0, 0);
        }
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check(em, 0, 10.0f)) {
            em->x05++;
            ang[0] = 0;
            ang[1] = em->dm_ang + 0x8000;
            ang[2] = 0;
            cpRotMatrix(ang, mat);
            in[0] = 0.0f;
            in[1] = 8.0f;
            in[2] = -21.0f;
            flvecApplyMat33(out, in, &mat);
            em_rate_clear_g(em);
            em->rate_x = out[0];
            em->adj_y = out[1];
            em->adj_z = out[2];
            em->x3C0[1] = -1.09f;
            em->x3C0[2] = 0.28f;
            em->x388 = 2;
        }
        break;
    case 2:
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (rate_add_g2(em)) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 63, 6, 0);
            em->work08 = 150;
        }
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 66, 6, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

void em_dm02_0058D2E0(EMW *em) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        ang[0] = (u16)(em->dm_ang - em->ang[1]);
        if (ang[0] < 0x8000) {
            em_char_set(em, 62, 0, 10);
        } else {
            em_char_set(em, 67, 0, 10);
        }
        ang[0] = 0;
        ang[1] = em->dm_ang + 0x8000;
        ang[2] = 0;
        cpRotMatrix(ang, mat);
        in[0] = 0.0f;
        in[1] = 8.0f;
        in[2] = -21.0f;
        flvecApplyMat33(out, in, &mat);
        em_rate_clear_g(em);
        em->rate_x = out[0];
        em->adj_y = out[1];
        em->adj_z = out[2];
        em->x3C0[1] = -1.09f;
        em->x3C0[2] = 0.28f;
        em_cmd_reset(em);
        break;
    case 1:
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (rate_add_g2(em)) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 63, 6, 0);
            em->work08 = 150;
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 66, 6, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}
