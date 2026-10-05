/* em_taisei_nm - game.bin 0x00559260-0x0055B054 (whole file, not built;
 * Em_Taisei_Damage_Check and em_eye_dmg_act_set match; Em_Taisei_Ck (2 off) and
 * Em_Dmg_Sys (10 off: register of the hagitori counter, one branch shape) are
 * near-matches): status effects on monsters
 * (poison, sleep, sleep2, paralysis "mahi"): stocking up damage until the
 * tolerance in em->*_tol is reached, then flagging the state in em->taisei
 * (bits 1 sleep2, 2 sleep, 4 poison, 8 paralysis). Meanings are guesses. */
#include "em_sys.h"
#include "game.h"

void Em_Sleep_Flag_Ck2(EMW *em);

u8 Em_Taisei_Damage_Check(EMW *em) {
    u8 ret = 0;
    u8 t;
    u8 u;

    if (em->x7D2 == 0 && em->x3F0 != 6 && em->x3F0 != 9) {
        t = em->taisei & ~4;
        if (t != 0) {
            if (em->x388 == 2) {
                Em_Sleep_Flag_Ck2(em);
                ret = 1;
            } else {
                u = t & 0xFF;
                if (u & 8) {
                    ret = 4;
                } else if (u & 1) {
                    ret = 2;
                } else if (u & 2) {
                    ret = 3;
                }
            }
        }
    }
    return ret;
}

void em01_act_set(EMW *, int, u16, u16);
void em08_act_set(EMW *, int, u16, u16);
void em21_act_set(EMW *, int, u16, u16);

void em02_act_set(EMW *, int, u16, u16);
void em04_act_set();
void em09_act_set();
void em33_act_set(EMW *, int, u16, u16);
void em_act_set(EMW *, int, u16);
void Eft24_set_em(EMW *, int, int, int, f32, f32);
s16 act_ck(EMW *, int, int);
extern s16 em_eye_dmg_timer_tbl[];

#define EYE_EFF(sz, no, sc) \
    em->x88B = 0; \
    em->x94E = em_eye_dmg_timer_tbl[em->kind]; \
    Eft24_set_em(em, 2, no, em->x94E, sz, sc)
#define EYE_OK() (em->x8C3 == 0 && em->x9EA == 0 && !(em->taisei & 8))

/* A monster's reaction when its eyes are hit: per kind, starts the action
 * (mode 4) that plays the eye-damage animation and a flash effect, unless
 * a protected state is active. */
void em_eye_dmg_act_set(EMW *em) {
    switch (em->kind) {
    case 1:
    case 6:
    case 11:
    case 17:
    case 20:
    case 22:
        EYE_EFF(150.0f, 0x22, 3.0f);
        if (EYE_OK()) {
            if (em->x388 == 2) {
                em->x839 = 0;
                em01_act_set(em, 4, 9, 2);
                return;
            }
            em->x839 = 0;
            em01_act_set(em, 4, 0xA, 2);
        }
        break;
    case 14:
    case 26:
        if (em->x388 == 4) {
            break;
        }
        EYE_EFF(150.0f, 0x22, 3.0f);
        if (EYE_OK()) {
            if (em->x388 == 2) {
                em->x839 = 0;
                em01_act_set(em, 4, 9, 2);
                return;
            }
            if (em->mode == 2 || (em->mode == 0 && em->x15 == 0x1C) || (em->mode == 0 && em->x15 == 0x1D) ||
                (em->mode == 0 && em->x15 == 0x21) || (em->mode == 4 && em->x15 == 6) ||
                (em->mode == 4 && em->x15 == 0xD) || (em->mode == 4 && em->x15 == 0xE) ||
                (em->mode == 4 && em->x15 == 0x11) || (em->mode == 4 && em->x15 == 0x12)) {
                break;
            }
            em->x839 = 0;
            em01_act_set(em, 4, 0xA, 2);
            return;
        }
        break;
    case 2:
        EYE_EFF(150.0f, 0x33, 3.0f);
        if (EYE_OK()) {
            if (em->x388 == 2) {
                em->x839 = 0;
                em02_act_set(em, 4, 8, 2);
                return;
            }
            em->x839 = 0;
            em02_act_set(em, 4, 7, 2);
            return;
        }
        break;
    case 9:
    case 23:
        if (EYE_OK() && em->x388 == 0) {
            if (em->mode != 4 && em->mode != 5 && em->mode != 6 && act_ck(em, 4, 2) == 0) {
                em->x839 = 0;
                em->x94E = em_eye_dmg_timer_tbl[em->kind];
                em->work08 = em->x94E;
                Eft24_set_em(em, 2, 0, em->x94E, 90.0f, 1.0f);
                em09_act_set(em, 4, 2, 0);
                return;
            }
        }
        break;
    case 4:
        if (em->x8C3 == 0 && em->x388 == 0) {
            if (em->mode != 4 && em->mode != 5 && em->mode != 6 && act_ck(em, 4, 4) == 0) {
                em->x839 = 0;
                em->x94E = em_eye_dmg_timer_tbl[em->kind];
                em->work08 = em->x94E;
                Eft24_set_em(em, 2, 0, em->x94E, 100.0f, 1.0f);
                em04_act_set(em, 4, 4, 0);
                return;
            }
        }
        break;
    case 13:
    case 16:
    case 27:
    case 28:
    case 30:
    case 31:
        EYE_EFF(50.0f, 0xE, 1.0f);
        if (EYE_OK()) {
            if (em->x388 == 2) {
                em->x839 = 0;
                em01_act_set(em, 4, 1, 2);
                return;
            }
            em->x839 = 0;
            em01_act_set(em, 4, 5, 2);
            return;
        }
        break;
    case 12:
    case 25:
        EYE_EFF(50.0f, 0xF, 1.5f);
        if (EYE_OK()) {
            em->x839 = 0;
            em_act_set(em, 4, 5);
            return;
        }
        break;
    case 21:
        EYE_EFF(150.0f, 0x22, 3.0f);
        if (EYE_OK()) {
            switch (em->x388) {
            case 0:
                em->x839 = 0;
                em21_act_set(em, 4, 0xA, 2);
                return;
            case 1:
                em->x839 = 0;
                em21_act_set(em, 4, 4, 2);
                return;
            case 4:
                em->x839 = 0;
                em21_act_set(em, 4, 0x11, 2);
                return;
            case 2:
                em->x839 = 0;
                em21_act_set(em, 4, 9, 2);
                return;
            default:
                em->x839 = 0;
                em21_act_set(em, 4, 0xA, 2);
                return;
            }
        }
        break;
    case 8:
    case 34:
        EYE_EFF(150.0f, 0x22, 3.0f);
        if (EYE_OK()) {
            switch (em->x388) {
            case 0:
                em->x839 = 0;
                em08_act_set(em, 4, 0xA, 2);
                return;
            case 1:
                em->x839 = 0;
                em08_act_set(em, 4, 4, 2);
                return;
            case 4:
                em->x839 = 0;
                em08_act_set(em, 4, 0x11, 2);
                return;
            case 2:
                em->x839 = 0;
                em08_act_set(em, 4, 9, 2);
                return;
            default:
                em->x839 = 0;
                em08_act_set(em, 4, 0xA, 2);
                return;
            }
        }
        break;
    case 3:
        EYE_EFF(50.0f, 0x17, 1.5f);
        if (EYE_OK()) {
            em->x839 = 0;
            em_act_set(em, 4, 4);
            return;
        }
        break;
    case 19:
    case 24:
        EYE_EFF(20.0f, 8, 1.0f);
        if (EYE_OK()) {
            switch (em->x388) {
            case 0:
                em_act_set(em, 4, 5);
                return;
            case 2:
                em_act_set(em, 4, 4);
                return;
            }
        }
        break;
    case 33:
        EYE_EFF(50.0f, 0x17, 1.5f);
        if (EYE_OK()) {
            em->x839 = 0;
            em33_act_set(em, 4, 4, 2);
        }
        break;
    }
}


void em_eye_dmg_reset_act_set(EMW *em) {
    switch (em->kind) {
    case 1:
    case 11:
        em01_act_set(em, 0, 25, 4);
        break;
    case 8:
    case 34:
        em08_act_set(em, 0, 7, 4);
        break;
    case 21:
        if (em->x388 == 0) {
            em21_act_set(em, 0, 7, 4);
        }
        break;
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
    case 10:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
        break;
    }
}

void Eft06_set2(f32, EMW *, int, int, f32 *);
void Em_Taisei_Ck(EMW *em) {
    EM_TAISEI_DATA *po = em_poison_data_tbl[em->kind];
    EM_TAISEI_DATA *ma = em_mahi_data_tbl[em->kind];
    EM_TAISEI_DATA *sl = em_sleep_data_tbl[em->kind];
    EM_TAISEI_DATA *s2 = em_sleep2_data_tbl[em->kind];

    if (em->taisei & 4) {
        if (--em->x7C0 > 0) {
            switch (em->kind) {
            case 1:
            case 6:
            case 8:
            case 11:
            case 14:
            case 15:
            case 17:
            case 20:
            case 21:
            case 22:
            case 26:
            case 34:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 100.0f;
                    o[2] = 0.0f;
                    Eft06_set2(2.0f, em, 3, 34, o);
                }
                break;
            case 2:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 200.0f;
                    o[2] = 0.0f;
                    Eft06_set2(2.0f, em, 3, 51, o);
                }
                break;
            case 3:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 50.0f;
                    o[2] = 0.0f;
                    Eft06_set2(1.0f, em, 3, 23, o);
                }
                break;
            case 12:
            case 25:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 50.0f;
                    o[2] = 0.0f;
                    Eft06_set2(1.2f, em, 3, 15, o);
                }
                break;
            case 9:
            case 23:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 20.0f;
                    o[2] = 0.0f;
                    Eft06_set2(1.2f, em, 3, 10, o);
                }
                break;
            case 13:
            case 16:
            case 27:
            case 28:
            case 30:
            case 31:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 20.0f;
                    o[2] = 0.0f;
                    Eft06_set2(1.2f, em, 3, 14, o);
                }
                break;
            case 4:
            case 5:
            case 32:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 10.0f;
                    o[2] = 0.0f;
                    Eft06_set2(1.0f, em, 3, 11, o);
                }
                break;
            case 19:
            case 24:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 0.0f;
                    o[2] = 0.0f;
                    Eft06_set2(0.5f, em, 3, 8, o);
                }
                break;
            case 33:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 50.0f;
                    o[2] = 0.0f;
                    Eft06_set2(1.0f, em, 3, 23, o);
                }
                break;
            default:
                if (*(u16 *)&game_w.x1E % 60 == 0) {
                    f32 o[3];

                    o[0] = 0.0f;
                    o[1] = 50.0f;
                    o[2] = 0.0f;
                    Eft06_set2(1.0f, em, 3, 0, o);
                }
                break;
            }
            if (--em->x7BE <= 0) {
                em->x7BE = po->tick;
                em->x302 -= po->time;
                if (em->x302 <= 0) {
                    if (em->x8BB > 0 || em->x9E2 != 0) {
                        em->x302 = 1;
                    }
                }
            }
        } else {
            em->x7C0 = 0;
            em->x7BA = 0;
            em->x7BE = 0;
            em->taisei &= 0xFB;
            if (em->x7F0 < 3) {
                em->x7F0++;
                em->poison_tol += po->add2;
            }
        }
    } else if (em->x7BA != 0) {
        if (--em->x7BE <= 0) {
            em->x7BE = po->decay;
            em->x7BA -= po->dec;
            if (em->x7BA <= 0) {
                em->x7BA = 0;
                em->x7BE = 0;
            }
        }
    }
    if (!(em->taisei & 8) && em->x7C4 != 0) {
        if (--em->x7C8 <= 0) {
            em->x7C8 = ma->decay;
            em->x7C4 -= ma->dec;
            if (em->x7C4 <= 0) {
                em->x7C4 = 0;
                em->x7C8 = 0;
            }
        }
    }
    if (!(em->taisei & 1) && em->x7CC != 0) {
        if (--em->x7D0 <= 0) {
            em->x7D0 = s2->decay;
            em->x7CC -= s2->dec;
            if (em->x7CC <= 0) {
                em->x7CC = 0;
                em->x7D0 = 0;
            }
        }
    }
    if (!(em->taisei & 2) && em->x7B2 != 0) {
        if (--em->x7B6 <= 0) {
            em->x7B6 = sl->decay;
            em->x7B2 -= sl->dec;
            if (em->x7B2 <= 0) {
                em->x7B2 = 0;
                em->x7B6 = 0;
            }
        }
    }
}

/* em_special_dmg_tbl[kind]: a weak spot that, hit in a given state, keeps
 * its damage in em->x776. */
typedef struct EM_SPDMG {
    u8 idx;             /* 0x0 damage slot */
    u8 mask;            /* 0x1 */
    u8 lv;              /* 0x2 0: always */
    u8 state;           /* 0x3 em->x388 */
} EM_SPDMG;

extern EM_SPDMG *em_special_dmg_tbl[];

void em_ikari_add(EMW *, s16);

void poison_stock_set(EMW *em, int n) {
    EM_TAISEI_DATA *d = em_poison_data_tbl[em->kind];

    if (n != 0 && em->poison_tol > 0) {
        em->x7BA = em->x7BA + n;
        em->x7BE = d->decay;
        if (em->x7BA >= em->poison_tol) {
            em->taisei |= 4;
            em->x7C0 = d->add;
            em->x7BE = 0;
        }
    }
}

void mahi_stock_set(EMW *em, int n) {
    EM_TAISEI_DATA *d = em_mahi_data_tbl[em->kind];

    if (n != 0 && em->mahi_tol > 0) {
        if (!(em->taisei & 8)) {
            em->x7C4 = em->x7C4 + n;
            em->x7C8 = d->decay;
        }
        if (em->x7C4 >= em->mahi_tol) {
            em->taisei |= 8;
        }
    }
}

void sleep_stock_set(EMW *em, int n) {
    EM_TAISEI_DATA *d = em_sleep_data_tbl[em->kind];

    if (n != 0 && em->sleep_tol > 0) {
        if (!(em->taisei & 2)) {
            em->x7B2 = em->x7B2 + n;
            em->x7B6 = d->decay;
        }
        if (em->x7B2 >= em->sleep_tol) {
            em->taisei |= 2;
        }
    }
}

void sleep2_stock_set(EMW *em, int n) {
    EM_TAISEI_DATA *d = em_sleep2_data_tbl[em->kind];

    if (n != 0 && em->sleep2_tol > 0) {
        if (!(em->taisei & 1)) {
            em->x7CC = em->x7CC + n;
            em->x7D0 = d->decay;
        }
        if (em->x7CC >= em->sleep2_tol) {
            em->taisei |= 1;
        }
    }
}

void em_mahi_dmg_timer_set(EMW *em) {
    em->work08 = em_mahi_data_tbl[em->kind]->time;
    em->x839 = 0;
}

void em_sleep_dmg_timer_set(EMW *em) {
    em->work08 = em_sleep_data_tbl[em->kind]->time;
    em->x839 = 0;
}

void em_sleep2_dmg_timer_set(EMW *em) {
    em->work08 = em_sleep2_data_tbl[em->kind]->time;
    em->x839 = 0;
}

void Em_Damage_Stock(EMW *em, s16 *dmg, s16 *heal) {
    u32 i;
    s16 max = 0;
    u8 best = 0;
    EM_SPDMG *sp = em_special_dmg_tbl[em->kind];
    s16 d;

    for (i = 0; i < 8; i++) {
        d = em->dmg[i] * em->x7DC;
        if (em->dmg[i] > 0 && d == 0) {
            d = 1;
        }
        if (d < 0) {
            *heal -= d;
        } else {
            *dmg += d;
            if (em->dmg[i] > max) {
                max = em->dmg[i];
                best = i;
            }
        }
    }
    if (sp != 0) {
        if ((em->x788[sp->idx] & sp->mask) && em->x388 == sp->state &&
            (sp->lv == 0 || em->x958 < (s8)sp->lv)) {
            em->x776 = em->dmg[sp->idx];
            if (em->x776 < 0) {
                em->x776 = 0;
            }
        } else {
            em->x776 = 0;
        }
    }
    if (em->x8B6 == 0 && *dmg > 0) {
        em_ikari_add(em, *dmg);
    }
    em->x38E = best;
    if (em->x8BB <= 0) {
        sleep2_stock_set(em, em->x7CE);
        sleep_stock_set(em, em->x7B4);
        poison_stock_set(em, em->x7BC);
        mahi_stock_set(em, em->x7C6);
    }
}

void Em_Mode_Chg(EMW *em, int mode, int timer);
void em_dur_set(EMW *em, int part);
void em_cmd_reset(EMW *em);
void pl_flag_clr(EMW *em, u32 bits);
void em_hinshi_end2(EMW *em);
void em_hagitori_lv_up(EMW *em, u8 bits);
void em_search_data_set(EMW *em, u8 no);
void em_range_set(EMW *em, s8 no);
extern s16 em_atk_mode_timer_tbl[];

#define HAGI_HP(i) (em->hagi[i].hp)
#define HAGI_CNT(i) (em->hagi[i].cnt)
#define DMG(i) (em->dmg[i])

/* Per-frame damage handling of a monster: returns what happened this frame
 * (2 dead without a hit, 1 killed, 0 nothing, 3/4 hagitori states, 5-8 status
 * effects, 9 eye damage, 0xA-0x10 reaction to a hit). *flag receives x888. */
u8 Em_Dmg_Sys(EMW *em, u8 *flag) {
    s16 stock = 0;
    s16 heal = 0;
    u8 hit = 0;
    u8 r;
    int s;
    s8 i;

    *flag = em->x888;
    if (em->mode == 5) {
        return 0;
    }
    em->x87D = 0;
    if (em->x302 <= 0 && em->dm_flag == 0) {
        em->x839 = 0;
        em->x7EE = 0;
        return 2;
    }
    if (em->dm_flag != 0) {
        em->x917 &= 0x8D;
        Em_Damage_Stock(em, &stock, &heal);
        em->x302 = heal + (em->x302 - stock);
        if (em->x302 > em->x792) {
            em->x302 = em->x792;
        }
        if (em->x8BB != 0) {
            if (em->x302 <= 0) {
                em->x302 = 1;
            }
            Em_Mode_Chg(em, 1, em_atk_mode_timer_tbl[em->kind]);
            if (em->kind == 2 && (em->x3F0 == 9 || em->x3F0 == 6)) {
                goto hagi;
            }
            return 0;
        }
        if (em->kind == 7 && em->stg != 0xC && em->x302 < 0x3E8) {
            em->x302 = 0x3E8;
        }
        if (em->x302 <= 0) {
            em->x839 = 0;
            if (em->x9E2 != 0) {
                if (*(u8 *)0x3F3404 == em->stg) {
                    em->x302 = 0;
                    em->x7EE = 0;
                    return 1;
                }
                em->x302 = 1;
            } else {
                em->x302 = 0;
                em->x7EE = 0;
                return 1;
            }
        }
        Em_Mode_Chg(em, 1, em_atk_mode_timer_tbl[em->kind]);
hagi:
        switch (em->x953) {
        case 1:
            for (i = 0; i < 8; i++) {
                s16 d = (f32)DMG(i) * em->x7DC;
                if (DMG(i) > 0 && d == 0) {
                    d = 1;
                }
                HAGI_HP(0) -= d;
                if (HAGI_HP(0) <= 0) {
                    u8 m;
                    hit = 1;
                    em_dur_set(em, 0);
                    m = 1 << i;
                    em->x87D |= m;
                    em->x87E |= m;
                    HAGI_CNT(0)++;
                    if (HAGI_CNT(0) >= 0x63) {
                        HAGI_CNT(0) = 0x63;
                    }
                }
            }
            break;
        case 8:
            for (i = 0; i < 8; i++) {
                s16 d = (f32)DMG(i) * em->x7DC;
                if (DMG(i) > 0 && d == 0) {
                    d = 1;
                }
                HAGI_HP(i) -= d;
                if (HAGI_HP(i) <= 0) {
                    u8 m;
                    HAGI_CNT(i)++;
                    m = 1 << i;
                    hit = 1;
                    em->x87D |= m;
                    em->x87E |= m;
                    em_dur_set(em, i);
                    if (HAGI_CNT(i) >= 0x63) {
                        HAGI_CNT(i) = 0x63;
                    }
                }
            }
            break;
        case 9:
            for (i = 0; i < 9; i++) {
                s16 d = (f32)DMG(i) * em->x7DC;
                if (DMG(i) > 0 && d == 0) {
                    d = 1;
                }
                HAGI_HP(i) -= d;
                if (HAGI_HP(i) <= 0) {
                    hit = 1;
                    HAGI_CNT(i)++;
                    if (i == 8) {
                        em->x957 = 1;
                    } else {
                        u8 m = 1 << i;
                        em->x87D |= m;
                        em->x87E |= m;
                    }
                    em_dur_set(em, i);
                    if (HAGI_CNT(i) >= 0x63) {
                        HAGI_CNT(i) = 0x63;
                    }
                }
            }
            break;
        }
        em_hagitori_lv_up(em, em->x87D);
        if (em->x9EA == 0) {
            if (em->x3F0 == 9) {
                r = 3;
                goto fin;
            }
            if (em->x3F0 == 6) {
                r = 4;
                goto fin;
            }
        }
    }
    s = Em_Taisei_Damage_Check(em) & 0xFF;
    if (s != 0 || em->x8BC != 0) {
        Em_Mode_Chg(em, 1, em_atk_mode_timer_tbl[em->kind]);
    }
    if ((u8)s == 1) {
        r = 5;
    } else if ((u8)s == 4) {
        r = 6;
    } else if ((u8)s == 2) {
        r = 7;
    } else if ((u8)s == 3) {
        r = 8;
    } else {
        if (em->x8BC != 0) {
            em->x8BC = 0;
            em_eye_dmg_act_set(em);
            r = 9;
            goto fin;
        }
        if (em->dm_flag != 0) {
            if ((u8)em->x762 == 2) {
                r = 0xA;
                goto fin;
            }
            em->x762 = 0;
            if (em->x957 != 0) {
                em->x957 = 0;
                r = 0xB;
                goto fin;
            }
            if (em->mode == 4) {
                return 0;
            }
            if (em->x3F0 == 5) {
                if (hit) {
                    r = 0x10;
                } else {
                    r = 0xF;
                }
                goto fin;
            }
            if (hit) {
                r = 0xC;
                goto fin;
            }
            r = 0xD;
            if ((u8)em->x762 != 3) {
                if (em->x9EA != 0) {
                    return 0;
                }
                r = 0xE;
                em_search_data_set(em, 0);
                em_range_set(em, 0);
                if (*flag == 0 || (u8)em->x762 != 0) {
                    em_cmd_reset(em);
                    em->x839 = 1;
                }
            }
            goto fin;
        }
        return 0;
    }
fin:
    if (r != 0xE) {
        em->x839 = 0;
        em_cmd_reset(em);
        pl_flag_clr(em, 0x20000);
    }
    em_hinshi_end2(em);
    return r;
}
