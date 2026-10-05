/* em04 - game.bin 0x0058BA40-0x0058F4A0. Per-monster AI for monster kind 4
 * (action setters, action steps em_act*, move states em_move*, damage and
 * death). Names of the steps follow the split (em_act00, em_move00...).
 * Meanings of most fields are guesses. */
#include "em.h"
#include "game.h"

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
u16 em_act_search(void *);
void target_kind_set(EMW *, f32 *);
void em_action_timer_calc(EMW *, int);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
u16 Em_Calc_angY(f32 *, f32 *);
void em09_dir_calc(s32 *, s32 *, int);
void cpRotMatrix(s32 *, f32 (*)[4]);
u32 ran_suu(int);

extern u8 em04_act_tbl[];
extern f32 em05_rev_set_tbl_st69[][6];
extern f32 em05_rev_set_tbl_st18[][6];

void em04_next_act_set(EMW *em);

void em04_act_set(EMW *em, int kind, u16 no) {
    em->act_spd = 1.0f;
    switch ((u16)kind) {
    case 0:
        switch ((u16)no) {
        case 1:
            if (em->x888 == 1) {
                if (em->kind == 4) {
                    no = 12;
                } else {
                    no = 6;
                }
            }
            break;
        }
        break;
    case 1: {
        f32 (*p)[2];
        switch ((u16)no) {
        case 1:
        case 2:
        case 3:
            em->work08 = 1800;
            target_kind_set(em, em->tgt_pos);
            break;
        case 4:
            em->work08 = 600;
            switch (em->stg) {
            case 0x12:
                p = (f32 (*)[2])em05_rev_set_tbl_st18[em->type];
                break;
            case 0x45:
            default:
                p = (f32 (*)[2])em05_rev_set_tbl_st69[em->type];
                break;
            }
            em->pos[0] = (*p)[0];
            em->pos[1] = (*p)[1];
            p++;
            em->pos[2] = (*p)[0];
            em->tgt_pos[0] = (*p)[1];
            p++;
            em->tgt_pos[1] = (*p)[0];
            em->tgt_pos[2] = (*p)[1];
            break;
        }
        break;
    }
    case 3:
        target_kind_set(em, em->tgt_pos);
        switch ((u16)no) {
        case 0:
            em->work08 = 90;
            em->act_spd = 0.8f;
            break;
        case 1:
            em->work08 = 90;
            em->act_spd = 0.8f;
            break;
        case 2:
            em->work08 = 90;
            em->act_spd = 1.0f;
            break;
        case 3:
            em->work08 = 120;
            em->act_spd = 1.0f;
            break;
        case 4:
            em->work08 = 30;
            em->act_spd = 0.8f;
            break;
        case 5:
            em->work08 = 45;
            em->act_spd = 0.8f;
            break;
        }
        break;
    }
    em_act_set(em, kind, no);
}

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

static void em_move00(EMW *em) {
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
    case 10: em_act10(em); break;
    case 11: em_act07(em); break;
    case 12: em_act08(em); break;
    }
}
