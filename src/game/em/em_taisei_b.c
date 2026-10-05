/* em_taisei_b - game.bin 0x0055A440-0x0055A8A4: stocking status damage
 * (poison/paralysis/sleep/sleep2 *_stock_set), state timers and
 * Em_Damage_Stock (sums the per-slot damage of a hit, scaled by
 * em->x7DC). Second matching run of the status-effect file (see
 * em_taisei_nm.c). Meanings are guesses. */
#include "em_sys.h"
#include "game.h"

void Em_Sleep_Flag_Ck2(EMW *em);

u8 Em_Taisei_Damage_Check(EMW *em);

void em01_act_set(EMW *, int, u16, u16);
void em08_act_set(EMW *, int, u16, u16);
void em21_act_set(EMW *, int, u16, u16);

void em_eye_dmg_reset_act_set(EMW *em);

void Eft06_set2(f32, EMW *, int, int, f32 *);
void Em_Taisei_Ck(EMW *em);

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
