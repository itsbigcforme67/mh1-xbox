/* em04 (part 2) - game.bin 0x0058D600-0x0058E4F8. Monster kind 4: damage
 * reaction em_dm04, move state 4 (damage dispatch), death sequences
 * (em_die00 / em_die01: fall, hagi/part loss timer; em_die_rev: revival or
 * escape), the demo state, em04_main (damage system, quest 0x87 special
 * case) and em04_main_sub (mode dispatch). */
#include "em04.h"

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number */
} QUEST_W;
extern QUEST_W quest_w;
extern GAME_W game_w;

static void em_dm04(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 71, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 72, 0, 0);
            em->ang[1] = em->ang[1] + 0x8000;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_move04(EMW *em) {
    switch (em->x15) {
    case 0: em_dm00_0058CFB0(em); break;
    case 1: em_dm01_0058D0A0(em); break;
    case 2: em_dm02_0058D2E0(em); break;
    case 3: em_dm03_0058D4E0(em); break;
    case 4: em_dm04(em); break;
    }
}

static void em_die00(EMW *em) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    em->x40C = 10;
    em->x40E = 10;
    Em_Mode_Chg(em, 0, 0);
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
        Quest_enemy_die(em);
        break;
    case 1:
        if (em_frame_check(em, 10.0f, 0)) {
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
            em_rate_clear(em);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 64, 0, 0);
            em->work08 = 60;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 4:
        Em_hagi_point_cnt_ck(em);
        if (em->x194 == 0) {
            em->x05++;
            if (em->kind == 4) {
                em->work08 = 2400;
            } else {
                em->work08 = 900;
            }
        }
        break;
    case 5:
        if (--em->work08 <= 0 || Em_hagi_point_cnt_ck(em) <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 6:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 2);
        }
        break;
    }
}

static void em_die01(EMW *em) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    em->x40C = 10;
    em->x40E = 10;
    Em_Mode_Chg(em, 0, 0);
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
        Quest_enemy_die(em);
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
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 64, 0, 0);
            em->work08 = 60;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        Em_hagi_point_cnt_ck(em);
        if (em->x194 == 0) {
            em->x05++;
            if (em->kind == 4) {
                em->work08 = 2400;
            } else {
                em->work08 = 900;
            }
        }
        break;
    case 4:
        if (--em->work08 <= 0 || Em_hagi_point_cnt_ck(em) <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 5:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 2);
        }
        break;
    }
}

static void em_die_rev(EMW *em) {
    switch (em->x05) {
    case 0:
        if (Quest_enemy_revival_ck(em) == 1) {
            em->x05++;
        } else {
            em->x04++;
        }
        break;
    case 1:
        em_status_init(em);
        em04_init(em);
        Quest_enemy_revival_set(em);
        if (em->kind == 5) {
            switch (em->stg) {
            case 0x16:
            case 0x2A:
            case 0x2C:
            case 0x2E:
            case 0x45:
                Quest_enemy_escape(em);
                em->x04++;
                em->x01 = 0;
                break;
            default:
                em04_act_set(em, 1, 4, 0);
                break;
            }
        } else {
            switch (em->stg) {
            case 1:
            case 0x16:
            case 0x17:
            case 0x20:
            case 0x21:
            case 0x23:
            case 0x2A:
            case 0x2C:
            case 0x2E:
            case 0x45:
                Quest_enemy_escape(em);
                em->x04++;
                em->x01 = 0;
                break;
            }
        }
        break;
    }
}

static void em_move05(EMW *em) {
    switch (em->x15) {
    case 0:
        em_die00(em);
        break;
    case 1:
        em_die01(em);
        break;
    case 2:
        em_die_rev(em);
        break;
    }
}

static void em_demo00(EMW *em) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 1, 0, 0);
        em->x01 = 0;
        em->x839 = 0;
        break;
    case 1:
        if (Event_flag_ck(0xF) == 1) {
            em->x05++;
            em->x01 = 1;
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_move06(EMW *em) {
    switch (em->x15) {
    case 0:
        em_demo00(em);
        break;
    }
}

void em04_main(EMW *em) {
    u8 dmg[4];
    EM04W *w = (EM04W *)em->ex;

    if (w->x0C != 0) {
        w->x0C--;
    }
    switch (Em_Dmg_Sys(em, dmg)) {
    case 0:
    case 3:
    case 4:
    case 9:
    case 11:
    case 14:
        break;
    case 1:
    case 2:
        if (em->x388 == 2) {
            em_act_set(em, 5, 0);
        } else {
            em_act_set(em, 5, 1);
        }
        break;
    case 5:
        if ((em->mode == 4 && em->x15 == 1) || (em->mode == 4 && em->x15 == 2)) {
            break;
        }
        em_act_set(em, 4, 2);
        break;
    case 6:
        if ((em->mode == 4 && em->x15 == 1) || (em->mode == 4 && em->x15 == 2)) {
            break;
        }
        em_mahi_dmg_timer_set(em);
        em04_act_set(em, 4, 3, 0);
        break;
    case 7:
        em_act_set(em, 4, 2);
        break;
    case 8:
        if ((em->mode == 4 && em->x15 == 1) || (em->mode == 4 && em->x15 == 2)) {
            break;
        }
        em_sleep_dmg_timer_set(em);
        em04_act_set(em, 0, 10, 0);
        break;
    case 10:
        em->x88B = 1;
        Em_Sleep_End(em);
        if (em->x388 == 2) {
            em_act_set(em, 4, 2);
        } else {
            em04_act_set(em, 4, 1, 0);
        }
        break;
    case 12:
        if (em->x388 == 2) {
            em_act_set(em, 4, 2);
        } else if ((s16)act_ck(em, 4, 0)) {
            em_act_set(em, 4, 1);
        } else {
            em_act_set(em, 4, 0);
        }
        break;
    case 13:
        if (em->x388 == 2) {
            em_act_set(em, 4, 2);
        } else {
            em_act_set(em, 4, 1);
        }
        em_dur_set(em, 0);
        break;
    }
    if (quest_w.x08 == 0x87 && em->kind == 5 && em->stg == 0x2A && Event_flag_ck(0xF) == 0) {
        if (game_w.info_stop == 1 && em->mode != 6) {
            em04_act_set(em, 6, 0, 1);
        }
    } else {
        switch (em->x734) {
        case 3:
            if (em->x839 != 0) {
                em_cmd_ck(em);
                em->x839 = 0;
            }
            break;
        }
    }
    em04_main_sub(em);
    if (em->x6FF != 0) {
        em04_main_sub(em);
        em->x6FF = 0;
    }
}

void em04_main_sub(EMW *em) {
    switch (em->mode) {
    case 0: em_move00_0058C350(em); break;
    case 1: em_move01_0058CBD0(em); break;
    case 2: em_move00_0058C350(em); break;
    case 3: em_move03_0058CF10(em); break;
    case 4: em_move04(em); break;
    case 5: em_move05(em); break;
    case 6: em_move06(em); break;
    case 7: em_move04(em); break;
    }
}

void move_default_0058E4F0(EMW *em) {
}
