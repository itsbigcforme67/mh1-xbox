/* em03 - game.bin 0x005873D0-0x0058ABxx. Per-monster AI for monster kind 3
 * (setup, main loop with the damage system, action steps, move states,
 * damage and death, sound/effect script). Names of the steps follow the
 * split (em_act00, em_move00...). Field meanings are mostly guesses. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number (0: free play) */
} QUEST_W;

extern QUEST_W quest_w;
extern GAME_W game_w;
extern EMW em_work[];
extern u8 em_boss_tbl[];

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void em_dur_init(EMW *);
u32 ran_suu(int);
int em_mode_timer_sub(EMW *);
void em_cmd_reset(EMW *);
int Pl_stg_ck_tw(EMW *, PLW *);
void em_escape_mind_set(EMW *, u8, u8);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void Em_Sleep_End(EMW *);
u8 Em_Smoke_Ck(EMW *);
void em_cmd_ck(EMW *);
void em03_move_sub(EMW *em);
void em03_act_set();
void em03_to_normal(EMW *em);
void em03_char_set(EMW *em, int no, int a, int b);

void em03_init(EMW *em) {
    if (quest_w.x08 == 0) {
        switch (game_w.stage) {
        case 0xF:
            em->pos[0] = 9500.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 9200.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        case 0x10:
            em->pos[0] = 6000.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 7000.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        case 0x13:
            em->pos[0] = 11000.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        case 0x25:
            em->pos[0] = 9500.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        default:
            em->pos[0] = 5000.0f - 80.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 5000.0f - 80.0f * (f32)(u32)em->x13;
            break;
        }
    }
    em->x88B = 1;
    em->x388 = 0;
    em->act_spd = 1.0f;
    if (em->kind == 3) {
        em->x8C3 = 0;
        if (quest_w.x08 == 0) {
            em->x11 = (u16)ran_suu(1) & 1;
            em->x11 = 0;
        } else {
            em->x11 = em->type;
        }
        em->x792 = em->x302 = em_hp_vital_set(em, 0x30);
        if (em->x11 != 0) {
            em->x792 = em->x302 = em_hp_vital_set(em, 0x30);
            em->scale[0] *= 0.85f;
            em->scale[1] *= 0.85f;
            em->scale[2] *= 0.85f;
        }
    }
    em03_char_set(em, 1, 0, 0);
    em_dur_init(em);
    em_act_set(em, 0, 1);
}

#define EM03_HAGI0(em) (*(s16 *)&(em)->hagi[0][0])

void em03_main(EMW *em) {
    u8 dmg[4];
    u8 b = 0;
    u8 c = 0;
    u8 a = 0;
    s16 i;
    EMW *p = em_work;
    s8 pn;

    if (em_mode_timer_sub(em)) {
        em_cmd_reset(em);
        for (pn = 0; pn < game_w.pl_num; pn++) {
            if (em->stg == ((PLW *)player_work)[pn].stg) {
                break;
            }
            if (pn == game_w.pl_num - 1) {
                em->x839 = 1;
            }
        }
    }
    if (em->x889 != 1 && em->mode != 6) {
        for (i = 0; i < 20; i++, p++) {
            if (p->be_flag && p->x01 && (u8)Pl_stg_ck_tw(em, (PLW *)p)) {
                if (em_boss_tbl[p->kind]) {
                    if (p->x888 == 1) {
                        b = 1;
                        break;
                    }
                    if (p->x888 == 0) {
                        a = 1;
                    }
                }
                if (p->type == 0 && p->mode == 6) {
                    c = 1;
                }
            }
        }
        if ((b & 0xFF) || ((a & 0xFF) && (c & 0xFF))) {
            em_escape_mind_set(em, 1, 0x90);
            em_cmd_reset(em);
            em->x839 = 1;
        }
    }
    switch (Em_Dmg_Sys(em, dmg)) {
    case 0:
    case 3:
    case 4:
    case 5:
    case 7:
    case 9:
    case 11:
        break;
    case 1:
    case 2:
        em_act_set(em, 5, 0);
        break;
    case 6:
        em_mahi_dmg_timer_set(em);
        em03_act_set(em, 4, 3, 2);
        break;
    case 8:
        em_sleep_dmg_timer_set(em);
        em03_act_set(em, 0, 8, 2);
        break;
    case 10:
        em->x88B = 1;
        Em_Sleep_End(em);
        em03_act_set(em, 4, 2, 2);
        break;
    case 12:
    case 13: {
        s32 d = (u16)em->dm_ang - em->ang[1];

        if ((u16)(d - 0x4000) < 0x8000 && EM03_HAGI0(em) > 0 && em->x388 == 0) {
            if ((u16)d < 0x8000) {
                em_act_set(em, 4, 0);
            } else {
                em_act_set(em, 4, 1);
            }
        } else {
            em_act_set(em, 4, 2);
        }
        break;
    }
    case 14:
        if (em->x388 == 0) {
            s32 d = (u16)(em->dm_ang - em->ang[1]);

            em->x839 = 0;
            if (d < 0x8000) {
                em_act_set(em, 4, 0);
            } else {
                em_act_set(em, 4, 1);
            }
        }
        break;
    }
    if (em->x889 != 1 && Em_Smoke_Ck(em) == 1) {
        em_escape_mind_set(em, 2, 0x90);
        em_cmd_reset(em);
        em->x839 = 1;
    }
    if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em03_move_sub(em);
    if (em->x6FF != 0) {
        em03_move_sub(em);
        em->x6FF = 0;
    }
}
