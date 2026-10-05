/* em29 - game.bin 0x006140B0-0x006147C8. Monster kind 29:
 * a stationary target that only waits, flinches and breaks. em29_init
 * places it (by spawn slot EMW+0x13 when no quest is set: on stage 15 in a
 * grid, elsewhere in a row) and takes hit points, scale and break sound
 * from em29_type_tbl by variant (EMW+0x1B). When it breaks (mode 5) it
 * plays the sound, splashes, and variant 4 also drops shells (Shell09).
 * Names of the action steps follow the split (em_act00, em_move00...). */
#include "em.h"
#include "game.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM29W {
    u8 eff;             /* 0x00 em29_effect_move step */
    u8 _pad01[3];
    s16 no_hp;          /* 0x04 1: no hit points in the table (unbreakable?) */
    u8 _pad06[0x90 - 0x06];
    u8 x90;             /* 0x90 (EMW+0x4D4) */
    u8 _pad91[0xA2 - 0x91];
    u8 part[5];         /* 0xA2 (EMW+0x4E6) one-hot from the type table */
} EM29W;

/* One row of em29_type_tbl (0xC bytes). */
typedef struct EM29_TYPE {
    s16 part;           /* 0x0 index into EM29W.part */
    s16 hp;             /* 0x2 hit points, <= 0: none */
    f32 scale;          /* 0x4 */
    s32 se;             /* 0x8 sound when broken */
} EM29_TYPE;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 non-zero: a quest is running */
} QUEST_W;

extern EM29_TYPE em29_type_tbl[];
extern QUEST_W quest_w;
extern GAME_W game_w;

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, int);
void em_dur_init(EMW *);
void em_dur_set(EMW *, int);
void em_cmd_ck(EMW *);
u8 Em_Dmg_Sys(EMW *, void *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Eft02_set3(EMW *, u16, int, int, f32 *, f32);
void Eft13_set_pos2(f32, EMW *, f32 *, int);
void eft14_set(f32 *, int, f32);
void Shell09_set(f32 *, int, u8);

void em29_to_normal(EMW *em);
void em29_main_sub(EMW *em);

void em29_init(EMW *em) {
    EM29W *w = (EM29W *)em->ex;
    EM29_TYPE *t = &em29_type_tbl[em->type];

    if (quest_w.x08 == 0) {
        if (game_w.stage == 0xF) {
            em->pos[0] = 9500.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 9200.0f + 150.0f * (f32)(em->x13 & 7);
        } else {
            em->pos[0] = 5000.0f + 200.0f * em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 5000.0f;
        }
    }
    em->x88B = 0;
    em->x9E1 = 5;
    em->ex[0x90] = 0;
    em->x8C3 = 0;
    em_char_set(em, 1, 0, 0);
    em->x388 = 2;
    em_act_set(em, 0, 1);
    if (t->hp <= 0) {
        w->no_hp = 1;
        em->x302 = 4000;
        em->x792 = 4000;
    } else {
        w->no_hp = 0;
        em->x792 = em->x302 = t->hp;
    }
    em->scale[0] = em->scale[1] = em->scale[2] = t->scale;
    em->ex[0xA2] = 0;
    em->ex[0xA3] = 0;
    em->ex[0xA4] = 0;
    em->ex[0xA5] = 0;
    em->ex[0xA6] = 0;
    em->ex[0xA2 + t->part] = 1;
    em_dur_init(em);
    em->x734 = 3;
}

void em29_act_set(EMW *em, int a, int b) {
    em_act_set(em, a, b);
}

void em29_to_normal(EMW *em) {
    em->x839 = 1;
}

static void em_act00(EMW *em) {
}

static void em_move00(EMW *em) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0:
        em_act00(em);
        break;
    }
}

static void em_mov00(EMW *em) {
    em29_to_normal(em);
}

static void em_move01(EMW *em) {
    switch (em->x15) {
    case 0:
        em_mov00(em);
        break;
    }
}

static void em_atk00(EMW *em) {
    em29_to_normal(em);
}

static void em_move03(EMW *em) {
    switch (em->x15) {
    case 0:
        em_atk00(em);
        break;
    }
}

static void em_dm00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 10;
        break;
    case 1:
        if (--em->work08 <= 0) {
            em29_to_normal(em);
        }
        break;
    }
}

static void em_dm01(EMW *em) {
    em29_to_normal(em);
}

static void em_move04(EMW *em) {
    switch (em->x15) {
    case 0:
        em_dm00(em);
        break;
    case 1:
        em_dm01(em);
        break;
    }
}

static void em_move05(EMW *em) {
    EM29_TYPE *t = &em29_type_tbl[em->type];

    em->x40C = 10;
    em->x40E = 10;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x3C;
        Em_se_req2(em, t->se, 0, em->pos, 1, 0);
        Eft02_set3(em, 0, 9, 0, em->pos, em->scale[0]);
        Eft02_set3(em, 0, 9, 1, em->pos, em->scale[0]);
        Eft13_set_pos2(5.0f * em->scale[0], em, em->pos, 0x22);
        if (em->type == 4) {
            if (em->stg == game_w.stage) {
                eft14_set(em->pos, 0, 3.0f);
            }
            Shell09_set(em->pos, 0x10, em->stg);
        }
        break;
    case 1:
        em->x04++;
        em->x01 = 0;
        break;
    }
}

void em29_main(EMW *em) {
    u8 dmg[4];
    EM29W *w = (EM29W *)em->ex;

    if (w->no_hp != 0) {
        em->x8BB = 10;
    }
    switch (Em_Dmg_Sys(em, dmg)) {
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 14:
        break;
    case 1:
        em_act_set(em, 5, 0);
        break;
    case 12:
    case 13:
        em_act_set(em, 4, 0);
        em_dur_set(em, 0);
        break;
    }
    switch (em->x734) {
    case 3:
        if (em->x839 != 0) {
            em_cmd_ck(em);
            em->x839 = 0;
        }
        break;
    }
    em29_main_sub(em);
    if (em->x6FF != 0) {
        em29_main_sub(em);
        em->x6FF = 0;
    }
}

void em29_main_sub(EMW *em) {
    switch (em->mode) {
    case 0:
        em_move00(em);
        break;
    case 1:
        em_move01(em);
        break;
    case 2:
        em_move00(em);
        break;
    case 3:
        em_move03(em);
        break;
    case 4:
        em_move04(em);
        break;
    case 5:
        em_move05(em);
        break;
    case 6:
        em_move04(em);
        break;
    case 7:
        em_move04(em);
        break;
    }
}

void em29_effect_move(EMW *em) {
    EM29W *w = (EM29W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
    case 1:
        break;
    }
}

/* A file static in the original; named by its address because data outside
 * this file (the monster program tables) points at it. */
void dummy_em_prog_006147C0(void) {
}
