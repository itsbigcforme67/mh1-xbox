/* em02_init - game.bin 0x0057EFA0-0x0057F1E0: monster 2 setup. Quest 0 on
 * stage 25 places it by spawn slot; quests 0xB9/0xCF give it 20000 hit
 * points, otherwise it uses (and rounds up) the hit points carried in
 * game_w.x218 (32000 when none). Meanings are guesses. */
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
    s32 x24;            /* 0x24 item kind (Em09_item_sub) */
    u8 _pad28[0x46 - 0x28];
    s16 item;           /* 0x46 stolen item, -1 none */
    s16 item_num;       /* 0x48 */
    u8 _pad4A;
    u8 x4B;             /* 0x4B */
    u8 _pad4C[0x52 - 0x4C];
    s16 hp;             /* 0x52 hit points at start (em02) */
} EM_INITW;

extern QUEST_W quest_w;

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void em_dur_init(EMW *);

extern s16 em02_stay_timer_tbl[];
extern s16 em02_runaway_timer_tbl[];

void em02_act_set(EMW *em, int kind, u16 no, u16 arg);

void em02_init(EMW *em) {
    EM_INITW *w = (EM_INITW *)em->ex;

    if (quest_w.no == 0) {
        switch (game_w.stage) {
        case 25:
            em->pos[0] = 12000.0f - 300.0f * em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 16000.0f - 300.0f * em->x13;
            break;
        }
    }
    if (quest_w.no == 0xB9 || quest_w.no == 0xCF) {
        em->x302 = 20000;
        em->x792 = 20000;
    } else {
        s16 *hp = &game_w.x218;

        if (*hp == 0) {
            *hp = 32000;
        } else if (*hp <= 0x1900) {
            *hp = 0x1900;
        } else if (*hp <= 0x3200) {
            *hp = 0x3200;
        } else if (*hp <= 0x4B00) {
            *hp = 0x4B00;
        } else if (*hp <= 0x6400) {
            *hp = 0x6400;
        }
        em->x792 = 32000;
        em->x302 = game_w.x218;
    }
    w->hp = em->x302;
    em->x88B = 1;
    em->x765 = 1;
    em->stay_tm = em02_stay_timer_tbl[em->stg];
    em->runaway_tm = em02_runaway_timer_tbl[em->stg];
    em->x7EE = 15;
    em->x888 = 1;
    em_char_set(em, 1, 0, 0);
    em->x388 = 0;
    em02_act_set(em, 0, 1, 0);
}
