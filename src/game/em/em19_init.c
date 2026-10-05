/* em19_init - game.bin 0x005E7920-0x005E805C: monster 19 setup (spawn
 * position from stage_start_pos, hit points and scale by size bit) and
 * em19_act_set (sets the target distance / work08 timer for move 0/1,
 * fly 5/8/9 and attack 3 before em_act_set). Meanings are guesses. */
#include "em.h"
#include "game.h"

/* quest_w (0x3C7440): only the quest number is used here. */
typedef struct QUEST_W {
    u8 _pad00[8];
    s16 no;             /* 0x08 current quest (0: free play?) */
} QUEST_W;

/* Part of the per-monster work at EMW+0x444 set up by the init code. */
typedef struct EM_INITW {
    u8 _pad00[0xC];
    s16 x0C;            /* 0x0C */
    s16 x0E;            /* 0x0E */
    u8 _pad10[4];
    f32 home[3];        /* 0x14 home position (x, 0, z) */
    u8 _pad20[4];
    u8 _pad24[0x34 - 0x24];
    f32 dist;           /* 0x34 distance to the target */
    u8 _pad38[3];
    u8 has_tgt;         /* 0x3B */
} EM_INITW;

extern QUEST_W quest_w;

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void em_dur_init(EMW *);


extern f32 stage_start_pos[][3];
extern f32 em19_scale_tbl[2];

f32 CalcDistanceXZ(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void flvecCopy(f32 *, f32 *);
void flvecNormalize(f32 *);
void PointToPoint(f32 *, f32 *, f32 *);
void em_rate_clear(EMW *);

void em19_init(EMW *em) {
    EM_INITW *w = (EM_INITW *)em->ex;

    if (quest_w.no == 0) {
        if (game_w.stage == 15) {
            em->pos[0] = 9500.0f + 150.0f * (int)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 9200.0f + 150.0f * (em->x13 & 7);
        } else {
            em->pos[0] = 150.0f * em->x13 + ((f32 *)stage_start_pos)[game_w.stage * 3];
            em->pos[1] = stage_start_pos[game_w.stage][1];
            em->pos[2] = stage_start_pos[game_w.stage][2];
        }
    }
    em_char_set(em, 1, 0, 0);
    em->x388 = 0;
    em_act_set(em, 0, 1);
    if (em->kind == 19) {
        em->x792 = em->x302 = em_hp_vital_set(em, ((em->type & 1) + 1) * 32);
    } else {
        em->x792 = em->x302 = em_hp_vital_set(em, ((em->type & 1) + 1) * 40);
    }
    em->scale[0] = em19_scale_tbl[em->type & 1];
    em->scale[1] = em19_scale_tbl[em->type & 1];
    em->scale[2] = em19_scale_tbl[em->type & 1];
    em->x839 = 1;
    em->x88B = 1;
    w->x0C = 0;
    w->x0E = 0;
    w->home[0] = em->pos[0];
    w->home[1] = 0.0f;
    w->home[2] = em->pos[2];
    em_dur_init(em);
    em->x8C3 = 0;
    em->x734 = 3;
}

void em19_act_set(EMW *em, int kind, u16 no) {
    EM_INITW *w = (EM_INITW *)em->ex;
    f32 v[3];

    switch ((u16)kind) {
    case 1:
        switch (no) {
        case 0:
        case 1:
            if (em->x827 == 0) {
                if (no == 0) {
                    w->has_tgt = 1;
                    flvecCopy(em->tgt_pos, w->home);
                    w->dist = 600.0f + CalcDistanceXZ(em->pos, em->tgt_pos);
                } else {
                    w->has_tgt = 0;
                }
                em->work08 = (em->x39A % 120 + 90) / 2;
                break;
            }
            w->has_tgt = 1;
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            if (em->x881 == 1 && em->x882 == 0) {
                w->dist -= 50.0f;
                if (w->dist <= 0.0f) {
                    em->x839 = 1;
                    return;
                }
            }
            em->work08 = (int)w->dist / 7;
            break;
        }
        break;
    case 2:
        switch (no) {
        case 5:
            em_rate_clear(em);
            if (em->x827 == 0) {
                w->has_tgt = 0;
                em->work08 = (em->x39A % 120 + 90) / 2;
                break;
            }
            w->has_tgt = 1;
            em->tgt_pos[1] += 20.0f;
            w->dist = flvecCalcDistance(em->pos, em->tgt_pos);
            if (em->x881 == 1 && em->x882 == 0) {
                w->dist -= 5.0f;
                if (w->dist <= 0.0f) {
                    em->x839 = 1;
                    return;
                }
            }
            PointToPoint(v, em->tgt_pos, em->pos);
            flvecNormalize(v);
            em->rate_x = 10.0f * v[0];
            em->adj_y = 10.0f * v[1];
            em->adj_z = 10.0f * v[2];
            em->work08 = (int)w->dist / 7;
            break;
        case 8:
            if (em->x827 == 0) {
                w->has_tgt = 0;
                break;
            }
            w->has_tgt = 1;
            break;
        case 9:
            em_rate_clear(em);
            flvecCopy(em->tgt_pos, w->home);
            em->tgt_pos[1] += 20.0f;
            w->dist = flvecCalcDistance(em->pos, em->tgt_pos);
            em->rate_x = 0.0f;
            em->adj_y = 0.0f;
            em->adj_z = 10.0f;
            break;
        }
        break;
    case 3:
        switch (no) {
        case 3:
            if (em->x827 == 0) {
                w->has_tgt = 0;
                em->work08 = (em->x39A % 120 + 90) / 2;
                break;
            }
            w->has_tgt = 1;
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            if (em->x881 == 1 && em->x882 == 0) {
                w->dist += 100.0f;
                if (w->dist <= 0.0f) {
                    em->x839 = 1;
                    return;
                }
            }
            em->work08 = (int)w->dist / 7;
            break;
        }
        break;
    }
    em_act_set(em, kind, no);
}
