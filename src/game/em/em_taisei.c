/* zzt, run 1: Em_Taisei_Damage_Check .. em_eye_dmg_reset_act_set (game.bin 0x00559260-0x00559DB4). Matching functions of em_taisei_nm.c (that file holds the
 * whole code including the near-matches); see it for the description. */
#include "em_sys.h"
#include "game.h"

void Em_Sleep_Flag_Ck2(EMW *em);

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

void Eft06_set2(f32, EMW *, int, int, f32 *);

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
