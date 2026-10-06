/* em_taisei_c - game.bin 0x00559DC0-0x0055A438: Em_Taisei_Ck (poison/paralysis/sleep resistance timers). Whole file in em_taisei_nm.c. */
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

/* A monster's reaction when its eyes are hit: per kind, starts the action
 * (mode 4) that plays the eye-damage animation and a flash effect, unless
 * a protected state is active. */

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

/* Per-frame damage handling of a monster: returns what happened this frame
 * (2 dead without a hit, 1 killed, 0 nothing, 3/4 hagitori states, 5-8 status
 * effects, 9 eye damage, 0xA-0x10 reaction to a hit). *flag receives x888. */

void Em_Taisei_Ck(EMW *em) {
    EM_TAISEI_DATA *po;
    EM_TAISEI_DATA *ma;
    EM_TAISEI_DATA *sl;
    EM_TAISEI_DATA *s2;

    po = em_poison_data_tbl[em->kind];
    ma = em_mahi_data_tbl[em->kind];
    s2 = em_sleep2_data_tbl[em->kind];
    sl = em_sleep_data_tbl[em->kind];

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
