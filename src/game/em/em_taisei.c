/* em_taisei - game.bin 0x00559D30-0x00559DB4: em_eye_dmg_reset_act_set (ends
 * the eye-damage reaction of monsters 1/11, 8/34 and 21). First matching
 * run of the status-effect file (see em_taisei_nm.c). */
#include "em_sys.h"
#include "game.h"

void Em_Sleep_Flag_Ck2(EMW *em);

u8 Em_Taisei_Damage_Check(EMW *em);

void em01_act_set(EMW *, int, u16, u16);
void em08_act_set(EMW *, int, u16, u16);
void em21_act_set(EMW *, int, u16, u16);

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

void poison_stock_set(EMW *em, int n);

void mahi_stock_set(EMW *em, int n);

void sleep_stock_set(EMW *em, int n);

void sleep2_stock_set(EMW *em, int n);

void em_mahi_dmg_timer_set(EMW *em);

void em_sleep_dmg_timer_set(EMW *em);

void em_sleep2_dmg_timer_set(EMW *em);

void Em_Damage_Stock(EMW *em, s16 *dmg, s16 *heal);

